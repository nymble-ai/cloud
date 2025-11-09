#ifndef STAR_H
#define STAR_H

#include <Arduino.h>
#include <math.h>

/*
 * STAR COMPONENT
 *
 * Renders multicolor rotating star shapes on the LED grid
 *
 * IMPROVEMENT LOG:
 * v1.0 - Initial implementation with 5-point stars and multicolor support
 *
 * FEEDBACK NOTES:
 * (Add user feedback here)
 */

class Star {
public:
    float centerX, centerY;
    float outerRadius, innerRadius;
    float rotation;
    uint8_t colors[5][3]; // Up to 5 different colors for points
    int numPoints;
    bool multicolor;

    Star(float x, float y, float outer, float inner, int points = 5)
        : centerX(x), centerY(y), outerRadius(outer), innerRadius(inner),
          rotation(0), numPoints(points), multicolor(false) {
        // Default single color
        for (int i = 0; i < 5; i++) {
            colors[i][0] = 255;
            colors[i][1] = 255;
            colors[i][2] = 0;
        }
    }

    // Set multicolor mode with different colors per point
    void setMulticolor(uint8_t c1[3], uint8_t c2[3], uint8_t c3[3], uint8_t c4[3], uint8_t c5[3]) {
        multicolor = true;
        memcpy(colors[0], c1, 3);
        memcpy(colors[1], c2, 3);
        memcpy(colors[2], c3, 3);
        memcpy(colors[3], c4, 3);
        memcpy(colors[4], c5, 3);
    }

    // Render star
    template<typename DisplayType>
    void render(DisplayType& display, uint8_t gridWidth, uint8_t gridHeight) {
        // Draw each point of the star
        for (int i = 0; i < numPoints; i++) {
            float angle1 = rotation + (i * 2 * PI / numPoints);
            float angle2 = rotation + ((i + 0.5) * 2 * PI / numPoints);
            float angle3 = rotation + ((i + 1) * 2 * PI / numPoints);

            // Outer point
            float x1 = centerX + outerRadius * cos(angle1);
            float y1 = centerY + outerRadius * sin(angle1);

            // Inner point
            float x2 = centerX + innerRadius * cos(angle2);
            float y2 = centerY + innerRadius * sin(angle2);

            // Next outer point
            float x3 = centerX + outerRadius * cos(angle3);
            float y3 = centerY + outerRadius * sin(angle3);

            // Get color for this point
            uint8_t* color = colors[multicolor ? i % 5 : 0];

            // Draw lines forming the point
            drawLine(display, centerX, centerY, x1, y1, color);
            drawLine(display, x1, y1, x2, y2, color);
        }
    }

    // Render filled star
    template<typename DisplayType>
    void renderFilled(DisplayType& display, uint8_t gridWidth, uint8_t gridHeight) {
        for (int x = 0; x < gridWidth; x++) {
            for (int y = 0; y < gridHeight; y++) {
                if (isInsideStar(x, y)) {
                    // Find which point segment this is closest to for multicolor
                    float angle = atan2(y - centerY, x - centerX) - rotation;
                    if (angle < 0) angle += 2 * PI;
                    int pointIndex = (int)(angle / (2 * PI / numPoints)) % 5;

                    uint8_t* color = colors[multicolor ? pointIndex : 0];
                    display.setPixelOnStrip(y, x, color[0], color[1], color[2]);
                }
            }
        }
    }

    bool isInsideStar(float px, float py) {
        float dx = px - centerX;
        float dy = py - centerY;
        float dist = sqrt(dx * dx + dy * dy);
        float angle = atan2(dy, dx) - rotation;
        if (angle < 0) angle += 2 * PI;

        float segmentAngle = (2 * PI) / (numPoints * 2);
        float angleInSegment = fmod(angle, segmentAngle * 2);

        if (angleInSegment < segmentAngle) {
            return dist <= outerRadius;
        } else {
            return dist <= innerRadius;
        }
    }

    // Line drawing helper
    template<typename DisplayType>
    void drawLine(DisplayType& display, float x0, float y0, float x1, float y1, uint8_t* color) {
        float dx = abs(x1 - x0);
        float dy = abs(y1 - y0);
        int steps = max(dx, dy);

        for (int i = 0; i <= steps; i++) {
            float t = (float)i / steps;
            int x = x0 + t * (x1 - x0);
            int y = y0 + t * (y1 - y0);
            if (x >= 0 && x < 90 && y >= 0 && y < 11) {
                display.setPixelOnStrip(y, x, color[0], color[1], color[2]);
            }
        }
    }

    // Update animation state
    void update(float rotationSpeed = 0.08) {
        rotation += rotationSpeed;
        if (rotation > 2 * PI) rotation -= 2 * PI;
    }
};

#endif
