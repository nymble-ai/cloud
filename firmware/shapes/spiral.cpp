// Spiral Shape Animation - Creates a rotating spiral pattern
// Uses the multi-strip LED display with physical coordinate mapping
// Draws a spiral emanating from the center point

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

// 12 STRIPS CONFIGURATION
const uint8_t LED_PINS[] = {0, 2, 4, 16, 17, 18, 19, 23, 14, 25, 26, 27};
const uint16_t LEDS_PER_STRIP = 90;
const uint8_t NUM_STRIPS = 12;

// Grid dimensions (working strips only)
const uint8_t GRID_WIDTH = 90;
const uint8_t GRID_HEIGHT = 10;

// Physical position to code strip mapping (Y=0 is physical position 1, closest to wall)
const uint8_t PHYSICAL_TO_CODE[] = {4, 5, 3, 6, 7, 8, 9, 10, 0, 2};

// Physical Y positions in inches for each strip
const uint8_t PHYSICAL_Y_POSITIONS[] = {0, 10, 20, 25, 30, 35, 40, 45, 55, 65};
const uint8_t TOTAL_PHYSICAL_HEIGHT = 65;

// Spiral parameters
const uint8_t CENTER_X = 45;        // Center X position in LEDs
const uint8_t CENTER_PHYS_Y = 32;   // Center Y position in inches
const uint8_t MAX_RADIUS_X = 30;    // Maximum radius X in LEDs
const uint8_t MAX_RADIUS_Y = 28;    // Maximum radius Y in inches
const float SPIRAL_TURNS = 3.5;     // Number of complete rotations
const uint16_t SPIRAL_POINTS = 200; // Points along the spiral
const float ROTATION_SPEED = 0.02;  // Speed of spiral rotation

// Color structure
struct Color {
  uint8_t r, g, b;
};

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

// Get color based on position along spiral (rainbow gradient)
Color getSpiralColor(float position, float brightness) {
  Color color;
  
  // Position goes from 0.0 (center) to 1.0 (outer edge)
  // Create rainbow gradient along the spiral
  float hue = fmod(position * 360.0 + millis() / 50.0, 360.0);
  
  // Convert HSV to RGB
  float h = hue / 60.0;
  int i = (int)h;
  float f = h - i;
  float p = 0;
  float q = 1.0 - f;
  float t = f;
  
  switch(i % 6) {
    case 0:
      color.r = (uint8_t)(255 * brightness);
      color.g = (uint8_t)(255 * t * brightness);
      color.b = (uint8_t)(255 * p * brightness);
      break;
    case 1:
      color.r = (uint8_t)(255 * q * brightness);
      color.g = (uint8_t)(255 * brightness);
      color.b = (uint8_t)(255 * p * brightness);
      break;
    case 2:
      color.r = (uint8_t)(255 * p * brightness);
      color.g = (uint8_t)(255 * brightness);
      color.b = (uint8_t)(255 * t * brightness);
      break;
    case 3:
      color.r = (uint8_t)(255 * p * brightness);
      color.g = (uint8_t)(255 * q * brightness);
      color.b = (uint8_t)(255 * brightness);
      break;
    case 4:
      color.r = (uint8_t)(255 * t * brightness);
      color.g = (uint8_t)(255 * p * brightness);
      color.b = (uint8_t)(255 * brightness);
      break;
    case 5:
      color.r = (uint8_t)(255 * brightness);
      color.g = (uint8_t)(255 * p * brightness);
      color.b = (uint8_t)(255 * q * brightness);
      break;
  }
  
  return color;
}

// Draw the spiral
void drawSpiral(float rotationOffset) {
  display.clear();
  
  // Draw spiral from center outward
  for (uint16_t i = 0; i < SPIRAL_POINTS; i++) {
    // Calculate position along spiral (0.0 to 1.0)
    float t = (float)i / SPIRAL_POINTS;
    
    // Calculate angle with rotation offset
    float angle = (t * SPIRAL_TURNS * 2.0 * 3.14159) + rotationOffset;
    
    // Calculate radius (grows linearly from center to edge)
    float radiusX = MAX_RADIUS_X * t;
    float radiusY = MAX_RADIUS_Y * t;
    
    // Calculate position
    int x = CENTER_X + radiusX * cos(angle);
    int physY = CENTER_PHYS_Y + radiusY * sin(angle);
    
    // Calculate brightness (brighter at outer edge, dimmer at center)
    float brightness = 0.3 + (t * 0.7);  // Range from 0.3 to 1.0
    
    // Get color for this point
    Color color = getSpiralColor(t, brightness);
    
    // Draw the point (with some thickness by drawing nearby points)
    if (x >= 0 && x < GRID_WIDTH && physY >= 0 && physY < TOTAL_PHYSICAL_HEIGHT) {
      setPhysicalPixel(x, physY, color.r, color.g, color.b);
      
      // Add thickness to spiral by drawing adjacent pixels
      if (x + 1 < GRID_WIDTH) {
        setPhysicalPixel(x + 1, physY, color.r, color.g, color.b);
      }
      if (x > 0) {
        setPhysicalPixel(x - 1, physY, color.r, color.g, color.b);
      }
    }
  }
  
  display.show();
}

// Animation state
float currentRotation = 0;
unsigned long lastUpdate = 0;

void setup() {
  // Configure all 12 LED strips
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    String stripName = "Strip " + String(i);
    display.addStrip(LED_PINS[i], LEDS_PER_STRIP, stripName);
  }

  display.begin();
  display.setBrightness(25);

  #ifndef HARDWARE_MODE
    Serial.println("=== SPIRAL SHAPE ===");
    Serial.println("Rotating spiral with rainbow gradient");
    Serial.println("10 working strips active");
  #endif

  lastUpdate = millis();
}

void loop() {
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastUpdate) / 1000.0;
  lastUpdate = currentTime;

  // Update rotation
  currentRotation += ROTATION_SPEED * deltaTime * 60.0;
  if (currentRotation > 2.0 * 3.14159) {
    currentRotation -= 2.0 * 3.14159;
  }

  // Draw the spiral
  drawSpiral(currentRotation);

  delay(16); // ~60 FPS
}

