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
  delay(1000);
}

void loop() {
  display.clear();
  display.show();
  delay(500);

  display.setPixel(0, 255, 0, 0);
  display.show();
  delay(500);

  display.setPixel(1, 0, 255, 0);
  display.show();
  delay(500);

  display.setPixel(2, 0, 0, 255);
  display.show();
  delay(500);

  display.fill(255, 255, 255);
  display.show();
  delay(500);
}