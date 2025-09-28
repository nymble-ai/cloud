#ifndef LED_DISPLAY_HARDWARE_H
#define LED_DISPLAY_HARDWARE_H

#include "LEDDisplay.h"
#include <FastLED.h>

template <uint16_t NUM_LEDS, uint8_t DATA_PIN = 5>
class LEDDisplayHardware : public LEDDisplay {
private:
    CRGB leds[NUM_LEDS];
    uint8_t brightness;

public:
    LEDDisplayHardware() : brightness(255) {}

    void begin() override {
        FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
        FastLED.setBrightness(brightness);
        clear();
        show();
    }

    void setPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) override {
        if (index < NUM_LEDS) {
            leds[index] = CRGB(r, g, b);
        }
    }

    void setPixelColor(uint16_t index, uint32_t color) override {
        if (index < NUM_LEDS) {
            leds[index] = CRGB(Red(color), Green(color), Blue(color));
        }
    }

    void show() override {
        FastLED.show();
    }

    void clear() override {
        FastLED.clear();
    }

    void setBrightness(uint8_t b) override {
        brightness = b;
        FastLED.setBrightness(b);
    }

    uint8_t getBrightness() override {
        return brightness;
    }

    uint16_t getPixelCount() override {
        return NUM_LEDS;
    }

    uint32_t getPixel(uint16_t index) override {
        if (index < NUM_LEDS) {
            return Color(leds[index].r, leds[index].g, leds[index].b);
        }
        return 0;
    }

    void fill(uint8_t r, uint8_t g, uint8_t b) override {
        fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
    }

    void fillColor(uint32_t color) override {
        fill_solid(leds, NUM_LEDS, CRGB(Red(color), Green(color), Blue(color)));
    }

    CRGB* getRawLEDs() {
        return leds;
    }
};

#endif