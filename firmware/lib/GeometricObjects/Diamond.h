#ifndef DIAMOND_H
#define DIAMOND_H

#include <Arduino.h>
#include <math.h>

/*
 * DIAMOND COMPONENT
 *
 * Renders rotating diamond/rhombus shapes on the LED grid
 *
 * IMPROVEMENT LOG:
 * v1.0 - Initial implementation with rotation and shimmer effect
 *
 * FEEDBACK NOTES:
 * (Add user feedback here)
 */

class Diamond {
public:
    float centerX, centerY;
    float width, height;
    float rotation;
    uint8_t r, g, b;
    float shimmerPhase;
    bool filled;

    Diamond(float x, float y, float w, float h, uint8_t red, uint8_t green, uint8_t blue, bool fill = false)
        : centerX(x), centerY(y), width(w), height(h), rotation(0),
          r(red), g(green), b(blue), shimmerPhase(0), filled(fill) {}

    // Render diamond
    template<typename DisplayType>
    void render(DisplayType& display, uint8_t gridWidth, uint8_t gridHeight) {
        // Calculate 4 vertices (top, right, bottom, left)
        float vertices[4][2];
        float cosR = cos(rotation);
        float sinR = sin(rotation);

        // Top
        vertices[0][0] = centerX;
        vertices[0][1] = centerY - height / 2;
        // Right
        vertices[1][0] = centerX + width / 2;
        vertices[1][1] = centerY;
        // Bottom
        vertices[2][0] = centerX;
        vertices[2][1] = centerY + height / 2;
        // Left
        vertices[3][0] = centerX - width / 2;
        vertices[3][1] = centerY;

        // Rotate vertices
        for (int i = 0; i < 4; i++) {
            float x = vertices[i][0] - centerX;
            float y = vertices[i][1] - centerY;
            vertices[i][0] = centerX + x * cosR - y * sinR;
            vertices[i][1] = centerY + x * sinR + y * cosR;
        }

        if (filled) {
            // Fill diamond
            for (int x = 0; x < gridWidth; x++) {
                for (int y = 0; y < gridHeight; y++) {
                    if (isInsideDiamond(x, y, vertices)) {
                        float shimmer = 0.7 + 0.3 * sin(shimmerPhase);
                        display.setPixelOnStrip(y, x, r * shimmer, g * shimmer, b * shimmer);
                    }
                }
            }
        } else {
            // Draw edges
            for (int i = 0; i < 4; i++) {
                int next = (i + 1) % 4;
                drawLine(display, vertices[i][0], vertices[i][1],
                        vertices[next][0], vertices[next][1]);
            }
        }
    }

    bool isInsideDiamond(float px, float py, float vertices[4][2]) {
        int crossings = 0;
        for (int i = 0; i < 4; i++) {
            int next = (i + 1) % 4;
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
    void update(float rotationSpeed = 0.06, float shimmerSpeed = 0.1) {
        rotation += rotationSpeed;
        if (rotation > 2 * PI) rotation -= 2 * PI;

        shimmerPhase += shimmerSpeed;
        if (shimmerPhase > 2 * PI) shimmerPhase -= 2 * PI;
    }
};

#endif
