#include <LEDDisplay.h>

#ifdef HARDWARE_MODE
  #include <MultiStripHardware.h>
  MultiStripHardware display;
#else
  #include <MultiStripSimulator.h>
  MultiStripSimulator display;
#endif

// 🎨 11 STRIPS AS A GRID! (11 x 90 = 990 LEDs)
const uint8_t LED_PINS[] = {0, 2, 4, 16, 17, 18, 19, 23, 14, 25, 26};
const uint16_t LEDS_PER_STRIP = 90;
const uint8_t NUM_STRIPS = 11;

// Grid dimensions for 2D animations!
const uint8_t GRID_WIDTH = 90;   // X axis (pixels per strip)
const uint8_t GRID_HEIGHT = 11;  // Y axis (number of strips)

// 🌈 YOUR CUSTOM COLOR PALETTE!
uint8_t stripColors[11][3] = {
  {255, 105, 180},  // 1: Pink
  {200, 100, 200},  // 2: Purple/Pink
  {255, 200, 150},  // 3: Peach
  {180, 150, 230},  // 4: Light Purple
  {135, 206, 250},  // 5: Light Blue
  {64, 224, 208},   // 6: Turquoise
  {152, 255, 152},  // 7: Mint Green
  {255, 255, 255},  // 8: White
  {176, 224, 230},  // 9: Powder Blue
  {191, 255, 0},    // 10: Lime
  {200, 255, 100}   // 11: Yellower Green
};

// Animation control
uint8_t currentEffect = 0;
unsigned long lastEffectChange = 0;
const unsigned long EFFECT_DURATION = 15000; // 15 seconds per effect
uint16_t animationStep = 0;

// Forward declarations - 2D GRID MAGIC!
void plasma2D();
void snakeTrail();
void matrixRain();
void radialWave();
void gameOfLife();
void xyScanner();
void spiralOut();
void waterRipple();
void diagonalWipe();
void perlinNoise2D();
uint32_t getStripColor(uint8_t strip, uint8_t brightness);
void setXY(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void setXYColor(uint8_t x, uint8_t y, uint32_t color);
float distance2D(float x1, float y1, float x2, float y2);

void setup() {
  // Configure all 11 LED strips as a GRID!
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    String stripName = "Y=" + String(i);
    display.addStrip(LED_PINS[i], LEDS_PER_STRIP, stripName);
  }

  display.begin();
  display.setBrightness(15);  // 6% brightness - SUPER VIBEY!

  #ifndef HARDWARE_MODE
    Serial.println("🎨 2D GRID MODE ACTIVATED 🎨");
    Serial.println("11 strips x 90 LEDs = 990 pixel canvas!");
    Serial.println("XY mapping enabled. Let's paint!");
  #endif

  randomSeed(analogRead(0));
}

void loop() {
  // Cycle through 2D grid effects
  if (millis() - lastEffectChange > EFFECT_DURATION) {
    lastEffectChange = millis();
    currentEffect = (currentEffect + 1) % 10;
    animationStep = 0;

    #ifndef HARDWARE_MODE
      const char* effectNames[] = {
        "🌊 PLASMA 2D", "🐍 SNAKE TRAIL", "🌧️ MATRIX RAIN",
        "📡 RADIAL WAVE", "🧬 GAME OF LIFE", "📺 XY SCANNER",
        "🌀 SPIRAL OUT", "💧 WATER RIPPLE", "📐 DIAGONAL WIPE", "🎨 PERLIN NOISE"
      };
      Serial.print("🎬 Now showing: ");
      Serial.println(effectNames[currentEffect]);
    #endif
  }

  // Run 2D grid effect
  switch (currentEffect) {
    case 0: plasma2D(); break;
    case 1: snakeTrail(); break;
    case 2: matrixRain(); break;
    case 3: radialWave(); break;
    case 4: gameOfLife(); break;
    case 5: xyScanner(); break;
    case 6: spiralOut(); break;
    case 7: waterRipple(); break;
    case 8: diagonalWipe(); break;
    case 9: perlinNoise2D(); break;
  }

  animationStep++;
  delay(20); // 50fps
}

// ═══════════════════════════════════════════════════════════════
// 🌊 EFFECT 1: PLASMA 2D - Flowing organic patterns across the grid
// ═══════════════════════════════════════════════════════════════
void plasma2D() {
  for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
    for (uint8_t x = 0; x < GRID_WIDTH; x++) {
      // Multiple sine waves create organic plasma
      float v1 = sin(x * 0.05 + animationStep * 0.03);
      float v2 = sin(y * 0.1 + animationStep * 0.02);
      float v3 = sin((x + y) * 0.05 + animationStep * 0.04);
      float plasma = (v1 + v2 + v3 + 3.0) / 6.0;

      uint8_t brightness = (uint8_t)(plasma * 255);
      uint32_t color = getStripColor(y, brightness);
      setXYColor(x, y, color);
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 🐍 EFFECT 2: SNAKE TRAIL - Classic snake moving across the grid!
// ═══════════════════════════════════════════════════════════════
void snakeTrail() {
  static uint8_t snakeX[100];
  static uint8_t snakeY[100];
  static uint8_t snakeLength = 20;
  static int8_t dirX = 1;
  static int8_t dirY = 0;
  static bool initialized = false;

  if (!initialized) {
    for (int i = 0; i < snakeLength; i++) {
      snakeX[i] = GRID_WIDTH / 2 - i;
      snakeY[i] = GRID_HEIGHT / 2;
    }
    initialized = true;
  }

  // Update snake every few frames
  if (animationStep % 3 == 0) {
    // Move snake head
    uint8_t newX = snakeX[0] + dirX;
    uint8_t newY = snakeY[0] + dirY;

    // Wrap around edges
    if (newX >= GRID_WIDTH) newX = 0;
    if (newY >= GRID_HEIGHT) newY = 0;

    // Shift body
    for (int i = snakeLength - 1; i > 0; i--) {
      snakeX[i] = snakeX[i - 1];
      snakeY[i] = snakeY[i - 1];
    }
    snakeX[0] = newX;
    snakeY[0] = newY;

    // Random direction change
    if (random(100) < 5) {
      dirX = random(-1, 2);
      dirY = (dirX == 0) ? (random(2) * 2 - 1) : 0;
    }
  }

  // Draw snake with fading trail
  display.clear();
  for (int i = 0; i < snakeLength; i++) {
    uint8_t brightness = 255 - (i * 255 / snakeLength);
    uint32_t color = getStripColor(snakeY[i], brightness);
    setXYColor(snakeX[i], snakeY[i], color);
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 🌧️ EFFECT 3: MATRIX RAIN - Digital rain falling down!
// ═══════════════════════════════════════════════════════════════
void matrixRain() {
  static uint8_t rainBuffer[90][11];

  // Shift everything down
  for (uint8_t x = 0; x < GRID_WIDTH; x++) {
    for (int y = GRID_HEIGHT - 1; y > 0; y--) {
      rainBuffer[x][y] = rainBuffer[x][y - 1] * 0.9;
    }

    // Random new drops at top
    rainBuffer[x][0] = (random(100) < 15) ? 255 : 0;
  }

  // Render
  for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
    for (uint8_t x = 0; x < GRID_WIDTH; x++) {
      uint32_t color = getStripColor(y, rainBuffer[x][y]);
      setXYColor(x, y, color);
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 📡 EFFECT 4: RADIAL WAVE - Expanding circles from center!
// ═══════════════════════════════════════════════════════════════
void radialWave() {
  float centerX = GRID_WIDTH / 2.0;
  float centerY = GRID_HEIGHT / 2.0;
  float radius = (animationStep * 0.3);

  for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
    for (uint8_t x = 0; x < GRID_WIDTH; x++) {
      float dist = distance2D(x, y, centerX, centerY);
      float wave = sin((dist - radius) * 0.3) + 1.0;
      uint8_t brightness = (uint8_t)(wave * 127.5);

      uint32_t color = getStripColor(y, brightness);
      setXYColor(x, y, color);
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 🧬 EFFECT 5: GAME OF LIFE - Conway's cellular automata!
// ═══════════════════════════════════════════════════════════════
void gameOfLife() {
  static bool grid[90][11];
  static bool newGrid[90][11];
  static bool initialized = false;

  if (!initialized || animationStep % 150 == 0) {
    // Random initialization
    for (uint8_t x = 0; x < GRID_WIDTH; x++) {
      for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
        grid[x][y] = random(100) < 30;
      }
    }
    initialized = true;
  }

  if (animationStep % 10 == 0) {
    // Update Game of Life
    for (uint8_t x = 0; x < GRID_WIDTH; x++) {
      for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
        int neighbors = 0;
        for (int8_t dx = -1; dx <= 1; dx++) {
          for (int8_t dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            int nx = (x + dx + GRID_WIDTH) % GRID_WIDTH;
            int ny = (y + dy + GRID_HEIGHT) % GRID_HEIGHT;
            if (grid[nx][ny]) neighbors++;
          }
        }

        // Conway's rules
        if (grid[x][y]) {
          newGrid[x][y] = (neighbors == 2 || neighbors == 3);
        } else {
          newGrid[x][y] = (neighbors == 3);
        }
      }
    }

    // Copy new grid
    for (uint8_t x = 0; x < GRID_WIDTH; x++) {
      for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
        grid[x][y] = newGrid[x][y];
      }
    }
  }

  // Render
  for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
    for (uint8_t x = 0; x < GRID_WIDTH; x++) {
      uint8_t brightness = grid[x][y] ? 255 : 0;
      uint32_t color = getStripColor(y, brightness);
      setXYColor(x, y, color);
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 📺 EFFECT 6: XY SCANNER - Classic raster scan pattern!
// ═══════════════════════════════════════════════════════════════
void xyScanner() {
  display.clear();

  // Horizontal scan line
  uint8_t scanY = (animationStep / 3) % GRID_HEIGHT;
  for (uint8_t x = 0; x < GRID_WIDTH; x++) {
    uint32_t color = getStripColor(scanY, 255);
    setXYColor(x, scanY, color);
  }

  // Vertical scan line
  uint8_t scanX = (animationStep * 2) % GRID_WIDTH;
  for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
    uint32_t color = getStripColor(y, 180);
    setXYColor(scanX, y, color);
  }

  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 🌀 EFFECT 7: SPIRAL OUT - Spiral pattern from center!
// ═══════════════════════════════════════════════════════════════
void spiralOut() {
  float centerX = GRID_WIDTH / 2.0;
  float centerY = GRID_HEIGHT / 2.0;

  for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
    for (uint8_t x = 0; x < GRID_WIDTH; x++) {
      float dx = x - centerX;
      float dy = y - centerY;
      float angle = atan2(dy, dx);
      float dist = distance2D(x, y, centerX, centerY);

      float spiral = fmod(dist * 0.3 - angle - animationStep * 0.05, 6.28);
      uint8_t brightness = (uint8_t)((sin(spiral) + 1.0) * 127.5);

      uint32_t color = getStripColor(y, brightness);
      setXYColor(x, y, color);
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 💧 EFFECT 8: WATER RIPPLE - Multiple ripples expanding!
// ═══════════════════════════════════════════════════════════════
void waterRipple() {
  static float rippleX[5];
  static float rippleY[5];
  static float rippleRadius[5];
  static bool initialized = false;

  if (!initialized) {
    for (int i = 0; i < 5; i++) {
      rippleX[i] = random(GRID_WIDTH);
      rippleY[i] = random(GRID_HEIGHT);
      rippleRadius[i] = random(20, 40);
    }
    initialized = true;
  }

  for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
    for (uint8_t x = 0; x < GRID_WIDTH; x++) {
      float totalBrightness = 0;

      for (int i = 0; i < 5; i++) {
        float dist = distance2D(x, y, rippleX[i], rippleY[i]);
        float wave = sin((dist - rippleRadius[i]) * 0.5);
        if (wave > 0) totalBrightness += wave * 80;
      }

      uint8_t brightness = (uint8_t)min(255.0f, totalBrightness);
      uint32_t color = getStripColor(y, brightness);
      setXYColor(x, y, color);
    }
  }

  // Expand ripples
  for (int i = 0; i < 5; i++) {
    rippleRadius[i] += 0.5;
    if (rippleRadius[i] > 80) {
      rippleX[i] = random(GRID_WIDTH);
      rippleY[i] = random(GRID_HEIGHT);
      rippleRadius[i] = 0;
    }
  }

  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 📐 EFFECT 9: DIAGONAL WIPE - Diagonal sweeping pattern!
// ═══════════════════════════════════════════════════════════════
void diagonalWipe() {
  float wipePos = (animationStep * 0.5);

  for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
    for (uint8_t x = 0; x < GRID_WIDTH; x++) {
      float diagonal = x + y * 8;
      float dist = abs(diagonal - wipePos);

      uint8_t brightness = (dist < 15) ? (255 - (dist * 17)) : 0;
      uint32_t color = getStripColor(y, brightness);
      setXYColor(x, y, color);
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 🎨 EFFECT 10: PERLIN NOISE 2D - Organic flowing noise patterns!
// ═══════════════════════════════════════════════════════════════
void perlinNoise2D() {
  for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
    for (uint8_t x = 0; x < GRID_WIDTH; x++) {
      // Pseudo-Perlin noise using multiple sine waves
      float noise = 0;
      noise += sin(x * 0.1 + animationStep * 0.01) * 0.5;
      noise += sin(y * 0.15 + animationStep * 0.02) * 0.3;
      noise += sin((x + y) * 0.08 + animationStep * 0.015) * 0.2;

      uint8_t brightness = (uint8_t)((noise + 1.0) * 127.5);
      uint32_t color = getStripColor(y, brightness);
      setXYColor(x, y, color);
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 🛠️ HELPER FUNCTIONS - Grid utilities
// ═══════════════════════════════════════════════════════════════

// Set pixel at XY coordinate with RGB
void setXY(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
  if (x >= GRID_WIDTH || y >= GRID_HEIGHT) return;
  display.setPixelOnStrip(y, x, r, g, b);
}

// Set pixel at XY coordinate with color
void setXYColor(uint8_t x, uint8_t y, uint32_t color) {
  if (x >= GRID_WIDTH || y >= GRID_HEIGHT) return;
  display.setPixelOnStrip(y, x,
    (uint8_t)((color >> 16) & 0xFF),
    (uint8_t)((color >> 8) & 0xFF),
    (uint8_t)(color & 0xFF));
}

// Get strip's base color with brightness applied
uint32_t getStripColor(uint8_t strip, uint8_t brightness) {
  if (strip >= NUM_STRIPS) strip = 0;

  uint8_t r = (stripColors[strip][0] * brightness) / 255;
  uint8_t g = (stripColors[strip][1] * brightness) / 255;
  uint8_t b = (stripColors[strip][2] * brightness) / 255;

  return LEDDisplay::Color(r, g, b);
}

// Calculate 2D distance
float distance2D(float x1, float y1, float x2, float y2) {
  float dx = x2 - x1;
  float dy = y2 - y1;
  return sqrt(dx * dx + dy * dy);
}

// ═══════════════════════════════════════════════════════════════
// 🎬 END OF 2D GRID ANIMATIONS
// ═══════════════════════════════════════════════════════════════
