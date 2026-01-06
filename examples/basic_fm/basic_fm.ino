/*
 * ESP32 SI4703 – Basic FM Test
 * Phase-1 validation sketch
 *
 * Purpose:
 *  - Verify I2C communication
 *  - Power up SI4703
 *  - Tune to a fixed frequency
 *  - Set volume
 *  - Read RSSI periodically
 */

#include <Arduino.h>
#include <SI4703.h>

// -------------------------------
// CONFIG (adjust if needed)
// -------------------------------
#define SI4703_SDA 21
#define SI4703_SCL 22

#define TEST_FREQUENCY_MHZ  98.7f   // change to a strong local station
#define TEST_VOLUME         8       // 0–15

SI4703 radio;

// -------------------------------
// SETUP
// -------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== ESP32 SI4703 BASIC FM TEST ===");

  // Init radio
  Serial.println("Initializing SI4703...");
  if (!radio.begin(SI4703_SDA, SI4703_SCL)) {
    Serial.println("ERROR: SI4703 not detected!");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("SI4703 detected");

  // Set volume
  radio.setVolume(TEST_VOLUME);
  Serial.print("Volume set to ");
  Serial.println(TEST_VOLUME);

  // Tune
  Serial.print("Tuning to ");
  Serial.print(TEST_FREQUENCY_MHZ);
  Serial.println(" MHz");

  radio.setFrequency(TEST_FREQUENCY_MHZ);

  Serial.println("Radio ready");
}

// -------------------------------
// LOOP
// -------------------------------
void loop() {
  // Required for future non-blocking features
  radio.process();

  // Read signal strength
  uint8_t rssi = radio.getRSSI();

  Serial.print("RSSI: ");
  Serial.println(rssi);

  delay(1000);
}
