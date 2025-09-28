# API Reference

## LEDDisplay Abstract Interface

The `LEDDisplay` class provides a hardware-agnostic interface for controlling addressable LEDs.

### Methods

#### `void begin()`
Initializes the LED display. Must be called in `setup()` before using any other methods.

```cpp
display.begin();
```

#### `void setPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b)`
Sets a single LED to the specified RGB color.

**Parameters:**
- `index`: LED position (0 to LED_COUNT-1)
- `r`: Red value (0-255)
- `g`: Green value (0-255)
- `b`: Blue value (0-255)

```cpp
display.setPixel(0, 255, 0, 0);  // First LED red
```

#### `void setPixelColor(uint16_t index, uint32_t color)`
Sets a single LED using a packed 32-bit color value.

**Parameters:**
- `index`: LED position
- `color`: Packed RGB color (use `Color()` helper)

```cpp
uint32_t red = LEDDisplay::Color(255, 0, 0);
display.setPixelColor(0, red);
```

#### `void show()`
Updates the physical LEDs with the current buffer state. Changes won't be visible until `show()` is called.

```cpp
display.setPixel(0, 255, 0, 0);
display.show();  // Now the LED will light up
```

#### `void clear()`
Sets all LEDs to off (black).

```cpp
display.clear();
display.show();
```

#### `void setBrightness(uint8_t brightness)`
Sets global brightness level.

**Parameters:**
- `brightness`: Brightness value (0-255)

```cpp
display.setBrightness(128);  // 50% brightness
```

#### `uint8_t getBrightness()`
Returns current brightness level (0-255).

```cpp
uint8_t brightness = display.getBrightness();
```

#### `uint16_t getPixelCount()`
Returns the total number of LEDs in the display.

```cpp
for (uint16_t i = 0; i < display.getPixelCount(); i++) {
    display.setPixel(i, 255, 0, 0);
}
```

#### `uint32_t getPixel(uint16_t index)`
Returns the color of the specified LED as a packed 32-bit value.

**Parameters:**
- `index`: LED position

```cpp
uint32_t color = display.getPixel(0);
```

#### `void fill(uint8_t r, uint8_t g, uint8_t b)`
Sets all LEDs to the specified color.

```cpp
display.fill(255, 0, 0);  // All red
display.show();
```

#### `void fillColor(uint32_t color)`
Sets all LEDs using a packed color value.

```cpp
display.fillColor(LEDDisplay::Color(0, 255, 0));  // All green
display.show();
```

### Static Helper Functions

#### `uint32_t Color(uint8_t r, uint8_t g, uint8_t b)`
Creates a packed 32-bit color value from RGB components.

```cpp
uint32_t purple = LEDDisplay::Color(128, 0, 128);
```

#### `uint8_t Red(uint32_t color)`
Extracts red component from packed color.

```cpp
uint32_t color = display.getPixel(0);
uint8_t red = LEDDisplay::Red(color);
```

#### `uint8_t Green(uint32_t color)`
Extracts green component from packed color.

#### `uint8_t Blue(uint32_t color)`
Extracts blue component from packed color.

---

## LEDDisplayHardware

Hardware implementation for physical ESP32 + WS2812B/WS2815 LEDs.

### Template Parameters

```cpp
template <uint16_t NUM_LEDS, uint8_t DATA_PIN = 5>
```

- `NUM_LEDS`: Total number of LEDs
- `DATA_PIN`: GPIO pin number (default: 5)

### Example

```cpp
#include <LEDDisplayHardware.h>

LEDDisplayHardware<60, 5> display;  // 60 LEDs on pin 5

void setup() {
    display.begin();
}
```

### Additional Methods

#### `CRGB* getRawLEDs()`
Returns pointer to internal FastLED CRGB array for advanced FastLED features.

```cpp
CRGB* leds = display.getRawLEDs();
leds[0].fadeToBlackBy(64);  // Use FastLED functions
```

---

## LEDDisplaySimulator

Simulator implementation that sends LED data over serial for visualization.

### Template Parameters

```cpp
template <uint16_t NUM_LEDS>
```

- `NUM_LEDS`: Total number of LEDs to simulate

### Example

```cpp
#include <LEDDisplaySimulator.h>

LEDDisplaySimulator<60> display;  // 60 virtual LEDs

void setup() {
    display.begin();  // Opens serial at 115200 baud
}
```

### Serial Protocol

See [SERIAL_PROTOCOL.md](SERIAL_PROTOCOL.md) for details on the binary protocol used.

---

## Conditional Compilation

Use preprocessor directives to switch between hardware and simulator:

```cpp
#include <LEDDisplay.h>

#ifdef HARDWARE_MODE
  #include <LEDDisplayHardware.h>
  LEDDisplayHardware<60, 5> display;
#else
  #include <LEDDisplaySimulator.h>
  LEDDisplaySimulator<60> display;
#endif

void setup() {
    display.begin();  // Works with both!
}
```

Configure in `platformio.ini`:

```ini
[env:hardware]
build_flags = -DHARDWARE_MODE=1

[env:simulator]
build_flags = -DSIMULATOR_MODE=1
```

---

## Common Patterns

### Color Wheel

```cpp
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
```

### Matrix XY Mapping (Serpentine)

```cpp
const uint8_t MATRIX_WIDTH = 16;
const uint8_t MATRIX_HEIGHT = 16;

uint16_t XY(uint8_t x, uint8_t y) {
    if (y % 2 == 0) {
        return y * MATRIX_WIDTH + x;
    } else {
        return y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
    }
}

display.setPixel(XY(5, 3), 255, 0, 0);
```

### Fade Effect

```cpp
void fadeAll(uint8_t amount) {
    for (uint16_t i = 0; i < display.getPixelCount(); i++) {
        uint32_t color = display.getPixel(i);
        uint8_t r = LEDDisplay::Red(color);
        uint8_t g = LEDDisplay::Green(color);
        uint8_t b = LEDDisplay::Blue(color);

        r = (r > amount) ? r - amount : 0;
        g = (g > amount) ? g - amount : 0;
        b = (b > amount) ? b - amount : 0;

        display.setPixel(i, r, g, b);
    }
}
```

---

## Performance Notes

- `show()` is a blocking operation (hardware implementation)
- For best performance, minimize calls to `show()` - batch updates when possible
- Simulator adds ~1-5ms overhead per frame for serial transmission
- Hardware can achieve 100+ FPS with small LED counts
- Matrix XY calculations should be cached when possible

---

## Troubleshooting

### LEDs not responding (Hardware)
- Check wiring: Data pin, 5V, GND
- Verify correct GPIO pin in template parameter
- Ensure adequate power supply (60mA per LED at full white)
- Check `build_flags` includes `-DHARDWARE_MODE=1`

### No data in visualizer (Simulator)
- Verify serial connection in visualizer
- Check baud rate is 115200
- Ensure `SIMULATOR_MODE=1` build flag is set
- Look for "LED Simulator Ready" message in serial monitor

### Colors are wrong
- Check color order in FastLED (GRB vs RGB vs BGR)
- Verify LED chipset (WS2812B vs WS2815 vs SK6812)
- Some strips have different color orders