#include <LEDDisplay.h>

#ifdef HARDWARE_MODE
  #include <MultiStripHardware.h>
  MultiStripHardware display;
  #define IS_SIMULATOR false
#else
  #include <MultiStripSimulator.h>
  MultiStripSimulator display;
  #define IS_SIMULATOR true
#endif

// 12 STRIPS CONFIGURATION (added Pin 27)
const uint8_t LED_PINS[] = {0, 2, 4, 16, 17, 18, 19, 23, 14, 25, 26, 27};
const uint16_t LEDS_PER_STRIP = 90;
const uint8_t NUM_STRIPS = 12;

// Grid dimensions (working strips only)
const uint8_t GRID_WIDTH = 90;
const uint8_t GRID_HEIGHT = 10;

// Physical position to code strip mapping (Y=0 is physical position 1, closest to wall)
const uint8_t PHYSICAL_TO_CODE[] = {4, 5, 3, 6, 7, 8, 9, 10, 0, 2};

// Physical Y positions in inches for each strip (for proper scaling)
// Y=0 at 0", Y=1 at 10", Y=2 at 20" (phys 3 missing), Y=3 at 25", etc.
const uint8_t PHYSICAL_Y_POSITIONS[] = {0, 10, 20, 25, 30, 35, 40, 45, 55, 65};
const uint8_t TOTAL_PHYSICAL_HEIGHT = 65;  // inches

// Circle colors (white outline)
const uint8_t WHITE_R = 255, WHITE_G = 255, WHITE_B = 255;

// Helper function to set pixel using grid coordinates
void setGridPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
  if (x >= GRID_WIDTH || y >= GRID_HEIGHT) return;
  uint8_t codeStrip = PHYSICAL_TO_CODE[y];
  display.setPixelOnStrip(codeStrip, x, r, g, b);
}

// Map physical Y coordinate (in inches) to closest strip index (0-9)
uint8_t physicalYToStrip(int physY) {
  if (physY < 0) return 0;
  if (physY >= TOTAL_PHYSICAL_HEIGHT) return GRID_HEIGHT - 1;

  // Find closest strip
  uint8_t closestStrip = 0;
  int minDist = abs(physY - PHYSICAL_Y_POSITIONS[0]);

  for (uint8_t i = 1; i < GRID_HEIGHT; i++) {
    int dist = abs(physY - (int)PHYSICAL_Y_POSITIONS[i]);
    if (dist < minDist) {
      minDist = dist;
      closestStrip = i;
    }
  }

  return closestStrip;
}

// Set pixel using physical coordinates (X in LEDs, Y in inches)
void setPhysicalPixel(uint8_t x, int physY, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t stripY = physicalYToStrip(physY);
  setGridPixel(x, stripY, r, g, b);
}

// Draw circle outline using midpoint circle algorithm with physical coordinates
// centerX in LEDs, centerPhysY in inches, radiusX in LEDs, radiusY in inches
void drawCircleOutline(uint8_t centerX, uint8_t centerPhysY, uint8_t radiusX, uint8_t radiusY, uint8_t r, uint8_t g, uint8_t b) {
  // Use parametric approach for ellipse (circle with different X and Y radii)
  // Draw points around the circle perimeter
  const uint8_t numPoints = 100;  // Number of points to draw around the circle

  for (uint16_t i = 0; i < numPoints; i++) {
    float angle = (2.0 * 3.14159 * i) / numPoints;
    int x = centerX + radiusX * cos(angle);
    int physY = centerPhysY + radiusY * sin(angle);

    if (x >= 0 && x < GRID_WIDTH && physY >= 0 && physY < TOTAL_PHYSICAL_HEIGHT) {
      setPhysicalPixel(x, physY, r, g, b);
    }
  }
}

// Render Circle Outline
void renderCircle() {
  display.clear();

  // Draw circle outline using physical coordinates
  // Grid is 90 LEDs wide x 65 inches tall
  // Center at X=45 LEDs, Y=32 inches (middle of physical space)
  // Radius: 30 LEDs wide, 28 inches tall (fits nicely in the space)
  drawCircleOutline(45, 32, 30, 28, WHITE_R, WHITE_G, WHITE_B);

  display.show();
}

void setup() {
  // Configure all 12 LED strips
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    String stripName = "Strip " + String(i);
    display.addStrip(LED_PINS[i], LEDS_PER_STRIP, stripName);
  }

  display.begin();
  display.setBrightness(20);  // Medium brightness for visibility

  #ifndef HARDWARE_MODE
    Serial.println("=== CIRCLE OUTLINE ===");
    Serial.println("White circle outline on black background");
    Serial.println("10 working strips active");
  #endif

  renderCircle();
}

void loop() {
  // Static display - no animation needed
  delay(1000);
}
