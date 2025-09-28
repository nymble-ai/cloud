#ifndef LED_DISPLAY_SIMULATOR_H
#define LED_DISPLAY_SIMULATOR_H

#include "LEDDisplay.h"

template <uint16_t NUM_LEDS>
class LEDDisplaySimulator : public LEDDisplay {
private:
    struct LED {
        uint8_t r, g, b;
    };

    LED leds[NUM_LEDS];
    uint8_t brightness;
    bool needsUpdate;

    static const uint8_t FRAME_START = 0xFF;
    static const uint8_t FRAME_END = 0xFE;
    static const uint8_t ESCAPE_BYTE = 0xFD;

    void sendEscaped(uint8_t byte) {
        if (byte == FRAME_START || byte == FRAME_END || byte == ESCAPE_BYTE) {
            Serial.write(ESCAPE_BYTE);
        }
        Serial.write(byte);
    }

public:
    LEDDisplaySimulator() : brightness(255), needsUpdate(false) {
        for (uint16_t i = 0; i < NUM_LEDS; i++) {
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
        Serial.println("LED Simulator Ready");
        clear();
        show();
    }

    void setPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) override {
        if (index < NUM_LEDS) {
            uint8_t scaledR = (r * brightness) / 255;
            uint8_t scaledG = (g * brightness) / 255;
            uint8_t scaledB = (b * brightness) / 255;

            leds[index].r = scaledR;
            leds[index].g = scaledG;
            leds[index].b = scaledB;
            needsUpdate = true;
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

        uint8_t countHigh = (NUM_LEDS >> 8) & 0xFF;
        uint8_t countLow = NUM_LEDS & 0xFF;
        sendEscaped(countHigh);
        sendEscaped(countLow);

        for (uint16_t i = 0; i < NUM_LEDS; i++) {
            sendEscaped(leds[i].r);
            sendEscaped(leds[i].g);
            sendEscaped(leds[i].b);
        }

        Serial.write(FRAME_END);
        Serial.flush();

        needsUpdate = false;
    }

    void clear() override {
        for (uint16_t i = 0; i < NUM_LEDS; i++) {
            leds[i].r = 0;
            leds[i].g = 0;
            leds[i].b = 0;
        }
        needsUpdate = true;
    }

    void setBrightness(uint8_t b) override {
        brightness = b;
        needsUpdate = true;
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
        for (uint16_t i = 0; i < NUM_LEDS; i++) {
            setPixel(i, r, g, b);
        }
    }

    void fillColor(uint32_t color) override {
        fill(Red(color), Green(color), Blue(color));
    }
};

#endif