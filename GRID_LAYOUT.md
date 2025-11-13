# LED Grid Layout for Animations

## Physical Grid Structure
- **Width:** 90 LEDs per strip
- **Height:** 10 working strips (12 total physical strips, 2 offline)
- **Total Working LEDs:** 900 LEDs

## Physical Position to Code Strip Mapping

| Physical Position | Code Strip # | GPIO Pin | Status |
|-------------------|--------------|----------|--------|
| 1 (closest to wall) | 4 | 17 | ✓ Working |
| 2 | 5 | 18 | ✓ Working |
| 3 | - | - | ❌ MISSING |
| 4 | 3 | 16 | ✓ Working |
| 5 | 6 | 19 | ✓ Working |
| 6 | 7 | 23 | ✓ Working |
| 7 | 8 | 14 | ✓ Working |
| 8 | 9 | 25 | ✓ Working |
| 9 | 10 | 26 | ✓ Working |
| 10 | 11 | 27 | ⚠️ OFFLINE |
| 11 | 0 | 0 | ✓ Working |
| 12 | 1 | 2 | ⚠️ OUT OF ORDER |
| 13 (farthest from wall) | 2 | 4 | ✓ Working |

## Grid Coordinate System

For animations, use this coordinate system:
- **X-axis:** 0-89 (LED position along strip, 0 = start of strip)
- **Y-axis:** 0-9 (physical strip index in working strips array)

### Working Strips Array (for Y-axis indexing)
```
Y=0 → Physical 1 → Code Strip 4
Y=1 → Physical 2 → Code Strip 5
Y=2 → Physical 4 → Code Strip 3
Y=3 → Physical 5 → Code Strip 6
Y=4 → Physical 6 → Code Strip 7
Y=5 → Physical 7 → Code Strip 8
Y=6 → Physical 8 → Code Strip 9
Y=7 → Physical 9 → Code Strip 10
Y=8 → Physical 11 → Code Strip 0
Y=9 → Physical 13 → Code Strip 2
```

## Notes
- Physical position 3 is missing (no strip connected)
- Physical position 10 (Strip 11) is temporarily offline due to power issues
- Physical position 12 (Strip 1) is out of order due to power issues
- Animations should only use the 10 working strips
