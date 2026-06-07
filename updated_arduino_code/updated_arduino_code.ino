#include <ESP32Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =========================================================================
// WasteGuard - ESP32 single-board build (2026-06-07)
//
// Migrated from the Arduino UNO + slave-Arduino setup to ONE ESP32 board.
// The PC still talks to this board over USB serial at 9600 baud using the
// exact same line protocol as before, so the Qt app (mainwindow.cpp /
// bacstatusdialog.cpp) needs no changes.
//
//   Stepper STEP/DIR        GPIO 14 / 27   (carousel, 50 steps = 90 deg)
//   Lid servos (x4)         GPIO 15, 13, 12, 25  (reinforced door, in sync)
//   RFID MFRC522 SS/RST     GPIO 5 / 4     (SPI: SCK18 MISO19 MOSI23)
//   Cycle button            GPIO 32        (INPUT_PULLUP, low = pressed)
//   IR obstacle (presence)  GPIO 33        (low = obstacle -> MOTION:1)
//   Ultrasonic TRIG         GPIO 2         (shared)
//   Ultrasonic ECHO x4      GPIO 34,35,36,39  (one per bin, input-only)
//   OLED 128x32 I2C         SDA21 / SCL22  (Labib animated face)
//
// Flow:
//   IR obstacle (33) detects presence -> wakes OLED, MOTION:1 + MACHINE:ON,
//                            plays the Labib welcome animation.
//   Button (32)           -> "BTN5:PRESSED" on Serial. The Qt app grabs an
//                            ESP32-CAM frame, classifies it, replies
//                            "AI:<MATERIAL>". The AI handler rotates the
//                            carousel (0/90/180/270 deg) and opens all 4
//                            lid servos to 90 deg.
//   Tech-armed + button   -> runs the local repair animation + opens the lid
//                            (no slave board anymore). Tech-armed is set by
//                            Qt via R:TECHNICIEN:<n>.
//   After each drop the 4 ultrasonic sensors report BAC_FILL:<bin>:<pct>
//   for all four compartments.
// =========================================================================

// ----- Stepper -----
const int stepPin = 14;
const int dirPin  = 27;

// ----- Lid servos (4 motors reinforcing the door, driven in sync) -----
const int lidPins[4] = {15, 13, 12, 25};
Servo lid[4];

// ----- Inputs -----
const int buttonPin = 32;   // cycle button (INPUT_PULLUP, low = pressed)
const int irPin     = 33;   // IR obstacle: presence trigger
// Most IR obstacle modules pull the output LOW when something is detected.
// If yours is inverted (HIGH on detect), change this to HIGH.
#define IR_DETECT_LEVEL LOW

// ----- Ultrasonic (1 shared trig, 4 echoes - one per bin) -----
const int trigPin     = 2;
const int echoPins[4] = {34, 35, 36, 39};

// ----- RFID -----
#define SS_PIN  5
#define RST_PIN 4
MFRC522 rfid(SS_PIN, RST_PIN);

// ----- OLED -----
// 0.91" SSD1306 panel: 128x32 (GND/VCC/SCL/SDA module).
#define OLED_W   128
#define OLED_H   32
#define OLED_ADDR 0x3C
Adafruit_SSD1306 oled(OLED_W, OLED_H, &Wire, -1);
bool oledReady = false;

// The Labib face is designed natively for the 32px-tall screen, so no
// vertical offset is needed (YO stays 0).
const int YO = 0;

bool appBtn[4] = {false, false, false, false};
String serialBuffer = "";

int currentBin = 0;
unsigned long lastBinMoveTime = 0;
bool isAwayFromHome = false;

// Visual "machine on" state. Latched true on the first presence edge after
// boot; the OLED then shows the welcome screen.
bool machineOn = false;

// Lid travel and bin geometry. All 4 servos open to 90 deg and close to
// 0 deg together (matches the ESP32 servo wiring).
const int LID_OPEN_DEG  = 90;
const int LID_CLOSE_DEG = 0;
const int BIN_DEPTH_CM  = 25;

enum LabibState {
  L_OFF, L_WAKE, L_HELLO, L_BLINK, L_LOOK, L_SMILE, L_HEART, L_SLEEP, L_FADE,
  L_REPAIR_WAKE, L_REPAIR_FACE, L_REPAIR_IDLE
};
LabibState lState = L_OFF;
unsigned long lStart = 0;

void labibStart(LabibState s) { lState = s; lStart = millis(); }

bool repairArmed    = false;
bool repairActive   = false;
unsigned long repairArmedAt = 0;
const unsigned long REPAIR_ARM_TIMEOUT = 15000;
String repairTechName = "";

const unsigned long DEBOUNCE_MS = 250;


void drawEye(int cx, int cy, int openness, int pupilDx = 0) {
  if (!oledReady) return;
  cy += YO;
  if (openness <= 0) {
    oled.drawLine(cx - 7, cy, cx + 7, cy, SSD1306_WHITE);
    return;
  }
  oled.drawCircle(cx, cy, 7, SSD1306_WHITE);
  oled.fillCircle(cx + pupilDx, cy, 3, SSD1306_WHITE);
  if (openness < 8) {
    int lid = 7 - openness;
    oled.fillRect(cx - 8, cy - 8, 17, lid + 1, SSD1306_BLACK);
    oled.drawLine(cx - 7, cy - 7 + lid, cx + 7, cy - 7 + lid, SSD1306_WHITE);
  }
}

void drawHeartEye(int cx, int cy) {
  if (!oledReady) return;
  cy += YO;
  oled.fillCircle(cx - 3, cy - 1, 3, SSD1306_WHITE);
  oled.fillCircle(cx + 3, cy - 1, 3, SSD1306_WHITE);
  oled.fillTriangle(cx - 5, cy + 1, cx + 5, cy + 1, cx, cy + 6, SSD1306_WHITE);
}

void drawMouth(int cx, int cy, int style) {
  if (!oledReady) return;
  cy += YO;
  switch (style) {
    case 0:
      oled.drawLine(cx - 5, cy, cx + 5, cy, SSD1306_WHITE);
      break;
    case 1:
      oled.drawLine(cx - 6, cy - 1, cx - 3, cy + 2, SSD1306_WHITE);
      oled.drawLine(cx - 3, cy + 2, cx + 3, cy + 2, SSD1306_WHITE);
      oled.drawLine(cx + 3, cy + 2, cx + 6, cy - 1, SSD1306_WHITE);
      break;
    case 2:
      oled.drawCircle(cx, cy + 1, 2, SSD1306_WHITE);
      break;
    case 3:
      oled.drawLine(cx - 4, cy, cx + 4, cy, SSD1306_WHITE);
      oled.drawLine(cx - 4, cy, cx - 2, cy + 2, SSD1306_WHITE);
      oled.drawLine(cx + 4, cy, cx + 2, cy + 2, SSD1306_WHITE);
      break;
  }
}

void drawHand(int x, int y) {
  if (!oledReady) return;
  y += YO;
  oled.drawCircle(x, y, 3, SSD1306_WHITE);
  oled.drawLine(x - 2, y - 3, x - 1, y - 5, SSD1306_WHITE);
  oled.drawLine(x,     y - 3, x,     y - 5, SSD1306_WHITE);
  oled.drawLine(x + 2, y - 3, x + 1, y - 5, SSD1306_WHITE);
}

void drawZzz(int x, int y, int phase) {
  if (!oledReady) return;
  y += YO;
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  int off = (phase % 3) * 2;
  oled.setCursor(x,      y     - off); oled.print('z');
  oled.setCursor(x +  6, y - 6 - off); oled.print('Z');
  oled.setCursor(x + 12, y - 12 - off); oled.print('Z');
}

void drawWrench(int cx, int cy) {
  if (!oledReady) return;
  cy += YO;
  oled.drawLine(cx - 2, cy - 5, cx + 2, cy + 5, SSD1306_WHITE);
  oled.drawCircle(cx - 3, cy - 6, 2, SSD1306_WHITE);
  oled.drawRect(cx + 1, cy + 4, 4, 3, SSD1306_WHITE);
}


// ----- Lid + HC-SR04 helpers --------------------------------------------
// All 4 servos move together: 0 deg closed, 90 deg open. If a pair is
// physically mounted mirrored, flip those entries to (LID_OPEN_DEG - deg).
void setLid(int deg) {
  for (int i = 0; i < 4; i++) lid[i].write(deg);
}

long readSingleEchoUs(int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(4);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return (long)pulseIn(echoPin, HIGH, 40000UL); // 40 ms cap (~6.8 m)
}

long readDistanceCm(int echoPin) {
  long samples[3];
  int valid = 0;
  for (int i = 0; i < 3; i++) {
    long us = readSingleEchoUs(echoPin);
    if (us > 0) samples[valid++] = us;
    delay(40);
  }
  if (valid == 0) return -1;
  for (int i = 1; i < valid; i++) {
    long key = samples[i];
    int j = i - 1;
    while (j >= 0 && samples[j] > key) { samples[j+1] = samples[j]; j--; }
    samples[j+1] = key;
  }
  return samples[valid / 2] / 58;
}

int computeFillPercent(long distCm) {
  if (distCm < 0) return -1;
  if (distCm >= BIN_DEPTH_CM) return 0;
  if (distCm <= 0) return 100;
  return (int)(((BIN_DEPTH_CM - distCm) * 100L) / BIN_DEPTH_CM);
}

// Read every bin's ultrasonic and report BAC_FILL:<bin>:<pct> for all four.
void reportAllBinFills() {
  for (int b = 0; b < 4; b++) {
    long distCm  = readDistanceCm(echoPins[b]);
    int  fillPct = computeFillPercent(distCm);
    Serial.print("BAC_FILL:");
    Serial.print(b);
    Serial.print(":");
    Serial.println(fillPct);
  }
}

// ----- OLED helpers -----------------------------------------------------
void oledTwoLines(const String &top, const String &bottom) {
  if (!oledReady) return;
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);   oled.print(top);
  oled.setCursor(0, 16);  oled.print(bottom);
  oled.display();
}

void oledClear() {
  if (!oledReady) return;
  oled.clearDisplay();
  oled.display();
}

void labibTick() {
  if (lState == L_OFF || !oledReady) return;
  unsigned long now = millis();
  unsigned long e = now - lStart;

  oled.clearDisplay();
  const int LX = 38, RX = 90, EY = 14;

  switch (lState) {
    case L_WAKE: {
      int prog = (int)(e * 100UL / 600UL); if (prog > 100) prog = 100;
      int op = (prog * 8) / 100; if (op < 1) op = 1;
      drawEye(LX, EY, op);
      drawEye(RX, EY, op);
      if (prog > 60) drawMouth(64, 24, 1);
      if (e >= 600) labibStart(L_HELLO);
      break;
    }
    case L_HELLO: {
      drawEye(LX, EY, 8);
      drawEye(RX, EY, 8);
      drawMouth(64, 24, 1);
      int wob = ((e / 120) % 2) ? -3 : 3;
      drawHand(112 + wob, 14);
      if (e > 300 && e < 1400) {
        oled.setTextSize(1);
        oled.setTextColor(SSD1306_WHITE);
        oled.setCursor(2, 2 + YO);
        oled.print("BONJOUR!");
      }
      if (e >= 1700) labibStart(L_BLINK);
      break;
    }
    case L_BLINK: {
      int op;
      if (e < 200) op = 8 - (int)(e * 8 / 200);
      else         op = (int)((e - 200) * 8 / 200);
      if (op < 0) op = 0; if (op > 8) op = 8;
      drawEye(LX, EY, op);
      drawEye(RX, EY, op);
      drawMouth(64, 24, 1);
      if (e >= 400) labibStart(L_LOOK);
      break;
    }
    case L_LOOK: {
      int dx = (e < 333) ? -3 : (e < 666) ? 3 : 0;
      drawEye(LX, EY, 8, dx);
      drawEye(RX, EY, 8, dx);
      drawMouth(64, 24, 0);
      if (e >= 1000) labibStart(L_SMILE);
      break;
    }
    case L_SMILE: {
      drawEye(LX, EY, ((e / 200) % 2) ? 8 : 1); // wink
      drawEye(RX, EY, 8);
      drawMouth(64, 23, 1);
      if (e >= 900) labibStart(L_HEART);
      break;
    }
    case L_HEART: {
      drawHeartEye(LX, EY);
      drawHeartEye(RX, EY);
      drawMouth(64, 24, 1);
      int h = OLED_H - (int)((e % 1200) * OLED_H / 1200);
      oled.fillCircle(60, h, 1, SSD1306_WHITE);
      oled.fillCircle(72, h + 6, 1, SSD1306_WHITE);
      if (e >= 1200) labibStart(L_SLEEP);
      break;
    }
    case L_SLEEP: {
      drawEye(LX, EY, 0);
      drawEye(RX, EY, 0);
      drawMouth(64, 24, 0);
      drawZzz(74, 14, e / 250);
      if (e >= 1600) labibStart(L_FADE);
      break;
    }
    case L_FADE: {
      drawEye(LX, EY, 0);
      drawEye(RX, EY, 0);
      int n = (int)(e * OLED_W / 400UL);
      oled.fillRect(0, 0, n, OLED_H, SSD1306_BLACK);
      if (e >= 400) {
        lState = L_OFF;
        // Don't leave the screen blank - show a persistent idle banner so
        // the user always sees the bin is alive and waiting.
        oledTwoLines("WasteGuard PRET", "Approchez un dechet");
        return;
      }
      break;
    }

    case L_REPAIR_WAKE: {
      int prog = (int)(e * 100UL / 400UL); if (prog > 100) prog = 100;
      int op = (prog * 8) / 100; if (op < 1) op = 1;
      drawEye(LX, EY, op);
      drawEye(RX, EY, op);
      drawMouth(64, 24, 2);
      if (e >= 500) labibStart(L_REPAIR_FACE);
      break;
    }
    case L_REPAIR_FACE: {
      drawEye(LX, EY, 8);
      drawEye(RX, EY, 8, (((e / 300) % 3) - 1) * 2);
      drawMouth(64, 24, 3);

      int wobble = ((e / 200) % 2) ? -2 : 2;
      drawWrench(10 + wobble, 16);

      oled.setTextSize(1);
      oled.setTextColor(SSD1306_WHITE);
      oled.setCursor(2, 0 + YO);
      oled.print("REPARATION");

      if (repairTechName.length() > 0) {
        oled.setCursor(30, 25 + YO);
        String displayName = repairTechName;
        if (displayName.length() > 12) displayName = displayName.substring(0, 12);
        oled.print(displayName);
      }

      if (e >= 2000) labibStart(L_REPAIR_IDLE);
      break;
    }
    case L_REPAIR_IDLE: {
      int cycle = (e / 3000) % 2;
      int subE = e % 3000;

      if (cycle == 0) {
        int dx = 0;
        if (subE > 1000 && subE < 1500) dx = -2;
        else if (subE > 1500 && subE < 2000) dx = 2;
        drawEye(LX, EY, 8, dx);
        drawEye(RX, EY, 8, dx);
        drawMouth(64, 24, 1);
      } else {
        int blinkPhase = subE % 400;
        int op;
        if (blinkPhase < 150) op = 8 - (blinkPhase * 8 / 150);
        else if (blinkPhase < 250) op = 0;
        else op = (blinkPhase - 250) * 8 / 150;
        if (op < 0) op = 0; if (op > 8) op = 8;
        drawEye(LX, EY, op);
        drawEye(RX, EY, op);
        drawMouth(64, 24, 1);
      }

      drawWrench(5, 8);

      oled.setTextSize(1);
      oled.setTextColor(SSD1306_WHITE);
      oled.setCursor(20, 25 + YO);
      String dn = repairTechName;
      if (dn.length() > 14) dn = dn.substring(0, 14);
      oled.print(dn);

      break;
    }

    default: break;
  }
  oled.display();
}


void repairOpenAnimation() {
  setLid(LID_OPEN_DEG);
  delay(900);
}

void repairCloseAnimation() {
  setLid(LID_CLOSE_DEG);
  delay(900);
}

void activateRepairMode() {
  repairArmed = false;
  repairActive = true;

  Serial.println("REPAIR_MODE:ACTIVE");

  labibStart(L_REPAIR_WAKE);

  repairOpenAnimation();
}

void deactivateRepairMode() {
  repairActive = false;
  repairArmed = false;
  repairTechName = "";

  repairCloseAnimation();

  lState = L_OFF;
  if (oledReady) {
    oled.clearDisplay();
    oled.display();
  }

  Serial.println("REPAIR_MODE:CLOSED");
}


void setup() {
  Serial.begin(9600);   // matches the Qt app (arduino.cpp opens at 9600)

  SPI.begin();
  rfid.PCD_Init();

  Wire.begin(21, 22);   // explicit ESP32 I2C pins: SDA=21, SCL=22
  if (oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    oledReady = true;
    oled.clearDisplay();
    oled.display();
    Serial.println("OLED:READY");
  } else {
    oledReady = false;
    Serial.println("OLED:NOT_FOUND");
  }

  pinMode(irPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  pinMode(trigPin, OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(echoPins[i], INPUT);
  digitalWrite(trigPin, LOW);

  // Allocate PWM timers and attach the 4 lid servos.
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  for (int i = 0; i < 4; i++) {
    lid[i].setPeriodHertz(50);
    lid[i].attach(lidPins[i], 500, 2400);
  }

  // Boot self-test: park closed, sweep open, snap closed. If you don't see
  // this motion at boot, the servo wiring is wrong.
  setLid(LID_CLOSE_DEG);
  delay(600);
  setLid(LID_OPEN_DEG);
  delay(700);
  setLid(LID_CLOSE_DEG);
  delay(600);

  Serial.println("IDENT:MAIN");
  Serial.println("MACHINE:OFF");
  Serial.println("MOTION:0");
  Serial.println("=== SYSTEM ONLINE ===");

  // Boot welcome + Labib animation. This runs regardless of the IR sensor,
  // so if the OLED is wired correctly you WILL see the face at power-on.
  // If nothing shows here, the problem is the OLED/I2C wiring, not the IR.
  oledTwoLines("WasteGuard PRET", "Approchez un dechet");
  delay(800);
  labibStart(L_WAKE);
}


void loop() {

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // Build the UID once (lowercase hex, same format the Qt app expects).
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 0x10) uid += "0";
      uid += String(rfid.uid.uidByte[i], HEX);
    }
    Serial.print("RFID_TAG:");
    Serial.println(uid);

    // Show the scanned badge on the OLED, like the older build did.
    if (oledReady) {
      oled.clearDisplay();
      oled.setTextSize(1);
      oled.setTextColor(SSD1306_WHITE);
      oled.setCursor(0, 0);
      oled.print("Badge detecte:");
      oled.setTextSize(2);
      oled.setCursor(0, 14);
      oled.print(uid);
      oled.display();
    }
    lState = L_OFF;   // keep the badge on screen (don't let Labib overwrite it)

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      serialBuffer.trim();
      if (serialBuffer.startsWith("S:") && serialBuffer.length() >= 6) {
        appBtn[0] = (serialBuffer.charAt(2) == '1');
        appBtn[1] = (serialBuffer.charAt(3) == '1');
        appBtn[2] = (serialBuffer.charAt(4) == '1');
        appBtn[3] = (serialBuffer.charAt(5) == '1');
      }

      else if (serialBuffer.startsWith("R:TECHNICIEN:")) {
        repairTechName = serialBuffer.substring(13);
        repairArmed = true;
        repairArmedAt = millis();
        Serial.print("REPAIR_ARMED:");
        Serial.println(repairTechName);

        if (oledReady) {
          oled.clearDisplay();
          oled.setTextSize(1);
          oled.setTextColor(SSD1306_WHITE);
          oled.setCursor(0, 0);
          oled.print("Technicien:");
          oled.setCursor(0, 11);
          String dn = repairTechName;
          if (dn.length() > 21) dn = dn.substring(0, 21);
          oled.print(dn);
          oled.setCursor(0, 22);
          oled.print("-> Mode reparation");
          oled.display();
        }
      }

      else if (serialBuffer == "R:CLOSE") {
        if (repairActive) {
          deactivateRepairMode();
        } else {
          repairArmed = false;
          repairTechName = "";
          if (oledReady) {
            oled.clearDisplay();
            oled.display();
          }
        }
      }

      // Manual hardware self-test. Close the Qt app, open the Arduino Serial
      // Monitor at 9600, and send "TEST". This exercises all 4 lid servos and
      // the stepper directly, with NO camera/AI involved - so you can confirm
      // the motor hardware works on its own.
      else if (serialBuffer == "TEST") {
        Serial.println("TEST:START");
        oledTwoLines("TEST MOTEURS", "Servos + stepper");

        Serial.println("TEST:servos open");
        setLid(LID_OPEN_DEG);  delay(1200);
        Serial.println("TEST:servos close");
        setLid(LID_CLOSE_DEG); delay(1200);

        Serial.println("TEST:stepper CW");
        digitalWrite(dirPin, HIGH);
        for (int i = 0; i < 200; i++) {
          digitalWrite(stepPin, HIGH); delayMicroseconds(2000);
          digitalWrite(stepPin, LOW);  delayMicroseconds(2000);
        }
        delay(400);
        Serial.println("TEST:stepper CCW");
        digitalWrite(dirPin, LOW);
        for (int i = 0; i < 200; i++) {
          digitalWrite(stepPin, HIGH); delayMicroseconds(2000);
          digitalWrite(stepPin, LOW);  delayMicroseconds(2000);
        }
        Serial.println("TEST:DONE");
      }

      // Parse AI material classification.
      // Bin layout (90 deg apart): 0=PLASTIC, 1=METAL, 2=GLASS, 3=GENERAL_WASTE.
      // Full cycle: display -> rotate -> open lid -> wait 1s -> close ->
      // measure all 4 bins.
      else if (serialBuffer.startsWith("AI:")) {
        String material = serialBuffer.substring(3);
        material.trim();
        int targetBin = currentBin;
        if      (material == "PLASTIC")        targetBin = 0;
        else if (material == "METAL")          targetBin = 1;
        else if (material == "GLASS")          targetBin = 2;
        else if (material == "GENERAL_WASTE")  targetBin = 3;

        oledTwoLines("Detecte:", material);

        // Rotate the carousel to the matching bin (shortest path, 90 deg/step).
        if (targetBin != currentBin) {
          int diff = targetBin - currentBin;
          if (diff > 2) diff -= 4;
          else if (diff < -2) diff += 4;

          if (diff != 0) {
            digitalWrite(dirPin, diff > 0 ? HIGH : LOW);
            int steps = abs(diff) * 50; // 50 steps per 90 degrees
            for (int i = 0; i < steps; i++) {
              digitalWrite(stepPin, HIGH);
              delayMicroseconds(2000);
              digitalWrite(stepPin, LOW);
              delayMicroseconds(2000);
            }
            currentBin = targetBin;
          }
        }

        // Open all 4 lid servos in sync, hold 1 s, close, then measure
        // every bin's fill level.
        setLid(LID_OPEN_DEG);
        delay(1000);
        setLid(LID_CLOSE_DEG);
        delay(500);          // let the lid settle before measuring

        reportAllBinFills();

        if (oledReady) {
          long distCm  = readDistanceCm(echoPins[currentBin]);
          int  fillPct = computeFillPercent(distCm);
          oled.clearDisplay();
          oled.setTextSize(1);
          oled.setTextColor(SSD1306_WHITE);
          oled.setCursor(0, 0);
          oled.print(material);
          oled.setCursor(0, 16);
          oled.print("Bac ");
          oled.print(currentBin);
          oled.print(" : ");
          if (fillPct < 0) oled.print("--");
          else { oled.print(fillPct); oled.print(" %"); }
          oled.display();
        }

        // Re-arm auto-home so we drift back to bin 0 after 10 s of idleness.
        if (currentBin != 0) {
          lastBinMoveTime = millis();
          isAwayFromHome = true;
        } else {
          isAwayFromHome = false;
        }
      }

      serialBuffer = "";
    } else {
      serialBuffer += c;
    }
  }

  // Repair arming timeout.
  if (repairArmed && !repairActive
      && millis() - repairArmedAt > REPAIR_ARM_TIMEOUT) {
    repairArmed = false;
    repairTechName = "";
    Serial.println("REPAIR_ARMED:TIMEOUT");
    if (oledReady) {
      oled.clearDisplay();
      oled.setTextSize(1);
      oled.setTextColor(SSD1306_WHITE);
      oled.setCursor(10, 12);
      oled.print("Delai expire");
      oled.display();
      delay(1500);
      oled.clearDisplay();
      oled.display();
    }
  }

  // ----- Presence (IR obstacle on GPIO33) ----------------------------
  // Replaces the old PIR. Low = obstacle/presence detected. On the rising
  // edge of presence: turn the OLED welcome on (latched), and emit MOTION:1
  // so the Qt app flips the bac to EN_SERVICE. Falling edge emits MOTION:0.
  static bool lastMotion = false;
  static unsigned long lastIrDbg = 0;
  int irRaw = digitalRead(irPin);
  if (millis() - lastIrDbg > 1000) {        // 1 Hz raw IR readout for diagnosis
    lastIrDbg = millis();
    Serial.print("IR_RAW:"); Serial.println(irRaw);
  }
  bool motion = (irRaw == IR_DETECT_LEVEL);
  if (motion != lastMotion) {
    lastMotion = motion;
    Serial.println(motion ? "MOTION:1" : "MOTION:0");
    if (motion && !repairActive && !repairArmed) {
      if (!machineOn) {
        machineOn = true;
        Serial.println("MACHINE:ON");
        oledTwoLines("WasteGuard PRET", "Approchez un dechet");
      }
      if (lState == L_OFF) labibStart(L_WAKE);
    }
  }

  // ----- Cycle button (GPIO32) ---------------------------------------
  // Rising edge of "pressed" (INPUT_PULLUP -> LOW = pressed). Two roles:
  //   - Tech-armed (R:TECHNICIEN:<name> received): run the local repair
  //     animation and open the lid.
  //   - Otherwise: emit "BTN5:PRESSED" so the Qt app fetches a frame from
  //     the ESP32-CAM, classifies it, and replies AI:<MAT>. The AI:<MAT>
  //     branch then rotates the stepper and opens the lid.
  static bool lastBtn = HIGH;
  static unsigned long lastBtnEdgeMs = 0;
  bool btn = (digitalRead(buttonPin) == LOW);
  unsigned long nowMs = millis();
  if (btn && !lastBtn && (nowMs - lastBtnEdgeMs) > DEBOUNCE_MS) {
    lastBtnEdgeMs = nowMs;

    if (repairArmed && !repairActive) {
      activateRepairMode();
    } else if (!repairActive) {
      Serial.println("BTN5:PRESSED");
    }
  }
  lastBtn = btn;

  // Auto-home after 10 seconds of inactivity.
  if (isAwayFromHome && (millis() - lastBinMoveTime >= 10000)) {
    int diff = 0 - currentBin; // target is 0
    if (diff > 2) diff -= 4;
    else if (diff < -2) diff += 4;

    if (diff != 0) {
      digitalWrite(dirPin, diff > 0 ? HIGH : LOW);
      int steps = abs(diff) * 50;
      for (int i = 0; i < steps; i++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(2000);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(2000);
      }
    }
    currentBin = 0;
    isAwayFromHome = false;
  }

  labibTick();

  delay(20);
}
