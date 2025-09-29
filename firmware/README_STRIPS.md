# LED Strip Configuration System

## Quick Start Guide

This system allows you to easily add, remove, and modify LED strips with different effects and colors. Each strip can have its own GPIO pin, LED count, effect, and color.

## 📚 How to Use

### 1. Simple Setup (QuickStart Template)

The easiest way to start is using the QuickStart template:

```cpp
// In examples/QuickStart/QuickStart.ino

void setup() {
  // Just add your strips here:
  config.addRainbowStrip(5, 30, "Front");
  config.addChaseStrip(18, 60, Colors::BLUE, "Side");
  config.addFireStrip(19, 45, "Back");

  display.begin();
}
```

### 2. Available Effects

Each strip can have one of these effects:

| Effect | Function | Parameters |
|--------|----------|------------|
| **Rainbow** | `addRainbowStrip(pin, leds, "name")` | Animated rainbow |
| **Solid Color** | `addSolidStrip(pin, leds, color, "name")` | Static single color |
| **Chase** | `addChaseStrip(pin, leds, color, "name")` | Moving dots with tail |
| **Breathing** | `addBreathingStrip(pin, leds, color, "name")` | Fading in/out |
| **Sparkle** | `addSparkleStrip(pin, leds, color, "name")` | Random twinkling |
| **Fire** | `addFireStrip(pin, leds, "name")` | Flame simulation |
| **Wave** | `addWaveStrip(pin, leds, color, "name")` | Sine wave brightness |
| **Gradient** | `addGradientStrip(pin, leds, color1, color2, "name")` | Color blend |
| **Strobe** | `addStrobeStrip(pin, leds, color, "name")` | Flashing effect |

### 3. Available Colors

Use these predefined colors:

```cpp
Colors::RED        // 0xFF0000
Colors::GREEN      // 0x00FF00
Colors::BLUE       // 0x0000FF
Colors::WHITE      // 0xFFFFFF
Colors::YELLOW     // 0xFFFF00
Colors::CYAN       // 0x00FFFF
Colors::MAGENTA    // 0xFF00FF
Colors::ORANGE     // 0xFF8000
Colors::PURPLE     // 0x8000FF
Colors::PINK       // 0xFF1493
Colors::WARM_WHITE // 0xFFD700
Colors::COOL_WHITE // 0xE0FFFF
```

Or use any hex color: `0xRRGGBB`

### 4. GPIO Pin Selection

Recommended ESP32 pins for LEDs:
- **Best**: 5, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33
- **OK**: 2, 4, 12, 13, 14, 15, 16, 17
- **Avoid**: 0, 1 (UART), 6-11 (Flash), 34-39 (input only)

## 🎨 Example Configurations

### Example 1: Party Mode
```cpp
config.addRainbowStrip(5, 30, "Front Rainbow");
config.addChaseStrip(18, 60, Colors::MAGENTA, "Side Chase");
config.addSparkleStrip(19, 45, Colors::CYAN, "Back Sparkle");
config.addStrobeStrip(21, 30, Colors::WHITE, "Strobe");
```

### Example 2: Ambient Mode
```cpp
config.addBreathingStrip(5, 30, Colors::WARM_WHITE, "Breathing");
config.addWaveStrip(18, 60, Colors::BLUE, "Ocean Wave");
config.addGradientStrip(19, 45, Colors::PURPLE, Colors::PINK, "Sunset");
```

### Example 3: Christmas Theme
```cpp
config.addChaseStrip(5, 30, Colors::RED, "Red Chase");
config.addChaseStrip(18, 60, Colors::GREEN, "Green Chase");
config.addSparkleStrip(19, 45, Colors::WHITE, "Snow");
```

### Example 4: Fire Display
```cpp
config.addFireStrip(5, 30, "Fire Left");
config.addFireStrip(18, 60, "Fire Center");
config.addFireStrip(19, 45, "Fire Right");
```

## 🔧 Advanced Usage

### Dynamic Strip Management

```cpp
// Add a strip at runtime
uint8_t stripId = stripManager.addStrip(22, 50, "New Strip");
stripManager.setEffect(stripId, Effects::rainbow(5));

// Remove a strip
stripManager.removeStrip(stripId);

// Enable/disable strips
stripManager.enableStrip(0, false);  // Disable first strip
stripManager.enableStrip(0, true);   // Re-enable it

// Adjust brightness per strip
stripManager.setStripBrightness(0, 128);  // 50% brightness
```

### Custom Effects

Create your own effect:

```cpp
// Custom effect function
EffectFunction myCustomEffect = [](uint8_t stripIndex, uint32_t currentMillis) {
  StripManager* mgr = static_cast<StripManager*>(this);
  ManagedStrip& strip = mgr->getStrip(stripIndex);

  // Your custom animation logic here
  for (uint16_t i = 0; i < strip.numLEDs; i++) {
    // Example: alternating red/blue
    if (i % 2 == 0) {
      mgr->setPixelOnStrip(stripIndex, i, 255, 0, 0);
    } else {
      mgr->setPixelOnStrip(stripIndex, i, 0, 0, 255);
    }
  }
};

// Apply custom effect
stripManager.setEffect(0, myCustomEffect);
```

### Preset Configurations

Use built-in presets:

```cpp
config.setupPartyMode();     // Colorful party effects
config.setupAmbientMode();   // Calm, relaxing
config.setupChristmasMode(); // Holiday themed
config.setupFireMode();      // All fire effects
```

## 📡 How It Works

1. **StripManager**: Manages multiple LED strips dynamically
2. **Effects Library**: Provides reusable animation effects
3. **StripConfig**: Easy configuration builder
4. **Multi-GPIO Support**: Each strip on its own pin
5. **Simulator Compatible**: Works with web visualizer

## 🚀 Compiling & Uploading

```bash
# For simulator (to test in browser)
pio run -e esp32_simulator -t upload

# For hardware (physical LEDs)
pio run -e esp32_hardware -t upload
```

## 💡 Tips

1. **Start Simple**: Use QuickStart.ino as a template
2. **Test First**: Use simulator mode before hardware
3. **Power Budget**: Each LED uses ~60mA at full white
4. **Common Ground**: Connect ESP32 GND to LED power GND
5. **Use Resistors**: 470Ω on data lines recommended

## 🎮 Interactive Control

The DynamicStrips example supports serial commands:
- `1` - Switch to Party Mode
- `2` - Switch to Ambient Mode
- `3` - Switch to Christmas Mode
- `4` - Switch to Fire Mode
- `a` - Add random strip
- `r` - Remove last strip
- `c` - Clear all strips
- `b` - Adjust brightness

## 📊 Memory Usage

- Each strip: ~50 bytes base
- Each LED: ~3 bytes
- Effect state: ~20 bytes per strip
- Maximum practical: ~10 strips, 1000 total LEDs

## 🐛 Troubleshooting

**No LEDs lighting:**
- Check GPIO pin assignments
- Verify common ground
- Ensure display.begin() is called

**Wrong colors:**
- Check LED type (WS2812B vs WS2815)
- Verify color format (RGB vs GRB)

**Effects not working:**
- Call stripManager.update() in loop()
- Check effect assignment
- Verify strip is enabled

**Performance issues:**
- Reduce total LED count
- Increase loop() delay
- Use simpler effects

## 📝 Example File Structure

```
firmware/
├── lib/
│   ├── StripManager/     # Dynamic strip management
│   ├── Effects/          # Animation effects library
│   └── StripConfig/      # Configuration helpers
├── examples/
│   ├── QuickStart/       # Simple template
│   └── DynamicStrips/    # Advanced example
└── src/
    └── main.cpp          # Your code here
```

## Happy LED Programming! 🌈✨