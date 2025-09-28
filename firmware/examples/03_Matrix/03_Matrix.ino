#include <LEDDisplay.h>

#ifdef HARDWARE_MODE
  #include <LEDDisplayHardware.h>
  LEDDisplayHardware<256, 5> display;
#else
  #include <LEDDisplaySimulator.h>
  LEDDisplaySimulator<256> display;
#endif

const uint8_t MATRIX_WIDTH = 16;
const uint8_t MATRIX_HEIGHT = 16;
const bool SERPENTINE = true;

void setup() {
  display.begin();
  display.setBrightness(64);
}

void loop() {
  plasma();
  delay(50);
}

uint16_t XY(uint8_t x, uint8_t y) {
  uint16_t index;

  if (SERPENTINE) {
    if (y % 2 == 0) {
      index = y * MATRIX_WIDTH + x;
    } else {
      index = y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
    }
  } else {
    index = y * MATRIX_WIDTH + x;
  }

  return index;
}

void plasma() {
  static uint8_t offset = 0;

  for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
      float v1 = sin((x + offset) * 0.3);
      float v2 = sin((y + offset) * 0.3);
      float v3 = sin((x + y + offset) * 0.2);
      float v4 = sin(sqrt((x - 8) * (x - 8) + (y - 8) * (y - 8)) * 0.5 + offset * 0.5);

      float value = (v1 + v2 + v3 + v4) / 4.0;

      uint8_t hue = (uint8_t)((value + 1.0) * 128 + offset);
      uint32_t color = colorWheel(hue);

      display.setPixelColor(XY(x, y), color);
    }
  }

  display.show();
  offset++;
}

uint32_t colorWheel(uint8_t wheelPos) {
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