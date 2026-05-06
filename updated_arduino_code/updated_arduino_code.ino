#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SS_PIN 10
#define RST_PIN A1
MFRC522 rfid(SS_PIN, RST_PIN);

#define OLED_W   128
#define OLED_H   32
#define OLED_ADDR 0x3C
Adafruit_SSD1306 oled(OLED_W, OLED_H, &Wire, -1);
bool oledReady = false;

// =========================================================================
// Final wiring (2026-05-06) - SINGLE Arduino (servos + sensors merged)
// Everything runs on one Uno now; servo_arduino.ino is no longer used.
//
//   D2  MG90S #2 (lid)     D9  Active buzzer
//   D3  HC-SR04 TRIG       D10..D13, A1  RFID MFRC522
//   D4  HC-SR04 ECHO       A0  PIR
//   D5  SG90  (auxiliary)  A4/A5  OLED I2C (SDA/SCL)
//   D6  MG90S #1 (lid)
//   D7  Stepper STEP
//   D8  Stepper DIR
//
// PIR (A0) is the user-facing trigger: motion turns the OLED welcome on
// and (when re-enabled in MainWindow) starts the classify -> rotate ->
// open -> wait 1s -> close -> measure cycle. The full cycle now runs
// locally on this Arduino - no LID:OPEN / MEASURE serial events.
// =========================================================================

const int pirPin    = A0;   // PIR motion sensor
const int stepPin   = 7;    // stepper STEP
const int dirPin    = 8;    // stepper DIR
const int buzzerPin = 9;    // active buzzer; tone() works for passive too

const int sgPin   = 5;      // SG90  (auxiliary, parked at neutral)
const int mg1Pin  = 6;      // MG90S #1 - bin lid
const int mg2Pin  = 2;      // MG90S #2 - bin lid (driven in sync with #1)
const int trigPin = 3;      // HC-SR04 TRIG
const int echoPin = 4;      // HC-SR04 ECHO

Servo sg, mg1, mg2;

bool appBtn[4] = {false, false, false, false};
String serialBuffer = "";

int currentBin = 0;
unsigned long lastBinMoveTime = 0;
bool isAwayFromHome = false;

// Visual "machine on" state. Latched true on the first PIR motion edge
// after boot; the OLED then shows the welcome screen.
bool machineOn = false;

// Lid travel and bin geometry. Tune mechanically once you've found the
// right angles with lid_servo_test.ino. Both MG90S (D6 and D2) get the
// same angle - no mirroring.
const int LID_OPEN_DEG  = 90;   // tune via the test sketch
const int LID_CLOSE_DEG = 0;    // both servos park here at rest
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
  oled.fillCircle(cx - 3, cy - 1, 3, SSD1306_WHITE);
  oled.fillCircle(cx + 3, cy - 1, 3, SSD1306_WHITE);
  oled.fillTriangle(cx - 5, cy + 1, cx + 5, cy + 1, cx, cy + 6, SSD1306_WHITE);
}

void drawMouth(int cx, int cy, int style) {
  if (!oledReady) return;
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
  oled.drawCircle(x, y, 3, SSD1306_WHITE);
  oled.drawLine(x - 2, y - 3, x - 1, y - 5, SSD1306_WHITE);
  oled.drawLine(x,     y - 3, x,     y - 5, SSD1306_WHITE);
  oled.drawLine(x + 2, y - 3, x + 1, y - 5, SSD1306_WHITE);
}

void drawZzz(int x, int y, int phase) {
  if (!oledReady) return;
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  int off = (phase % 3) * 2;
  oled.setCursor(x,      y     - off); oled.print('z');
  oled.setCursor(x +  6, y - 6 - off); oled.print('Z');
  oled.setCursor(x + 12, y - 12 - off); oled.print('Z');
}

void drawWrench(int cx, int cy) {
  if (!oledReady) return;
  oled.drawLine(cx - 2, cy - 5, cx + 2, cy + 5, SSD1306_WHITE);
  oled.drawCircle(cx - 3, cy - 6, 2, SSD1306_WHITE);
  oled.drawRect(cx + 1, cy + 4, 4, 3, SSD1306_WHITE);
}


// ----- Buzzer helper ----------------------------------------------------
// Short audible cue; works for both passive (PWM-driven) and active
// (just a HIGH/LOW toggle) buzzers via tone().
void beep(unsigned int hz, unsigned long ms) {
  tone(buzzerPin, hz, ms);
}

// ----- Lid + HC-SR04 helpers --------------------------------------------
// Both MG90S now get the same angle (no mirror on D6). The previous
// 180-deg mirror rotated D6 the wrong way; un-mirroring sends it the
// other clockwise direction.
//   setLid(0)  -> mg1=0,  mg2=0   (closed)
//   setLid(90) -> mg1=90, mg2=90  (open)
void setLid(int deg) {
  mg1.write(deg);   // D6
  mg2.write(deg);   // D2
}

long readSingleEchoUs() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(4);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return (long)pulseIn(echoPin, HIGH, 40000UL); // 40 ms cap (~6.8 m)
}

long readDistanceCm() {
  long samples[3];
  int valid = 0;
  for (int i = 0; i < 3; i++) {
    long us = readSingleEchoUs();
    Serial.print("US_RAW:"); Serial.println(us);
    if (us > 0) samples[valid++] = us;
    delay(60); // datasheet >=60 ms between pings
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
        oled.setCursor(2, 2);
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
        oled.clearDisplay();
        oled.display();
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
      oled.setCursor(2, 0);
      oled.print("REPARATION");

      if (repairTechName.length() > 0) {
        oled.setCursor(30, 25);
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
      oled.setCursor(20, 25);
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
  beep(2000, 120); delay(150);
  beep(2400, 120); delay(150);
  setLid(LID_OPEN_DEG);
  delay(900);
}

void repairCloseAnimation() {
  beep(1800, 200);
  setLid(LID_CLOSE_DEG);
  delay(900);
}

void activateRepairMode() {
  repairArmed = false;
  repairActive = true;

  Serial.println("REPAIR_MODE:ACTIVE");

  labibStart(L_REPAIR_WAKE);

  // Three short beeps as the repair-mode entry cue.
  for (int i = 0; i < 3; i++) {
    beep(2200, 80); delay(160);
  }

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
  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

  Wire.begin();
  if (oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    oledReady = true;
    oled.clearDisplay();
    oled.display();
    Serial.println("OLED:READY");
  } else {
    oledReady = false;
    Serial.println("OLED:NOT_FOUND");
  }

  pinMode(buzzerPin, OUTPUT);
  pinMode(pirPin, INPUT);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);

  mg1.attach(mg1Pin);
  mg2.attach(mg2Pin);
  sg.attach(sgPin);
  sg.write(90); // park SG90 at neutral (auxiliary, unused for now)

  // Boot self-test: park lid closed, sweep open, snap closed. If you
  // don't see this motion at boot, the SG90/MG90S wiring is wrong.
  setLid(LID_CLOSE_DEG);
  delay(600);
  setLid(LID_OPEN_DEG);
  delay(700);
  setLid(LID_CLOSE_DEG);
  delay(600);

  // Boot ack chirp - confirms the buzzer is wired correctly.
  beep(1800, 100);

  Serial.println("IDENT:MAIN");
  Serial.println("MACHINE:OFF");
  Serial.println("MOTION:0");
  Serial.println("=== SYSTEM ONLINE ===");
}


void loop() {

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("RFID_TAG:");
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();

    // Audible ack on RFID scan.
    beep(2400, 80); delay(80);
    beep(2400, 80);

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
          oled.setCursor(0, 19);
          String dn = repairTechName;
          if (dn.length() > 21) dn = dn.substring(0, 21);
          oled.print(dn);
          oled.setCursor(0, 25);
          oled.print("-> Mode reparation");
          oled.display();
        }

        beep(2200, 120); delay(150);
        beep(2200, 120);
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
      
      // Parse distance data from second Arduino
      else if (serialBuffer.startsWith("D:")) {
        int distance = serialBuffer.substring(2).toInt();
        // Forward distance data to the Qt application
        Serial.print("DISTANCE:");
        Serial.println(distance);
      }

      // Parse AI material classification.
      // Bin layout (90 deg apart): 0=PLASTIC, 1=METAL, 2=GLASS, 3=GENERAL_WASTE.
      // Full cycle: display -> rotate -> LID:OPEN -> wait 1s -> LID:CLOSE
      // -> MEASURE. The Qt app forwards the LID:* / MEASURE events to the
      // servo Arduino, which physically drives the servos and HC-SR04.
      else if (serialBuffer.startsWith("AI:")) {
        String material = serialBuffer.substring(3);
        material.trim();
        int targetBin = currentBin;
        if      (material == "PLASTIC")        targetBin = 0;
        else if (material == "METAL")          targetBin = 1;
        else if (material == "GLASS")          targetBin = 2;
        else if (material == "GENERAL_WASTE")  targetBin = 3;

        oledTwoLines("Detecte:", material);
        beep(2400, 60); // brief "got it" cue

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

        // Open the lid (both MG90S in sync), hold 1 s, close it, then
        // ping the HC-SR04 for the new fill level. All local now - no
        // more LID:OPEN / MEASURE serial round-trips.
        beep(2800, 100);     // audible "drop it now" cue
        setLid(LID_OPEN_DEG);
        delay(1000);
        setLid(LID_CLOSE_DEG);
        delay(500);          // let the lid settle before measuring

        long distCm  = readDistanceCm();
        int  fillPct = computeFillPercent(distCm);
        Serial.print("BAC_FILL:");
        Serial.print(currentBin);
        Serial.print(":");
        Serial.println(fillPct);

        if (oledReady) {
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

  // Repair arming timeout (the physical confirmation button is gone with
  // the new wiring; the app can still trigger / close repair via serial).
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

  // ----- PIR is the single trigger now -------------------------------
  // On the rising edge of motion: turn the OLED welcome on (latched
  // forever for now), beep once, and emit MOTION:1 so the Qt app can
  // start a classification cycle. Falling edge just emits MOTION:0.
  static bool lastMotion = false;
  bool motion = (digitalRead(pirPin) == HIGH);
  if (motion != lastMotion) {
    lastMotion = motion;
    Serial.println(motion ? "MOTION:1" : "MOTION:0");
    if (motion && !repairActive && !repairArmed) {
      if (!machineOn) {
        machineOn = true;
        Serial.println("MACHINE:ON");
        oledTwoLines("WasteGuard PRET", "Approchez un dechet");
        beep(2000, 120);
      }
      if (lState == L_OFF) labibStart(L_WAKE);
    }
  }

  // Auto-home after 10 seconds of inactivity
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