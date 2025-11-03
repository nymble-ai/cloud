# Multi-Strip LED Control Guide

## Overview

The LED Simulator now supports controlling multiple LED strips in parallel, both in simulation and on physical hardware. This feature allows you to create complex multi-dimensional LED installations and visualize them accurately before deployment.

## Features

- **Parallel Strip Control**: Control 1-16 LED strips independently or synchronously
- **Flexible GPIO Assignment**: Assign each strip to different GPIO pins on ESP32
- **Visual Spacing Control**: Adjust the spacing between strips in the visualizer
- **Backward Compatible**: Existing single-strip code continues to work
- **2D Coordinate System**: Use (x, y) coordinates where x = pixel position, y = strip index

## Quick Start

### Simulator Mode

1. **Configure Visualizer**:
   - Open the visualizer in your browser
   - Select "Parallel Strips" from the Layout dropdown
   - Set "Number of Strips" (1-16)
   - Adjust "Strip Spacing" for visual clarity
   - Click "Apply Layout"

2. **Write Multi-Strip Code**:
```cpp
#include <LEDDisplaySimulatorMulti.h>

// 90 LEDs per strip, 3 strips total
LEDDisplaySimulatorMulti<90, 3> display;

void setup() {
    display.begin();

    // Set different colors on each strip
    for (uint8_t strip = 0; strip < display.getStripCount(); strip++) {
        for (uint16_t i = 0; i < display.getPixelsPerStrip(); i++) {
            uint8_t hue = (strip * 85); // Different hue per strip
            display.setStripPixel(strip, i, hue, 255, 255);
        }
    }
    display.show();
}
```

### Hardware Mode

1. **Wire Multiple Strips**:
```
ESP32          LED Strip 1
GPIO 5  -----> DIN

ESP32          LED Strip 2
GPIO 18 -----> DIN

ESP32          LED Strip 3
GPIO 19 -----> DIN

All strips share: 5V power and GND
```

2. **Configure Hardware**:
```cpp
#include <LEDDisplayHardwareMulti.h>

// Define GPIO pins for each strip
PinArray<3> pins = {5, 18, 19};

// 90 LEDs per strip, 3 strips, using defined pins
LEDDisplayHardwareMulti<90, 3, PinArray<3>> display(pins);
```

## API Reference

### Multi-Strip Specific Methods

#### `uint8_t getStripCount()`
Returns the number of LED strips.

#### `uint16_t getPixelsPerStrip()`
Returns the number of pixels in each strip.

#### `void setStripPixel(strip, index, r, g, b)`
Sets a specific pixel on a specific strip.
- `strip`: Strip index (0 to getStripCount()-1)
- `index`: Pixel position on the strip (0 to getPixelsPerStrip()-1)
- `r, g, b`: Color values (0-255)

#### `void setStripPixelColor(strip, index, color)`
Sets a pixel using packed color value.

#### `void setAllStrips(index, r, g, b)`
Sets the same pixel position on all strips to the same color.

#### `void setPixel2D(x, y, r, g, b)`
2D coordinate system where:
- `x`: Pixel position along the strip
- `y`: Strip index

### Example: Wave Pattern Across Strips

```cpp
void synchronizedWave() {
    static uint16_t offset = 0;

    for (uint16_t i = 0; i < display.getPixelsPerStrip(); i++) {
        uint8_t hue = (i * 256 / display.getPixelsPerStrip() + offset) % 256;
        uint32_t color = colorWheel(hue);

        // Same pattern on all strips
        display.setAllStripsColor(i, color);
    }

    display.show();
    offset += 2;
}
```

### Example: Different Pattern Per Strip

```cpp
void individualStrips() {
    for (uint8_t strip = 0; strip < display.getStripCount(); strip++) {
        // Each strip gets a different animation
        if (strip == 0) {
            // Strip 0: Solid color
            for (uint16_t i = 0; i < display.getPixelsPerStrip(); i++) {
                display.setStripPixel(0, i, 255, 0, 0); // Red
            }
        } else if (strip == 1) {
            // Strip 1: Chase pattern
            static uint16_t chasePos = 0;
            display.setStripPixel(1, chasePos, 0, 255, 0); // Green
            chasePos = (chasePos + 1) % display.getPixelsPerStrip();
        } else if (strip == 2) {
            // Strip 2: Sparkle
            uint16_t pixel = random(display.getPixelsPerStrip());
            display.setStripPixel(2, pixel, 255, 255, 255); // White
        }
    }

    display.show();
}
```

## Serial Protocol Extension

The serial protocol has been extended to support multiple strips:

### Single Strip Frame (Backward Compatible)
```
[0xFF][COUNT_HIGH][COUNT_LOW][R0][G0][B0]...[Rn][Gn][Bn][0xFE]
```

### Multi-Strip Frame
```
[0xFF][0xFC][STRIP_COUNT][COUNT_HIGH][COUNT_LOW][Strip0_Data][Strip1_Data]...[0xFE]
```

Where:
- `0xFC`: Multi-strip marker
- `STRIP_COUNT`: Number of strips (1-16)
- `COUNT_HIGH/LOW`: LEDs per strip
- Strip data is sent sequentially

## Hardware Considerations

### Power Requirements
- Calculate power per strip: `LEDS_PER_STRIP × 60mA`
- Total power: `NUM_STRIPS × LEDS_PER_STRIP × 60mA`
- Example: 3 strips × 90 LEDs × 60mA = 16.2A max

### GPIO Selection
Recommended ESP32 pins for LED data:
- Good: 2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27
- Avoid: 0, 1 (UART), 6-11 (Flash), 34-39 (input only)

### Wiring Best Practices
1. Keep data wires short (<3 feet)
2. Use separate power injection for each strip if >60 LEDs
3. Ensure common ground between all strips and ESP32
4. Add 1000µF capacitor at each strip's power connection

## Visualizer Configuration

### Layout Settings
- **Layout Type**: Select "Parallel Strips"
- **LED Count**: Sets LEDs per strip
- **Number of Strips**: 1-16 strips
- **Strip Spacing**: Visual gap between strips (10-100 pixels)
- **Orientation**: Horizontal or Vertical arrangement

### Visual Tips
- Increase strip spacing for clearer visualization
- Enable "Show LED Indices" to debug pixel positions
- Use test patterns to verify strip ordering

## Performance Considerations

### Frame Rates (Serial at 115200 baud)

| Config | Single Strip | 3 Strips | 5 Strips |
|--------|-------------|----------|----------|
| 30 LEDs | ~80 FPS | ~40 FPS | ~25 FPS |
| 60 LEDs | ~54 FPS | ~27 FPS | ~16 FPS |
| 90 LEDs | ~37 FPS | ~18 FPS | ~11 FPS |

### Optimization Tips
1. Update only changed pixels when possible
2. Use `setAllStrips()` for synchronized patterns
3. Minimize `show()` calls
4. Consider reducing serial baud rate for stability with many strips

## Migration Guide

### From Single to Multi-Strip

**Before** (Single Strip):
```cpp
LEDDisplaySimulator<90> display;

for (uint16_t i = 0; i < 90; i++) {
    display.setPixel(i, 255, 0, 0);
}
```

**After** (Multi-Strip):
```cpp
LEDDisplaySimulatorMulti<90, 3> display;

// Option 1: Set all strips
for (uint16_t i = 0; i < 90; i++) {
    display.setAllStrips(i, 255, 0, 0);
}

// Option 2: Set individual strips
for (uint8_t s = 0; s < 3; s++) {
    for (uint16_t i = 0; i < 90; i++) {
        display.setStripPixel(s, i, 255, 0, 0);
    }
}
```

## Complete Example

See `firmware/examples/06_ParallelStrips/` for a complete working example that demonstrates:
- Synchronized wave patterns
- Individual strip control
- Chase effects across multiple strips
- Cross-fading between strips
- 2D coordinate system usage

## Troubleshooting

### Visualizer shows wrong strip count
- Ensure you clicked "Apply Layout" after changing settings
- Check that your firmware is compiled with matching strip count

### Some strips don't light up (hardware)
- Verify GPIO pin assignments in code match wiring
- Check each strip has power and common ground
- Test each strip individually first

### Frame rate too low
- Reduce number of strips or LEDs per strip
- Optimize animation code to update fewer pixels
- Consider using hardware SPI-based LED protocols

### Colors different between strips
- Ensure all strips are same LED type (WS2812B/WS2815)
- Check for voltage drop on longer runs
- Verify color order (GRB vs RGB) matches for all strips