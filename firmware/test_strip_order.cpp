#include <LEDDisplay.h>

#ifdef HARDWARE_MODE
  #include <MultiStripHardware.h>
  MultiStripHardware display;
#else
  #include <MultiStripSimulator.h>
  MultiStripSimulator display;
#endif

/*
 * LED STRIP ORDER TEST
 *
 * This test helps identify which physical strip corresponds to which pin/index
 *
 * SETUP:
 * - 11 LED strips on pins: 0, 2, 4, 16, 17, 18, 19, 23, 14, 25, 26
 * - Each strip shows a DIFFERENT COLOR
 * - Each strip shows a DIFFERENT NUMBER of LEDs lit
 *
 * PATTERN:
 * Strip Index 0:  5 LEDs lit  - RED
 * Strip Index 1:  10 LEDs lit - GREEN
 * Strip Index 2:  15 LEDs lit - BLUE
 * Strip Index 3:  20 LEDs lit - YELLOW
 * Strip Index 4:  25 LEDs lit - CYAN
 * Strip Index 5:  30 LEDs lit - MAGENTA
 * Strip Index 6:  35 LEDs lit - ORANGE
 * Strip Index 7:  40 LEDs lit - PURPLE
 * Strip Index 8:  45 LEDs lit - PINK
 * Strip Index 9:  50 LEDs lit - LIME
 * Strip Index 10: 55 LEDs lit - WHITE
 *
 * INSTRUCTIONS:
 * 1. Upload this test file
 * 2. Look at your physical LED cloud
 * 3. Count how many LEDs are lit on each colored strip
 * 4. Note which color has which count
 * 5. This tells you the strip index for each physical position
 */

const uint8_t LED_PINS[] = {0, 2, 4, 16, 17, 18, 19, 23, 14, 25, 26};
const uint16_t LEDS_PER_STRIP = 90;
const uint8_t NUM_STRIPS = 11;

// Distinct colors for each strip
uint8_t stripColors[11][3] = {
  {255, 0, 0},      // 0: RED
  {0, 255, 0},      // 1: GREEN
  {0, 0, 255},      // 2: BLUE
  {255, 255, 0},    // 3: YELLOW
  {0, 255, 255},    // 4: CYAN
  {255, 0, 255},    // 5: MAGENTA
  {255, 128, 0},    // 6: ORANGE
  {128, 0, 255},    // 7: PURPLE
  {255, 105, 180},  // 8: PINK
  {191, 255, 0},    // 9: LIME
  {255, 255, 255}   // 10: WHITE
};

// Number of LEDs to light per strip (increments of 5)
uint8_t ledsToLight[11] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55};

void setup() {
  #ifndef HARDWARE_MODE
    Serial.begin(115200);
    while (!Serial && millis() < 3000) {
      delay(10);
    }
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║    LED STRIP ORDER TEST MODE          ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println();
    Serial.println("Strip Index | LEDs Lit | Color");
    Serial.println("------------|----------|--------");
    for (uint8_t i = 0; i < NUM_STRIPS; i++) {
      Serial.print("   ");
      Serial.print(i);
      Serial.print("        |    ");
      Serial.print(ledsToLight[i]);
      Serial.print("    | ");

      // Print color name
      const char* colorNames[] = {"RED", "GREEN", "BLUE", "YELLOW", "CYAN",
                                  "MAGENTA", "ORANGE", "PURPLE", "PINK", "LIME", "WHITE"};
      Serial.println(colorNames[i]);
    }
    Serial.println();
    Serial.println("Instructions:");
    Serial.println("1. Look at each colored strip on your cloud");
    Serial.println("2. Count how many LEDs are lit");
    Serial.println("3. Match the count to the table above");
    Serial.println("4. This tells you which strip index it is!");
    Serial.println();
  #endif

  // Configure all 11 LED strips
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    String stripName = "Strip-" + String(i);
    display.addStrip(LED_PINS[i], LEDS_PER_STRIP, stripName);
  }

  display.begin();
  display.setBrightness(50); // 20% brightness for testing
  display.clear();

  // Light up each strip with its test pattern
  for (uint8_t stripIndex = 0; stripIndex < NUM_STRIPS; stripIndex++) {
    uint8_t numLEDs = ledsToLight[stripIndex];
    uint8_t r = stripColors[stripIndex][0];
    uint8_t g = stripColors[stripIndex][1];
    uint8_t b = stripColors[stripIndex][2];

    // Light up the specified number of LEDs on this strip
    for (uint8_t led = 0; led < numLEDs; led++) {
      display.setPixelOnStrip(stripIndex, led, r, g, b);
    }
  }

  display.show();

  #ifndef HARDWARE_MODE
    Serial.println("Test pattern displayed!");
    Serial.println("The LEDs are now showing the test pattern.");
  #endif
}

void loop() {
  // Static display - no updates needed
  delay(1000);
}
