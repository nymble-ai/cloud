#ifndef CIRCLE_H
#define CIRCLE_H

#include <Arduino.h>
#include <math.h>

/*
 * CIRCLE COMPONENT
 *
 * Renders rotating, pulsing, and expanding circles on the LED grid
 *
 * IMPROVEMENT LOG:
 * v1.0 - Initial implementation with rotation and pulsing
 *
 * FEEDBACK NOTES:
 * (Add user feedback here)
 */

class Circle {
public:
    float centerX, centerY;
    float radius;
    float rotation;
    uint8_t r, g, b;
    float pulsePhase;
    bool filled;

    Circle(float x, float y, float rad, uint8_t red, uint8_t green, uint8_t blue, bool fill = false)
        : centerX(x), centerY(y), radius(rad), rotation(0), r(red), g(green), b(blue), pulsePhase(0), filled(fill) {}

    // Render circle outline or filled
    template<typename DisplayType>
    void render(DisplayType& display, uint8_t gridWidth, uint8_t gridHeight) {
        for (int x = 0; x < gridWidth; x++) {
            for (int y = 0; y < gridHeight; y++) {
                float dx = x - centerX;
                float dy = y - centerY;
                float dist = sqrt(dx * dx + dy * dy);

                if (filled) {
                    if (dist <= radius) {
                        float brightness = 1.0 - (dist / radius) * 0.5;
                        display.setPixelOnStrip(y, x, r * brightness, g * brightness, b * brightness);
                    }
                } else {
                    if (abs(dist - radius) < 1.5) {
                        display.setPixelOnStrip(y, x, r, g, b);
                    }
                }
            }
        }
    }

    // Render with rotation effect
    template<typename DisplayType>
    void renderRotating(DisplayType& display, uint8_t gridWidth, uint8_t gridHeight, int numDots = 8) {
        for (int i = 0; i < numDots; i++) {
            float angle = rotation + (i * 2 * PI / numDots);
            int x = centerX + radius * cos(angle);
            int y = centerY + radius * sin(angle);

            if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
                display.setPixelOnStrip(y, x, r, g, b);
            }
        }
    }

    // Update animation state
    void update(float rotationSpeed = 0.1, float pulseSpeed = 0.05) {
        rotation += rotationSpeed;
        if (rotation > 2 * PI) rotation -= 2 * PI;

        pulsePhase += pulseSpeed;
        if (pulsePhase > 2 * PI) pulsePhase -= 2 * PI;
    }

    // Pulsing radius
    float getPulsingRadius() {
        return radius + sin(pulsePhase) * (radius * 0.3);
    }
};

#endif
