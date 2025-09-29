/*
 * Dynamic Strip Configuration Example
 *
 * This example shows how to easily add, remove, and modify LED strips
 * with different effects and colors. Works with both hardware and simulator.
 *
 * INSTRUCTIONS:
 * 1. Uncomment the configuration you want to use
 * 2. Upload to your ESP32
 * 3. Connect to visualizer at http://localhost:8080
 */

#include <LEDDisplay.h>
#include <StripManager.h>
#include <Effects.h>
#include <StripConfig.h>

// Choose your display type
#ifdef HARDWARE_MODE
  #include <MultiStripHardware.h>
  MultiStripHardware display;
#else
  #include <MultiStripSimulator.h>
  MultiStripSimulator display;
#endif

// Create managers
StripManager stripManager(&display);
Effects effects(&stripManager);
StripConfigBuilder config(&stripManager, &effects);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Dynamic Strip Configuration Starting...");

  // ============================================================
  // CHOOSE YOUR CONFIGURATION (uncomment one)
  // ============================================================

  // ---------- OPTION 1: Simple 3-Strip Setup ----------
  setupSimpleThreeStrips();

  // ---------- OPTION 2: Party Mode ----------
  // setupPartyMode();

  // ---------- OPTION 3: Ambient Mode ----------
  // setupAmbientMode();

  // ---------- OPTION 4: Custom Configuration ----------
  // setupCustomConfiguration();

  // ---------- OPTION 5: Single Strip Test ----------
  // setupSingleStrip();

  // ---------- OPTION 6: Maximum Strips Demo ----------
  // setupMaxStrips();

  // ============================================================

  // Initialize the display with configured strips
  display.begin();

  // Print configuration
  Serial.print("Configured ");
  Serial.print(stripManager.getStripCount());
  Serial.println(" strips:");

  for (uint8_t i = 0; i < stripManager.getStripCount(); i++) {
    ManagedStrip& strip = stripManager.getStrip(i);
    Serial.print("  Strip ");
    Serial.print(i);
    Serial.print(": Pin ");
    Serial.print(strip.pin);
    Serial.print(", ");
    Serial.print(strip.numLEDs);
    Serial.print(" LEDs - ");
    Serial.println(strip.name);
  }
}

void loop() {
  // Update all strips with their assigned effects
  stripManager.update();

  // Optional: Change effects dynamically
  // checkForUserInput();

  delay(10); // Small delay for smooth animation
}

// ============================================================
// CONFIGURATION FUNCTIONS
// ============================================================

void setupSimpleThreeStrips() {
  // Three strips with different effects
  config.addRainbowStrip(5, 30, "Front Rainbow");
  config.addChaseStrip(18, 60, Colors::BLUE, "Main Chase");
  config.addBreathingStrip(19, 45, Colors::ORANGE, "Back Breathing");
}

void setupPartyMode() {
  config.setupPartyMode();
}

void setupAmbientMode() {
  config.setupAmbientMode();
}

void setupCustomConfiguration() {
  // Create your own custom configuration

  // Strip 1: Fire effect on pin 5
  config.addFireStrip(5, 30, "Fire Strip");

  // Strip 2: Wave effect in purple on pin 18
  config.addWaveStrip(18, 60, Colors::PURPLE, "Purple Wave");

  // Strip 3: Gradient from blue to green on pin 19
  config.addGradientStrip(19, 45, Colors::BLUE, Colors::GREEN, "Ocean Gradient");

  // Strip 4: Sparkle effect in white on pin 21
  config.addSparkleStrip(21, 30, Colors::WHITE, "Stars");

  // You can also manually configure strips
  uint8_t stripIndex = stripManager.addStrip(22, 20, "Manual Strip");
  stripManager.setEffect(stripIndex, Effects::theaterChase(255, 0, 255, 100));
}

void setupSingleStrip() {
  // Test with just one strip
  config.addRainbowStrip(5, 90, "Single Rainbow Strip");
}

void setupMaxStrips() {
  // Demo with many strips (adjust pins as needed)
  config.addRainbowStrip(5, 30, "Strip 1");
  config.addChaseStrip(18, 30, Colors::RED, "Strip 2");
  config.addBreathingStrip(19, 30, Colors::GREEN, "Strip 3");
  config.addSparkleStrip(21, 30, Colors::BLUE, "Strip 4");
  config.addFireStrip(22, 30, "Strip 5");
  config.addWaveStrip(23, 30, Colors::YELLOW, "Strip 6");
  config.addGradientStrip(25, 30, Colors::PURPLE, Colors::ORANGE, "Strip 7");
  config.addStrobeStrip(26, 30, Colors::WHITE, "Strip 8");
}

// ============================================================
// DYNAMIC CONTROL (Optional)
// ============================================================

void checkForUserInput() {
  if (Serial.available()) {
    char cmd = Serial.read();

    switch(cmd) {
      case '1': // Switch to party mode
        clearAllStrips();
        config.setupPartyMode();
        Serial.println("Switched to Party Mode");
        break;

      case '2': // Switch to ambient mode
        clearAllStrips();
        config.setupAmbientMode();
        Serial.println("Switched to Ambient Mode");
        break;

      case '3': // Switch to Christmas mode
        clearAllStrips();
        config.setupChristmasMode();
        Serial.println("Switched to Christmas Mode");
        break;

      case '4': // Switch to fire mode
        clearAllStrips();
        config.setupFireMode();
        Serial.println("Switched to Fire Mode");
        break;

      case 'a': // Add a random strip
        addRandomStrip();
        break;

      case 'r': // Remove last strip
        if (stripManager.getStripCount() > 0) {
          stripManager.removeStrip(stripManager.getStripCount() - 1);
          Serial.println("Removed last strip");
        }
        break;

      case 'c': // Clear all
        clearAllStrips();
        Serial.println("Cleared all strips");
        break;

      case 'b': // Adjust brightness
        adjustBrightness();
        break;
    }
  }
}

void clearAllStrips() {
  // Remove all strips
  while (stripManager.getStripCount() > 0) {
    stripManager.removeStrip(0);
  }
}

void addRandomStrip() {
  static uint8_t nextPin = 27;
  uint8_t effectType = random(8);

  switch(effectType) {
    case 0:
      config.addRainbowStrip(nextPin, 30, "New Rainbow");
      break;
    case 1:
      config.addChaseStrip(nextPin, 30, random(0xFFFFFF), "New Chase");
      break;
    case 2:
      config.addBreathingStrip(nextPin, 30, random(0xFFFFFF), "New Breathing");
      break;
    case 3:
      config.addSparkleStrip(nextPin, 30, random(0xFFFFFF), "New Sparkle");
      break;
    case 4:
      config.addFireStrip(nextPin, 30, "New Fire");
      break;
    case 5:
      config.addWaveStrip(nextPin, 30, random(0xFFFFFF), "New Wave");
      break;
    case 6:
      config.addGradientStrip(nextPin, 30, random(0xFFFFFF), random(0xFFFFFF), "New Gradient");
      break;
    case 7:
      config.addStrobeStrip(nextPin, 30, random(0xFFFFFF), "New Strobe");
      break;
  }

  Serial.print("Added new strip on pin ");
  Serial.println(nextPin);
  nextPin++;
}

void adjustBrightness() {
  Serial.println("Enter strip index and brightness (0-255):");
  // Wait for input format: "0 128" (strip 0, brightness 128)
  while (!Serial.available());

  uint8_t stripIdx = Serial.parseInt();
  uint8_t brightness = Serial.parseInt();

  if (stripIdx < stripManager.getStripCount()) {
    stripManager.setStripBrightness(stripIdx, brightness);
    Serial.print("Set strip ");
    Serial.print(stripIdx);
    Serial.print(" brightness to ");
    Serial.println(brightness);
  }
}