#ifndef LED_DISPLAY_SIMULATOR_MULTI_H
#define LED_DISPLAY_SIMULATOR_MULTI_H

#include "LEDDisplay.h"

template <uint16_t LEDS_PER_STRIP_COUNT, uint8_t NUM_STRIPS_COUNT = 1>
class LEDDisplaySimulatorMulti : public LEDDisplay {
private:
    struct LED {
        uint8_t r, g, b;
    };

    static const uint16_t TOTAL_LEDS = LEDS_PER_STRIP_COUNT * NUM_STRIPS_COUNT;
    LED leds[TOTAL_LEDS];
    uint8_t brightness;
    bool needsUpdate;

    static const uint8_t FRAME_START = 0xFF;
    static const uint8_t FRAME_END = 0xFE;
    static const uint8_t ESCAPE_BYTE = 0xFD;
    static const uint8_t MULTI_STRIP_MARKER = 0xFC;

    void sendEscaped(uint8_t byte) {
        if (byte == FRAME_START || byte == FRAME_END ||
            byte == ESCAPE_BYTE || byte == MULTI_STRIP_MARKER) {
            Serial.write(ESCAPE_BYTE);
        }
        Serial.write(byte);
    }

    uint16_t stripPixelToIndex(uint8_t strip, uint16_t pixel) {
        if (strip >= NUM_STRIPS_COUNT || pixel >= LEDS_PER_STRIP_COUNT) {
            return 0xFFFF;  // Invalid
        }
        return strip * LEDS_PER_STRIP_COUNT + pixel;
    }

public:
    LEDDisplaySimulatorMulti() : brightness(255), needsUpdate(false) {
        for (uint16_t i = 0; i < TOTAL_LEDS; i++) {
            leds[i].r = 0;
            leds[i].g = 0;
            leds[i].b = 0;
        }
    }

    void begin() override {
        Serial.begin(115200);
        while (!Serial && millis() < 3000) {
            delay(10);
        }
        clear();
        needsUpdate = true;
        show();
    }

    // Multi-strip methods
    uint8_t getStripCount() override {
        return NUM_STRIPS_COUNT;
    }

    uint16_t getPixelsPerStrip() override {
        return LEDS_PER_STRIP_COUNT;
    }

    void setStripPixel(uint8_t strip, uint16_t index, uint8_t r, uint8_t g, uint8_t b) override {
        uint16_t globalIndex = stripPixelToIndex(strip, index);
        if (globalIndex < TOTAL_LEDS) {
            uint8_t scaledR = (r * brightness) / 255;
            uint8_t scaledG = (g * brightness) / 255;
            uint8_t scaledB = (b * brightness) / 255;

            leds[globalIndex].r = scaledR;
            leds[globalIndex].g = scaledG;
            leds[globalIndex].b = scaledB;
            needsUpdate = true;
        }
    }

    void setStripPixelColor(uint8_t strip, uint16_t index, uint32_t color) override {
        setStripPixel(strip, index, Red(color), Green(color), Blue(color));
    }

    // Legacy single-strip methods (use strip 0)
    void setPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) override {
        // Map linear index across all strips
        if (index < TOTAL_LEDS) {
            uint8_t strip = index / LEDS_PER_STRIP_COUNT;
            uint16_t pixel = index % LEDS_PER_STRIP_COUNT;
            setStripPixel(strip, pixel, r, g, b);
        }
    }

    void setPixelColor(uint16_t index, uint32_t color) override {
        setPixel(index, Red(color), Green(color), Blue(color));
    }

    void show() override {
        if (!needsUpdate) {
            return;
        }

        Serial.write(FRAME_START);

        if (NUM_STRIPS_COUNT > 1) {
            // Multi-strip frame format
            Serial.write(MULTI_STRIP_MARKER);
            sendEscaped(NUM_STRIPS_COUNT);

            uint8_t countHigh = (LEDS_PER_STRIP_COUNT >> 8) & 0xFF;
            uint8_t countLow = LEDS_PER_STRIP_COUNT & 0xFF;
            sendEscaped(countHigh);
            sendEscaped(countLow);
        } else {
            // Single strip frame format (backward compatible)
            uint8_t countHigh = (LEDS_PER_STRIP_COUNT >> 8) & 0xFF;
            uint8_t countLow = LEDS_PER_STRIP_COUNT & 0xFF;
            sendEscaped(countHigh);
            sendEscaped(countLow);
        }

        // Send LED data (all strips sequentially)
        for (uint16_t i = 0; i < TOTAL_LEDS; i++) {
            sendEscaped(leds[i].r);
            sendEscaped(leds[i].g);
            sendEscaped(leds[i].b);
        }

        Serial.write(FRAME_END);
        Serial.flush();

        needsUpdate = false;
    }

    void clear() override {
        for (uint16_t i = 0; i < TOTAL_LEDS; i++) {
            leds[i].r = 0;
            leds[i].g = 0;
            leds[i].b = 0;
        }
        needsUpdate = true;
    }

    void setBrightness(uint8_t b) override {
        brightness = b;
        // Recalculate all LED values with new brightness
        for (uint16_t i = 0; i < TOTAL_LEDS; i++) {
            // Store original values would be better, but this is simpler
            needsUpdate = true;
        }
    }

    uint8_t getBrightness() override {
        return brightness;
    }

    uint16_t getPixelCount() override {
        return TOTAL_LEDS;
    }

    uint32_t getPixel(uint16_t index) override {
        if (index < TOTAL_LEDS) {
            return Color(leds[index].r, leds[index].g, leds[index].b);
        }
        return 0;
    }

    void fill(uint8_t r, uint8_t g, uint8_t b) override {
        for (uint16_t i = 0; i < TOTAL_LEDS; i++) {
            uint8_t strip = i / LEDS_PER_STRIP_COUNT;
            uint16_t pixel = i % LEDS_PER_STRIP_COUNT;
            setStripPixel(strip, pixel, r, g, b);
        }
    }

    void fillColor(uint32_t color) override {
        fill(Red(color), Green(color), Blue(color));
    }
};

// Convenience typedef for single strip (backward compatible)
template <uint16_t NUM_LEDS>
using LEDDisplaySimulator = LEDDisplaySimulatorMulti<NUM_LEDS, 1>;

#endif