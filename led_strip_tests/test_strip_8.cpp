#include <LEDDisplay.h>

#ifdef HARDWARE_MODE
  #include <MultiStripHardware.h>
  MultiStripHardware display;
  #define IS_SIMULATOR false
#else
  #include <MultiStripSimulator.h>
  MultiStripSimulator display;
  #define IS_SIMULATOR true
#endif

// 11 STRIPS CONFIGURATION
const uint8_t LED_PINS[] = {0, 2, 4, 16, 17, 18, 19, 23, 14, 25, 26};
const uint16_t LEDS_PER_STRIP = 90;
const uint8_t NUM_STRIPS = 11;

// TESTING: Strip 8 (Pin 14)
const uint8_t TEST_STRIP = 8;

void setup() {
  // Configure all 11 LED strips
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    String stripName = "Strip " + String(i);
    display.addStrip(LED_PINS[i], LEDS_PER_STRIP, stripName);
  }

  display.begin();
  display.setBrightness(50);  // Medium brightness for visibility

  #ifndef HARDWARE_MODE
    Serial.println("=== LED STRIP TEST ===");
    Serial.print("Testing Strip ");
    Serial.print(TEST_STRIP);
    Serial.print(" (Pin ");
    Serial.print(LED_PINS[TEST_STRIP]);
    Serial.println(")");
    Serial.println("All 90 LEDs should be WHITE");
  #endif
}

void loop() {
  display.clear();

  // Light up all 90 LEDs on the test strip in white
  for (uint16_t i = 0; i < LEDS_PER_STRIP; i++) {
    display.setPixelOnStrip(TEST_STRIP, i, 255, 255, 255);
  }

  display.show();
  delay(100);
}
