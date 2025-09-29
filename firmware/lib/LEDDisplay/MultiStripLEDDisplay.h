#ifndef MULTI_STRIP_LED_DISPLAY_H
#define MULTI_STRIP_LED_DISPLAY_H

#include "LEDDisplay.h"
#include <vector>

// Structure to define a strip configuration
struct StripConfig {
    uint8_t pin;           // GPIO pin number
    uint16_t numLEDs;      // Number of LEDs on this strip
    uint16_t startIndex;   // Global start index for this strip
    String name;           // Optional name for the strip

    StripConfig(uint8_t p, uint16_t n, const String& nm = "")
        : pin(p), numLEDs(n), startIndex(0), name(nm) {}
};

// Base class for multi-strip LED displays
class MultiStripLEDDisplay : public LEDDisplay {
protected:
    std::vector<StripConfig> strips;
    uint16_t totalLEDs;
    uint8_t brightness;

    // Calculate total LEDs and assign start indices
    void calculateIndices() {
        totalLEDs = 0;
        for (auto& strip : strips) {
            strip.startIndex = totalLEDs;
            totalLEDs += strip.numLEDs;
        }
    }

    // Find which strip an LED index belongs to
    StripConfig* findStrip(uint16_t index, uint16_t& localIndex) {
        for (auto& strip : strips) {
            if (index >= strip.startIndex && index < strip.startIndex + strip.numLEDs) {
                localIndex = index - strip.startIndex;
                return &strip;
            }
        }
        return nullptr;
    }

public:
    MultiStripLEDDisplay() : totalLEDs(0), brightness(128) {}

    // Add a strip configuration
    void addStrip(uint8_t pin, uint16_t numLEDs, const String& name = "") {
        strips.push_back(StripConfig(pin, numLEDs, name));
        calculateIndices();
    }

    // Get number of strips
    size_t getStripCount() const {
        return strips.size();
    }

    // Get strip configuration
    const StripConfig& getStrip(size_t index) const {
        return strips[index];
    }

    // Get total pixel count across all strips
    uint16_t getPixelCount() override {
        return totalLEDs;
    }

    // Set/get brightness
    void setBrightness(uint8_t b) override {
        brightness = b;
    }

    uint8_t getBrightness() override {
        return brightness;
    }

    // Helper method to set pixel on a specific strip
    virtual void setPixelOnStrip(uint8_t stripIndex, uint16_t pixelIndex, uint8_t r, uint8_t g, uint8_t b) = 0;

    // Helper method to show a specific strip
    virtual void showStrip(uint8_t stripIndex) = 0;

    // Show all strips
    void show() override {
        for (size_t i = 0; i < strips.size(); i++) {
            showStrip(i);
        }
    }
};

#endif