// =========================================================================
// MG90S standalone test sketch
// -------------------------------------------------------------------------
// Sweeps the MG90S back and forth so you can confirm the wiring + power
// before trying to integrate it with the rest of the project.
//
// Wiring (matches the main project):
//   MG90S signal (orange/yellow) -> Arduino A3
//   MG90S VCC    (red)           -> 5V        *** see POWER NOTE below ***
//   MG90S GND    (brown/black)   -> Arduino GND
//
// >>> POWER NOTE <<<
// MG90S is a metal-gear servo and can draw up to ~600 mA stall.
// Arduino's on-board 5 V regulator can only supply ~500 mA total, and
// powering the servo from it is the most common reason MG90S buzzes,
// jitters, or refuses to move. For a real test:
//   1. Use an EXTERNAL 5 V supply (4xAA pack, USB 5V/2A wall adapter,
//      or a bench supply) for the servo's red wire.
//   2. Tie the external supply's GND to the Arduino's GND. (Common ground
//      is mandatory - the signal won't be referenced correctly otherwise.)
//   3. Keep the signal wire on Arduino A3.
//
// How to use:
//   1. Disconnect the rest of the project so the MG90S has the cleanest
//      possible setup. Connect external 5 V as described above.
//   2. Upload this sketch.
//   3. Open Serial Monitor at 9600 baud.
//   4. The horn should sweep from 30 -> 150 deg, pause, then back, on
//      a 4-second loop. The serial log mirrors each position.
//
// Diagnosing common failures:
//   - Buzzes, no movement ........ underpowered. Use external 5V.
//   - Twitches ~10 deg only ...... still underpowered, or VCC=3.3V.
//   - Sweeps once then stops ..... external supply tripped its overload
//                                   protection (try a stronger one).
//   - Nothing at all ............. signal wire on wrong pin, or GND not
//                                   tied between supply and Arduino.
// =========================================================================

#include <Servo.h>

const int servoPin = A3;

// Conservative end-stops. Some MG90S horns hit a mechanical wall outside
// of 10..170 deg; staying inside 30..150 keeps things safe for testing.
const int ANG_MIN = 30;
const int ANG_MAX = 150;

Servo mg;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  Serial.println();
  Serial.println("MG90S standalone test - 9600 baud");
  Serial.println("Signal=A3, VCC=external 5V, GND=common");

  mg.attach(servoPin);
  mg.write(ANG_MIN);
  delay(800); // give the horn time to reach the start position
  Serial.print("START at "); Serial.println(ANG_MIN);
}

void loop() {
  // Sweep slowly so you can see whether it moves smoothly or jitters.
  for (int a = ANG_MIN; a <= ANG_MAX; a += 5) {
    mg.write(a);
    Serial.print("ANGLE:"); Serial.println(a);
    delay(50);
  }
  delay(800);
  for (int a = ANG_MAX; a >= ANG_MIN; a -= 5) {
    mg.write(a);
    Serial.print("ANGLE:"); Serial.println(a);
    delay(50);
  }
  delay(800);
}
