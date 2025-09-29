# Multi-Strip LED Display Guide

## Overview

The Multi-Strip LED Display system allows you to control multiple LED strips on different GPIO pins using a single unified interface. This is useful for complex installations where LEDs are distributed across multiple physical strips or need to be controlled by different pins due to power or layout constraints.

## Key Features

- **Multiple GPIO Support**: Control LED strips on any combination of ESP32 GPIO pins
- **Unified Interface**: Single display object manages all strips
- **Strip Identification**: Each strip can have a name for easy identification
- **Flexible Configuration**: Add strips dynamically with different LED counts
- **Hardware & Simulator Support**: Works with both physical LEDs and the simulator

## Architecture

```
┌─────────────────────────┐
│   MultiStripLEDDisplay  │  (Base Class)
│  - Manages strip configs│
│  - Routes pixels to strips
└───────────┬─────────────┘
            │
    ┌───────┴────────┐
    ▼                ▼
┌──────────────┐  ┌──────────────┐
│MultiStripHardware│  │MultiStripSimulator│
│ - NeoPixel strips │  │ - Serial protocol │
└──────────────┘  └──────────────┘
```

## Basic Usage

### 1. Include Headers

```cpp
#include <LEDDisplay.h>

#ifdef HARDWARE_MODE
  #include <MultiStripHardware.h>
  MultiStripHardware display;
#else
  #include <MultiStripSimulator.h>
  MultiStripSimulator display;
#endif
```

### 2. Configure Strips

```cpp
void setup() {
  // Add strips with: pin, LED count, optional name
  display.addStrip(5, 30, "Front Strip");
  display.addStrip(18, 60, "Side Strip");
  display.addStrip(19, 45, "Back Strip");

  // Initialize all strips
  display.begin();
  display.setBrightness(128);
}
```

### 3. Control LEDs

You can control LEDs in three ways:

#### Global Indexing
Access any LED across all strips using a global index:

```cpp
// Global index 0-134 (for 135 total LEDs)
display.setPixel(50, 255, 0, 0);  // Red pixel at global index 50
```

#### Strip-Specific Control
Access LEDs on a specific strip:

```cpp
// Set pixel 10 on strip 1 (Side Strip) to green
display.setPixelOnStrip(1, 10, 0, 255, 0);
```

#### Bulk Operations
Control all LEDs at once:

```cpp
display.fill(255, 255, 255);  // All white
display.clear();               // All off
display.show();                // Update all strips
```

## API Reference

### Configuration Methods

#### `addStrip(pin, numLEDs, name)`
Add a new LED strip configuration.

**Parameters:**
- `pin`: GPIO pin number (uint8_t)
- `numLEDs`: Number of LEDs on this strip (uint16_t)
- `name`: Optional name for the strip (String)

```cpp
display.addStrip(5, 30, "Front");
display.addStrip(18, 60);  // Name is optional
```

#### `getStripCount()`
Returns the number of configured strips.

```cpp
size_t count = display.getStripCount();  // Returns 3
```

#### `getStrip(index)`
Returns configuration for a specific strip.

```cpp
const StripConfig& config = display.getStrip(0);
Serial.println(config.pin);      // GPIO pin
Serial.println(config.numLEDs);  // LED count
Serial.println(config.name);     // Strip name
```

### LED Control Methods

#### `setPixelOnStrip(stripIndex, pixelIndex, r, g, b)`
Set a specific pixel on a specific strip.

```cpp
display.setPixelOnStrip(0, 5, 255, 0, 0);  // Strip 0, pixel 5, red
```

#### `showStrip(stripIndex)`
Update only a specific strip (hardware only).

```cpp
display.showStrip(1);  // Update only strip 1
```

### Global Methods

All standard LEDDisplay methods work across all strips:

- `setPixel(index, r, g, b)` - Set pixel by global index
- `getPixelCount()` - Get total LED count across all strips
- `show()` - Update all strips
- `clear()` - Clear all strips
- `fill(r, g, b)` - Fill all strips with color
- `setBrightness(value)` - Set brightness for all strips

## Pin Selection Guidelines

### Recommended ESP32 Pins

**Best pins for LED data:**
- GPIO 2, 4, 5, 12, 13, 14, 15
- GPIO 16, 17, 18, 19, 21, 22, 23
- GPIO 25, 26, 27, 32, 33

**Avoid these pins:**
- GPIO 0, 1 (UART)
- GPIO 6-11 (Flash memory)
- GPIO 34-39 (Input only)

### Power Considerations

When using multiple strips:

1. **Calculate Total Current**
   ```
   Total Current = Strip1_LEDs × 60mA + Strip2_LEDs × 60mA + ...
   ```

2. **Power Distribution**
   - Use separate power injection for each strip if needed
   - Ensure common ground between all strips and ESP32

3. **Example Power Setup**
   ```
   Strip 1 (30 LEDs): 1.8A → 2A supply
   Strip 2 (60 LEDs): 3.6A → 5A supply
   Strip 3 (45 LEDs): 2.7A → 3A supply

   Or single supply: 8.1A total → 10A supply
   ```

## Examples

### Example 1: Different Colors Per Strip

```cpp
void setup() {
  display.addStrip(5, 30);
  display.addStrip(18, 60);
  display.addStrip(19, 45);
  display.begin();
}

void loop() {
  // Red on strip 0
  for (uint16_t i = 0; i < 30; i++) {
    display.setPixelOnStrip(0, i, 255, 0, 0);
  }

  // Green on strip 1
  for (uint16_t i = 0; i < 60; i++) {
    display.setPixelOnStrip(1, i, 0, 255, 0);
  }

  // Blue on strip 2
  for (uint16_t i = 0; i < 45; i++) {
    display.setPixelOnStrip(2, i, 0, 0, 255);
  }

  display.show();
}
```

### Example 2: Sequential Animation

```cpp
void sequentialFade() {
  for (uint8_t strip = 0; strip < display.getStripCount(); strip++) {
    const StripConfig& config = display.getStrip(strip);

    // Fade in this strip
    for (uint8_t brightness = 0; brightness < 255; brightness += 5) {
      for (uint16_t i = 0; i < config.numLEDs; i++) {
        display.setPixelOnStrip(strip, i, brightness, brightness, brightness);
      }
      display.show();
      delay(10);
    }

    // Fade out
    for (int brightness = 250; brightness >= 0; brightness -= 5) {
      for (uint16_t i = 0; i < config.numLEDs; i++) {
        display.setPixelOnStrip(strip, i, brightness, brightness, brightness);
      }
      display.show();
      delay(10);
    }
  }
}
```

### Example 3: Global Pattern

```cpp
void rainbowAcrossAll() {
  static uint16_t hue = 0;

  // Treat all strips as one continuous strip
  for (uint16_t i = 0; i < display.getPixelCount(); i++) {
    uint16_t pixelHue = hue + (i * 65536L / display.getPixelCount());
    uint32_t color = colorWheel((pixelHue >> 8) & 0xFF);
    display.setPixelColor(i, color);
  }

  display.show();
  hue += 256;
}
```

## Serial Protocol Enhancement

The simulator uses an enhanced protocol to support multiple strips:

### Configuration Frame
Sent once at startup to define strip layout:

```
[FRAME_START:0xFF]
[MULTI_STRIP_MARKER:0xFC]
[NUM_STRIPS]
For each strip:
  [PIN_NUMBER]
  [LED_COUNT_HIGH]
  [LED_COUNT_LOW]
  [NAME_LENGTH]
  [NAME_CHARS...]
[FRAME_END:0xFE]
```

### Data Frame
Standard frames with global LED indexing:

```
[FRAME_START:0xFF]
[TOTAL_COUNT_HIGH]
[TOTAL_COUNT_LOW]
[R0][G0][B0]...[Rn][Gn][Bn]
[FRAME_END:0xFE]
```

## Visualizer Support

The web-based visualizer will:
- Display each strip separately with labels
- Show GPIO pin assignments
- Optionally arrange strips in custom layouts
- Display strip names and LED counts

## Migration from Single Strip

To migrate existing code to multi-strip:

### Before (Single Strip)
```cpp
LEDDisplayHardware<90, 5> display;

void setup() {
  display.begin();
  display.setPixel(45, 255, 0, 0);
  display.show();
}
```

### After (Multi-Strip)
```cpp
MultiStripHardware display;

void setup() {
  display.addStrip(5, 90, "Main");  // Same 90 LEDs on pin 5
  display.begin();
  display.setPixel(45, 255, 0, 0);  // Works the same!
  display.show();
}
```

## Troubleshooting

### Common Issues

1. **LEDs not responding on some strips**
   - Verify GPIO pin is correct and not reserved
   - Check each strip has proper power
   - Ensure common ground between all strips

2. **Incorrect LED indexing**
   - Remember global indexing starts at 0
   - Use `getStrip()` to verify strip configurations
   - Print `startIndex` for each strip to debug

3. **Performance issues**
   - Minimize `show()` calls - update all strips at once
   - Use `showStrip()` only when necessary (hardware only)
   - Consider reducing brightness for large installations

4. **Memory issues**
   - Each NeoPixel uses ~3 bytes RAM per LED
   - ESP32 has limited RAM, consider total LED count
   - Maximum practical limit: ~2000 LEDs total

## Advanced Usage

### Dynamic Strip Management

```cpp
// Add strips based on configuration
void configureFromSettings() {
  int numStrips = readSetting("strip_count");

  for (int i = 0; i < numStrips; i++) {
    uint8_t pin = readSetting("strip_" + i + "_pin");
    uint16_t leds = readSetting("strip_" + i + "_leds");
    String name = readSetting("strip_" + i + "_name");

    display.addStrip(pin, leds, name);
  }

  display.begin();
}
```

### Custom Strip Patterns

```cpp
// Apply different animation to each strip
void customPattern() {
  for (size_t s = 0; s < display.getStripCount(); s++) {
    switch(s) {
      case 0:
        applyRainbow(s);
        break;
      case 1:
        applySparkle(s);
        break;
      case 2:
        applyChase(s);
        break;
    }
  }
  display.show();
}
```

## Limitations

- Maximum strips: Limited by available GPIO pins and memory
- Maximum LEDs per strip: 1000 (practical limit)
- Total maximum LEDs: ~2000 (ESP32 memory constraint)
- All strips must use same LED type (WS2812B/WS2815)

## Future Enhancements

- Support for different LED types per strip
- Parallel update for better performance
- Zone-based control within strips
- Save/load strip configurations
- Web UI for strip configuration