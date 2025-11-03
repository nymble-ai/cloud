#ifndef LED_DISPLAY_HARDWARE_MULTI_H
#define LED_DISPLAY_HARDWARE_MULTI_H

#include "LEDDisplay.h"
#include <Adafruit_NeoPixel.h>

// Helper to store pin array
template<uint8_t N>
struct PinArray {
    uint8_t pins[N];
    PinArray(std::initializer_list<uint8_t> list) : pins{} {
        uint8_t i = 0;
        for (auto pin : list) {
            if (i < N) pins[i++] = pin;
        }
    }
};

template <uint16_t LEDS_PER_STRIP_COUNT, uint8_t NUM_STRIPS_COUNT, typename PinArrayType>
class LEDDisplayHardwareMulti : public LEDDisplay {
private:
    Adafruit_NeoPixel* strips[NUM_STRIPS_COUNT];
    PinArrayType dataPins;
    uint8_t brightness;
    static const uint16_t TOTAL_LEDS = LEDS_PER_STRIP_COUNT * NUM_STRIPS_COUNT;

    uint16_t stripPixelToIndex(uint8_t strip, uint16_t pixel) {
        if (strip >= NUM_STRIPS_COUNT || pixel >= LEDS_PER_STRIP_COUNT) {
            return 0xFFFF;
        }
        return strip * LEDS_PER_STRIP_COUNT + pixel;
    }

public:
    LEDDisplayHardwareMulti(PinArrayType pins) :
        dataPins(pins),
        brightness(50) {
        for (uint8_t i = 0; i < NUM_STRIPS_COUNT; i++) {
            strips[i] = new Adafruit_NeoPixel(LEDS_PER_STRIP_COUNT, dataPins.pins[i], NEO_GRB + NEO_KHZ800);
        }
    }

    ~LEDDisplayHardwareMulti() {
        for (uint8_t i = 0; i < NUM_STRIPS_COUNT; i++) {
            delete strips[i];
        }
    }

    void begin() override {
        for (uint8_t i = 0; i < NUM_STRIPS_COUNT; i++) {
            strips[i]->begin();
            strips[i]->setBrightness(brightness);
            strips[i]->clear();
            strips[i]->show();
        }
    }

    // Multi-strip methods
    uint8_t getStripCount() override {
        return NUM_STRIPS_COUNT;
    }

    uint16_t getPixelsPerStrip() override {
        return LEDS_PER_STRIP_COUNT;
    }

    void setStripPixel(uint8_t strip, uint16_t index, uint8_t r, uint8_t g, uint8_t b) override {
        if (strip < NUM_STRIPS_COUNT && index < LEDS_PER_STRIP_COUNT) {
            strips[strip]->setPixelColor(index, strips[strip]->Color(r, g, b));
        }
    }

    void setStripPixelColor(uint8_t strip, uint16_t index, uint32_t color) override {
        if (strip < NUM_STRIPS_COUNT && index < LEDS_PER_STRIP_COUNT) {
            strips[strip]->setPixelColor(index, color);
        }
    }

    // Legacy single-strip methods (distribute across all strips)
    void setPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) override {
        if (index < TOTAL_LEDS) {
            uint8_t strip = index / LEDS_PER_STRIP_COUNT;
            uint16_t pixel = index % LEDS_PER_STRIP_COUNT;
            setStripPixel(strip, pixel, r, g, b);
        }
    }

    void setPixelColor(uint16_t index, uint32_t color) override {
        if (index < TOTAL_LEDS) {
            uint8_t strip = index / LEDS_PER_STRIP_COUNT;
            uint16_t pixel = index % LEDS_PER_STRIP_COUNT;
            setStripPixelColor(strip, pixel, color);
        }
    }

    void show() override {
        // Update all strips simultaneously
        for (uint8_t i = 0; i < NUM_STRIPS_COUNT; i++) {
            strips[i]->show();
        }
    }

    void clear() override {
        for (uint8_t i = 0; i < NUM_STRIPS_COUNT; i++) {
            strips[i]->clear();
        }
    }

    void setBrightness(uint8_t b) override {
        brightness = b;
        for (uint8_t i = 0; i < NUM_STRIPS_COUNT; i++) {
            strips[i]->setBrightness(b);
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
            uint8_t strip = index / LEDS_PER_STRIP_COUNT;
            uint16_t pixel = index % LEDS_PER_STRIP_COUNT;
            if (strip < NUM_STRIPS_COUNT) {
                return strips[strip]->getPixelColor(pixel);
            }
        }
        return 0;
    }

    void fill(uint8_t r, uint8_t g, uint8_t b) override {
        for (uint8_t i = 0; i < NUM_STRIPS_COUNT; i++) {
            strips[i]->fill(strips[i]->Color(r, g, b), 0, LEDS_PER_STRIP_COUNT);
        }
    }

    void fillColor(uint32_t color) override {
        for (uint8_t i = 0; i < NUM_STRIPS_COUNT; i++) {
            strips[i]->fill(color, 0, LEDS_PER_STRIP_COUNT);
        }
    }

    // Get specific strip's NeoPixel object
    Adafruit_NeoPixel* getNeoPixel(uint8_t strip = 0) {
        if (strip < NUM_STRIPS_COUNT) {
            return strips[strip];
        }
        return nullptr;
    }
};

#endif