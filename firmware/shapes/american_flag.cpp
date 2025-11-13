// American Flag Animation - Waving red, white, and blue flag
// Creates a flowing wave effect across the LED grid
// Flag oriented horizontally with stars field on left, stripes across

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

// Grid dimensions
const uint8_t GRID_WIDTH = 90;
const uint8_t GRID_HEIGHT = 10;

// Physical position to code strip mapping
const uint8_t PHYSICAL_TO_CODE[] = {4, 5, 3, 6, 7, 8, 9, 10, 0, 2};

// American flag colors - RED AND GREEN CHANNELS ARE SWAPPED ON THIS HARDWARE!
// To get RED: use (0, 255, 0) - put value in GREEN channel
// To get GREEN: use (255, 0, 0) - put value in RED channel
const uint8_t FLAG_RED_R = 0;      // Red requires GREEN channel!
const uint8_t FLAG_RED_G = 255;
const uint8_t FLAG_RED_B = 0;

const uint8_t FLAG_WHITE_R = 255;  // White needs all channels
const uint8_t FLAG_WHITE_G = 255;
const uint8_t FLAG_WHITE_B = 255;

const uint8_t FLAG_BLUE_R = 0;     // Blue is correct
const uint8_t FLAG_BLUE_G = 0;
const uint8_t FLAG_BLUE_B = 255;

// Wave parameters
float wavePhase = 0.0;
const float WAVE_SPEED = 0.05;
const float WAVE_AMPLITUDE = 1.5;  // Pixels of wave height

// Flag layout
const uint8_t STARS_FIELD_WIDTH = 36;  // Stars field is ~40% of flag
const uint8_t TOTAL_STRIPES = 13;      // 13 stripes (7 red, 6 white)
const uint8_t STRIPE_HEIGHT = 1;       // Each stripe is approximately 1 grid row

// Helper function to set pixel using grid coordinates
void setGridPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
  if (x >= GRID_WIDTH || y >= GRID_HEIGHT) return;
  uint8_t codeStrip = PHYSICAL_TO_CODE[y];
  display.setPixelOnStrip(codeStrip, x, r, g, b);
}

void setDirectPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
  if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return;
  setGridPixel((uint8_t)x, (uint8_t)y, r, g, b);
}

// Calculate wave offset for given x position
float getWaveOffset(int x, float phase) {
  // Create a sine wave across the flag
  float frequency = 0.15;  // Wave frequency
  return sin(x * frequency + phase) * WAVE_AMPLITUDE;
}

// Check if position should be a star (simplified pattern)
bool isStarPosition(int x, int y) {
  // Create a simple star pattern in the blue field
  // Stars roughly every 6 pixels in x, every 2 pixels in y
  int starSpacingX = 6;
  int starSpacingY = 2;
  
  int offsetX = x % starSpacingX;
  int offsetY = y % starSpacingY;
  
  // Star is at center of each cell
  if (offsetX >= 2 && offsetX <= 3 && offsetY == 1) {
    return true;
  }
  
  return false;
}

// Draw the American flag with wave effect
void drawFlag() {
  for (int x = 0; x < GRID_WIDTH; x++) {
    // Calculate wave offset for this column
    float waveOffset = getWaveOffset(x, wavePhase);
    
    for (int y = 0; y < GRID_HEIGHT; y++) {
      // Apply wave offset to y coordinate
      float wavedY = y + waveOffset;
      int drawY = (int)round(wavedY);
      
      // Keep within bounds
      if (drawY < 0) drawY = 0;
      if (drawY >= GRID_HEIGHT) drawY = GRID_HEIGHT - 1;
      
      uint8_t r, g, b;
      
      // Stars field (upper left corner)
      if (x < STARS_FIELD_WIDTH && y < 4) {
        // Blue field with white stars
        if (isStarPosition(x, y)) {
          r = FLAG_WHITE_R;
          g = FLAG_WHITE_G;
          b = FLAG_WHITE_B;
        } else {
          r = FLAG_BLUE_R;
          g = FLAG_BLUE_G;
          b = FLAG_BLUE_B;
        }
      }
      // Stripes (rest of flag)
      else {
        // Determine stripe (alternating red and white)
        // Top stripe is red
        int stripeIndex = (y * TOTAL_STRIPES) / GRID_HEIGHT;
        
        if (stripeIndex % 2 == 0) {
          // Red stripe
          r = FLAG_RED_R;
          g = FLAG_RED_G;
          b = FLAG_RED_B;
        } else {
          // White stripe
          r = FLAG_WHITE_R;
          g = FLAG_WHITE_G;
          b = FLAG_WHITE_B;
        }
      }
      
      // Add wave shading (darker in troughs, brighter on peaks)
      float shadeFactor = 0.7 + (sin(x * 0.15 + wavePhase) * 0.3);
      r = (uint8_t)(r * shadeFactor);
      g = (uint8_t)(g * shadeFactor);
      b = (uint8_t)(b * shadeFactor);
      
      setDirectPixel(x, drawY, r, g, b);
    }
  }
}

void setup() {
  // Configure all 12 LED strips
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    String stripName = "Strip " + String(i);
    display.addStrip(LED_PINS[i], LEDS_PER_STRIP, stripName);
  }

  display.begin();
  display.setBrightness(35);  // Slightly brighter for visibility
  
  randomSeed(analogRead(0));

  #ifndef HARDWARE_MODE
    Serial.println("=== AMERICAN FLAG ===");
    Serial.println("Waving flag with red, white, and blue");
    Serial.println("Stars field on left, stripes across");
    Serial.println("NOTE: Colors may appear different on hardware");
  #endif
}

void loop() {
  display.clear();
  
  // Update wave phase
  wavePhase += WAVE_SPEED;
  if (wavePhase > 2.0 * 3.14159 * 10.0) {
    wavePhase = 0.0;
  }
  
  // Draw the flag
  drawFlag();
  
  display.show();
  delay(20); // ~50 FPS
}

