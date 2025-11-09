#ifndef SPIRAL_H
#define SPIRAL_H

#include <Arduino.h>
#include <math.h>

/*
 * SPIRAL COMPONENT
 *
 * Renders rotating spiral patterns on the LED grid
 *
 * IMPROVEMENT LOG:
 * v1.0 - Initial implementation with Archimedean and logarithmic spirals
 *
 * FEEDBACK NOTES:
 * (Add user feedback here)
 */

class Spiral {
public:
    float centerX, centerY;
    float spacing;
    float rotation;
    float expansion;
    uint8_t r, g, b;
    bool rainbow;
    int numArms;

    Spiral(float x, float y, float space = 0.5, int arms = 3)
        : centerX(x), centerY(y), spacing(space), rotation(0), expansion(0),
          r(255), g(100), b(255), rainbow(false), numArms(arms) {}

    // Render Archimedean spiral
    template<typename DisplayType>
    void render(DisplayType& display, uint8_t gridWidth, uint8_t gridHeight) {
        for (int x = 0; x < gridWidth; x++) {
            for (int y = 0; y < gridHeight; y++) {
                float dx = x - centerX;
                float dy = y - centerY;
                float dist = sqrt(dx * dx + dy * dy);
                float angle = atan2(dy, dx) - rotation;
                if (angle < 0) angle += 2 * PI;

                // Multi-arm spiral
                for (int arm = 0; arm < numArms; arm++) {
                    float armOffset = (arm * 2 * PI / numArms);
                    float spiralDist = spacing * (angle + armOffset + expansion) / (2 * PI);

                    if (abs(dist - spiralDist) < 0.8) {
                        if (rainbow) {
                            uint8_t hue = (uint8_t)((angle / (2 * PI)) * 255);
                            uint8_t sr, sg, sb;
                            HSVtoRGB(hue, 255, 255, sr, sg, sb);
                            display.setPixelOnStrip(y, x, sr, sg, sb);
                        } else {
                            display.setPixelOnStrip(y, x, r, g, b);
                        }
                        break;
                    }
                }
            }
        }
    }

    // Render with gradient/fade
    template<typename DisplayType>
    void renderGradient(DisplayType& display, uint8_t gridWidth, uint8_t gridHeight) {
        for (int x = 0; x < gridWidth; x++) {
            for (int y = 0; y < gridHeight; y++) {
                float dx = x - centerX;
                float dy = y - centerY;
                float dist = sqrt(dx * dx + dy * dy);
                float angle = atan2(dy, dx) - rotation;
                if (angle < 0) angle += 2 * PI;

                // Multi-arm spiral with gradient
                for (int arm = 0; arm < numArms; arm++) {
                    float armOffset = (arm * 2 * PI / numArms);
                    float spiralDist = spacing * (angle + armOffset + expansion) / (2 * PI);
                    float diff = abs(dist - spiralDist);

                    if (diff < 1.5) {
                        float brightness = 1.0 - (diff / 1.5);
                        display.setPixelOnStrip(y, x, r * brightness, g * brightness, b * brightness);
                        break;
                    }
                }
            }
        }
    }

    // HSV to RGB conversion for rainbow effect
    void HSVtoRGB(uint8_t h, uint8_t s, uint8_t v, uint8_t &r, uint8_t &g, uint8_t &b) {
        uint8_t region, remainder, p, q, t;

        if (s == 0) {
            r = g = b = v;
            return;
        }

        region = h / 43;
        remainder = (h - (region * 43)) * 6;

        p = (v * (255 - s)) >> 8;
        q = (v * (255 - ((s * remainder) >> 8))) >> 8;
        t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

        switch (region) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            default: r = v; g = p; b = q; break;
        }
    }

    // Update animation state
    void update(float rotationSpeed = 0.05, float expansionSpeed = 0.1) {
        rotation += rotationSpeed;
        if (rotation > 2 * PI) rotation -= 2 * PI;

        expansion += expansionSpeed;
        if (expansion > 2 * PI * 10) expansion = 0;
    }

    void enableRainbow() { rainbow = true; }
    void setColor(uint8_t red, uint8_t green, uint8_t blue) {
        r = red; g = green; b = blue;
        rainbow = false;
    }
};

#endif
