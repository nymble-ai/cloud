// Pulsing Circles Animation - Circles expanding outward AND contracting inward simultaneously
// Creates a breathing/pulsing effect with dynamic color gradients
// Faster speed with waves growing and shrinking at the same time

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

// Animation parameters
const uint8_t CENTER_X = 45;        // Center X position in LEDs
const uint8_t CENTER_PHYS_Y = 32;   // Center Y position in inches
const uint8_t BASE_RADIUS_X = 30;   // Base radius X in LEDs
const uint8_t BASE_RADIUS_Y = 28;   // Base radius Y in inches
const uint8_t NUM_EXPANDING = 4;    // Number of expanding circles
const uint8_t NUM_CONTRACTING = 4;  // Number of contracting circles
const float WAVE_SPACING = 5.0;     // Spacing between waves
const float WAVE_SPEED = 0.045;     // Slow, meditative speed (4x slower)

// Color palette structure
struct WaveColor {
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

// Get dynamic rainbow color based on position and time
WaveColor getRainbowColor(float phase, float brightness) {
  WaveColor color;
  
  // Create a flowing rainbow gradient with slow, gradual color transitions
  // Phase goes from 0.0 to 1.0 as wave expands/contracts
  float hue = fmod(phase * 360.0 + millis() / 100.0, 360.0); // Slow rotating rainbow (5x slower)
  
  // Convert HSV to RGB (simplified)
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

// Draw a single circle outline with specified radius and color
void drawCircleWave(float radiusScale, bool isExpanding) {
  const uint8_t numPoints = 120;  // Smooth circles
  
  // Calculate current radius
  float currentRadiusX = BASE_RADIUS_X * radiusScale;
  float currentRadiusY = BASE_RADIUS_Y * radiusScale;
  
  // Calculate brightness - fade at edges
  float brightness;
  if (isExpanding) {
    // Expanding: brightest at start, fade as it grows
    brightness = 1.0 - radiusScale;
  } else {
    // Contracting: brightest at max size, fade as it shrinks
    brightness = radiusScale;
  }
  
  // Keep brightness in valid range
  if (brightness < 0.2) brightness = 0.2;  // Minimum visibility
  if (brightness > 1.0) brightness = 1.0;
  
  // Get rainbow color based on radius scale
  WaveColor color = getRainbowColor(radiusScale, brightness);
  
  // Draw the circle
  for (uint16_t i = 0; i < numPoints; i++) {
    float angle = (2.0 * 3.14159 * i) / numPoints;
    int x = CENTER_X + currentRadiusX * cos(angle);
    int physY = CENTER_PHYS_Y + currentRadiusY * sin(angle);
    
    if (x >= 0 && x < GRID_WIDTH && physY >= 0 && physY < TOTAL_PHYSICAL_HEIGHT) {
      setPhysicalPixel(x, physY, color.r, color.g, color.b);
    }
  }
}

// Animation state
float expandingPhases[NUM_EXPANDING];
float contractingPhases[NUM_CONTRACTING];
unsigned long lastUpdate = 0;

void setup() {
  // Configure all 12 LED strips
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    String stripName = "Strip " + String(i);
    display.addStrip(LED_PINS[i], LEDS_PER_STRIP, stripName);
  }

  display.begin();
  display.setBrightness(25);  // Slightly brighter for more color vibrancy

  // Initialize expanding wave phases (staggered, growing from center)
  for (uint8_t i = 0; i < NUM_EXPANDING; i++) {
    expandingPhases[i] = (i * WAVE_SPACING) / BASE_RADIUS_X;
  }
  
  // Initialize contracting wave phases (staggered, shrinking from edge)
  for (uint8_t i = 0; i < NUM_CONTRACTING; i++) {
    contractingPhases[i] = 1.0 - (i * WAVE_SPACING) / BASE_RADIUS_X;
  }

  #ifndef HARDWARE_MODE
    Serial.println("=== PULSING CIRCLES ===");
    Serial.println("Expanding and contracting circles with rainbow gradient");
    Serial.println("Faster animation speed");
    Serial.println("10 working strips active");
  #endif

  lastUpdate = millis();
}

void loop() {
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastUpdate) / 1000.0;
  lastUpdate = currentTime;

  display.clear();

  // Update and draw expanding waves (growing outward)
  for (uint8_t i = 0; i < NUM_EXPANDING; i++) {
    // Expand the wave (0.0 to 1.0)
    expandingPhases[i] += (WAVE_SPEED / BASE_RADIUS_X) * deltaTime * 60.0;
    
    // Reset wave when it gets too large
    if (expandingPhases[i] > 1.0) {
      expandingPhases[i] = 0.0;
    }
    
    // Draw this expanding wave
    if (expandingPhases[i] > 0.05) {  // Skip very small circles
      drawCircleWave(expandingPhases[i], true);
    }
  }
  
  // Update and draw contracting waves (shrinking inward)
  for (uint8_t i = 0; i < NUM_CONTRACTING; i++) {
    // Contract the wave (1.0 to 0.0)
    contractingPhases[i] -= (WAVE_SPEED / BASE_RADIUS_X) * deltaTime * 60.0;
    
    // Reset wave when it gets too small
    if (contractingPhases[i] < 0.0) {
      contractingPhases[i] = 1.0;
    }
    
    // Draw this contracting wave
    if (contractingPhases[i] > 0.05 && contractingPhases[i] < 0.95) {
      drawCircleWave(contractingPhases[i], false);
    }
  }

  display.show();
  delay(16); // ~60 FPS
}

