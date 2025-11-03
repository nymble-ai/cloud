/*
 * Parallel LED Strips Example
 *
 * This example demonstrates how to control multiple LED strips in parallel.
 * The animation creates synchronized wave patterns across all strips while
 * also showing individual strip control.
 *
 * Hardware Setup:
 * - Strip 1: GPIO 5
 * - Strip 2: GPIO 18
 * - Strip 3: GPIO 19
 *
 * Simulator Setup:
 * - Set Layout Type to "Parallel Strips"
 * - Set Number of Strips to 3
 * - Adjust strip spacing as desired
 */

#include <LEDDisplay.h>

// Configuration
#define LEDS_PER_STRIP 90
#define NUM_STRIPS 3

#ifdef HARDWARE_MODE
  #include <LEDDisplayHardwareMulti.h>
  // Define the GPIO pins for each strip
  PinArray<NUM_STRIPS> pins = {5, 18, 19};
  LEDDisplayHardwareMulti<LEDS_PER_STRIP, NUM_STRIPS, PinArray<NUM_STRIPS>> display(pins);
#else
  #include <LEDDisplaySimulatorMulti.h>
  LEDDisplaySimulatorMulti<LEDS_PER_STRIP, NUM_STRIPS> display;
#endif

// Animation variables
uint16_t waveOffset = 0;
uint8_t animationMode = 0;
unsigned long lastModeChange = 0;
const unsigned long MODE_DURATION = 5000; // Switch animation every 5 seconds

void setup() {
    Serial.begin(115200);
    Serial.println("Parallel Strips Example Starting...");
    Serial.print("Number of strips: ");
    Serial.println(display.getStripCount());
    Serial.print("LEDs per strip: ");
    Serial.println(display.getPixelsPerStrip());
    Serial.print("Total LED count: ");
    Serial.println(display.getPixelCount());

    display.begin();
    display.setBrightness(100);
}

void loop() {
    // Switch animation mode periodically
    if (millis() - lastModeChange > MODE_DURATION) {
        animationMode = (animationMode + 1) % 4;
        lastModeChange = millis();
        display.clear();
    }

    switch (animationMode) {
        case 0:
            synchronizedWave();
            break;
        case 1:
            stripChase();
            break;
        case 2:
            rainbowStrips();
            break;
        case 3:
            crossFade();
            break;
    }

    display.show();
    delay(20);
    waveOffset += 2;
}

// Animation 1: Synchronized wave across all strips
void synchronizedWave() {
    for (uint16_t i = 0; i < display.getPixelsPerStrip(); i++) {
        uint8_t hue = (i * 256 / display.getPixelsPerStrip() + waveOffset) % 256;
        uint32_t color = colorWheel(hue);

        // Set the same pattern on all strips
        display.setAllStripsColor(i, color);
    }
}

// Animation 2: Chase pattern with different colors per strip
void stripChase() {
    display.clear();

    uint8_t chaseLength = 10;
    uint16_t position = (waveOffset / 2) % display.getPixelsPerStrip();

    for (uint8_t strip = 0; strip < display.getStripCount(); strip++) {
        // Each strip gets a different color
        uint8_t stripHue = (strip * 256 / display.getStripCount()) % 256;
        uint32_t color = colorWheel(stripHue);

        // Draw chase pattern
        for (uint8_t i = 0; i < chaseLength; i++) {
            uint16_t pixel = (position + i) % display.getPixelsPerStrip();
            uint8_t brightness = 255 - (i * 255 / chaseLength);
            uint8_t r = ((color >> 16) & 0xFF) * brightness / 255;
            uint8_t g = ((color >> 8) & 0xFF) * brightness / 255;
            uint8_t b = (color & 0xFF) * brightness / 255;
            display.setStripPixel(strip, pixel, r, g, b);
        }
    }
}

// Animation 3: Different rainbow offset per strip
void rainbowStrips() {
    for (uint8_t strip = 0; strip < display.getStripCount(); strip++) {
        uint16_t stripOffset = strip * 256 / display.getStripCount();

        for (uint16_t i = 0; i < display.getPixelsPerStrip(); i++) {
            uint8_t hue = (i * 256 / display.getPixelsPerStrip() + waveOffset + stripOffset) % 256;
            uint32_t color = colorWheel(hue);
            display.setStripPixelColor(strip, i, color);
        }
    }
}

// Animation 4: Cross-fade between strips
void crossFade() {
    uint8_t fadePosition = (waveOffset * 2) % 256;

    for (uint16_t i = 0; i < display.getPixelsPerStrip(); i++) {
        for (uint8_t strip = 0; strip < display.getStripCount(); strip++) {
            // Calculate brightness based on strip position and fade
            uint8_t stripPhase = (strip * 256 / display.getStripCount() + fadePosition) % 256;
            uint8_t brightness = (sin8(stripPhase) + 1) / 2; // sine wave 0-255

            // Color varies along the strip
            uint8_t hue = (i * 256 / display.getPixelsPerStrip()) % 256;
            uint32_t color = colorWheel(hue);

            uint8_t r = ((color >> 16) & 0xFF) * brightness / 255;
            uint8_t g = ((color >> 8) & 0xFF) * brightness / 255;
            uint8_t b = (color & 0xFF) * brightness / 255;

            display.setStripPixel(strip, i, r, g, b);
        }
    }
}

// Helper function: Convert hue (0-255) to RGB color
uint32_t colorWheel(uint8_t pos) {
    pos = 255 - pos;
    if (pos < 85) {
        return LEDDisplay::Color(255 - pos * 3, 0, pos * 3);
    }
    if (pos < 170) {
        pos -= 85;
        return LEDDisplay::Color(0, pos * 3, 255 - pos * 3);
    }
    pos -= 170;
    return LEDDisplay::Color(pos * 3, 255 - pos * 3, 0);
}

// Fast 8-bit sine approximation
uint8_t sin8(uint8_t theta) {
    static const uint8_t sine_table[256] = {
        128,131,134,137,140,143,146,149,152,155,158,161,164,167,170,173,
        176,179,182,184,187,190,193,195,198,201,203,206,208,211,213,216,
        218,220,223,225,227,229,231,233,235,237,239,241,243,244,246,248,
        249,251,252,253,255,256,257,258,259,260,261,262,263,263,264,265,
        265,266,266,266,267,267,267,267,267,267,267,267,266,266,266,265,
        265,264,263,263,262,261,260,259,258,257,256,255,253,252,251,249,
        248,246,244,243,241,239,237,235,233,231,229,227,225,223,220,218,
        216,213,211,208,206,203,201,198,195,193,190,187,184,182,179,176,
        173,170,167,164,161,158,155,152,149,146,143,140,137,134,131,128,
        125,122,119,116,113,110,107,104,101,98,95,92,89,86,83,80,
        77,74,72,69,66,63,61,58,55,53,50,48,45,43,40,38,
        36,33,31,29,27,25,23,21,19,17,15,13,12,10,8,7,
        5,4,3,2,1,0,0,0,0,0,0,0,0,1,1,1,
        2,2,3,4,4,5,6,7,8,9,10,11,13,14,15,17,
        18,20,22,23,25,27,29,31,33,35,37,39,42,44,46,49,
        51,54,56,59,62,64,67,70,73,76,79,82,85,88,91,94,
        97,100,103,106,109,112,115,118,121,124,127
    };
    return sine_table[theta];
}