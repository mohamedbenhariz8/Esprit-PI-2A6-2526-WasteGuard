// =========================================================================
// HC-SR04 standalone test sketch
// -------------------------------------------------------------------------
// Use this to verify the ultrasonic sensor in isolation, with NOTHING else
// attached. If this sketch shows valid distances, the sensor + wiring are
// fine and the integration with the main sketch is what's broken.
//
// Wiring (matches the main project):
//   HC-SR04 TRIG -> Arduino D2
//   HC-SR04 ECHO -> Arduino D4
//   HC-SR04 VCC  -> Arduino 5V        (NOT 3.3V - HC-SR04 needs 5V)
//   HC-SR04 GND  -> Arduino GND
//
// How to use:
//   1. Disconnect everything else from the Arduino so power is clean.
//   2. Upload this sketch.
//   3. Open Serial Monitor at 9600 baud.
//   4. Put a flat object 10-30 cm in front of the sensor and watch.
//
// Expected output every 500 ms:
//   US_RAW:1740  cm:30
//   US_RAW:1745  cm:30
//   US_RAW:0     cm:-1   <-- a "0" raw means the echo timed out
//
// Diagnosing common failures:
//   - All zeros forever ........ wiring issue (TRIG/ECHO swapped, no 5V,
//                                no GND, or sensor broken)
//   - Random huge numbers ...... echo wire picking up noise or running
//                                next to the trigger / a long jumper
//   - Always ~0 cm ............. object too close (<2 cm minimum range)
//   - Always >400 cm ........... object too far (>4 m max range)
// =========================================================================

const int trigPin = 2;
const int echoPin = 4;

void setup() {
  Serial.begin(9600);
  while (!Serial) { /* wait for USB CDC on boards that need it */ }
  Serial.println();
  Serial.println("HC-SR04 standalone test - 9600 baud");
  Serial.println("TRIG=D2, ECHO=D4, VCC=5V, GND=GND");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  delay(100); // sensor power-up settle time
}

void loop() {
  // 10 us trigger pulse, preceded by a clean LOW.
  digitalWrite(trigPin, LOW);
  delayMicroseconds(4);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // 40 ms timeout = ~6.8 m max range. 0 means the echo never came back.
  unsigned long us = pulseIn(echoPin, HIGH, 40000UL);

  Serial.print("US_RAW:");
  Serial.print(us);
  Serial.print("  cm:");
  if (us == 0) {
    Serial.println(-1);
  } else {
    Serial.println(us / 58); // standard speed-of-sound conversion
  }

  delay(500);
}
