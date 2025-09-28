# LED Simulator

> Hardware-accurate LED visualization for Arduino/ESP32 development

Develop and test WS2812B/WS2815 LED animations in a virtual environment that behaves identically to physical hardware. Write your animation code once, test instantly in the browser, then deploy to ESP32 without any changes.

## Features

- ✨ **Same Code, Two Environments** - Write once, run on hardware or simulator
- 🎨 **Real-time Visualization** - See changes instantly in browser
- 🚀 **Fast Development** - No more flash-test-repeat cycles
- 🎯 **Hardware Accurate** - Faithful reproduction of WS2812B/WS2815 behavior
- 🔧 **Multiple Layouts** - Strips, matrices, and rings supported
- 📊 **Performance Metrics** - FPS counter and connection status
- 🌈 **Example Animations** - Rainbow, chase, plasma, and more

## Quick Start

### 1. Run the Visualizer

```bash
cd simulator/visualizer
python3 -m http.server 8080
```

Open browser to `http://localhost:8080`

### 2. Test Without Hardware

Click test buttons (Rainbow, Chase, Random) to see the visualizer in action.

### 3. Upload Example to ESP32

```bash
cd firmware
pio run -e esp32_simulator -t upload
```

### 4. Connect

Click "Connect to Serial" in the visualizer and select your ESP32's port.

Watch your animation come to life! 🎉

## Project Structure

```
led-simulator/
├── docs/                          # Documentation
│   ├── PROJECT_OVERVIEW.md        # Architecture and design
│   ├── API_REFERENCE.md           # Complete API docs
│   ├── HARDWARE_SETUP.md          # Wiring and hardware guide
│   ├── DEVELOPMENT_GUIDE.md       # Animation development
│   └── SERIAL_PROTOCOL.md         # Protocol specification
│
├── firmware/                      # Arduino/ESP32 code
│   ├── lib/
│   │   └── LEDDisplay/            # Abstraction layer
│   ├── examples/                  # Example sketches
│   └── platformio.ini             # Build configuration
│
├── simulator/
│   └── visualizer/                # Web-based LED renderer
│
└── config/                        # Hardware and layout configs
```

## Architecture

The system uses an **abstraction layer** pattern:

```
┌─────────────────────────┐
│   Your Animation Code   │
│  (Same for both modes)  │
└───────────┬─────────────┘
            │
    ┌───────┴────────┐
    │  LEDDisplay    │  ← Abstract interface
    │   (API)        │
    └───────┬────────┘
            │
    ┌───────┴────────────┐
    ▼                    ▼
┌─────────┐        ┌──────────┐
│Hardware │        │Simulator │
│ (ESP32) │        │(Browser) │
└─────────┘        └──────────┘
```

Your animation code calls a unified API. The implementation switches between real hardware (Adafruit NeoPixel) and simulator (serial protocol) based on compile flags.

## Usage Examples

### Basic Animation

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
  // Rainbow animation
  for (uint16_t i = 0; i < display.getPixelCount(); i++) {
    uint8_t hue = (i * 256 / display.getPixelCount()) + millis() / 50;
    uint32_t color = colorWheel(hue);
    display.setPixelColor(i, color);
  }
  display.show();
  delay(20);
}
```

### Build for Simulator

```bash
cd firmware
pio run -e esp32_simulator -t upload
```

### Build for Hardware

```bash
pio run -e esp32_hardware -t upload
```

**Same code. Different targets. Zero changes.**

## Hardware Requirements

- **ESP32** development board
- **WS2812B or WS2815** LED strip/matrix
- **5V power supply** (rated for LED count × 60mA)
- **USB cable** for programming

See [docs/HARDWARE_SETUP.md](docs/HARDWARE_SETUP.md) for detailed wiring instructions.

## Software Requirements

- **PlatformIO** or Arduino IDE
- **Python 3** (for local web server)
- **Chrome/Edge/Opera** browser (for Web Serial API)

## Example Animations

All examples work with both simulator and hardware:

- **01_BasicTest** - Simple color cycling test
- **02_Rainbow** - Classic rainbow wave effect
- **03_Matrix** - 2D plasma effect for matrices
- **04_CustomAnimation** - Template for your own creations

## API Overview

```cpp
// Initialize
display.begin();

// Set individual LED
display.setPixel(index, r, g, b);
display.setPixelColor(index, color);

// Update display
display.show();

// Utilities
display.clear();
display.fill(r, g, b);
display.setBrightness(0-255);
display.getPixelCount();
display.getPixel(index);

// Color helpers
uint32_t c = LEDDisplay::Color(r, g, b);
uint8_t r = LEDDisplay::Red(color);
```

Full documentation: [docs/API_REFERENCE.md](docs/API_REFERENCE.md)

## Development Workflow

1. **Write** animation code using LEDDisplay API
2. **Build** with simulator target
3. **Upload** to ESP32
4. **Visualize** in browser with instant feedback
5. **Iterate** rapidly without hardware wear
6. **Deploy** to hardware when satisfied (same code!)

## Serial Protocol

The simulator uses a simple binary protocol over serial (115200 baud):

```
[FRAME_START][LED_COUNT][R0][G0][B0]...[Rn][Gn][Bn][FRAME_END]
```

Includes byte-stuffing for reliability. Typical frame rates:
- 60 LEDs: ~54 FPS
- 144 LEDs: ~23 FPS
- 256 LEDs: ~13 FPS

See [docs/SERIAL_PROTOCOL.md](docs/SERIAL_PROTOCOL.md) for details.

## Visualizer Features

### Layout Options
- **Strip** - Horizontal or vertical
- **Matrix** - Configurable dimensions, serpentine layout
- **Ring** - Circular arrangement

### Appearance Controls
- LED size and spacing
- Glow intensity and radius
- Background color
- LED index overlay

### Test Mode
Try animations without ESP32:
- Rainbow pattern
- Chase effect
- Random colors

## Configuration

### Hardware Configs (`config/hardware-configs/`)
- `strip-60.json` - 60 LED strip
- `strip-144.json` - 144 LED high-density strip
- `matrix-8x8.json` - 8×8 matrix
- `matrix-16x16.json` - 16×16 matrix

### Build Targets (`platformio.ini`)
- `esp32_hardware` - For physical LEDs
- `esp32_simulator` - For visualization
- `esp32_matrix` - For matrix configurations

## Troubleshooting

### Visualizer won't connect
- Use Chrome, Edge, or Opera (Web Serial API required)
- Check ESP32 is connected via USB
- Verify correct baud rate (115200)
- Look for "LED Simulator Ready" in serial monitor

### LEDs don't light up (hardware)
- Check wiring: Data pin, 5V, GND
- Verify adequate power supply
- Ensure build flag: `-DHARDWARE_MODE=1`
- See [docs/HARDWARE_SETUP.md](docs/HARDWARE_SETUP.md#troubleshooting)

### Colors are wrong
- Check color order (GRB vs RGB) in FastLED config
- Verify LED chipset matches your hardware

## Performance Tips

- Minimize `show()` calls - batch updates when possible
- Use integer math instead of floating-point
- Cache calculations outside the main loop
- Start with low brightness during development

## Contributing

Contributions welcome! Areas of interest:
- Additional animation examples
- New layout types
- Performance optimizations
- Documentation improvements

## Roadmap

- [ ] Multiple LED strip support
- [ ] Animation recording/playback
- [ ] Mobile visualizer app
- [ ] Cloud-based animation sharing
- [ ] Audio-reactive patterns
- [ ] DMX/Art-Net protocol support

## License

MIT License - See LICENSE file for details

## Credits

Built with:
- [FastLED](https://fastled.io/) - LED control library
- [PlatformIO](https://platformio.org/) - Build system
- [Web Serial API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API) - Browser communication

---

## Resources

- **Documentation**: [docs/](docs/)
- **Examples**: [firmware/examples/](firmware/examples/)
- **Configuration**: [config/](config/)

## Support

- 📖 Read the docs in `docs/`
- 💡 Check example sketches in `firmware/examples/`
- 🐛 Report issues on GitHub

---

**Made with ❤️ for the LED art community**

Start creating amazing LED animations today! ✨