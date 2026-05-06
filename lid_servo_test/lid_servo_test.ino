// =========================================================================
// Lid servo angle + direction tuning sketch
// -------------------------------------------------------------------------
// Drives the two MG90S motors on the same pins as the main project:
//   D6 -> MG90S #1   (referred to as "1" on serial)
//   D2 -> MG90S #2   (referred to as "2" on serial)
// Both servos park at 0 deg at boot.
//
// Open the Arduino IDE Serial Monitor at 9600 baud, set line ending to
// "Newline", and type one of:
//
//   --- direction sweeps (visual check) ---
//   1 cw           D6 slow sweep 0 -> 180   (increasing angle)
//   1 ccw          D6 slow sweep 180 -> 0   (decreasing angle)
//   2 cw           D2 slow sweep 0 -> 180
//   2 ccw          D2 slow sweep 180 -> 0
//   b cw           both servos sweep 0 -> 180 in sync
//   b ccw          both servos sweep 180 -> 0 in sync
//   b mix          D6: 0 -> 180,  D2: 180 -> 0  at the same time
//                  (cross-sweep, useful to confirm mirror logic)
//
//   --- absolute angles (precise positioning) ---
//   <num>          both servos go to <num>      (0..180)   e.g. "90"
//   1 <num>        only MG90S #1 (D6) goes there            e.g. "1 45"
//   2 <num>        only MG90S #2 (D2) goes there            e.g. "2 135"
//
//   --- misc ---
//   ?              print the current commanded angles
//   h              show this help again
//
// Note: "cw" labels the direction of INCREASING angle (0 -> 180). Whether
// that physically looks clockwise or counter-clockwise depends on which
// side of the servo you're viewing - just watch the horn during the
// sweep and you'll know which is which for your mounting.
// =========================================================================

#include <Servo.h>

const int MG1_PIN = 6;   // "1"
const int MG2_PIN = 2;   // "2"

Servo mg1, mg2;

int ang1 = 0;
int ang2 = 0;

String buf = "";

const int SWEEP_STEP = 5;     // degrees per increment
const int SWEEP_DELAY_MS = 60; // ms between increments (lower = faster)

void writeBoth(int a) {
  a = constrain(a, 0, 180);
  ang1 = a; ang2 = a;
  mg1.write(a);
  mg2.write(a);
  Serial.print("BOTH -> "); Serial.println(a);
}

void writeOne(int which, int a) {
  a = constrain(a, 0, 180);
  if (which == 1) {
    ang1 = a;
    mg1.write(a);
    Serial.print("MG1 (D6) -> "); Serial.println(a);
  } else if (which == 2) {
    ang2 = a;
    mg2.write(a);
    Serial.print("MG2 (D2) -> "); Serial.println(a);
  }
}

// Sweep a single servo from `from` to `to` (inclusive). Direction
// inferred from sign of (to - from).
void sweepOne(int which, int from, int to) {
  Serial.print("SWEEP MG"); Serial.print(which);
  Serial.print(" "); Serial.print(from);
  Serial.print(" -> "); Serial.println(to);
  int step = (to >= from) ? SWEEP_STEP : -SWEEP_STEP;
  for (int a = from; (step > 0) ? (a <= to) : (a >= to); a += step) {
    if (which == 1) { mg1.write(a); ang1 = a; }
    else            { mg2.write(a); ang2 = a; }
    delay(SWEEP_DELAY_MS);
  }
  // Land exactly on `to`
  if (which == 1) { mg1.write(to); ang1 = to; }
  else            { mg2.write(to); ang2 = to; }
}

// Sweep both servos at the same time. Each can have its own start/end
// so you can run a "cross sweep" to compare directions side-by-side.
void sweepBoth(int from1, int to1, int from2, int to2) {
  Serial.print("SWEEP BOTH | MG1 ");
  Serial.print(from1); Serial.print("->"); Serial.print(to1);
  Serial.print("  MG2 ");
  Serial.print(from2); Serial.print("->"); Serial.println(to2);

  int totalSteps = max(abs(to1 - from1), abs(to2 - from2)) / SWEEP_STEP;
  if (totalSteps <= 0) totalSteps = 1;

  for (int i = 0; i <= totalSteps; i++) {
    int a1 = from1 + (long)(to1 - from1) * i / totalSteps;
    int a2 = from2 + (long)(to2 - from2) * i / totalSteps;
    mg1.write(a1); ang1 = a1;
    mg2.write(a2); ang2 = a2;
    delay(SWEEP_DELAY_MS);
  }
}

void printHelp() {
  Serial.println();
  Serial.println("--- lid servo test ---");
  Serial.println(" Direction sweeps (visual check):");
  Serial.println("   1 cw   1 ccw     | D6 sweep increasing / decreasing angle");
  Serial.println("   2 cw   2 ccw     | D2 sweep increasing / decreasing angle");
  Serial.println("   b cw   b ccw     | both same direction");
  Serial.println("   b mix             | D6 cw + D2 ccw simultaneously");
  Serial.println(" Absolute angles:");
  Serial.println("   <num>             | both -> num    (0..180)");
  Serial.println("   1 <num>           | only MG90S #1 (D6)");
  Serial.println("   2 <num>           | only MG90S #2 (D2)");
  Serial.println(" Misc:");
  Serial.println("   ?                 | print current angles");
  Serial.println("   h                 | show this help");
  Serial.println();
}

void handleLine(String line) {
  line.trim();
  if (line.length() == 0) return;

  String low = line; low.toLowerCase();

  if (low == "h")  { printHelp(); return; }
  if (low == "?")  {
    Serial.print("MG1 (D6) = "); Serial.print(ang1);
    Serial.print("   MG2 (D2) = "); Serial.println(ang2);
    return;
  }

  // Direction commands
  if (low == "1 cw")  { sweepOne(1, 0, 180); return; }
  if (low == "1 ccw") { sweepOne(1, 180, 0); return; }
  if (low == "2 cw")  { sweepOne(2, 0, 180); return; }
  if (low == "2 ccw") { sweepOne(2, 180, 0); return; }
  if (low == "b cw")  { sweepBoth(0, 180, 0, 180); return; }
  if (low == "b ccw") { sweepBoth(180, 0, 180, 0); return; }
  if (low == "b mix") { sweepBoth(0, 180, 180, 0); return; }

  // Absolute angles per servo
  if (low.startsWith("1 ")) { writeOne(1, line.substring(2).toInt()); return; }
  if (low.startsWith("2 ")) { writeOne(2, line.substring(2).toInt()); return; }

  // Plain number: both servos
  bool isNum = true;
  for (unsigned int i = 0; i < line.length(); i++) {
    if (!isDigit(line.charAt(i))) { isNum = false; break; }
  }
  if (isNum) { writeBoth(line.toInt()); return; }

  Serial.print("Unknown command: '"); Serial.print(line);
  Serial.println("' - type 'h' for help");
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  mg1.attach(MG1_PIN);
  mg2.attach(MG2_PIN);

  // Park both at 0 deg.
  writeBoth(0);

  Serial.println();
  Serial.println("=== Lid servo direction + angle tuning ===");
  Serial.println("MG1 on D6, MG2 on D2, both parked at 0 deg.");
  printHelp();
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (buf.length() > 0) {
        handleLine(buf);
        buf = "";
      }
    } else {
      buf += c;
      if (buf.length() > 32) buf = ""; // overflow guard
    }
  }
}
