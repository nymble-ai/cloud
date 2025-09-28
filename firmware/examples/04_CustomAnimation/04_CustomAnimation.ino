#include <LEDDisplay.h>

#ifdef HARDWARE_MODE
  #include <LEDDisplayHardware.h>
  LEDDisplayHardware<60, 5> display;
#else
  #include <LEDDisplaySimulator.h>
  LEDDisplaySimulator<60> display;
#endif

void setup() {
  display.begin();
  display.setBrightness(128);
}

void loop() {
  yourCustomAnimation();
}

void yourCustomAnimation() {
  static uint8_t pos = 0;

  display.clear();

  for (uint8_t i = 0; i < 5; i++) {
    uint8_t index = (pos + i) % display.getPixelCount();
    uint8_t brightness = 255 - (i * 50);
    display.setPixel(index, brightness, 0, brightness / 2);
  }

  display.show();
  delay(50);

  pos++;
  if (pos >= display.getPixelCount()) {
    pos = 0;
  }
}