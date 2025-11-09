#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <Arduino.h>
#include <math.h>

/*
 * TRIANGLE COMPONENT
 *
 * Renders rotating triangles on the LED grid
 *
 * IMPROVEMENT LOG:
 * v1.0 - Initial implementation with rotation and sizing
 *
 * FEEDBACK NOTES:
 * (Add user feedback here)
 */

class Triangle {
public:
    float centerX, centerY;
    float size;
    float rotation;
    uint8_t r, g, b;
    bool filled;

    Triangle(float x, float y, float sz, uint8_t red, uint8_t green, uint8_t blue, bool fill = false)
        : centerX(x), centerY(y), size(sz), rotation(0), r(red), g(green), b(blue), filled(fill) {}

    // Helper to check if point is inside triangle
    bool isInsideTriangle(float px, float py, float x1, float y1, float x2, float y2, float x3, float y3) {
        float d1, d2, d3;
        bool has_neg, has_pos;

        d1 = sign(px, py, x1, y1, x2, y2);
        d2 = sign(px, py, x2, y2, x3, y3);
        d3 = sign(px, py, x3, y3, x1, y1);

        has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(has_neg && has_pos);
    }

    float sign(float px, float py, float x1, float y1, float x2, float y2) {
        return (px - x2) * (y1 - y2) - (x1 - x2) * (py - y2);
    }

    // Render triangle
    template<typename DisplayType>
    void render(DisplayType& display, uint8_t gridWidth, uint8_t gridHeight) {
        // Calculate three vertices of equilateral triangle
        float x1 = centerX + size * cos(rotation);
        float y1 = centerY + size * sin(rotation);
        float x2 = centerX + size * cos(rotation + 2 * PI / 3);
        float y2 = centerY + size * sin(rotation + 2 * PI / 3);
        float x3 = centerX + size * cos(rotation + 4 * PI / 3);
        float y3 = centerY + size * sin(rotation + 4 * PI / 3);

        if (filled) {
            // Fill triangle
            for (int x = 0; x < gridWidth; x++) {
                for (int y = 0; y < gridHeight; y++) {
                    if (isInsideTriangle(x, y, x1, y1, x2, y2, x3, y3)) {
                        display.setPixelOnStrip(y, x, r, g, b);
                    }
                }
            }
        } else {
            // Draw edges
            drawLine(display, x1, y1, x2, y2);
            drawLine(display, x2, y2, x3, y3);
            drawLine(display, x3, y3, x1, y1);
        }
    }

    // Simple line drawing
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
    void update(float rotationSpeed = 0.1) {
        rotation += rotationSpeed;
        if (rotation > 2 * PI) rotation -= 2 * PI;
    }
};

#endif
