// Two Balls Collision - Warm and cool colored balls with rainbow confetti on collision
// Uses the multi-strip LED display with physical coordinate mapping
// Creates rainbow spark effects when balls collide

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

// Physical position to code strip mapping
const uint8_t PHYSICAL_TO_CODE[] = {4, 5, 3, 6, 7, 8, 9, 10, 0, 2};

// Ball structure
struct Ball {
  float x, y;              // Position
  float vx, vy;            // Velocity
  float hueStart, hueEnd;  // Color range for this ball
  bool isWarm;             // Warm or cool colors
  const float size = 0.8;  // Ball radius
};

Ball ball1;  // Warm colored ball
Ball ball2;  // Cool colored ball

// Physics constants
const float GRAVITY = 0.12;
const float BOUNCE_DAMPING = 0.82;  // Less energy loss
const float MIN_VELOCITY = 0.12;
const float COLLISION_DISTANCE = 2.5;  // Larger collision detection
bool lastFrameCollision = false;  // Track if we collided last frame

// Auto-reset system
unsigned long lastResetTime = 0;
const unsigned long RESET_INTERVAL = 25000;  // Reset every 25 seconds

// Confetti particle structure
struct Confetti {
  float x, y;
  float vx, vy;
  float hue;
  uint8_t life;  // Frames remaining
  bool active;
};

const uint8_t MAX_CONFETTI = 30;
Confetti confetti[MAX_CONFETTI];

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

// Convert HSV to RGB
void hsvToRgb(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
  float hh = fmod(h, 360.0) / 60.0;
  int i = (int)hh;
  float f = hh - i;
  float p = v * (1.0 - s);
  float q = v * (1.0 - s * f);
  float t = v * (1.0 - s * (1.0 - f));
  
  switch(i % 6) {
    case 0: r = v*255; g = t*255; b = p*255; break;
    case 1: r = q*255; g = v*255; b = p*255; break;
    case 2: r = p*255; g = v*255; b = t*255; break;
    case 3: r = p*255; g = q*255; b = v*255; break;
    case 4: r = t*255; g = p*255; b = v*255; break;
    default: r = v*255; g = p*255; b = q*255; break;
  }
}

// Get color for ball based on its color range
float getBallHue(Ball &ball) {
  // Cycle through the ball's color range
  float range = ball.hueEnd - ball.hueStart;
  float cycleHue = ball.hueStart + fmod(millis() / 30.0, range);
  return cycleHue;
}

// Draw a single ball
void drawBall(Ball &ball) {
  float ballHue = getBallHue(ball);
  uint8_t r, g, b;
  hsvToRgb(ballHue, 1.0, 1.0, r, g, b);
  
  int centerX = (int)round(ball.x);
  int centerY = (int)round(ball.y);
  
  // Center pixel - full brightness
  setDirectPixel(centerX, centerY, r, g, b);
  
  // Horizontal neighbors (glow)
  if (centerX > 0) {
    setDirectPixel(centerX - 1, centerY, r/2, g/2, b/2);
  }
  if (centerX < GRID_WIDTH - 1) {
    setDirectPixel(centerX + 1, centerY, r/2, g/2, b/2);
  }
  
  // Vertical glow based on position
  float yFraction = ball.y - centerY;
  if (yFraction < -0.3 && centerY > 0) {
    setDirectPixel(centerX, centerY - 1, r/3, g/3, b/3);
  } else if (yFraction > 0.3 && centerY < GRID_HEIGHT - 1) {
    setDirectPixel(centerX, centerY + 1, r/3, g/3, b/3);
  }
}

// Create confetti explosion at collision point
void createConfetti(float x, float y) {
  for (uint8_t i = 0; i < MAX_CONFETTI; i++) {
    if (!confetti[i].active) {
      confetti[i].x = x;
      confetti[i].y = y;
      // Random velocity in all directions
      float angle = random(0, 628) / 100.0; // 0 to 2*PI
      float speed = random(50, 200) / 100.0;
      confetti[i].vx = speed * cos(angle);
      confetti[i].vy = speed * sin(angle);
      confetti[i].hue = random(0, 360);  // Rainbow colors
      confetti[i].life = random(15, 30); // 15-30 frames
      confetti[i].active = true;
      
      // Only create a few at a time
      if (i % 3 == 0) break;
    }
  }
}

// Update and draw confetti
void updateConfetti() {
  for (uint8_t i = 0; i < MAX_CONFETTI; i++) {
    if (confetti[i].active) {
      // Apply gravity
      confetti[i].vy += GRAVITY * 0.5;
      
      // Update position
      confetti[i].x += confetti[i].vx;
      confetti[i].y += confetti[i].vy;
      
      // Decrease life
      confetti[i].life--;
      
      if (confetti[i].life <= 0) {
        confetti[i].active = false;
        continue;
      }
      
      // Draw confetti particle
      int px = (int)round(confetti[i].x);
      int py = (int)round(confetti[i].y);
      
      if (px >= 0 && px < GRID_WIDTH && py >= 0 && py < GRID_HEIGHT) {
        uint8_t r, g, b;
        // Fade out based on life
        float brightness = (float)confetti[i].life / 30.0;
        hsvToRgb(confetti[i].hue, 1.0, brightness, r, g, b);
        setDirectPixel(px, py, r, g, b);
      } else {
        // Out of bounds, deactivate
        confetti[i].active = false;
      }
    }
  }
}

// Check collision between two balls
bool checkCollision(Ball &b1, Ball &b2) {
  float dx = b1.x - b2.x;
  float dy = b1.y - b2.y;
  float distance = sqrt(dx * dx + dy * dy);
  return distance < COLLISION_DISTANCE;
}

// Handle collision between balls
void handleCollision(Ball &b1, Ball &b2) {
  // Calculate collision normal
  float dx = b2.x - b1.x;
  float dy = b2.y - b1.y;
  float distance = sqrt(dx * dx + dy * dy);
  
  if (distance == 0) return; // Avoid division by zero
  
  // Normalize
  float nx = dx / distance;
  float ny = dy / distance;
  
  // Relative velocity
  float dvx = b1.vx - b2.vx;
  float dvy = b1.vy - b2.vy;
  
  // Relative velocity in collision normal direction
  float dvn = dvx * nx + dvy * ny;
  
  // Do not resolve if velocities are separating
  if (dvn > 0) return;
  
  // Collision impulse (gentler bounce)
  float impulse = -1.2 * dvn; // Softer, less energetic
  
  // Apply impulse to both balls
  b1.vx += impulse * nx;
  b1.vy += impulse * ny;
  b2.vx -= impulse * nx;
  b2.vy -= impulse * ny;
  
  // Add small bounce energy to prevent sticking (reduced)
  b1.vx += nx * 0.15;
  b1.vy += ny * 0.15;
  b2.vx -= nx * 0.15;
  b2.vy -= ny * 0.15;
  
  // Separate balls to prevent overlap (more aggressive)
  float overlap = COLLISION_DISTANCE - distance;
  float separationX = nx * overlap * 0.6;
  float separationY = ny * overlap * 0.6;
  b1.x -= separationX;
  b1.y -= separationY;
  b2.x += separationX;
  b2.y += separationY;
  
  // Create confetti at collision point (ONLY if not colliding last frame)
  if (!lastFrameCollision) {
    float collisionX = (b1.x + b2.x) / 2.0;
    float collisionY = (b1.y + b2.y) / 2.0;
    createConfetti(collisionX, collisionY);
  }
}

// Reset both balls to top with fresh energy
void resetBalls() {
  // Ball 1 - WARM colors (from left side)
  ball1.x = random(10, 30);
  ball1.y = 1.0;  // Near top
  float angle1 = random(40, 140) / 100.0 * 3.14159;
  float speed1 = random(220, 320) / 100.0;
  ball1.vx = speed1 * cos(angle1);
  ball1.vy = speed1 * sin(angle1);  // Downward
  
  // Ball 2 - COOL colors (from right side)
  ball2.x = random(60, 80);
  ball2.y = 1.0;  // Near top
  float angle2 = random(40, 140) / 100.0 * 3.14159;
  float speed2 = random(220, 320) / 100.0;
  ball2.vx = -speed2 * cos(angle2);  // Toward left
  ball2.vy = speed2 * sin(angle2);   // Downward
  
  // Clear all confetti
  for (uint8_t i = 0; i < MAX_CONFETTI; i++) {
    confetti[i].active = false;
  }
  
  lastResetTime = millis();
}

// Update ball physics
void updateBall(Ball &ball) {
  // Apply gravity
  ball.vy += GRAVITY;
  
  // Update position
  ball.x += ball.vx;
  ball.y += ball.vy;
  
  // Wall collisions
  if (ball.x - ball.size <= 0) {
    ball.x = ball.size;
    ball.vx = -ball.vx * BOUNCE_DAMPING;
  }
  if (ball.x + ball.size >= GRID_WIDTH) {
    ball.x = GRID_WIDTH - ball.size;
    ball.vx = -ball.vx * BOUNCE_DAMPING;
  }
  
  if (ball.y - ball.size <= 0) {
    ball.y = ball.size;
    ball.vy = -ball.vy * BOUNCE_DAMPING;
  }
  if (ball.y + ball.size >= GRID_HEIGHT - 0.5) {
    ball.y = GRID_HEIGHT - 0.5 - ball.size;
    ball.vy = -ball.vy * BOUNCE_DAMPING;
    
    // Extra horizontal velocity on floor
    if (abs(ball.vx) < 0.8) {
      ball.vx += random(-100, 100) / 100.0;
    }
  }
  
  // Limit velocity
  if (ball.vx > 3.5) ball.vx = 3.5;
  if (ball.vx < -3.5) ball.vx = -3.5;
  if (ball.vy > 3.5) ball.vy = 3.5;
  if (ball.vy < -3.5) ball.vy = -3.5;
}

void setup() {
  // Configure all 12 LED strips
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    String stripName = "Strip " + String(i);
    display.addStrip(LED_PINS[i], LEDS_PER_STRIP, stripName);
  }

  display.begin();
  display.setBrightness(30);
  
  randomSeed(analogRead(0));
  
  // Initialize Ball 1 - WARM colors (red, orange, yellow)
  ball1.x = 20.0;
  ball1.y = 5.0;
  ball1.isWarm = true;
  ball1.hueStart = 0;    // Red
  ball1.hueEnd = 60;     // Yellow
  float angle1 = random(30, 150) / 100.0 * 3.14159;
  float speed1 = random(200, 300) / 100.0;
  ball1.vx = speed1 * cos(angle1);
  ball1.vy = -speed1 * sin(angle1);
  
  // Initialize Ball 2 - COOL colors (cyan, blue, purple)
  ball2.x = 70.0;
  ball2.y = 5.0;
  ball2.isWarm = false;
  ball2.hueStart = 180;  // Cyan
  ball2.hueEnd = 270;    // Purple
  float angle2 = random(30, 150) / 100.0 * 3.14159;
  float speed2 = random(200, 300) / 100.0;
  ball2.vx = -speed2 * cos(angle2);  // Opposite direction
  ball2.vy = -speed2 * sin(angle2);
  
  // Initialize confetti as inactive
  for (uint8_t i = 0; i < MAX_CONFETTI; i++) {
    confetti[i].active = false;
  }
  
  // Start the reset timer
  lastResetTime = millis();

  #ifndef HARDWARE_MODE
    Serial.println("=== TWO BALLS COLLISION ===");
    Serial.println("Warm and cool colored balls");
    Serial.println("Rainbow confetti on collision");
    Serial.println("Auto-resets every 25 seconds from top");
    Serial.println("10 working strips active");
  #endif
}

void loop() {
  // Check if it's time to reset (every 25 seconds)
  if (millis() - lastResetTime >= RESET_INTERVAL) {
    resetBalls();
  }
  
  display.clear();
  
  // Update physics
  updateBall(ball1);
  updateBall(ball2);
  
  // Check for collision between balls
  bool currentCollision = checkCollision(ball1, ball2);
  if (currentCollision) {
    handleCollision(ball1, ball2);
  }
  
  // Update collision tracking for next frame
  lastFrameCollision = currentCollision;
  
  // Update and draw confetti
  updateConfetti();
  
  // Draw balls
  drawBall(ball1);
  drawBall(ball2);
  
  display.show();
  delay(16); // ~60 FPS
}

