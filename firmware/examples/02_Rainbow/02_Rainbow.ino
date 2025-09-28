#include <LEDDisplay.h>

#ifdef HARDWARE_MODE
  #include <LEDDisplayHardware.h>
  LEDDisplayHardware<60, 5> display;
#else
  #include <LEDDisplaySimulator.h>
  LEDDisplaySimulator<60> display;
#endif

uint16_t hue = 0;

void setup() {
  display.begin();
  display.setBrightness(128);
}

void loop() {
  rainbowCycle();
  delay(20);
}

void rainbowCycle() {
  for (uint16_t i = 0; i < display.getPixelCount(); i++) {
    uint16_t pixelHue = hue + (i * 65536L / display.getPixelCount());
    uint32_t color = colorWheel((pixelHue >> 8) & 0xFF);
    display.setPixelColor(i, color);
  }
  display.show();
  hue += 256;
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