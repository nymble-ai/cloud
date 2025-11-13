# LED Strip Physical Mapping

This document maps the code strip numbers to their physical positions.
The physical strip number represents distance from the wall (1 = closest to wall, 11 = farthest from wall).

## Strip Mapping Table

| Code Strip # | GPIO Pin | Physical Strip # (from wall) | Physical Spacing | Status      |
|--------------|----------|------------------------------|------------------|-------------|
| 4            | 17       | 1 (closest to wall)          | -                | ✓ Working   |
| 5            | 18       | 2                            | 10" from #1      | ✓ Working   |
| -            | -        | 3                            | (5" from #2)     | ❌ MISSING  |
| 3            | 16       | 4                            | 10" from #2      | ✓ Working   |
| 6            | 19       | 5                            | 5" from #4       | ✓ Working   |
| 7            | 23       | 6                            | 5" from #5       | ✓ Working   |
| 8            | 14       | 7                            | 5" from #6       | ✓ Working   |
| 9            | 25       | 8                            | 5" from #7       | ✓ Working   |
| 10           | 26       | 9                            | 5" from #8       | ✓ Working   |
| 11           | 27       | 10                           | (5" from #9)     | ⚠️ OFFLINE  |
| 0            | 0        | 11                           | 10" from #9      | ✓ Working   |
| 1            | 2        | 12                           | (5" from #11)    | ⚠️ OUT OF ORDER |
| 2            | 4        | 13 (farthest from wall)      | 10" from #11     | ✓ Working   |

**Physical Spacing Notes:**
- Standard spacing between adjacent strips: 5 inches
- Exception: Physical strips #1 to #2: 10 inches apart
- Missing/offline strips still occupy physical space in the layout

## Code Configuration

In your code, the LED_PINS array is:
```cpp
const uint8_t LED_PINS[] = {0, 2, 4, 16, 17, 18, 19, 23, 14, 25, 26, 27};
```

## Next Steps

Continue testing each strip and update this table with the physical strip numbers.
Once complete, we can create a mapping function for animations that need to reference physical positions.

## Animation Considerations

When creating animations based on physical position (e.g., wave from wall), use this mapping:
- Physical Strip #1 = closest to wall (Code Strip 4, Pin 17)
- Physical Strip #11 = farthest from wall (Code Strip 0, Pin 0)
- Physical Strip #12 (Code Strip 1, Pin 2) = **OUT OF ORDER** - Skip in animations
- Physical Strip #10 (Code Strip 11, Pin 27) = **TEMPORARILY OFFLINE** - Skip in animations

**IMPORTANT:**
- Strip 1 (Pin 2 / Physical #12) should be excluded from animations due to power issues (fritzing).
- Strip 11 (Pin 27 / Physical #10) should be excluded from animations temporarily due to power issues (fritzing).

**Working strips for animations: 0, 2, 3, 4, 5, 6, 7, 8, 9, 10 (10 total strips)**
