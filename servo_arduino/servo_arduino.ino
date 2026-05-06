// =========================================================================
// *** DEPRECATED - NO LONGER USED (2026-05-06) ***
// The project went back to a single Arduino. All the logic this sketch
// provided (servos + HC-SR04) lives in updated_arduino_code.ino now.
// Kept on disk for reference / in case the two-board split is revived.
// =========================================================================
// Servo + ultrasonic controller (Arduino #1)
// -------------------------------------------------------------------------
// Owns the bin-lid mechanism AND the HC-SR04 fill sensor. Talks to the
// Qt app over USB serial.
//
// Wiring:
//   D3 -> SG90  (auxiliary servo, parked at neutral - reserve for later)
//   D4 -> MG90S #1   ┐  the "two motors" the user wants opened together
//   D5 -> MG90S #2   ┘  (each on its own pin, but driven in sync)
//   D6 -> HC-SR04 TRIG
//   D7 -> HC-SR04 ECHO
//   5V -> servo VCC (external supply recommended for the MG90S pair)
//   GND -> servos + sensor + Arduino GND (common ground mandatory)
//
// Commands accepted on serial (one per line, '\n' terminated):
//   OPEN     -> drive both MG90S to LID_OPEN_DEG
//   CLOSE    -> drive both MG90S to LID_CLOSE_DEG
//   MEASURE  -> ping HC-SR04, reply with FILL:<pct>
//   PING     -> reply with PONG
//
// Replies:
//   IDENT:SERVO     - sent once at boot
//   LID:OPENED      - after OPEN completes
//   LID:CLOSED      - after CLOSE completes
//   FILL:<pct>      - after MEASURE (-1 if the echo timed out)
//   US_RAW:<us>     - per-sample raw echo time, useful for diagnosis
//   PONG            - reply to PING
// =========================================================================

#include <Servo.h>

Servo mg1;   // MG90S on D4
Servo mg2;   // MG90S on D5
Servo sg;    // SG90  on D3 (reserved)

const int MG1_PIN = 4;
const int MG2_PIN = 5;
const int SG_PIN  = 3;

const int TRIG_PIN = 6;
const int ECHO_PIN = 7;

// Tune mechanically. Both MG90S receive the same angle.
const int LID_OPEN_DEG  = 90;
const int LID_CLOSE_DEG = 0;

// HC-SR04 baseline: distance from sensor mount to an empty bin floor.
const int BIN_DEPTH_CM = 25;

String buf = "";

// ----- HC-SR04 helpers ---------------------------------------------------
long readSingleEchoUs() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return (long)pulseIn(ECHO_PIN, HIGH, 40000UL);
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
  // insertion sort for the median
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

// ----- Lid helpers -------------------------------------------------------
void setLid(int deg) {
  mg1.write(deg);
  mg2.write(deg);
}

void doMeasure() {
  long distCm = readDistanceCm();
  int pct = computeFillPercent(distCm);
  Serial.print("FILL:");
  Serial.println(pct);
}

void handleCommand(const String &cmd) {
  if (cmd == "OPEN") {
    setLid(LID_OPEN_DEG);
    Serial.println("LID:OPENED");
  } else if (cmd == "CLOSE") {
    setLid(LID_CLOSE_DEG);
    Serial.println("LID:CLOSED");
  } else if (cmd == "MEASURE") {
    doMeasure();
  } else if (cmd == "PING") {
    Serial.println("PONG");
  } else if (cmd.length() > 0) {
    Serial.print("ERR:unknown:");
    Serial.println(cmd);
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  mg1.attach(MG1_PIN);
  mg2.attach(MG2_PIN);
  sg.attach(SG_PIN);

  // Park everything at a known position. Boot self-test: closed -> open
  // -> closed so a wiring problem is immediately visible.
  setLid(LID_CLOSE_DEG);
  sg.write(90);
  delay(600);
  setLid(LID_OPEN_DEG);
  delay(700);
  setLid(LID_CLOSE_DEG);
  delay(600);

  Serial.println("IDENT:SERVO");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      buf.trim();
      if (buf.length() > 0) handleCommand(buf);
      buf = "";
    } else {
      buf += c;
      if (buf.length() > 32) buf = ""; // overflow guard
    }
  }
}
