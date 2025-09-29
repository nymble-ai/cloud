/*
 * Quick Start Template for LED Strips
 *
 * This is the simplest way to get started with multiple LED strips.
 * Just modify the setup() function to add your strips!
 */

#include <LEDDisplay.h>
#include <StripManager.h>
#include <Effects.h>
#include <StripConfig.h>

// Display setup (automatically works with hardware or simulator)
#ifdef HARDWARE_MODE
  #include <MultiStripHardware.h>
  MultiStripHardware display;
#else
  #include <MultiStripSimulator.h>
  MultiStripSimulator display;
#endif

StripManager stripManager(&display);
Effects effects(&stripManager);
StripConfigBuilder config(&stripManager, &effects);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // ============================================================
  // ADD YOUR STRIPS HERE
  // ============================================================
  // Format: config.add[Effect]Strip(pin, numLEDs, color, "name");
  //
  // Available effects:
  //   - addRainbowStrip(pin, leds, "name")
  //   - addSolidStrip(pin, leds, color, "name")
  //   - addChaseStrip(pin, leds, color, "name")
  //   - addBreathingStrip(pin, leds, color, "name")
  //   - addSparkleStrip(pin, leds, color, "name")
  //   - addFireStrip(pin, leds, "name")
  //   - addWaveStrip(pin, leds, color, "name")
  //   - addGradientStrip(pin, leds, color1, color2, "name")
  //   - addStrobeStrip(pin, leds, color, "name")
  //
  // Available colors: RED, GREEN, BLUE, WHITE, YELLOW, CYAN,
  //                  MAGENTA, ORANGE, PURPLE, PINK
  // ============================================================

  // Example 1: Three different effects
  config.addRainbowStrip(5, 30, "Strip 1");
  config.addChaseStrip(18, 60, Colors::BLUE, "Strip 2");
  config.addFireStrip(19, 45, "Strip 3");

  // Example 2: All the same effect, different colors
  // config.addBreathingStrip(5, 30, Colors::RED, "Red");
  // config.addBreathingStrip(18, 30, Colors::GREEN, "Green");
  // config.addBreathingStrip(19, 30, Colors::BLUE, "Blue");

  // Example 3: Single strip
  // config.addRainbowStrip(5, 90, "Main");

  // ============================================================

  display.begin();
  printConfiguration();
}

void loop() {
  stripManager.update();
  delay(10);
}

void printConfiguration() {
  Serial.println("\n=== LED Strip Configuration ===");
  Serial.print("Total strips: ");
  Serial.println(stripManager.getStripCount());

  for (uint8_t i = 0; i < stripManager.getStripCount(); i++) {
    ManagedStrip& strip = stripManager.getStrip(i);
    Serial.print("  [");
    Serial.print(i);
    Serial.print("] Pin ");
    Serial.print(strip.pin);
    Serial.print(": ");
    Serial.print(strip.numLEDs);
    Serial.print(" LEDs - \"");
    Serial.print(strip.name);
    Serial.println("\"");
  }
  Serial.println("==============================\n");
}