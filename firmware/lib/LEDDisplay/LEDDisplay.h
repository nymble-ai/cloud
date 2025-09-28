#ifndef LED_DISPLAY_H
#define LED_DISPLAY_H

#include <Arduino.h>

class LEDDisplay {
public:
    virtual ~LEDDisplay() {}

    virtual void begin() = 0;

    virtual void setPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) = 0;

    virtual void setPixelColor(uint16_t index, uint32_t color) = 0;

    virtual void show() = 0;

    virtual void clear() = 0;

    virtual void setBrightness(uint8_t brightness) = 0;

    virtual uint8_t getBrightness() = 0;

    virtual uint16_t getPixelCount() = 0;

    virtual uint32_t getPixel(uint16_t index) = 0;

    virtual void fill(uint8_t r, uint8_t g, uint8_t b) = 0;

    virtual void fillColor(uint32_t color) = 0;

    static uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
        return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    }

    static uint8_t Red(uint32_t color) {
        return (color >> 16) & 0xFF;
    }

    static uint8_t Green(uint32_t color) {
        return (color >> 8) & 0xFF;
    }

    static uint8_t Blue(uint32_t color) {
        return color & 0xFF;
    }
};

#endif