#ifndef MULTI_STRIP_HARDWARE_H
#define MULTI_STRIP_HARDWARE_H

#include "MultiStripLEDDisplay.h"
#include <Adafruit_NeoPixel.h>
#include <memory>

class MultiStripHardware : public MultiStripLEDDisplay {
private:
    std::vector<std::unique_ptr<Adafruit_NeoPixel>> neoPixelStrips;

public:
    MultiStripHardware() : MultiStripLEDDisplay() {}

    void begin() override {
        // Initialize each NeoPixel strip
        for (size_t i = 0; i < strips.size(); i++) {
            auto& config = strips[i];
            auto strip = std::make_unique<Adafruit_NeoPixel>(
                config.numLEDs, config.pin, NEO_GRB + NEO_KHZ800
            );
            strip->begin();
            strip->setBrightness(brightness);
            strip->clear();
            strip->show();
            neoPixelStrips.push_back(std::move(strip));
        }
    }

    void setPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) override {
        uint16_t localIndex;
        StripConfig* strip = findStrip(index, localIndex);
        if (strip) {
            size_t stripIdx = strip - &strips[0];
            if (stripIdx < neoPixelStrips.size() && neoPixelStrips[stripIdx]) {
                neoPixelStrips[stripIdx]->setPixelColor(localIndex,
                    neoPixelStrips[stripIdx]->Color(r, g, b));
            }
        }
    }

    void setPixelColor(uint16_t index, uint32_t color) override {
        setPixel(index, Red(color), Green(color), Blue(color));
    }

    void setPixelOnStrip(uint8_t stripIndex, uint16_t pixelIndex, uint8_t r, uint8_t g, uint8_t b) override {
        if (stripIndex < neoPixelStrips.size() && neoPixelStrips[stripIndex]) {
            if (pixelIndex < strips[stripIndex].numLEDs) {
                neoPixelStrips[stripIndex]->setPixelColor(pixelIndex,
                    neoPixelStrips[stripIndex]->Color(r, g, b));
            }
        }
    }

    void showStrip(uint8_t stripIndex) override {
        if (stripIndex < neoPixelStrips.size() && neoPixelStrips[stripIndex]) {
            neoPixelStrips[stripIndex]->show();
        }
    }

    void clear() override {
        for (auto& strip : neoPixelStrips) {
            if (strip) {
                strip->clear();
            }
        }
    }

    uint32_t getPixel(uint16_t index) override {
        uint16_t localIndex;
        StripConfig* strip = findStrip(index, localIndex);
        if (strip) {
            size_t stripIdx = strip - &strips[0];
            if (stripIdx < neoPixelStrips.size() && neoPixelStrips[stripIdx]) {
                return neoPixelStrips[stripIdx]->getPixelColor(localIndex);
            }
        }
        return 0;
    }

    void fill(uint8_t r, uint8_t g, uint8_t b) override {
        for (size_t i = 0; i < neoPixelStrips.size(); i++) {
            if (neoPixelStrips[i]) {
                neoPixelStrips[i]->fill(neoPixelStrips[i]->Color(r, g, b), 0, strips[i].numLEDs);
            }
        }
    }

    void fillColor(uint32_t color) override {
        fill(Red(color), Green(color), Blue(color));
    }

    void setBrightness(uint8_t b) override {
        MultiStripLEDDisplay::setBrightness(b);
        for (auto& strip : neoPixelStrips) {
            if (strip) {
                strip->setBrightness(b);
            }
        }
    }

    // Get NeoPixel object for a specific strip (for advanced features)
    Adafruit_NeoPixel* getNeoPixelStrip(size_t stripIndex) {
        if (stripIndex < neoPixelStrips.size()) {
            return neoPixelStrips[stripIndex].get();
        }
        return nullptr;
    }
};

#endif