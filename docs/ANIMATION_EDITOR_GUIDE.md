# Animation Editor Guide

## Overview

The Animation Editor allows you to write LED animations in JavaScript, test them instantly in the browser, and export them as Arduino code for deployment to ESP32 hardware.

## Workflow

```
1. Write JS Animation → 2. Test in Browser → 3. Export to Arduino → 4. Upload to ESP32 → 5. Deploy to LEDs
   (no hardware)         (instant feedback)    (one-click export)      (verify timing)    (production)
```

## Getting Started

### 1. Open the Animation Editor

1. Start the visualizer: `cd simulator/visualizer && python3 -m http.server 8080`
2. Open browser to `http://localhost:8080`
3. Click **"Open Editor"** button in the control panel

### 2. Choose an Example or Write Your Own

**Load an Example:**
- Use the "Load Example" dropdown to select a pre-made animation
- Examples include: Rainbow, Chase, Breathing, Sparkle, Fire, and more

**Write Custom Animation:**
- Write your animation function using the `display` API
- Use the `myAnimation()` function as your main animation loop

### 3. Run Your Animation

1. Click **"▶ Run Animation"** to test your code instantly
2. The animation runs directly in the browser (no ESP32 needed!)
3. Click **"⏹ Stop"** to stop the animation

### 4. Export to Arduino

1. Click **"📤 Export to Arduino"** 
2. Arduino C++ code appears in the right panel
3. Click **"📋 Copy to Clipboard"** to copy the code

### 5. Upload to ESP32

1. Create a new file in `firmware/src/` or use an example folder
2. Paste the Arduino code
3. Build and upload:
   ```bash
   cd firmware
   pio run -e esp32_simulator -t upload
   ```
4. Test in the visualizer with ESP32 connected
5. Switch to hardware mode:
   ```bash
   pio run -e esp32_hardware -t upload
   ```

## JavaScript API Reference

The Animation Editor provides a `display` object that mirrors the Arduino LEDDisplay API:

### Basic Functions

```javascript
// Set individual LED color (RGB values 0-255)
display.setPixel(index, r, g, b);

// Set LED using packed color value
display.setPixelColor(index, color);

// Update the display (required to show changes)
display.show();

// Clear all LEDs
display.clear();

// Fill all LEDs with one color
display.fill(r, g, b);

// Get total LED count
let count = display.getPixelCount();

// Set brightness (0-255)
display.setBrightness(128);

// Get brightness
let brightness = display.getBrightness();

// Get pixel color
let color = display.getPixel(index);
```

### Color Helpers

```javascript
// Create packed color value
let color = display.Color(255, 0, 0); // Red

// Extract color components
let r = display.Red(color);
let g = display.Green(color);
let b = display.Blue(color);

// Generate rainbow colors (0-255 input)
let rainbowColor = colorWheel(hue);
```

### Multi-Strip Support

```javascript
// Get number of strips
let strips = display.getStripCount();

// Get LEDs per strip
let ledsPerStrip = display.getPixelsPerStrip();

// Set pixel on specific strip
display.setStripPixel(stripIndex, pixelIndex, r, g, b);

// Set same pixel on all strips
display.setAllStrips(pixelIndex, r, g, b);
```

## Example Animation Templates

### Simple Color Cycle

```javascript
let hue = 0;

function myAnimation() {
  display.fill(0, 0, 0);
  
  for (let i = 0; i < display.getPixelCount(); i++) {
    let color = colorWheel((hue + i) & 0xFF);
    display.setPixelColor(i, color);
  }
  
  display.show();
  hue = (hue + 2) & 0xFF;
}
```

### Chase Pattern

```javascript
let position = 0;

function myAnimation() {
  display.clear();
  
  for (let i = 0; i < 5; i++) {
    let idx = (position + i) % display.getPixelCount();
    let brightness = 255 - (i * 50);
    display.setPixel(idx, brightness, brightness, brightness);
  }
  
  display.show();
  position = (position + 1) % display.getPixelCount();
}
```

### Matrix Pattern

```javascript
const WIDTH = 16;
const HEIGHT = 16;
let offset = 0;

function myAnimation() {
  for (let y = 0; y < HEIGHT; y++) {
    for (let x = 0; x < WIDTH; x++) {
      let hue = (x + y + offset) & 0xFF;
      let color = colorWheel(hue);
      let index = XY(x, y, WIDTH, HEIGHT);
      display.setPixelColor(index, color);
    }
  }
  
  display.show();
  offset++;
}
```

## JavaScript to Arduino Conversion

### What Gets Converted Automatically

✅ Variable declarations (`let` → typed integers)
✅ Function declarations (`function myAnimation()` → `void myAnimation()`)
✅ Math functions (`Math.sin()` → `sin()`)
✅ Display API calls (identical syntax)
✅ Color functions (identical syntax)
✅ Loop structures (identical syntax)

### What Requires Manual Adjustment

⚠️ **Timer functions** - `setTimeout`/`setInterval` → use `millis()` in Arduino
⚠️ **Array methods** - `.forEach()`, `.map()` → use `for` loops
⚠️ **Console logging** - `console.log()` → `Serial.println()`
⚠️ **Variable types** - Check auto-detected types (uint8_t, uint16_t)

### Conversion Example

**JavaScript:**
```javascript
let counter = 0;

function myAnimation() {
  for (let i = 0; i < display.getPixelCount(); i++) {
    display.setPixel(i, 255, 0, 0);
  }
  display.show();
  counter++;
}
```

**Arduino (Auto-Generated):**
```cpp
uint8_t counter = 0;

void myAnimation() {
  for (uint16_t i = 0; i < display.getPixelCount(); i++) {
    display.setPixel(i, 255, 0, 0);
  }
  display.show();
  counter++;
}
```

## Tips & Best Practices

### JavaScript Development

1. **Use `myAnimation()` function** - The editor expects this function name
2. **Declare globals outside the function** - Define counters, offsets at the top
3. **Call `display.show()`** - Required to update the display
4. **Use bitwise AND for wrapping** - `(value + 1) & 0xFF` instead of modulo
5. **Test with different layouts** - Try strip, matrix, parallel strips

### Performance

1. **Minimize calculations** - Pre-calculate values when possible
2. **Avoid nested loops** - Keep complexity low for smooth animation
3. **Use integer math** - Faster than floating-point on ESP32
4. **Batch updates** - Set all pixels, then call `show()` once

### Debugging

1. **Use editor status** - Shows errors and warnings
2. **Test incrementally** - Start simple, add complexity
3. **Check LED count** - Ensure `display.getPixelCount()` matches layout
4. **Verify timing** - Animations may run faster/slower on ESP32

## Common Patterns

### Timing and Delays

**JavaScript (Editor):**
```javascript
// No delay needed - editor controls timing
function myAnimation() {
  // animation code
  display.show();
}
```

**Arduino (ESP32):**
```cpp
void loop() {
  myAnimation();
  delay(20);  // Control frame rate
}
```

### Random Numbers

**JavaScript:**
```javascript
let random = Math.floor(Math.random() * 256);
```

**Arduino:**
```cpp
uint8_t randomValue = random(256);
```

### Conditional Rendering

```javascript
function myAnimation() {
  for (let i = 0; i < display.getPixelCount(); i++) {
    if (i % 2 === 0) {
      display.setPixel(i, 255, 0, 0);
    } else {
      display.setPixel(i, 0, 0, 255);
    }
  }
  display.show();
}
```

## Troubleshooting

### Animation doesn't run in editor

- **Check for syntax errors** - Look at the error display
- **Verify function name** - Must be `myAnimation()`
- **Check console** - Open browser DevTools for detailed errors

### Exported Arduino code won't compile

- **Check variable types** - Ensure correct integer sizes
- **Remove JS-specific code** - setTimeout, array methods, etc.
- **Verify includes** - Make sure `LEDDisplay.h` is included
- **Check helper functions** - colorWheel, XY functions are generated

### Animation works in JS but not on ESP32

- **Timing differences** - Add appropriate delays in `loop()`
- **Memory constraints** - Reduce array sizes if needed
- **Floating-point** - Replace with integer math for performance
- **Serial interference** - In simulator mode, don't use Serial.println()

### Colors look different

- **Brightness** - Check setBrightness() value
- **Color order** - Hardware uses GRB, but display API handles this
- **Power supply** - Ensure adequate power for hardware

## Advanced Features

### Custom Helper Functions

Add your own helper functions in the JavaScript editor:

```javascript
function fade(color, amount) {
  let r = display.Red(color);
  let g = display.Green(color);
  let b = display.Blue(color);
  
  r = r > amount ? r - amount : 0;
  g = g > amount ? g - amount : 0;
  b = b > amount ? b - amount : 0;
  
  return display.Color(r, g, b);
}

function myAnimation() {
  // Use your custom function
  for (let i = 0; i < display.getPixelCount(); i++) {
    let color = display.getPixel(i);
    let fadedColor = fade(color, 10);
    display.setPixelColor(i, fadedColor);
  }
  display.show();
}
```

These helper functions will be included in the Arduino export.

### State Management

Use closures or global variables for animation state:

```javascript
let state = {
  mode: 0,
  offset: 0,
  brightness: 255
};

function myAnimation() {
  if (state.mode === 0) {
    // Pattern 1
  } else {
    // Pattern 2
  }
  
  state.offset++;
  if (state.offset > 255) {
    state.mode = (state.mode + 1) % 2;
    state.offset = 0;
  }
}
```

## Next Steps

1. **Experiment with examples** - Modify and test different patterns
2. **Create custom animations** - Combine techniques from examples
3. **Test on ESP32** - Verify timing and performance
4. **Deploy to hardware** - Enjoy your LED animations!

## Resources

- [API Reference](API_REFERENCE.md) - Complete API documentation
- [Development Guide](DEVELOPMENT_GUIDE.md) - Animation techniques
- [Serial Protocol](SERIAL_PROTOCOL.md) - Protocol specification
- [Hardware Setup](HARDWARE_SETUP.md) - Wiring and hardware guide

---

**Happy Animating! ✨**

