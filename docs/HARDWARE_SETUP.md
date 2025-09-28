# Hardware Setup Guide

## Required Components

### Microcontroller
- **ESP32 Development Board**
  - Any ESP32 variant (ESP32-WROOM, ESP32-WROVER, etc.)
  - Recommended: ESP32 DevKit v1 or NodeMCU-32S
  - USB cable for programming and power

### LED Hardware
- **WS2812B or WS2815 LED Strip/Matrix**
  - Individual addressable RGB LEDs
  - Available in strips (30, 60, 144 LEDs/meter) or matrices
  - Check voltage: Most are 5V

### Power Supply
- **5V DC Power Supply**
  - Current rating depends on LED count
  - Calculate: LED_COUNT × 60mA (for full white brightness)
  - Examples:
    - 60 LEDs: 3.6A minimum (use 5A supply)
    - 144 LEDs: 8.6A minimum (use 10A supply)
    - 256 LEDs (16×16): 15.4A minimum (use 20A supply)

### Additional Components
- **Capacitor**: 1000µF 6.3V+ across power rails (reduces voltage spikes)
- **Resistor**: 470Ω between ESP32 data pin and LED data in (optional but recommended)
- **Breadboard or PCB**: For prototyping
- **Jumper wires**: For connections

## Wiring Diagram

### Basic Strip Connection

```
ESP32                    LED Strip
─────────────────────    ──────────────────

[GPIO 5] ────470Ω────────> [DIN]

[GND]    ────────────────> [GND]

                         [5V]
                           │
                         ┌─┴─┐
                         │ + │  Power Supply 5V
                         │ - │
                         └─┬─┘
                           │
[GND]    <─────────────────┘

 1000µF Cap
 ─┤├─ (across 5V/GND near LEDs)
```

### Important Notes

1. **Separate Power for LEDs**
   - Never power LEDs from ESP32's 5V pin (insufficient current)
   - Use external 5V power supply rated for total LED current
   - Connect power supply GND to ESP32 GND (common ground)

2. **Data Line**
   - Connect ESP32 GPIO pin directly to LED strip DIN
   - 470Ω resistor helps prevent signal reflections
   - Keep data wire short (<6 inches ideal, <3 feet maximum)

3. **Power Injection**
   - For >100 LEDs, inject power at multiple points
   - Every 100 LEDs, connect 5V and GND directly to supply

## Pin Configuration

### Default Pin Assignment
- **Data Pin**: GPIO 5
- Configurable in code via template parameter

### Recommended ESP32 Pins for LED Data
- **Good**: GPIO 2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27
- **Avoid**: GPIO 0, 1 (UART), GPIO 6-11 (Flash), GPIO 34-39 (input only)

### Changing the Pin

In your sketch:
```cpp
LEDDisplayHardware<60, 16> display;  // 60 LEDs on GPIO 16
```

## LED Strip Specifications

### WS2812B
- **Voltage**: 5V
- **Current**: ~60mA per LED (full white)
- **Data Protocol**: Single-wire
- **Color Order**: Usually GRB
- **Refresh Rate**: ~400 Hz maximum

### WS2815
- **Voltage**: 5V or 12V (check your strip!)
- **Current**: ~15mA per LED (12V version)
- **Data Protocol**: Single-wire with backup
- **Color Order**: Usually RGB
- **Refresh Rate**: ~400 Hz maximum
- **Advantage**: Continues working if one LED fails

## Power Supply Sizing

### Current Calculation

**Conservative (safe):**
```
Current (A) = LED_COUNT × 0.060
```

**Typical (average usage):**
```
Current (A) = LED_COUNT × 0.035
```

**Examples:**

| LED Count | Conservative | Typical | Recommended Supply |
|-----------|--------------|---------|-------------------|
| 30        | 1.8A         | 1.05A   | 2A                |
| 60        | 3.6A         | 2.1A    | 5A                |
| 144       | 8.6A         | 5.0A    | 10A               |
| 300       | 18A          | 10.5A   | 20A               |

### Power Supply Selection
- Choose switching power supply (not linear)
- Get 20-30% more capacity than calculated
- Ensure voltage is stable 5V (±5%)
- Short, thick wires from supply to LEDs

## Assembly Steps

### Step 1: Prepare Components
1. Cut LED strip to desired length (cut only at marked copper pads)
2. If using matrix, ensure pre-wired or plan serpentine wiring
3. Prepare power wires (thick gauge: 18-20 AWG for power)

### Step 2: Connect Power
1. Connect power supply 5V to LED strip 5V pad
2. Connect power supply GND to LED strip GND pad
3. Solder 1000µF capacitor across 5V/GND near LED strip
4. Test: Power on briefly, LEDs may show startup color

### Step 3: Connect Data
1. Identify LED strip DIN (Data In) connection
2. Solder resistor to ESP32 GPIO pin
3. Connect resistor to LED strip DIN
4. **Critical**: Ensure common ground between ESP32 and power supply

### Step 4: Program ESP32
1. Connect ESP32 to computer via USB
2. Upload test sketch (see below)
3. Open Serial Monitor to verify connection

### Step 5: Test
1. Upload Basic Test sketch
2. Verify LEDs respond to code
3. Check for flickering (indicates power or signal issues)

## Test Sketch

```cpp
#include <LEDDisplayHardware.h>

LEDDisplayHardware<60, 5> display;  // Adjust count and pin

void setup() {
    Serial.begin(115200);
    Serial.println("LED Test Starting...");

    display.begin();
    display.setBrightness(64);  // Start at low brightness
}

void loop() {
    // Red
    display.fill(255, 0, 0);
    display.show();
    delay(1000);

    // Green
    display.fill(0, 255, 0);
    display.show();
    delay(1000);

    // Blue
    display.fill(0, 0, 255);
    display.show();
    delay(1000);

    // Off
    display.clear();
    display.show();
    delay(1000);
}
```

## Troubleshooting

### No LEDs light up
- **Power**: Check 5V supply is on and connected
- **Polarity**: Verify 5V/GND not reversed
- **Data direction**: Ensure data connected to DIN, not DOUT
- **Code**: Verify GPIO pin matches wiring

### First few LEDs work, rest don't
- **Power**: Insufficient power supply or thin wires
- **Solution**: Add power injection every 100 LEDs
- **Check**: Voltage drop along strip (should stay >4.5V)

### LEDs flicker or show wrong colors
- **Common ground**: Ensure ESP32 GND connected to power supply GND
- **Signal integrity**: Add 470Ω resistor if not present
- **Capacitor**: Add 1000µF cap if not present
- **Power**: Check power supply can deliver enough current

### Random colors at startup
- **Normal**: LEDs show random colors before first `show()` call
- **Fix**: Call `display.clear()` and `display.show()` in `setup()`

### Only first LED works
- **Damaged LEDs**: One failed LED breaks chain
- **Solution**: Cut out failed LED, reconnect data line

### Colors are wrong (R/G/B swapped)
- **Color order**: WS2812B is usually GRB, not RGB
- **Fix in code**:
```cpp
FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
                                    // ^^^ Change to RGB or BGR
```

## Safety Considerations

- **Never hot-plug**: Always disconnect power before wiring
- **Fuse power supply**: Add inline fuse for safety
- **Ventilation**: High-power setups generate heat
- **Wire gauge**: Use appropriate gauge for current
- **Insulation**: Ensure no exposed conductors touching

## Matrix-Specific Setup

### Wiring Patterns

**Serpentine (Zigzag):**
```
→ → → →
      ↓
← ← ← ←
↓
→ → → →
```

**Row-Major (Straight):**
```
→ → → →
↓
→ → → →
↓
→ → → →
```

Configure in code:
```cpp
const bool SERPENTINE = true;  // or false

uint16_t XY(uint8_t x, uint8_t y) {
    if (SERPENTINE && y % 2 == 1) {
        return y * WIDTH + (WIDTH - 1 - x);
    }
    return y * WIDTH + x;
}
```

## Next Steps

Once hardware is working:
1. Try example sketches in `firmware/examples/`
2. Test with simulator (upload with SIMULATOR_MODE flag)
3. Develop custom animations
4. See [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) for animation techniques