# LED Simulator Project

## Overview

This project creates a hardware-accurate LED simulation environment for Arduino-based LED installations. The goal is to develop and test LED animations in a virtual environment that behaves identically to physical hardware, enabling rapid iteration without requiring physical LED strips or matrices connected.

## Problem Statement

When developing LED animations for ESP32 microcontrollers with WS2812B/WS2815 LED strips, the traditional workflow requires:
- Physical hardware setup
- Flashing firmware to test each change
- Physical access to the installation
- Time-consuming debugging cycles

This project solves these problems by creating a simulator that runs the same animation code in a visual environment, allowing developers to see exactly what the hardware would display without deploying to physical devices.

## Hardware Target Specifications

### Microcontroller
- **Platform**: ESP32
- **Framework**: Arduino
- **Communication**: Serial/WebSocket (for simulator bridge)

### LED Hardware
- **Supported Models**: WS2812B, WS2815
- **Protocol**: Addressable RGB LEDs (one-wire control)
- **Common Libraries**: FastLED, Adafruit NeoPixel
- **Configurations**: LED strips and matrices of various sizes

## Architecture

### Core Principle: Abstraction Layer

The system uses an abstraction layer pattern to decouple animation logic from hardware implementation. This allows the same animation code to run in two environments:

1. **Hardware Environment**: Real ESP32 controlling physical LEDs
2. **Simulation Environment**: Virtual visualization on a computer screen

### System Components

```
┌─────────────────────────────────────┐
│     Animation Code (User Logic)    │
│  - Pattern generators               │
│  - Color calculations               │
│  - Timing and sequencing            │
└──────────────┬──────────────────────┘
               │
               ▼
┌─────────────────────────────────────┐
│      Abstraction Layer (API)        │
│  - setPixel(index, r, g, b)         │
│  - show()                           │
│  - clear()                          │
│  - getPixelCount()                  │
└──────────────┬──────────────────────┘
               │
       ┌───────┴────────┐
       ▼                ▼
┌─────────────┐  ┌─────────────┐
│  Hardware   │  │  Simulator  │
│  (ESP32)    │  │  (Virtual)  │
└─────────────┘  └─────────────┘
```

### Hardware Implementation

```cpp
class LEDDisplay {
private:
  CRGB* leds;
  int count;

public:
  void setPixel(int index, uint8_t r, uint8_t g, uint8_t b);
  void show();    // Calls FastLED.show()
  void clear();
  int getPixelCount();
};
```

The hardware implementation wraps existing LED libraries (FastLED/NeoPixel) and communicates directly with GPIO pins.

### Simulator Implementation

The simulator implements the same API but:
- Maintains LED state in memory
- Sends state updates to a visualizer (web-based or desktop)
- Renders LEDs as visual elements on screen

Communication options:
1. **Serial Protocol**: Simulator sends LED state over serial/USB
2. **WebSocket**: Real-time updates to web-based visualizer
3. **Direct Rendering**: If simulator and visualizer are same process

## Project Structure (Proposed)

```
led-simulator/
├── docs/
│   ├── PROJECT_OVERVIEW.md          (this file)
│   ├── API_REFERENCE.md             (abstraction layer API)
│   ├── HARDWARE_SETUP.md            (ESP32 wiring, setup)
│   └── DEVELOPMENT_GUIDE.md         (how to create animations)
├── firmware/
│   ├── lib/
│   │   ├── LEDDisplay/              (abstraction layer)
│   │   │   ├── LEDDisplay.h
│   │   │   ├── HardwareImpl.cpp     (FastLED wrapper)
│   │   │   └── SimulatorImpl.cpp    (serial/websocket bridge)
│   │   └── Animations/              (reusable animation patterns)
│   ├── examples/
│   │   ├── basic_test/
│   │   ├── rainbow/
│   │   └── matrix_patterns/
│   └── platformio.ini               (or Arduino project structure)
├── simulator/
│   ├── visualizer/                  (web-based or desktop app)
│   │   ├── index.html
│   │   ├── renderer.js              (canvas/WebGL rendering)
│   │   └── serial-bridge.js         (communication layer)
│   └── bridge/                      (optional: serial-to-websocket)
└── README.md
```

## Technical Approach

### Phase 1: Abstraction Layer
Create the core abstraction API that both hardware and simulator implement.

**Key Design Goals:**
- API must be simple and intuitive
- Must support both strips (1D) and matrices (2D)
- Must handle common operations: set color, show, clear, brightness
- Must be performant (minimize overhead on ESP32)

### Phase 2: Hardware Implementation
Implement the abstraction layer for real hardware using FastLED.

**Requirements:**
- Support WS2812B and WS2815 configurations
- Maintain compatibility with existing FastLED features
- Minimal performance overhead
- Easy configuration (LED count, pin assignments)

### Phase 3: Simulator Bridge
Create communication layer between Arduino code and visualizer.

**Protocol Options:**
- **Serial (ASCII)**: Simple, human-readable, good for debugging
- **Serial (Binary)**: More efficient for large LED counts
- **WebSocket**: Best for web-based visualizer, allows remote control

**Example Protocol (Binary):**
```
[START_BYTE][LED_COUNT_H][LED_COUNT_L][R0][G0][B0][R1][G1][B1]...[END_BYTE]
```

### Phase 4: Visualizer
Build the visual representation of LEDs.

**Technology Options:**
- **HTML Canvas**: Simple, widely compatible
- **WebGL**: Better performance for large installations
- **Desktop (Python/Processing)**: Native performance, no browser needed

**Features:**
- Configurable layout (strip vs matrix, dimensions)
- Realistic LED rendering (glow, brightness)
- Performance metrics (FPS, update rate)
- Configuration UI (select layout, connection settings)

### Phase 5: Example Animations
Create a library of example animations demonstrating the system.

**Examples:**
- Solid colors and fades
- Rainbow patterns
- Perlin noise / plasma effects
- Reactive patterns (audio, sensor data)
- Matrix-specific: text scrolling, sprites

## Development Workflow

### For Animation Development

1. Write animation code using abstraction layer
2. Compile with simulator implementation
3. Run visualizer and iterate quickly
4. When satisfied, recompile with hardware implementation
5. Flash to ESP32 - animation runs identically

### For Hardware Testing

1. Use simulator to verify basic functionality
2. Deploy to physical hardware for real-world testing
3. Fine-tune timing, brightness, power considerations
4. Can use simulator alongside hardware for comparison

## Benefits

- **Rapid Iteration**: Test animations instantly without hardware
- **Remote Development**: Work on animations from anywhere
- **Debugging**: Easier to inspect state and timing in simulator
- **Collaboration**: Share animations as code, others can visualize
- **Education**: Learn LED programming without buying hardware
- **Portability**: Same animation code works across different installations

## Future Enhancements

- **Multiple LED Strips**: Simulate complex installations with multiple controllers
- **Real-time Sync**: Sync simulator with physical hardware for comparison
- **Recording**: Capture animations as video
- **Physics Simulation**: Model power consumption, heat, brightness limitations
- **Cloud Integration**: Upload animations, share with community
- **Mobile App**: Visualizer on phone/tablet

## Technical Constraints

- **Timing Accuracy**: Simulator may not perfectly match hardware timing due to OS scheduling
- **Color Accuracy**: Screen colors vs LED colors differ (can add calibration)
- **Performance**: Large installations (>1000 LEDs) may need optimization
- **Platform Differences**: ESP32 vs simulator may have floating-point precision differences

## Getting Started

(To be written after implementation)

1. Clone repository
2. Install dependencies (PlatformIO, Node.js, etc.)
3. Run example animation in simulator
4. Modify example and see changes in real-time
5. Deploy to hardware

## Contributing

(To be defined)

## License

(To be determined)

---

**Document Version**: 1.0
**Last Updated**: 2025-09-28
**Authors**: Initial design and architecture