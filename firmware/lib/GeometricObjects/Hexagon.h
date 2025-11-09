#ifndef HEXAGON_H
#define HEXAGON_H

#include <Arduino.h>
#include <math.h>

/*
 * HEXAGON COMPONENT
 *
 * Renders rotating hexagons on the LED grid
 *
 * IMPROVEMENT LOG:
 * v1.0 - Initial implementation with rotation and pulsing
 *
 * FEEDBACK NOTES:
 * (Add user feedback here)
 */

class Hexagon {
public:
    float centerX, centerY;
    float radius;
    float rotation;
    uint8_t r, g, b;
    bool filled;

    Hexagon(float x, float y, float rad, uint8_t red, uint8_t green, uint8_t blue, bool fill = false)
        : centerX(x), centerY(y), radius(rad), rotation(0), r(red), g(green), b(blue), filled(fill) {}

    // Render hexagon
    template<typename DisplayType>
    void render(DisplayType& display, uint8_t gridWidth, uint8_t gridHeight) {
        // Calculate 6 vertices
        float vertices[6][2];
        for (int i = 0; i < 6; i++) {
            float angle = rotation + (i * PI / 3);
            vertices[i][0] = centerX + radius * cos(angle);
            vertices[i][1] = centerY + radius * sin(angle);
        }

        if (filled) {
            // Fill hexagon
            for (int x = 0; x < gridWidth; x++) {
                for (int y = 0; y < gridHeight; y++) {
                    if (isInsideHexagon(x, y, vertices)) {
                        display.setPixelOnStrip(y, x, r, g, b);
                    }
                }
            }
        } else {
            // Draw edges
            for (int i = 0; i < 6; i++) {
                int next = (i + 1) % 6;
                drawLine(display, vertices[i][0], vertices[i][1],
                        vertices[next][0], vertices[next][1]);
            }
        }
    }

    bool isInsideHexagon(float px, float py, float vertices[6][2]) {
        // Simple point-in-polygon test
        int crossings = 0;
        for (int i = 0; i < 6; i++) {
            int next = (i + 1) % 6;
            if (((vertices[i][1] <= py && py < vertices[next][1]) ||
                 (vertices[next][1] <= py && py < vertices[i][1])) &&
                (px < (vertices[next][0] - vertices[i][0]) * (py - vertices[i][1]) /
                      (vertices[next][1] - vertices[i][1]) + vertices[i][0])) {
                crossings++;
            }
        }
        return (crossings % 2) == 1;
    }

    template<typename DisplayType>
    void drawLine(DisplayType& display, float x0, float y0, float x1, float y1) {
        float dx = abs(x1 - x0);
        float dy = abs(y1 - y0);
        int steps = max(dx, dy);

        for (int i = 0; i <= steps; i++) {
            float t = (float)i / steps;
            int x = x0 + t * (x1 - x0);
            int y = y0 + t * (y1 - y0);
            if (x >= 0 && x < 90 && y >= 0 && y < 11) {
                display.setPixelOnStrip(y, x, r, g, b);
            }
        }
    }

    // Update animation state
    void update(float rotationSpeed = 0.05) {
        rotation += rotationSpeed;
        if (rotation > 2 * PI) rotation -= 2 * PI;
    }
};

#endif
