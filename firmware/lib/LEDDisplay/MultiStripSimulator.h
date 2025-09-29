#ifndef MULTI_STRIP_SIMULATOR_H
#define MULTI_STRIP_SIMULATOR_H

#include "MultiStripLEDDisplay.h"

class MultiStripSimulator : public MultiStripLEDDisplay {
private:
    struct LED {
        uint8_t r, g, b;
    };

    std::vector<LED> leds;
    bool needsUpdate;

    static const uint8_t FRAME_START = 0xFF;
    static const uint8_t FRAME_END = 0xFE;
    static const uint8_t ESCAPE_BYTE = 0xFD;
    static const uint8_t MULTI_STRIP_MARKER = 0xFC;  // New marker for multi-strip mode

    void sendEscaped(uint8_t byte) {
        if (byte >= MULTI_STRIP_MARKER) {  // Escape bytes >= 0xFC
            Serial.write(ESCAPE_BYTE);
        }
        Serial.write(byte);
    }

public:
    MultiStripSimulator() : MultiStripLEDDisplay(), needsUpdate(false) {}

    void begin() override {
        Serial.begin(115200);
        while (!Serial && millis() < 3000) {
            delay(10);
        }

        // Allocate LED buffer
        leds.resize(totalLEDs);
        clear();

        // Send initial configuration frame
        sendConfiguration();

        needsUpdate = true;
        show();
    }

    // Send strip configuration to visualizer
    void sendConfiguration() {
        Serial.write(FRAME_START);
        Serial.write(MULTI_STRIP_MARKER);  // Indicates multi-strip mode

        // Send number of strips
        sendEscaped(strips.size());

        // Send each strip's configuration
        for (const auto& strip : strips) {
            sendEscaped(strip.pin);
            sendEscaped((strip.numLEDs >> 8) & 0xFF);
            sendEscaped(strip.numLEDs & 0xFF);

            // Send strip name length and name (up to 16 chars)
            uint8_t nameLen = min((size_t)16, strip.name.length());
            sendEscaped(nameLen);
            for (uint8_t i = 0; i < nameLen; i++) {
                sendEscaped(strip.name[i]);
            }
        }

        Serial.write(FRAME_END);
        Serial.flush();
    }

    void setPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) override {
        if (index < totalLEDs) {
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

    void setPixelOnStrip(uint8_t stripIndex, uint16_t pixelIndex, uint8_t r, uint8_t g, uint8_t b) override {
        if (stripIndex < strips.size() && pixelIndex < strips[stripIndex].numLEDs) {
            uint16_t globalIndex = strips[stripIndex].startIndex + pixelIndex;
            setPixel(globalIndex, r, g, b);
        }
    }

    void showStrip(uint8_t stripIndex) override {
        // In simulator, we send all strips at once in show()
        needsUpdate = true;
    }

    void show() override {
        if (!needsUpdate) {
            return;
        }

        // Send frame with strip information
        Serial.write(FRAME_START);

        // Send total LED count
        uint8_t countHigh = (totalLEDs >> 8) & 0xFF;
        uint8_t countLow = totalLEDs & 0xFF;
        sendEscaped(countHigh);
        sendEscaped(countLow);

        // Send all LED data
        for (uint16_t i = 0; i < totalLEDs; i++) {
            sendEscaped(leds[i].r);
            sendEscaped(leds[i].g);
            sendEscaped(leds[i].b);
        }

        Serial.write(FRAME_END);
        Serial.flush();

        needsUpdate = false;
    }

    void clear() override {
        for (auto& led : leds) {
            led.r = led.g = led.b = 0;
        }
        needsUpdate = true;
    }

    uint32_t getPixel(uint16_t index) override {
        if (index < totalLEDs) {
            return Color(leds[index].r, leds[index].g, leds[index].b);
        }
        return 0;
    }

    void fill(uint8_t r, uint8_t g, uint8_t b) override {
        for (uint16_t i = 0; i < totalLEDs; i++) {
            setPixel(i, r, g, b);
        }
    }

    void fillColor(uint32_t color) override {
        fill(Red(color), Green(color), Blue(color));
    }
};

#endif