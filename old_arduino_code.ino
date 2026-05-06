#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== RFID =====
#define SS_PIN 10
#define RST_PIN A1
MFRC522 rfid(SS_PIN, RST_PIN);

// ===== OLED 128x32 (0.91"/0.81" SSD1306) =====
// Si tu as le 128x64 (0.96"), change OLED_H a 64.
#define OLED_W   128
#define OLED_H   32
#define OLED_ADDR 0x3C
Adafruit_SSD1306 oled(OLED_W, OLED_H, &Wire, -1);

// ===== Servos / boutons / LEDs =====
Servo servo1, servo2;
const int led1 = 2, led2 = 3;
const int btn1 = 4, btn2 = 5, btn3 = 6, btn4 = 7;
const int pirPin = A0;

bool appBtn[4] = {false, false, false, false};
String serialBuffer = "";

// ===== Mini machine a etats Labib =====
enum LabibState {
  L_OFF, L_WAKE, L_HELLO, L_BLINK, L_LOOK, L_SMILE, L_HEART, L_SLEEP, L_FADE
};
LabibState lState = L_OFF;
unsigned long lStart = 0;

void labibStart(LabibState s) { lState = s; lStart = millis(); }

// ----- Helpers de dessin -----
void drawEye(int cx, int cy, int openness, int pupilDx = 0) {
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
  oled.fillCircle(cx - 3, cy - 1, 3, SSD1306_WHITE);
  oled.fillCircle(cx + 3, cy - 1, 3, SSD1306_WHITE);
  oled.fillTriangle(cx - 5, cy + 1, cx + 5, cy + 1, cx, cy + 6, SSD1306_WHITE);
}

void drawMouth(int cx, int cy, int style) {
  switch (style) {
    case 0: // neutre
      oled.drawLine(cx - 5, cy, cx + 5, cy, SSD1306_WHITE);
      break;
    case 1: // sourire
      oled.drawLine(cx - 6, cy - 1, cx - 3, cy + 2, SSD1306_WHITE);
      oled.drawLine(cx - 3, cy + 2, cx + 3, cy + 2, SSD1306_WHITE);
      oled.drawLine(cx + 3, cy + 2, cx + 6, cy - 1, SSD1306_WHITE);
      break;
    case 2: // o surprise
      oled.drawCircle(cx, cy + 1, 2, SSD1306_WHITE);
      break;
  }
}

void drawHand(int x, int y) {
  oled.drawCircle(x, y, 3, SSD1306_WHITE);
  oled.drawLine(x - 2, y - 3, x - 1, y - 5, SSD1306_WHITE);
  oled.drawLine(x,     y - 3, x,     y - 5, SSD1306_WHITE);
  oled.drawLine(x + 2, y - 3, x + 1, y - 5, SSD1306_WHITE);
}

void drawZzz(int x, int y, int phase) {
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  int off = (phase % 3) * 2;
  oled.setCursor(x,      y     - off); oled.print('z');
  oled.setCursor(x +  6, y - 6 - off); oled.print('Z');
  oled.setCursor(x + 12, y - 12 - off); oled.print('Z');
}

// ----- Tick anime appele a chaque loop() -----
void labibTick() {
  if (lState == L_OFF) return;
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
      drawEye(LX, EY, ((e / 200) % 2) ? 8 : 1); // clin d oeil
      drawEye(RX, EY, 8);
      drawMouth(64, 23, 1);
      if (e >= 900) labibStart(L_HEART);
      break;
    }
    case L_HEART: {
      drawHeartEye(LX, EY);
      drawHeartEye(RX, EY);
      drawMouth(64, 24, 1);
      // mini coeurs qui montent
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
      // rideau noir qui balaie
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
    default: break;
  }
  oled.display();
}

void setup() {
  Serial.begin(9600);

  // RFID
  SPI.begin();
  rfid.PCD_Init();

  // OLED
  Wire.begin();
  if (oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    oled.clearDisplay();
    oled.display();
    Serial.println("OLED:READY");
  } else {
    Serial.println("OLED:NOT_FOUND");
  }

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(btn4, INPUT_PULLUP);
  pinMode(pirPin, INPUT);

  servo1.attach(8);
  servo2.attach(9);
  servo1.write(90);
  servo2.write(90);

  Serial.println("MOTION:0");
  Serial.println("=== SYSTEM ONLINE ===");
}

void loop() {
  // 1. RFID
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("RFID_TAG:");
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
    digitalWrite(led1, HIGH); delay(100); digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH); delay(100); digitalWrite(led2, LOW);
    digitalWrite(led1, HIGH); digitalWrite(led2, HIGH); delay(100);
    digitalWrite(led1, LOW);  digitalWrite(led2, LOW);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  // 2. Commandes serie depuis Qt
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
      serialBuffer = "";
    } else {
      serialBuffer += c;
    }
  }

  // 3. LEDs
  digitalWrite(led1, (digitalRead(btn1) == LOW || appBtn[0]) ? HIGH : LOW);
  digitalWrite(led2, (digitalRead(btn2) == LOW || appBtn[1]) ? HIGH : LOW);

  // 4. Servos
  servo1.write((digitalRead(btn3) == LOW || appBtn[2]) ? 0   : 90);
  servo2.write((digitalRead(btn4) == LOW || appBtn[3]) ? 180 : 90);

  // 5. PIR : edge detection -> Qt + reveille Labib
  static bool lastMotion = false;
  bool motion = (digitalRead(pirPin) == HIGH);
  if (motion != lastMotion) {
    lastMotion = motion;
    Serial.println(motion ? "MOTION:1" : "MOTION:0");
    if (motion && lState == L_OFF) labibStart(L_WAKE);
  }
  if (motion) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
  }

  // 6. Anime Labib (non-bloquant)
  labibTick();

  delay(20);  // Reduit pour anim plus fluide (~50fps cap)
}
