#ifndef NUMBER_DISPLAY_H
#define NUMBER_DISPLAY_H

#include <Arduino.h>

/*
 * NUMBER DISPLAY COMPONENT
 *
 * Renders numbers 0-9 on the LED grid using pixel patterns
 *
 * IMPROVEMENT LOG:
 * v1.0 - Initial implementation with 5x7 pixel font for digits
 *
 * FEEDBACK NOTES:
 * (Add user feedback here for each number's appearance)
 *
 * Number 0:
 * Number 1:
 * Number 2:
 * Number 3:
 * Number 4:
 * Number 5:
 * Number 6:
 * Number 7:
 * Number 8:
 * Number 9:
 */

class NumberDisplay {
public:
    // 5x7 pixel patterns for each digit (1 = lit, 0 = dark)
    static const uint8_t DIGIT_PATTERNS[10][7];

    // Render multi-digit number (up to 3 digits)
    template<typename DisplayType>
    static void renderMultiDigit(int number, int startX, int startY, uint8_t r, uint8_t g, uint8_t b, DisplayType& display) {
        if (number < 0 || number > 999) return;

        String numStr = String(number);
        int xOffset = 0;

        for (size_t i = 0; i < numStr.length(); i++) {
            int digit = numStr.charAt(i) - '0';
            renderDigit(digit, startX + xOffset, startY, r, g, b, display);
            xOffset += 6; // 5 pixels wide + 1 pixel spacing
        }
    }

    template<typename DisplayType>
    static void renderDigit(int digit, int startX, int startY, uint8_t r, uint8_t g, uint8_t b, DisplayType& display) {
        if (digit < 0 || digit > 9) return;

        for (int row = 0; row < 7; row++) {
            uint8_t pattern = DIGIT_PATTERNS[digit][row];
            for (int col = 0; col < 5; col++) {
                if (pattern & (1 << (4 - col))) {
                    int x = startX + col;
                    int y = startY + row;
                    if (x >= 0 && x < 90 && y >= 0 && y < 11) {
                        display.setPixelOnStrip(y, x, r, g, b);
                    }
                }
            }
        }
    }
};

// 5x7 pixel font definitions for digits 0-9
// Each row is encoded as a 5-bit number (MSB = leftmost pixel)
const uint8_t NumberDisplay::DIGIT_PATTERNS[10][7] = {
    // 0
    {
        0b01110,
        0b10001,
        0b10011,
        0b10101,
        0b11001,
        0b10001,
        0b01110
    },
    // 1
    {
        0b00100,
        0b01100,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b01110
    },
    // 2
    {
        0b01110,
        0b10001,
        0b00001,
        0b00110,
        0b01000,
        0b10000,
        0b11111
    },
    // 3
    {
        0b11111,
        0b00010,
        0b00100,
        0b00010,
        0b00001,
        0b10001,
        0b01110
    },
    // 4
    {
        0b00010,
        0b00110,
        0b01010,
        0b10010,
        0b11111,
        0b00010,
        0b00010
    },
    // 5
    {
        0b11111,
        0b10000,
        0b11110,
        0b00001,
        0b00001,
        0b10001,
        0b01110
    },
    // 6
    {
        0b00110,
        0b01000,
        0b10000,
        0b11110,
        0b10001,
        0b10001,
        0b01110
    },
    // 7
    {
        0b11111,
        0b00001,
        0b00010,
        0b00100,
        0b01000,
        0b01000,
        0b01000
    },
    // 8
    {
        0b01110,
        0b10001,
        0b10001,
        0b01110,
        0b10001,
        0b10001,
        0b01110
    },
    // 9
    {
        0b01110,
        0b10001,
        0b10001,
        0b01111,
        0b00001,
        0b00010,
        0b01100
    }
};

#endif
