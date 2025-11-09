#ifndef WAVE_H
#define WAVE_H

#include <Arduino.h>
#include <math.h>

/*
 * WAVE COMPONENT
 *
 * Renders flowing wave patterns on the LED grid
 *
 * IMPROVEMENT LOG:
 * v1.0 - Initial implementation with sine waves and interference patterns
 *
 * FEEDBACK NOTES:
 * (Add user feedback here)
 */

class Wave {
public:
    float phase;
    float frequency;
    float amplitude;
    uint8_t r, g, b;
    bool horizontal;
    int numWaves;

    Wave(float freq = 0.5, float amp = 3.0, uint8_t red = 100, uint8_t green = 150, uint8_t blue = 255, bool horiz = true, int waves = 1)
        : phase(0), frequency(freq), amplitude(amp), r(red), g(green), b(blue), horizontal(horiz), numWaves(waves) {}

    // Render wave pattern
    template<typename DisplayType>
    void render(DisplayType& display, uint8_t gridWidth, uint8_t gridHeight) {
        for (int x = 0; x < gridWidth; x++) {
            for (int y = 0; y < gridHeight; y++) {
                if (horizontal) {
                    float waveY = (gridHeight / 2.0) + amplitude * sin(frequency * x + phase);
                    if (abs(y - waveY) < 1.5) {
                        display.setPixelOnStrip(y, x, r, g, b);
                    }
                } else {
                    float waveX = (gridWidth / 2.0) + amplitude * sin(frequency * y + phase);
                    if (abs(x - waveX) < 1.5) {
                        display.setPixelOnStrip(y, x, r, g, b);
                    }
                }
            }
        }
    }

    // Render multiple interfering waves
    template<typename DisplayType>
    void renderInterference(DisplayType& display, uint8_t gridWidth, uint8_t gridHeight) {
        for (int x = 0; x < gridWidth; x++) {
            for (int y = 0; y < gridHeight; y++) {
                float value = 0;

                // Add multiple wave components
                for (int i = 0; i < numWaves; i++) {
                    float freq = frequency * (i + 1);
                    float phaseOffset = phase + (i * PI / 4);

                    if (horizontal) {
                        value += sin(freq * x + phaseOffset);
                    } else {
                        value += sin(freq * y + phaseOffset);
                    }
                }

                // Normalize and create brightness
                value = (value / numWaves + 1.0) / 2.0; // 0 to 1
                uint8_t brightness = value * 255;

                if (brightness > 50) { // Threshold to avoid too dim pixels
                    display.setPixelOnStrip(y, x,
                        (r * brightness) / 255,
                        (g * brightness) / 255,
                        (b * brightness) / 255);
                }
            }
        }
    }

    // Render gradient wave
    template<typename DisplayType>
    void renderGradient(DisplayType& display, uint8_t gridWidth, uint8_t gridHeight) {
        for (int x = 0; x < gridWidth; x++) {
            for (int y = 0; y < gridHeight; y++) {
                float value;
                if (horizontal) {
                    float waveY = (gridHeight / 2.0) + amplitude * sin(frequency * x + phase);
                    float dist = abs(y - waveY);
                    value = max(0.0f, 1.0f - (dist / 2.0f));
                } else {
                    float waveX = (gridWidth / 2.0) + amplitude * sin(frequency * y + phase);
                    float dist = abs(x - waveX);
                    value = max(0.0f, 1.0f - (dist / 2.0f));
                }

                if (value > 0.1) {
                    display.setPixelOnStrip(y, x, r * value, g * value, b * value);
                }
            }
        }
    }

    // Update animation state
    void update(float speed = 0.15) {
        phase += speed;
        if (phase > 2 * PI) phase -= 2 * PI;
    }

    void setColor(uint8_t red, uint8_t green, uint8_t blue) {
        r = red; g = green; b = blue;
    }
};

#endif
