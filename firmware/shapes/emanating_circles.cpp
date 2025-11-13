// Emanating Circles Animation - Creates a whirlpool/wave effect with multiple expanding circles
// Uses the multi-strip LED display with physical coordinate mapping
// Animates concentric circles that pulse outward from center like ripples in water

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
const uint8_t NUM_WAVES = 5;        // Number of emanating circles
const float WAVE_SPACING = 6.0;     // Spacing between waves
const float WAVE_SPEED = 0.08;      // Speed of wave expansion

// Color palette for waves (gradient from blue to cyan to white)
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

// Get color based on wave index and brightness
WaveColor getWaveColor(uint8_t waveIndex, float brightness) {
  WaveColor color;
  
  // Create gradient from blue (inner) to cyan to white (outer)
  float wavePhase = (float)waveIndex / NUM_WAVES;
  
  if (wavePhase < 0.5) {
    // Blue to Cyan gradient
    float t = wavePhase * 2.0;
    color.r = 0;
    color.g = (uint8_t)(255 * t * brightness);
    color.b = (uint8_t)(255 * brightness);
  } else {
    // Cyan to White gradient
    float t = (wavePhase - 0.5) * 2.0;
    color.r = (uint8_t)(255 * t * brightness);
    color.g = (uint8_t)(255 * brightness);
    color.b = (uint8_t)(255 * brightness);
  }
  
  return color;
}

// Draw a single circle outline with specified radius offset and color
void drawCircleWave(float radiusOffset, uint8_t waveIndex) {
  const uint8_t numPoints = 120;  // More points for smoother circles
  
  // Calculate current radius for this wave
  float currentRadiusX = BASE_RADIUS_X * (radiusOffset / BASE_RADIUS_X);
  float currentRadiusY = BASE_RADIUS_Y * (radiusOffset / BASE_RADIUS_Y);
  
  // Fade out as circles get larger
  float maxRadius = BASE_RADIUS_X * 1.5;
  float brightness = 1.0 - (radiusOffset / maxRadius);
  if (brightness < 0) brightness = 0;
  if (brightness > 1) brightness = 1;
  
  WaveColor color = getWaveColor(waveIndex, brightness);
  
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
float wavePhases[NUM_WAVES];
unsigned long lastUpdate = 0;

void setup() {
  // Configure all 12 LED strips
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    String stripName = "Strip " + String(i);
    display.addStrip(LED_PINS[i], LEDS_PER_STRIP, stripName);
  }

  display.begin();
  display.setBrightness(20);

  // Initialize wave phases (staggered)
  for (uint8_t i = 0; i < NUM_WAVES; i++) {
    wavePhases[i] = i * WAVE_SPACING;
  }

  #ifndef HARDWARE_MODE
    Serial.println("=== EMANATING CIRCLES ===");
    Serial.println("Whirlpool/wave effect with expanding circles");
    Serial.println("10 working strips active");
  #endif

  lastUpdate = millis();
}

void loop() {
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastUpdate) / 1000.0;
  lastUpdate = currentTime;

  display.clear();

  // Update and draw each wave
  for (uint8_t i = 0; i < NUM_WAVES; i++) {
    // Expand the wave
    wavePhases[i] += WAVE_SPEED * deltaTime * 60.0; // Normalize to ~60fps
    
    // Reset wave when it gets too large
    if (wavePhases[i] > BASE_RADIUS_X * 1.5) {
      wavePhases[i] = 0;
    }
    
    // Draw this wave
    if (wavePhases[i] > 0) {
      drawCircleWave(wavePhases[i], i);
    }
  }

  display.show();
  delay(16); // ~60 FPS
}

