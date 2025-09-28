#ifndef LED_DISPLAY_HARDWARE_H
#define LED_DISPLAY_HARDWARE_H

#include "LEDDisplay.h"
#include <Adafruit_NeoPixel.h>

template <uint16_t NUM_LEDS, uint8_t DATA_PIN = 5>
class LEDDisplayHardware : public LEDDisplay {
private:
    Adafruit_NeoPixel strip;
    uint8_t brightness;

public:
    LEDDisplayHardware() :
        strip(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800),
        brightness(50) {}  // Start with low brightness like your working code

    void begin() override {
        strip.begin();
        strip.setBrightness(brightness);
        clear();
        show();
    }

    void setPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) override {
        if (index < NUM_LEDS) {
            strip.setPixelColor(index, strip.Color(r, g, b));
        }
    }

    void setPixelColor(uint16_t index, uint32_t color) override {
        if (index < NUM_LEDS) {
            strip.setPixelColor(index, color);
        }
    }

    void show() override {
        strip.show();
    }

    void clear() override {
        strip.clear();
    }

    void setBrightness(uint8_t b) override {
        brightness = b;
        strip.setBrightness(b);
    }

    uint8_t getBrightness() override {
        return brightness;
    }

    uint16_t getPixelCount() override {
        return NUM_LEDS;
    }

    uint32_t getPixel(uint16_t index) override {
        if (index < NUM_LEDS) {
            return strip.getPixelColor(index);
        }
        return 0;
    }

    void fill(uint8_t r, uint8_t g, uint8_t b) override {
        strip.fill(strip.Color(r, g, b), 0, NUM_LEDS);
    }

    void fillColor(uint32_t color) override {
        strip.fill(color, 0, NUM_LEDS);
    }

    // For compatibility with existing code
    Adafruit_NeoPixel* getNeoPixel() {
        return &strip;
    }
};

#endif