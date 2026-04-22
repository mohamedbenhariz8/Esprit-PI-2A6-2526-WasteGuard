#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

// RFID Configuration
#define SS_PIN 10
#define RST_PIN A1
MFRC522 rfid(SS_PIN, RST_PIN);

// Servos
Servo servo1;
Servo servo2;

// Updated Pins
const int led1 = 2;
const int led2 = 3;

const int btn1 = 4;
const int btn2 = 5;
const int btn3 = 6;
const int btn4 = 7;

const int pirPin = A0; // Moved to Analog 0

// App-driven button states
bool appBtn[4] = {false, false, false, false};
String serialBuffer = "";

void setup() {
  Serial.begin(9600);
  
  // Init SPI and RFID
  SPI.begin();
  rfid.PCD_Init();

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(btn4, INPUT_PULLUP);

  pinMode(pirPin, INPUT);

  // New Servo Pins
  servo1.attach(8);
  servo2.attach(9);

  servo1.write(90);
  servo2.write(90);

  Serial.println("=== SYSTEM ONLINE ===");
}

void loop() {
  
  // ──────── 1. RFID SCANNER (With Sequential LED Animation) ────────
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("RFID_TAG:");
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();

    // --- Animation Sequence ---
    digitalWrite(led1, HIGH); // Turn on first LED
    delay(100); 
    digitalWrite(led1, LOW); 
    digitalWrite(led2, HIGH); // Turn on second LED
    delay(100);               
    digitalWrite(led2, LOW);

    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    delay(100);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    // -------------------------

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
  // ──────── 2. QT SERIAL COMMANDS ────────
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

  // ──────── 3. LED LOGIC (Physical + App) ────────
  if (digitalRead(btn1) == LOW || appBtn[0]) {
    digitalWrite(led1, HIGH);
  } else {
    digitalWrite(led1, LOW);
  }

  if (digitalRead(btn2) == LOW || appBtn[1]) {
    digitalWrite(led2, HIGH);
  } else {
    digitalWrite(led2, LOW);
  }

  // ──────── 4. SERVO LOGIC ────────
  if (digitalRead(btn3) == LOW || appBtn[2]) {
    servo1.write(0);
  } else {
    servo1.write(90);
  }

  if (digitalRead(btn4) == LOW || appBtn[3]) {
    servo2.write(180);
  } else {
    servo2.write(90);
  }

  // ──────── 5. PIR SENSOR ────────
  if (digitalRead(pirPin) == HIGH) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    // Serial.println("PIR:Motion"); // Optional: Alert Qt
  }

  delay(50); 
}