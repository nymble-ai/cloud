// Star Animation - A 5-pointed star with rainbow color cycling
// Creates a centered star shape that pulses to 130 BPM

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

// BPM parameters
const float BPM = 130.0;
const float BEAT_DURATION = 60000.0 / BPM;

// Star parameters
const float CENTER_X = 45.0;  // Center of 90 LEDs
const float CENTER_Y = 4.5;   // Center of 10 strips
const float STAR_RADIUS = 25.0;  // Outer radius
const float INNER_RADIUS = 10.0; // Inner radius (for star points)

// Color cycling
float hueOffset = 0.0;
const float HUE_SPEED = 1.0;

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

// Convert HSV to RGB (with R/G swap for this hardware)
void hsvToRgb(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
  float hh = fmod(h, 360.0) / 60.0;
  int i = (int)hh;
  float f = hh - i;
  float p = v * (1.0 - s);
  float q = v * (1.0 - s * f);
  float t = v * (1.0 - s * (1.0 - f));
  
  uint8_t temp_r, temp_g, temp_b;
  
  switch(i % 6) {
    case 0: temp_r = v*255; temp_g = t*255; temp_b = p*255; break;
    case 1: temp_r = q*255; temp_g = v*255; temp_b = p*255; break;
    case 2: temp_r = p*255; temp_g = v*255; temp_b = t*255; break;
    case 3: temp_r = p*255; temp_g = q*255; temp_b = v*255; break;
    case 4: temp_r = t*255; temp_g = p*255; temp_b = v*255; break;
    default: temp_r = v*255; temp_g = p*255; temp_b = q*255; break;
  }
  
  // SWAP R and G for this hardware!
  r = temp_g;
  g = temp_r;
  b = temp_b;
}

// Get pulse intensity based on time (synced to 130 BPM)
float getPulseIntensity(unsigned long currentTime) {
  float beatPhase = fmod(currentTime, BEAT_DURATION) / BEAT_DURATION;
  
  // Sharp rise, gradual fall
  float intensity;
  if (beatPhase < 0.1) {
    intensity = beatPhase / 0.1;
  } else {
    intensity = 1.0 - ((beatPhase - 0.1) / 0.9);
  }
  
  return intensity;
}

// Calculate 5-pointed star points
void getStarPoint(int index, float &px, float &py) {
  // Star has 10 points total - 5 outer, 5 inner
  // Start at top (270 degrees)
  float angle = -90.0 + (index * 36.0); // 360/10 = 36 degrees per point
  float angleRad = angle * PI / 180.0;
  
  // Alternate between outer and inner radius
  float radius = (index % 2 == 0) ? STAR_RADIUS : INNER_RADIUS;
  
  px = CENTER_X + radius * cos(angleRad);
  py = CENTER_Y + radius * sin(angleRad);
}

// Check if a point is inside the star using ray casting
bool isInsideStar(float x, float y) {
  // Get all 10 star points
  float points[10][2];
  for (int i = 0; i < 10; i++) {
    getStarPoint(i, points[i][0], points[i][1]);
  }
  
  // Ray casting algorithm
  int crossings = 0;
  for (int i = 0; i < 10; i++) {
    int j = (i + 1) % 10;
    
    float x1 = points[i][0];
    float y1 = points[i][1];
    float x2 = points[j][0];
    float y2 = points[j][1];
    
    // Check if ray from point to right crosses this edge
    if (((y1 <= y) && (y < y2)) || ((y2 <= y) && (y < y1))) {
      float xIntersect = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
      if (x < xIntersect) {
        crossings++;
      }
    }
  }
  
  return (crossings % 2 == 1);
}

// Check if point is on star outline (thick line)
bool isOnStarOutline(float x, float y, float thickness) {
  float points[10][2];
  for (int i = 0; i < 10; i++) {
    getStarPoint(i, points[i][0], points[i][1]);
  }
  
  // Check distance to each edge
  for (int i = 0; i < 10; i++) {
    int j = (i + 1) % 10;
    
    float x1 = points[i][0];
    float y1 = points[i][1];
    float x2 = points[j][0];
    float y2 = points[j][1];
    
    // Distance from point to line segment
    float dx = x2 - x1;
    float dy = y2 - y1;
    float lengthSquared = dx * dx + dy * dy;
    
    if (lengthSquared < 0.001) continue;
    
    float t = ((x - x1) * dx + (y - y1) * dy) / lengthSquared;
    t = constrain(t, 0.0, 1.0);
    
    float closestX = x1 + t * dx;
    float closestY = y1 + t * dy;
    
    float dist = sqrt((x - closestX) * (x - closestX) + (y - closestY) * (y - closestY));
    
    if (dist <= thickness) {
      return true;
    }
  }
  
  return false;
}

// Draw the star
void drawStar(float pulseIntensity) {
  for (int x = 0; x < GRID_WIDTH; x++) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
      uint8_t r = 0, g = 0, b = 0;
      
      float fx = (float)x;
      float fy = (float)y;
      
      // Calculate distance from center for color variation
      float dx = fx - CENTER_X;
      float dy = fy - CENTER_Y;
      float distFromCenter = sqrt(dx * dx + dy * dy);
      
      // Check if on outline (brighter) or inside (dimmer)
      bool onOutline = isOnStarOutline(fx, fy, 1.2);
      bool inside = isInsideStar(fx, fy);
      
      if (onOutline || inside) {
        // Color based on distance from center
        float hue = hueOffset + (distFromCenter * 8.0);
        float saturation = 0.85;
        float brightness;
        
        if (onOutline) {
          // Outline is brighter and pulses more
          brightness = 0.5 + (pulseIntensity * 0.3);
        } else {
          // Inside is dimmer
          brightness = 0.25 + (pulseIntensity * 0.15);
        }
        
        hsvToRgb(hue, saturation, brightness, r, g, b);
      }
      
      setDirectPixel(x, y, r, g, b);
    }
  }
}

// Variables for timing
unsigned long startTime = 0;

void setup() {
  // Configure all 12 LED strips
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    String stripName = "Strip " + String(i);
    display.addStrip(LED_PINS[i], LEDS_PER_STRIP, stripName);
  }

  display.begin();
  display.setBrightness(35);
  
  randomSeed(analogRead(0));
  startTime = millis();

  #ifndef HARDWARE_MODE
    Serial.println("=== 5-POINTED STAR - 130 BPM ===");
    Serial.println("Centered star with rainbow colors");
    Serial.println("Synced to 130 beats per minute");
  #endif
}

void loop() {
  unsigned long currentTime = millis() - startTime;
  
  display.clear();
  
  // Get pulse intensity synced to 130 BPM
  float pulseIntensity = getPulseIntensity(currentTime);
  
  // Update color cycling
  hueOffset += HUE_SPEED;
  if (hueOffset >= 360.0) {
    hueOffset -= 360.0;
  }
  
  // Draw star
  drawStar(pulseIntensity);
  
  display.show();
  delay(16); // ~60 FPS
}

