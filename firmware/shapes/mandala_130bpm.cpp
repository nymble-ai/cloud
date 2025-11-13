// Eastern Mandala Animation - Pulsing mandala pattern synced to 130 BPM
// Creates concentric geometric patterns with symmetry and color cycling
// Pulses expand and contract in rhythm with the beat

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
const float BEAT_DURATION = 60000.0 / BPM;  // Milliseconds per beat
const float PULSE_FREQUENCY = BPM / 60.0;    // Beats per second

// Mandala center
const float CENTER_X = 45.0;
const float CENTER_Y = 5.0;

// Mandala parameters
const uint8_t NUM_LAYERS = 6;      // Concentric layers
const uint8_t SYMMETRY = 8;        // 8-fold symmetry (like lotus petals)

// Color cycling
float hueOffset = 0.0;
const float HUE_SPEED = 0.5;  // Hue rotation speed

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
  r = temp_g;  // Put green value in red channel
  g = temp_r;  // Put red value in green channel
  b = temp_b;
}

// Get pulse intensity based on time (synced to 130 BPM)
float getPulseIntensity(unsigned long currentTime) {
  // Calculate position in beat cycle (0.0 to 1.0)
  float beatPhase = fmod(currentTime, BEAT_DURATION) / BEAT_DURATION;
  
  // Create a pulse shape - sharp rise, gradual fall
  float intensity;
  if (beatPhase < 0.1) {
    // Quick rise
    intensity = beatPhase / 0.1;
  } else {
    // Gradual fall
    intensity = 1.0 - ((beatPhase - 0.1) / 0.9);
  }
  
  return intensity;
}

// Check if point should be part of mandala pattern
bool isMandalaPoint(float x, float y, float radius, float angle, float pulseIntensity, bool &isBorder) {
  isBorder = false;
  
  // Create CLEAR concentric rings
  float ringSpacing = 5.0;
  float ringIndex = floor(radius / ringSpacing);
  float ringPosition = fmod(radius, ringSpacing);
  
  // Thick rings with clear gaps
  bool inRing = (ringPosition < 2.0);
  
  // Create 8-fold petal/spoke pattern
  float angleNormalized = fmod(angle + 3.14159, 2.0 * 3.14159 / SYMMETRY);
  float spokeWidth = 0.3;  // Width of each spoke
  bool inSpoke = (angleNormalized < spokeWidth);
  
  // Create diamond/square pattern at intersections
  float checkerAngle = fmod(angle * SYMMETRY / 2.0, 3.14159);
  bool inCheckerSpoke = (checkerAngle < 0.4 || checkerAngle > 2.7);
  
  // Border detection (edges of rings and spokes)
  bool isRingBorder = (ringPosition > 1.8 && ringPosition < 2.2);
  bool isSpokeBorder = (angleNormalized > (spokeWidth - 0.05) && angleNormalized < (spokeWidth + 0.05));
  
  isBorder = (isRingBorder || isSpokeBorder);
  
  // Show geometry: rings OR spokes OR checker pattern
  return (inRing || inSpoke || (inCheckerSpoke && inRing));
}

// Draw the mandala
void drawMandala(float pulseIntensity) {
  // Maximum radius changes with pulse
  float maxRadius = 25.0 + (pulseIntensity * 8.0);
  
  // Scan through grid
  for (int px = 0; px < GRID_WIDTH; px++) {
    for (int py = 0; py < GRID_HEIGHT; py++) {
      // Calculate distance and angle from center
      float dx = px - CENTER_X;
      float dy = py - CENTER_Y;
      float distance = sqrt(dx * dx + dy * dy);
      float angle = atan2(dy, dx);
      
      if (distance < maxRadius && distance > 1.0) {
        // Normalize radius (0 to 1)
        float normalizedRadius = distance / maxRadius;
        
        // Check if point is part of mandala pattern
        bool isBorder = false;
        if (isMandalaPoint(dx, dy, distance, angle, pulseIntensity, isBorder)) {
          // Calculate color based on radius and angle
          float hue = hueOffset + (normalizedRadius * 120.0);
          
          // Brightness based on pulse and radius
          float brightness = 0.6 + (pulseIntensity * 0.4);
          
          // Borders are brighter white
          if (isBorder) {
            brightness = 1.0;
            hue = 0.0;  // Will be white
          }
          
          // Saturation - high for visible colors
          float saturation = isBorder ? 0.0 : 0.9;
          
          uint8_t r, g, b;
          hsvToRgb(hue, saturation, brightness, r, g, b);
          
          setDirectPixel(px, py, r, g, b);
        }
      }
      
      // Draw bright center point that pulses
      if (distance < 2.0) {
        uint8_t r, g, b;
        float centerBrightness = 0.5 + (pulseIntensity * 0.5);
        hsvToRgb(hueOffset, 1.0, centerBrightness, r, g, b);
        setDirectPixel(px, py, r, g, b);
      }
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
  display.setBrightness(30);
  
  randomSeed(analogRead(0));
  startTime = millis();

  #ifndef HARDWARE_MODE
    Serial.println("=== EASTERN MANDALA - 130 BPM ===");
    Serial.println("Pulsing mandala with 8-fold symmetry");
    Serial.println("Synced to 130 beats per minute");
    Serial.println("Color cycling with geometric patterns");
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
  
  // Draw mandala
  drawMandala(pulseIntensity);
  
  display.show();
  delay(16); // ~60 FPS
}

