/*
 * Multi-Strip LED Example
 *
 * This example demonstrates how to control multiple LED strips
 * on different GPIO pins using a single display object.
 *
 * Hardware Setup:
 * - Strip 1: 30 LEDs on GPIO 5
 * - Strip 2: 60 LEDs on GPIO 18
 * - Strip 3: 45 LEDs on GPIO 19
 *
 * Total: 135 LEDs across 3 strips
 */

#include <LEDDisplay.h>

#ifdef HARDWARE_MODE
  #include <MultiStripHardware.h>
  MultiStripHardware display;
#else
  #include <MultiStripSimulator.h>
  MultiStripSimulator display;
#endif

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Configure multiple LED strips on different pins
  display.addStrip(5, 30, "Front Strip");     // 30 LEDs on GPIO 5
  display.addStrip(18, 60, "Side Strip");     // 60 LEDs on GPIO 18
  display.addStrip(19, 45, "Back Strip");     // 45 LEDs on GPIO 19

  // Initialize all strips
  display.begin();
  display.setBrightness(64);  // Start with low brightness

  // Clear all strips
  display.clear();
  display.show();

  Serial.println("Multi-Strip LED Display Initialized");
  Serial.print("Total strips: ");
  Serial.println(display.getStripCount());
  Serial.print("Total LEDs: ");
  Serial.println(display.getPixelCount());
}

void loop() {
  // Demo different patterns on each strip
  staticStripColors();
  delay(2000);

  chasingPattern();
  delay(2000);

  rainbowWave();
  delay(2000);

  sparkleEffect();
  delay(2000);

  fadeInOut();
  delay(2000);
}

// Set each strip to a different solid color
void staticStripColors() {
  display.clear();

  // Strip 1 (Front) - Red
  for (uint16_t i = 0; i < 30; i++) {
    display.setPixelOnStrip(0, i, 255, 0, 0);
  }

  // Strip 2 (Side) - Green
  for (uint16_t i = 0; i < 60; i++) {
    display.setPixelOnStrip(1, i, 0, 255, 0);
  }

  // Strip 3 (Back) - Blue
  for (uint16_t i = 0; i < 45; i++) {
    display.setPixelOnStrip(2, i, 0, 0, 255);
  }

  display.show();
}

// Chasing pattern across all strips
void chasingPattern() {
  static uint16_t position = 0;
  const uint8_t tailLength = 10;

  for (uint8_t frame = 0; frame < 100; frame++) {
    display.clear();

    // Draw the chaser across all LEDs
    for (uint8_t i = 0; i < tailLength; i++) {
      uint16_t idx = (position + i) % display.getPixelCount();
      uint8_t brightness = 255 - (i * 25);  // Fade tail
      display.setPixel(idx, brightness, brightness, brightness);
    }

    display.show();
    delay(30);

    position = (position + 1) % display.getPixelCount();
  }
}

// Rainbow wave across all strips
void rainbowWave() {
  static uint16_t hue = 0;

  for (uint8_t frame = 0; frame < 100; frame++) {
    for (uint16_t i = 0; i < display.getPixelCount(); i++) {
      uint16_t pixelHue = hue + (i * 65536L / display.getPixelCount());
      uint32_t color = colorWheel((pixelHue >> 8) & 0xFF);
      display.setPixelColor(i, color);
    }

    display.show();
    hue += 512;
    delay(20);
  }
}

// Sparkle effect on random strips
void sparkleEffect() {
  for (uint8_t frame = 0; frame < 100; frame++) {
    // Fade all LEDs
    for (uint16_t i = 0; i < display.getPixelCount(); i++) {
      uint32_t c = display.getPixel(i);
      uint8_t r = LEDDisplay::Red(c);
      uint8_t g = LEDDisplay::Green(c);
      uint8_t b = LEDDisplay::Blue(c);

      r = r > 10 ? r - 10 : 0;
      g = g > 10 ? g - 10 : 0;
      b = b > 10 ? b - 10 : 0;

      display.setPixel(i, r, g, b);
    }

    // Add new sparkles on random strips
    for (uint8_t i = 0; i < 3; i++) {
      uint8_t stripIdx = random(display.getStripCount());
      const StripConfig& strip = display.getStrip(stripIdx);
      uint16_t pixelIdx = random(strip.numLEDs);

      // Different color per strip
      switch (stripIdx) {
        case 0:
          display.setPixelOnStrip(stripIdx, pixelIdx, 255, 100, 100);
          break;
        case 1:
          display.setPixelOnStrip(stripIdx, pixelIdx, 100, 255, 100);
          break;
        case 2:
          display.setPixelOnStrip(stripIdx, pixelIdx, 100, 100, 255);
          break;
      }
    }

    display.show();
    delay(50);
  }
}

// Fade in and out each strip sequentially
void fadeInOut() {
  for (uint8_t stripIdx = 0; stripIdx < display.getStripCount(); stripIdx++) {
    const StripConfig& strip = display.getStrip(stripIdx);

    // Fade in
    for (uint8_t brightness = 0; brightness < 255; brightness += 5) {
      for (uint16_t i = 0; i < strip.numLEDs; i++) {
        display.setPixelOnStrip(stripIdx, i, brightness, brightness, brightness);
      }
      display.show();
      delay(10);
    }

    // Fade out
    for (int brightness = 250; brightness >= 0; brightness -= 5) {
      for (uint16_t i = 0; i < strip.numLEDs; i++) {
        display.setPixelOnStrip(stripIdx, i, brightness, brightness, brightness);
      }
      display.show();
      delay(10);
    }
  }
}

// Color wheel helper function
uint32_t colorWheel(uint8_t wheelPos) {
  wheelPos = 255 - wheelPos;
  if (wheelPos < 85) {
    return LEDDisplay::Color(255 - wheelPos * 3, 0, wheelPos * 3);
  }
  if (wheelPos < 170) {
    wheelPos -= 85;
    return LEDDisplay::Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
  wheelPos -= 170;
  return LEDDisplay::Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}