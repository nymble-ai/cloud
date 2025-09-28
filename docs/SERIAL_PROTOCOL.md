# Serial Protocol Specification

## Overview

The LED Simulator uses a simple binary serial protocol to transmit LED state data from the ESP32 to the visualizer. This protocol is optimized for speed and reliability.

## Protocol Details

### Baud Rate
- **Default**: 115200 bps
- Configurable in both firmware and visualizer

### Frame Structure

Each frame contains the complete state of all LEDs:

```
[FRAME_START] [COUNT_HIGH] [COUNT_LOW] [R0] [G0] [B0] [R1] [G1] [B1] ... [Rn] [Gn] [Bn] [FRAME_END]
```

### Special Bytes

- **FRAME_START**: `0xFF` - Marks the beginning of a frame
- **FRAME_END**: `0xFE` - Marks the end of a frame
- **ESCAPE_BYTE**: `0xFD` - Escape character for data that conflicts with control bytes

### Byte Stuffing (Escaping)

To prevent data bytes from being interpreted as control bytes, any data byte that equals `0xFF`, `0xFE`, or `0xFD` is preceded by the escape byte `0xFD`.

**Example:**
- If R value is `0xFF`, it's sent as: `0xFD 0xFF`
- If G value is `0x45`, it's sent as: `0x45` (no escaping needed)

### Frame Format Details

1. **FRAME_START** (1 byte)
   - Value: `0xFF`
   - Indicates start of new LED frame

2. **COUNT_HIGH** (1 byte, escaped)
   - High byte of LED count
   - Value: `(LED_COUNT >> 8) & 0xFF`

3. **COUNT_LOW** (1 byte, escaped)
   - Low byte of LED count
   - Value: `LED_COUNT & 0xFF`

4. **LED Data** (3 × LED_COUNT bytes, each escaped)
   - For each LED (index 0 to LED_COUNT-1):
     - R: Red value (0-255)
     - G: Green value (0-255)
     - B: Blue value (0-255)

5. **FRAME_END** (1 byte)
   - Value: `0xFE`
   - Indicates end of frame

## Example Frames

### Example 1: 3 LEDs - Red, Green, Blue

```
LED 0: RGB(255, 0, 0)     - Red
LED 1: RGB(0, 255, 0)     - Green
LED 2: RGB(0, 0, 255)     - Blue
```

**Transmitted bytes (with escaping):**
```
0xFF                      FRAME_START
0x00                      COUNT_HIGH (0)
0x03                      COUNT_LOW (3)
0xFD 0xFF                 R0 (255, escaped)
0x00                      G0 (0)
0x00                      B0 (0)
0x00                      R1 (0)
0xFD 0xFF                 G1 (255, escaped)
0x00                      B1 (0)
0x00                      R2 (0)
0x00                      G2 (0)
0xFD 0xFF                 B2 (255, escaped)
0xFE                      FRAME_END
```

### Example 2: 1 LED - All control bytes

```
LED 0: RGB(255, 254, 253)
```

**Transmitted bytes:**
```
0xFF                      FRAME_START
0x00                      COUNT_HIGH (0)
0x01                      COUNT_LOW (1)
0xFD 0xFF                 R0 (255, escaped)
0xFD 0xFE                 G0 (254, escaped)
0xFD 0xFD                 B0 (253, escaped)
0xFE                      FRAME_END
```

## Performance Considerations

### Bandwidth Calculation

For `N` LEDs at `F` fps:

**Worst case** (all bytes need escaping):
- Bytes per frame: `1 + 2×2 + N×3×2 + 1 = 6 + 6N`
- Bytes per second: `(6 + 6N) × F`
- Bits per second: `(6 + 6N) × F × 10` (including start/stop bits)

**Typical case** (≈10% bytes need escaping):
- Bytes per frame: `1 + 2×1.1 + N×3×1.1 + 1 ≈ 4.2 + 3.3N`
- Bytes per second: `(4.2 + 3.3N) × F`

### Example Frame Rates

At 115200 baud (≈11520 bytes/sec):

| LED Count | Typical FPS | Worst Case FPS |
|-----------|-------------|----------------|
| 60        | 54          | 30             |
| 144       | 23          | 13             |
| 256       | 13          | 7              |
| 300       | 11          | 6              |

## Parser Implementation (JavaScript)

```javascript
class SerialProtocolParser {
    constructor() {
        this.state = 'WAITING_START';
        this.buffer = [];
        this.ledCount = 0;
        this.expectedBytes = 0;
        this.escaped = false;
    }

    parse(byte) {
        if (this.escaped) {
            this.buffer.push(byte);
            this.escaped = false;
            return;
        }

        if (byte === 0xFD) {
            this.escaped = true;
            return;
        }

        if (byte === 0xFF) {
            this.state = 'COUNT_HIGH';
            this.buffer = [];
            return;
        }

        if (byte === 0xFE && this.state === 'LED_DATA') {
            return this.processFrame();
        }

        if (this.state === 'COUNT_HIGH') {
            this.ledCount = byte << 8;
            this.state = 'COUNT_LOW';
        } else if (this.state === 'COUNT_LOW') {
            this.ledCount |= byte;
            this.expectedBytes = this.ledCount * 3;
            this.state = 'LED_DATA';
        } else if (this.state === 'LED_DATA') {
            this.buffer.push(byte);
        }
    }

    processFrame() {
        if (this.buffer.length !== this.expectedBytes) {
            console.error('Frame size mismatch');
            this.state = 'WAITING_START';
            return null;
        }

        const leds = [];
        for (let i = 0; i < this.buffer.length; i += 3) {
            leds.push({
                r: this.buffer[i],
                g: this.buffer[i + 1],
                b: this.buffer[i + 2]
            });
        }

        this.state = 'WAITING_START';
        return leds;
    }
}
```

## Error Handling

### Synchronization Loss
If parser loses sync (corrupted data), it will:
1. Discard current buffer
2. Wait for next `FRAME_START` byte
3. Resume parsing

### Incomplete Frames
- Frames with incorrect byte count are discarded
- Parser resets and waits for next frame

### Timeout
- If no complete frame received within 1 second, connection may be considered stale
- Visualizer should indicate "no signal" state

## Future Enhancements

- **Compression**: Run-length encoding for repeated colors
- **Delta Encoding**: Only send changed pixels
- **CRC/Checksum**: Add frame validation
- **Bidirectional**: Allow visualizer to send commands to ESP32
- **Multiple Strips**: Support for multiple LED strip IDs in single stream

## Testing

Use the serial monitor tool for debugging:

```bash
node tools/serial-monitor.js
```

This will display decoded frames in human-readable format.