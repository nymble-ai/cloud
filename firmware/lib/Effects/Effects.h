#ifndef EFFECTS_H
#define EFFECTS_H

#include <Arduino.h>
#include "StripManager.h"

// Effect state storage for each strip
struct EffectState {
    uint16_t hue;
    uint16_t position;
    uint32_t lastUpdate;
    uint8_t step;
    uint8_t colorIndex;
    bool direction;
};

class Effects {
private:
    StripManager* manager;
    static EffectState states[10]; // Support up to 10 strips

public:
    Effects(StripManager* mgr) : manager(mgr) {
        // Initialize states
        for (int i = 0; i < 10; i++) {
            states[i] = {0, 0, 0, 0, 0, true};
        }
    }

    // ==================== COLOR UTILITIES ====================

    static uint32_t colorWheel(uint8_t wheelPos) {
        wheelPos = 255 - wheelPos;
        if (wheelPos < 85) {
            return LEDDisplay::Color(255 - wheelPos * 3, 0, wheelPos * 3);
        }
        if (wheelPos < 170) {
            wheelPos -= 85;
            return LEDDisplay::Color(0, wheelPos * 3, 255 - wheelPos * 3);
        }
        wheelPos -= 170;
        return LEDDisplay::Color(wheelPos * 3, 255 - wheelPos * 3, 0);
    }

    static uint32_t HSVtoRGB(uint16_t hue, uint8_t sat, uint8_t val) {
        uint8_t r, g, b;
        hue = hue % 360;

        uint8_t sector = hue / 60;
        uint8_t remainder = (hue % 60) * 255 / 60;

        uint8_t p = (val * (255 - sat)) / 255;
        uint8_t q = (val * (255 - ((sat * remainder) / 255))) / 255;
        uint8_t t = (val * (255 - ((sat * (255 - remainder)) / 255))) / 255;

        switch (sector) {
            case 0: r = val; g = t; b = p; break;
            case 1: r = q; g = val; b = p; break;
            case 2: r = p; g = val; b = t; break;
            case 3: r = p; g = q; b = val; break;
            case 4: r = t; g = p; b = val; break;
            default: r = val; g = p; b = q; break;
        }

        return LEDDisplay::Color(r, g, b);
    }

    // ==================== SOLID COLOR EFFECTS ====================

    // Solid color effect
    static EffectFunction solidColor(uint8_t r, uint8_t g, uint8_t b) {
        return [r, g, b](uint8_t stripIndex, uint32_t currentMillis) {
            StripManager* mgr = static_cast<StripManager*>(this);
            ManagedStrip& strip = mgr->getStrip(stripIndex);

            for (uint16_t i = 0; i < strip.numLEDs; i++) {
                mgr->setPixelOnStrip(stripIndex, i, r, g, b);
            }
        };
    }

    // Breathing effect (fading in and out)
    static EffectFunction breathing(uint8_t r, uint8_t g, uint8_t b, uint16_t speed = 10) {
        return [r, g, b, speed](uint8_t stripIndex, uint32_t currentMillis) {
            StripManager* mgr = static_cast<StripManager*>(this);
            ManagedStrip& strip = mgr->getStrip(stripIndex);
            EffectState& state = states[stripIndex];

            if (currentMillis - state.lastUpdate > speed) {
                state.lastUpdate = currentMillis;

                // Calculate breathing brightness
                float breath = (exp(sin(state.step * PI / 180.0)) - 0.36787944) * 108.0;
                uint8_t brightness = constrain(breath, 0, 255);

                uint8_t rr = (r * brightness) / 255;
                uint8_t gg = (g * brightness) / 255;
                uint8_t bb = (b * brightness) / 255;

                for (uint16_t i = 0; i < strip.numLEDs; i++) {
                    mgr->setPixelOnStrip(stripIndex, i, rr, gg, bb);
                }

                state.step += 2;
                if (state.step >= 360) state.step = 0;
            }
        };
    }

    // ==================== RAINBOW EFFECTS ====================

    // Rainbow cycle across the strip
    static EffectFunction rainbow(uint16_t speed = 5) {
        return [speed](uint8_t stripIndex, uint32_t currentMillis) {
            StripManager* mgr = static_cast<StripManager*>(this);
            ManagedStrip& strip = mgr->getStrip(stripIndex);
            EffectState& state = states[stripIndex];

            if (currentMillis - state.lastUpdate > speed) {
                state.lastUpdate = currentMillis;

                for (uint16_t i = 0; i < strip.numLEDs; i++) {
                    uint16_t pixelHue = state.hue + (i * 65536L / strip.numLEDs);
                    uint32_t color = colorWheel((pixelHue >> 8) & 0xFF);
                    mgr->setPixelOnStrip(stripIndex, i,
                        LEDDisplay::Red(color),
                        LEDDisplay::Green(color),
                        LEDDisplay::Blue(color));
                }

                state.hue += 256;
            }
        };
    }

    // ==================== CHASE EFFECTS ====================

    // Simple chase effect
    static EffectFunction chase(uint8_t r, uint8_t g, uint8_t b, uint8_t tailLength = 5, uint16_t speed = 50) {
        return [r, g, b, tailLength, speed](uint8_t stripIndex, uint32_t currentMillis) {
            StripManager* mgr = static_cast<StripManager*>(this);
            ManagedStrip& strip = mgr->getStrip(stripIndex);
            EffectState& state = states[stripIndex];

            if (currentMillis - state.lastUpdate > speed) {
                state.lastUpdate = currentMillis;

                // Clear strip
                for (uint16_t i = 0; i < strip.numLEDs; i++) {
                    mgr->setPixelOnStrip(stripIndex, i, 0, 0, 0);
                }

                // Draw chase with tail
                for (uint8_t t = 0; t < tailLength; t++) {
                    int16_t pos = state.position - t;
                    if (pos < 0) pos += strip.numLEDs;

                    uint8_t brightness = 255 - (t * (255 / tailLength));
                    mgr->setPixelOnStrip(stripIndex, pos,
                        (r * brightness) / 255,
                        (g * brightness) / 255,
                        (b * brightness) / 255);
                }

                state.position++;
                if (state.position >= strip.numLEDs) state.position = 0;
            }
        };
    }

    // Theater chase effect
    static EffectFunction theaterChase(uint8_t r, uint8_t g, uint8_t b, uint16_t speed = 100) {
        return [r, g, b, speed](uint8_t stripIndex, uint32_t currentMillis) {
            StripManager* mgr = static_cast<StripManager*>(this);
            ManagedStrip& strip = mgr->getStrip(stripIndex);
            EffectState& state = states[stripIndex];

            if (currentMillis - state.lastUpdate > speed) {
                state.lastUpdate = currentMillis;

                for (uint16_t i = 0; i < strip.numLEDs; i++) {
                    if ((i + state.step) % 3 == 0) {
                        mgr->setPixelOnStrip(stripIndex, i, r, g, b);
                    } else {
                        mgr->setPixelOnStrip(stripIndex, i, 0, 0, 0);
                    }
                }

                state.step++;
                if (state.step >= 3) state.step = 0;
            }
        };
    }

    // ==================== SPARKLE EFFECTS ====================

    // Sparkle/twinkle effect
    static EffectFunction sparkle(uint8_t r, uint8_t g, uint8_t b, uint8_t density = 5, uint16_t speed = 50) {
        return [r, g, b, density, speed](uint8_t stripIndex, uint32_t currentMillis) {
            StripManager* mgr = static_cast<StripManager*>(this);
            ManagedStrip& strip = mgr->getStrip(stripIndex);
            EffectState& state = states[stripIndex];

            if (currentMillis - state.lastUpdate > speed) {
                state.lastUpdate = currentMillis;

                // Fade all pixels
                for (uint16_t i = 0; i < strip.numLEDs; i++) {
                    uint16_t globalIdx = mgr->getGlobalIndex(stripIndex, i);
                    // Get current color and fade it
                    // This is simplified - you'd need access to current pixel values
                    mgr->setPixelOnStrip(stripIndex, i, 0, 0, 0);
                }

                // Add random sparkles
                for (uint8_t s = 0; s < density; s++) {
                    if (random(100) < 30) { // 30% chance
                        uint16_t pos = random(strip.numLEDs);
                        mgr->setPixelOnStrip(stripIndex, pos, r, g, b);
                    }
                }
            }
        };
    }

    // ==================== GRADIENT EFFECTS ====================

    // Gradient between two colors
    static EffectFunction gradient(uint8_t r1, uint8_t g1, uint8_t b1,
                                  uint8_t r2, uint8_t g2, uint8_t b2) {
        return [r1, g1, b1, r2, g2, b2](uint8_t stripIndex, uint32_t currentMillis) {
            StripManager* mgr = static_cast<StripManager*>(this);
            ManagedStrip& strip = mgr->getStrip(stripIndex);

            for (uint16_t i = 0; i < strip.numLEDs; i++) {
                float ratio = (float)i / (strip.numLEDs - 1);
                uint8_t r = r1 + (r2 - r1) * ratio;
                uint8_t g = g1 + (g2 - g1) * ratio;
                uint8_t b = b1 + (b2 - b1) * ratio;
                mgr->setPixelOnStrip(stripIndex, i, r, g, b);
            }
        };
    }

    // ==================== WAVE EFFECTS ====================

    // Color wave effect
    static EffectFunction wave(uint8_t r, uint8_t g, uint8_t b, uint16_t waveLength = 20, uint16_t speed = 30) {
        return [r, g, b, waveLength, speed](uint8_t stripIndex, uint32_t currentMillis) {
            StripManager* mgr = static_cast<StripManager*>(this);
            ManagedStrip& strip = mgr->getStrip(stripIndex);
            EffectState& state = states[stripIndex];

            if (currentMillis - state.lastUpdate > speed) {
                state.lastUpdate = currentMillis;

                for (uint16_t i = 0; i < strip.numLEDs; i++) {
                    float wave = sin(((float)(i + state.position) / waveLength) * 2 * PI);
                    uint8_t brightness = (wave + 1) * 127.5; // Map -1 to 1 => 0 to 255

                    mgr->setPixelOnStrip(stripIndex, i,
                        (r * brightness) / 255,
                        (g * brightness) / 255,
                        (b * brightness) / 255);
                }

                state.position++;
                if (state.position >= waveLength) state.position = 0;
            }
        };
    }

    // ==================== STROBE EFFECTS ====================

    // Strobe effect
    static EffectFunction strobe(uint8_t r, uint8_t g, uint8_t b, uint16_t onTime = 50, uint16_t offTime = 100) {
        return [r, g, b, onTime, offTime](uint8_t stripIndex, uint32_t currentMillis) {
            StripManager* mgr = static_cast<StripManager*>(this);
            ManagedStrip& strip = mgr->getStrip(stripIndex);
            EffectState& state = states[stripIndex];

            uint32_t cycleTime = currentMillis % (onTime + offTime);

            if (cycleTime < onTime) {
                // LEDs on
                for (uint16_t i = 0; i < strip.numLEDs; i++) {
                    mgr->setPixelOnStrip(stripIndex, i, r, g, b);
                }
            } else {
                // LEDs off
                for (uint16_t i = 0; i < strip.numLEDs; i++) {
                    mgr->setPixelOnStrip(stripIndex, i, 0, 0, 0);
                }
            }
        };
    }

    // ==================== FIRE EFFECTS ====================

    // Fire/flame effect
    static EffectFunction fire(uint8_t cooling = 50, uint8_t sparking = 120) {
        return [cooling, sparking](uint8_t stripIndex, uint32_t currentMillis) {
            StripManager* mgr = static_cast<StripManager*>(this);
            ManagedStrip& strip = mgr->getStrip(stripIndex);

            static uint8_t heat[256]; // Heat array (max 256 LEDs per strip)

            // Cool down every cell
            for (int i = 0; i < strip.numLEDs; i++) {
                heat[i] = max(0, heat[i] - random(0, ((cooling * 10) / strip.numLEDs) + 2));
            }

            // Heat drift upwards
            for (int k = strip.numLEDs - 1; k >= 2; k--) {
                heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
            }

            // Randomly ignite new sparks
            if (random(255) < sparking) {
                int y = random(7);
                heat[y] = min(255, heat[y] + random(160, 255));
            }

            // Convert heat to LED colors
            for (int j = 0; j < strip.numLEDs; j++) {
                uint8_t temperature = heat[j];
                uint8_t r, g, b;

                // Calculate color based on temperature
                if (temperature < 128) {
                    r = temperature * 2;
                    g = 0;
                    b = 0;
                } else {
                    r = 255;
                    g = (temperature - 128) * 2;
                    b = 0;
                }

                mgr->setPixelOnStrip(stripIndex, j, r, g, b);
            }
        };
    }
};

// Initialize static member
EffectState Effects::states[10];

#endif