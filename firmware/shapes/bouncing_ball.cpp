// Bouncing Ball Animation - A colorful ball bouncing with realistic physics
// Uses the multi-strip LED display with physical coordinate mapping
// Simulates gravity, velocity, and elastic collisions with walls

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

// Physics parameters - NOW USING GRID COORDINATES (not physical inches)
float ballX = 45.0;              // Ball X position (in LEDs, 0-89)
float ballY = 5.0;               // Ball Y position (in grid rows, 0-9, with decimals)
float velocityX = 1.8;           // X velocity (LEDs per frame)
float velocityY = 0.0;           // Y velocity (grid rows per frame)
const float GRAVITY = 0.12;      // Gravity acceleration (grid rows per frame^2)
const float BOUNCE_DAMPING = 0.78; // More energy loss on bounce (78% retained)
const float BALL_SIZE = 0.8;     // Ball radius - SMALLER! (less than 1 strip)
const float MIN_VELOCITY = 0.12; // Minimum velocity threshold before reset
unsigned long stoppedTime = 0;   // Time when ball stopped
bool isStopped = false;          // Is ball currently stopped

// Ball color (changes on bounce)
float hue = 0.0;

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

// Set pixel directly using grid coordinates (no physical conversion needed)
void setDirectPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
  if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return;
  setGridPixel((uint8_t)x, (uint8_t)y, r, g, b);
}

// Convert HSV to RGB
void hsvToRgb(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
  float hh = fmod(h, 360.0) / 60.0;
  int i = (int)hh;
  float f = hh - i;
  float p = v * (1.0 - s);
  float q = v * (1.0 - s * f);
  float t = v * (1.0 - s * (1.0 - f));
  
  switch(i) {
    case 0:
      r = (uint8_t)(v * 255);
      g = (uint8_t)(t * 255);
      b = (uint8_t)(p * 255);
      break;
    case 1:
      r = (uint8_t)(q * 255);
      g = (uint8_t)(v * 255);
      b = (uint8_t)(p * 255);
      break;
    case 2:
      r = (uint8_t)(p * 255);
      g = (uint8_t)(v * 255);
      b = (uint8_t)(t * 255);
      break;
    case 3:
      r = (uint8_t)(p * 255);
      g = (uint8_t)(q * 255);
      b = (uint8_t)(v * 255);
      break;
    case 4:
      r = (uint8_t)(t * 255);
      g = (uint8_t)(p * 255);
      b = (uint8_t)(v * 255);
      break;
    default:
      r = (uint8_t)(v * 255);
      g = (uint8_t)(p * 255);
      b = (uint8_t)(q * 255);
      break;
  }
}

// Draw the ball - SMALL and TIGHT
void drawBall(float x, float y, float ballHue) {
  uint8_t r, g, b;
  hsvToRgb(ballHue, 1.0, 1.0, r, g, b);
  
  int centerX = (int)round(x);
  int centerY = (int)round(y);
  
  // Draw a small, bright ball (just center pixel + immediate neighbors)
  // Center pixel - full brightness
  setDirectPixel(centerX, centerY, r, g, b);
  
  // Horizontal neighbors (slight glow)
  if (centerX > 0) {
    setDirectPixel(centerX - 1, centerY, r/2, g/2, b/2);
  }
  if (centerX < GRID_WIDTH - 1) {
    setDirectPixel(centerX + 1, centerY, r/2, g/2, b/2);
  }
  
  // Add slight glow to adjacent strips only if close to strip boundary
  float yFraction = y - centerY;
  if (yFraction < -0.3 && centerY > 0) {
    // Close to strip above
    setDirectPixel(centerX, centerY - 1, r/3, g/3, b/3);
  } else if (yFraction > 0.3 && centerY < GRID_HEIGHT - 1) {
    // Close to strip below
    setDirectPixel(centerX, centerY + 1, r/3, g/3, b/3);
  }
}

// Update ball physics
void updatePhysics() {
  // Check if ball has come to a stop
  float totalVelocity = sqrt(velocityX * velocityX + velocityY * velocityY);
  
  if (!isStopped && totalVelocity < MIN_VELOCITY) {
    // Ball has stopped
    isStopped = true;
    stoppedTime = millis();
    velocityX = 0;
    velocityY = 0;
    return;
  }
  
  // If stopped, wait 1.5 seconds then launch again with high energy
  if (isStopped) {
    if (millis() - stoppedTime > 1500) {
      // Launch ball with high velocity in VARIED directions (not just upward!)
      float angle = random(30, 150) / 100.0 * 3.14159; // 30-150 degrees
      float speed = random(250, 350) / 100.0;
      velocityX = speed * cos(angle) * (random(0, 2) == 0 ? 1 : -1);
      velocityY = -speed * sin(angle); // Negative = upward
      hue = random(0, 360); // Random new color
      isStopped = false;
    }
    return;
  }
  
  // Apply gravity to Y velocity
  velocityY += GRAVITY;
  
  // Update position
  ballX += velocityX;
  ballY += velocityY;
  
  // Check collision with left/right walls
  if (ballX - BALL_SIZE <= 0) {
    ballX = BALL_SIZE;
    velocityX = -velocityX * BOUNCE_DAMPING;
    hue += 60.0;  // Change color on bounce
    if (hue >= 360.0) hue -= 360.0;
  }
  if (ballX + BALL_SIZE >= GRID_WIDTH) {
    ballX = GRID_WIDTH - BALL_SIZE;
    velocityX = -velocityX * BOUNCE_DAMPING;
    hue += 60.0;  // Change color on bounce
    if (hue >= 360.0) hue -= 360.0;
  }
  
  // Check collision with top/bottom walls (GRID coordinates: 0 = top, 9 = bottom)
  if (ballY - BALL_SIZE <= 0) {
    ballY = BALL_SIZE;
    velocityY = -velocityY * BOUNCE_DAMPING;
    hue += 60.0;  // Change color on bounce
    if (hue >= 360.0) hue -= 360.0;
  }
  if (ballY + BALL_SIZE >= GRID_HEIGHT - 0.5) {
    ballY = GRID_HEIGHT - 0.5 - BALL_SIZE;
    velocityY = -velocityY * BOUNCE_DAMPING;
    hue += 60.0;  // Change color on bounce
    if (hue >= 360.0) hue -= 360.0;
    
    // Add extra horizontal velocity on floor bounce for more varied movement
    if (abs(velocityX) < 0.8) {
      velocityX += random(-100, 100) / 100.0;
    }
  }
  
  // Limit max velocity to keep it visible
  if (velocityX > 3.5) velocityX = 3.5;
  if (velocityX < -3.5) velocityX = -3.5;
  if (velocityY > 3.5) velocityY = 3.5;
  if (velocityY < -3.5) velocityY = -3.5;
}

unsigned long lastUpdate = 0;

void setup() {
  // Configure all 12 LED strips
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    String stripName = "Strip " + String(i);
    display.addStrip(LED_PINS[i], LEDS_PER_STRIP, stripName);
  }

  display.begin();
  display.setBrightness(30);
  
  // Initialize random seed
  randomSeed(analogRead(0));
  
  // Start with HIGH random velocity at a varied angle!
  float angle = random(30, 150) / 100.0 * 3.14159; // 30-150 degrees
  float speed = random(250, 350) / 100.0;
  velocityX = speed * cos(angle) * (random(0, 2) == 0 ? 1 : -1);
  velocityY = -speed * sin(angle); // Negative = upward
  hue = random(0, 360);

  #ifndef HARDWARE_MODE
    Serial.println("=== BOUNCING BALL ===");
    Serial.println("Colorful ball with realistic physics");
    Serial.println("Gravity, velocity, and elastic collisions");
    Serial.println("10 working strips active");
  #endif

  lastUpdate = millis();
}

void loop() {
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastUpdate) / 1000.0;
  lastUpdate = currentTime;

  // Clear display
  display.clear();
  
  // Update physics
  updatePhysics();
  
  // Draw the ball
  drawBall(ballX, ballY, hue);
  
  display.show();
  
  delay(16); // ~60 FPS
}

