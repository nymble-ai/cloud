# Development Guide

## Getting Started

This guide walks through developing LED animations using the LED Simulator system, from basic concepts to advanced techniques.

## Workflow Overview

```
1. Write animation code
     ↓
2. Test in simulator (instant feedback)
     ↓
3. Iterate and refine
     ↓
4. Deploy to hardware (same code!)
```

## Development Environment Setup

### Prerequisites

1. **PlatformIO** (recommended) or Arduino IDE
2. **Python 3** (for running local visualizer server)
3. **Chrome/Edge/Opera** browser (for Web Serial API)
4. **ESP32** board definitions installed

### Project Setup

```bash
# Clone/navigate to project
cd led-simulator

# Install npm dependencies (optional)
npm install

# Open firmware in PlatformIO
code firmware/
```

## Writing Your First Animation

### Step 1: Create a New Sketch

Create `firmware/examples/05_MyAnimation/05_MyAnimation.ino`:

```cpp
#include <LEDDisplay.h>

#ifdef HARDWARE_MODE
  #include <LEDDisplayHardware.h>
  LEDDisplayHardware<90, 5> display;
#else
  #include <LEDDisplaySimulator.h>
  LEDDisplaySimulator<90> display;
#endif

void setup() {
  display.begin();
  display.setBrightness(128);
}

void loop() {
  // Your animation code here
  myAnimation();
}

void myAnimation() {
  // TODO: Implement your idea
}
```

### Step 2: Compile for Simulator

In `platformio.ini`, ensure simulator environment exists:

```ini
[env:esp32_simulator]
platform = espressif32
board = esp32dev
framework = arduino
build_flags = -DSIMULATOR_MODE=1
```

Build:
```bash
cd firmware
pio run -e esp32_simulator
```

### Step 3: Upload and Run

```bash
# Upload to ESP32
pio run -e esp32_simulator -t upload

# Start visualizer
cd ../simulator/visualizer
python3 -m http.server 8080
```

Open browser to `http://localhost:8080` and click "Connect to Serial"

### Step 4: Iterate

Make changes → save → upload → instantly see results!

## Animation Patterns

### Pattern 1: Static Color

```cpp
void setup() {
  display.begin();
  display.fill(255, 100, 0);  // Orange
  display.show();
}

void loop() {
  // Nothing needed for static display
}
```

### Pattern 2: Chase Effect

```cpp
uint8_t position = 0;

void loop() {
  display.clear();

  for (uint8_t i = 0; i < 5; i++) {
    uint8_t idx = (position + i) % display.getPixelCount();
    display.setPixel(idx, 0, 0, 255);
  }

  display.show();
  delay(50);

  position++;
  if (position >= display.getPixelCount()) {
    position = 0;
  }
}
```

### Pattern 3: Breathing Effect

```cpp
void loop() {
  static uint8_t brightness = 0;
  static int8_t direction = 1;

  display.fill(255, 0, 0);
  display.setBrightness(brightness);
  display.show();

  brightness += direction * 5;

  if (brightness >= 250) direction = -1;
  if (brightness <= 5) direction = 1;

  delay(30);
}
```

### Pattern 4: Rainbow Wave

```cpp
uint16_t hue = 0;

void loop() {
  for (uint16_t i = 0; i < display.getPixelCount(); i++) {
    uint16_t pixelHue = hue + (i * 65536L / display.getPixelCount());
    uint32_t color = colorWheel((pixelHue >> 8) & 0xFF);
    display.setPixelColor(i, color);
  }

  display.show();
  hue += 256;
  delay(20);
}

uint32_t colorWheel(uint8_t pos) {
  pos = 255 - pos;
  if (pos < 85) {
    return LEDDisplay::Color(255 - pos * 3, 0, pos * 3);
  }
  if (pos < 170) {
    pos -= 85;
    return LEDDisplay::Color(0, pos * 3, 255 - pos * 3);
  }
  pos -= 170;
  return LEDDisplay::Color(pos * 3, 255 - pos * 3, 0);
}
```

### Pattern 5: Sparkle

```cpp
void loop() {
  // Fade all LEDs
  for (uint16_t i = 0; i < display.getPixelCount(); i++) {
    uint32_t c = display.getPixel(i);
    uint8_t r = LEDDisplay::Red(c);
    uint8_t g = LEDDisplay::Green(c);
    uint8_t b = LEDDisplay::Blue(c);

    r = r > 10 ? r - 10 : 0;
    g = g > 10 ? g - 10 : 0;
    b = b > 10 ? b - 10 : 0;

    display.setPixel(i, r, g, b);
  }

  // Add new sparkles
  if (random(10) < 3) {
    uint16_t pos = random(display.getPixelCount());
    display.setPixel(pos, 255, 255, 255);
  }

  display.show();
  delay(50);
}
```

## Matrix Patterns

For 2D matrices, use an XY() helper function:

```cpp
const uint8_t WIDTH = 16;
const uint8_t HEIGHT = 16;
const bool SERPENTINE = true;

uint16_t XY(uint8_t x, uint8_t y) {
  if (y >= HEIGHT) return 0;
  if (x >= WIDTH) return 0;

  if (SERPENTINE && y % 2 == 1) {
    return y * WIDTH + (WIDTH - 1 - x);
  }
  return y * WIDTH + x;
}
```

### Pattern: Bouncing Ball

```cpp
float ballX = 8.0;
float ballY = 8.0;
float velX = 0.5;
float velY = 0.3;

void loop() {
  display.clear();

  // Update position
  ballX += velX;
  ballY += velY;

  // Bounce off walls
  if (ballX <= 0 || ballX >= WIDTH - 1) velX = -velX;
  if (ballY <= 0 || ballY >= HEIGHT - 1) velY = -velY;

  // Draw ball (3x3)
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      int x = (int)ballX + dx;
      int y = (int)ballY + dy;
      if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        display.setPixel(XY(x, y), 255, 0, 0);
      }
    }
  }

  display.show();
  delay(50);
}
```

## Performance Optimization

### Minimize show() Calls

**Bad:**
```cpp
for (int i = 0; i < 60; i++) {
  display.setPixel(i, 255, 0, 0);
  display.show();  // 60 calls = slow!
  delay(10);
}
```

**Good:**
```cpp
for (int i = 0; i < 60; i++) {
  display.setPixel(i, 255, 0, 0);
}
display.show();  // 1 call = fast!
```

### Use Fixed-Point Math

```cpp
// Integer math is faster than float
uint16_t position = 0;  // Position in 256ths
const uint16_t speed = 10;

void loop() {
  position += speed;
  uint8_t ledIndex = position >> 8;  // Divide by 256
  display.setPixel(ledIndex, 255, 0, 0);
  display.show();
}
```

### Cache Calculations

```cpp
// Calculate once, reuse many times
uint32_t palette[256];

void setup() {
  for (int i = 0; i < 256; i++) {
    palette[i] = colorWheel(i);
  }
}

void loop() {
  for (int i = 0; i < display.getPixelCount(); i++) {
    uint8_t index = (i + offset) % 256;
    display.setPixelColor(i, palette[index]);  // Fast lookup
  }
  display.show();
}
```

## Debugging Tips

### Use Serial.println()

```cpp
void loop() {
  Serial.print("FPS: ");
  Serial.println(fps);

  myAnimation();
}
```

### Visualizer Test Mode

Use test buttons to verify layouts before connecting hardware:
- "Rainbow" - Shows color order and LED indices
- "Chase" - Verifies LED sequence
- "Random" - Tests refresh rate

### LED Index Overlay

Enable "Show LED Indices" in visualizer to see LED numbering

### Check LED Count

```cpp
void setup() {
  display.begin();
  Serial.print("LED Count: ");
  Serial.println(display.getPixelCount());
}
```

## Switching Between Simulator and Hardware

### Option 1: Build Flags (Recommended)

**platformio.ini:**
```ini
[env:esp32_hardware]
build_flags = -DHARDWARE_MODE=1

[env:esp32_simulator]
build_flags = -DSIMULATOR_MODE=1
```

**Sketch:**
```cpp
#ifdef HARDWARE_MODE
  #include <LEDDisplayHardware.h>
  LEDDisplayHardware<90, 5> display;
#else
  #include <LEDDisplaySimulator.h>
  LEDDisplaySimulator<90> display;
#endif
```

**Build:**
```bash
pio run -e esp32_simulator  # For testing
pio run -e esp32_hardware   # For deployment
```

### Option 2: Manual Switch

Comment/uncomment as needed:
```cpp
//#define USE_HARDWARE

#ifdef USE_HARDWARE
  #include <LEDDisplayHardware.h>
  LEDDisplayHardware<90, 5> display;
#else
  #include <LEDDisplaySimulator.h>
  LEDDisplaySimulator<90> display;
#endif
```

## Common Mistakes

### Forgetting to call show()
```cpp
display.setPixel(0, 255, 0, 0);
// Missing display.show() - nothing will happen!
```

### Index out of bounds
```cpp
display.setPixel(100, 255, 0, 0);  // If only 60 LEDs, this is ignored
```

### Wrong loop timing
```cpp
void loop() {
  display.fill(255, 0, 0);
  display.show();
  // No delay - will refresh as fast as possible, may flicker
}
```

### Power considerations ignored
```cpp
display.setBrightness(255);  // Full brightness
display.fill(255, 255, 255); // All white
// This draws maximum current - may exceed power supply!
```

## Best Practices

1. **Start with low brightness** during development
2. **Use delays appropriately** (20-50ms typical for animations)
3. **Test with small LED counts** first, then scale up
4. **Version control your animations** using git
5. **Document your code** - future you will thank you
6. **Profile performance** - aim for >30 FPS
7. **Consider power limits** when designing animations

## Next Steps

- Explore example animations in `firmware/examples/`
- Check [API_REFERENCE.md](API_REFERENCE.md) for all available methods
- Join the community to share your creations
- Build something awesome!