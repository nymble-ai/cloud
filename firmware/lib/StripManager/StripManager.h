#ifndef STRIP_MANAGER_H
#define STRIP_MANAGER_H

#include <Arduino.h>
#include <vector>
#include <functional>
#include "LEDDisplay.h"

// Effect function type - takes strip index and current time
typedef std::function<void(uint8_t stripIndex, uint32_t currentMillis)> EffectFunction;

// Strip configuration with effect
struct ManagedStrip {
    uint8_t pin;
    uint16_t numLEDs;
    String name;
    EffectFunction effect;
    bool enabled;
    uint8_t brightness;

    ManagedStrip(uint8_t p, uint16_t n, const String& nm = "")
        : pin(p), numLEDs(n), name(nm), effect(nullptr), enabled(true), brightness(255) {}
};

class StripManager {
private:
    std::vector<ManagedStrip> strips;
    LEDDisplay* display;

public:
    StripManager(LEDDisplay* disp) : display(disp) {}

    // Add a new strip
    uint8_t addStrip(uint8_t pin, uint16_t numLEDs, const String& name = "") {
        strips.push_back(ManagedStrip(pin, numLEDs, name));
        return strips.size() - 1; // Return strip index
    }

    // Remove a strip by index
    void removeStrip(uint8_t index) {
        if (index < strips.size()) {
            strips.erase(strips.begin() + index);
        }
    }

    // Set effect for a strip
    void setEffect(uint8_t stripIndex, EffectFunction effect) {
        if (stripIndex < strips.size()) {
            strips[stripIndex].effect = effect;
        }
    }

    // Enable/disable a strip
    void enableStrip(uint8_t stripIndex, bool enable) {
        if (stripIndex < strips.size()) {
            strips[stripIndex].enabled = enable;
        }
    }

    // Set strip brightness
    void setStripBrightness(uint8_t stripIndex, uint8_t brightness) {
        if (stripIndex < strips.size()) {
            strips[stripIndex].brightness = brightness;
        }
    }

    // Update all strips
    void update() {
        uint32_t currentMillis = millis();

        for (uint8_t i = 0; i < strips.size(); i++) {
            if (strips[i].enabled && strips[i].effect) {
                strips[i].effect(i, currentMillis);
            }
        }

        display->show();
    }

    // Get strip info
    ManagedStrip& getStrip(uint8_t index) {
        return strips[index];
    }

    size_t getStripCount() const {
        return strips.size();
    }

    // Clear all strips
    void clearAll() {
        display->clear();
        display->show();
    }

    // Set pixel on specific strip (helper for effects)
    void setPixelOnStrip(uint8_t stripIndex, uint16_t pixel, uint8_t r, uint8_t g, uint8_t b) {
        // This would interface with your MultiStripDisplay
        // For now, using a simple calculation
        if (stripIndex < strips.size() && pixel < strips[stripIndex].numLEDs) {
            uint16_t globalIndex = 0;
            for (uint8_t i = 0; i < stripIndex; i++) {
                globalIndex += strips[i].numLEDs;
            }
            globalIndex += pixel;

            // Apply strip brightness
            uint8_t br = strips[stripIndex].brightness;
            r = (r * br) / 255;
            g = (g * br) / 255;
            b = (b * br) / 255;

            display->setPixel(globalIndex, r, g, b);
        }
    }

    // Get global LED index for a strip pixel
    uint16_t getGlobalIndex(uint8_t stripIndex, uint16_t pixel) {
        uint16_t globalIndex = 0;
        for (uint8_t i = 0; i < stripIndex; i++) {
            globalIndex += strips[i].numLEDs;
        }
        return globalIndex + pixel;
    }
};

#endif