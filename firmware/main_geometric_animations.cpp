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

// Import geometric objects
#include <Circle.h>
#include <Triangle.h>
#include <Star.h>
#include <Spiral.h>
#include <Hexagon.h>
#include <Diamond.h>
#include <Wave.h>
#include <NumberDisplay.h>

// 🎨 11 STRIPS AS A GRID! (11 x 90 = 990 LEDs)
const uint8_t LED_PINS[] = {0, 2, 4, 16, 17, 18, 19, 23, 14, 25, 26};
const uint16_t LEDS_PER_STRIP = 90;
const uint8_t NUM_STRIPS = 11;

// Grid dimensions for 2D animations!
const uint8_t GRID_WIDTH = 90;   // X axis (pixels per strip)
const uint8_t GRID_HEIGHT = 11;  // Y axis (number of strips)

// Animation control
uint8_t currentEffect = 0;
unsigned long lastEffectChange = 0;
const unsigned long EFFECT_DURATION = 15000; // 15 seconds per effect
const unsigned long NUMBER_DISPLAY_TIME = 2000; // 2 seconds for number
unsigned long effectStartTime = 0;
bool showingNumber = true;
uint16_t animationStep = 0;

// Random color arrays - regenerated each effect
uint8_t randomStripColors[11][3];
uint8_t objectColors[10][3];

// Forward declarations
void generateRandomColors();
void generateRandomObjectColors();
void displayAnimationNumber(int num);
void animation1_RotatingSpiralsWithCircles();
void animation2_TriangleStarDance();
void animation3_HexagonalWaves();
void animation4_DiamondSpiral();
void animation5_MultiShapeCollision();
void animation6_OozingCircles();
void animation7_StarburstTriangles();
void animation8_WavyHexagons();
void animation9_SpiralingDiamonds();
void animation10_GeometricChaos();

void setup() {
  // Configure all 11 LED strips as a GRID!
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    String stripName = "Y=" + String(i);
    display.addStrip(LED_PINS[i], LEDS_PER_STRIP, stripName);
  }

  display.begin();
  display.setBrightness(15);  // 6% brightness - SUPER VIBEY!

  #ifndef HARDWARE_MODE
    Serial.println("🎨 GEOMETRIC PATTERN MODE ACTIVATED 🎨");
    Serial.println("11 strips x 90 LEDs = 990 pixel canvas!");
    Serial.println("Component-based geometric animations!");
  #endif

  randomSeed(analogRead(0));
  generateRandomColors();
  generateRandomObjectColors();
  effectStartTime = millis();
}

void loop() {
  unsigned long currentTime = millis();

  // Handle effect switching
  if (currentTime - lastEffectChange > EFFECT_DURATION + NUMBER_DISPLAY_TIME) {
    lastEffectChange = currentTime;
    currentEffect = (currentEffect + 1) % 10;
    animationStep = 0;
    showingNumber = true;
    effectStartTime = currentTime;
    generateRandomColors();
    generateRandomObjectColors();

    #ifndef HARDWARE_MODE
      Serial.print("🎬 Animation ");
      Serial.println(currentEffect + 1);
    #endif
  }

  // Show number for first 2 seconds
  if (showingNumber && (currentTime - effectStartTime < NUMBER_DISPLAY_TIME)) {
    displayAnimationNumber(currentEffect + 1);
  } else {
    showingNumber = false;

    // Run geometric animation
    switch (currentEffect) {
      case 0: animation1_RotatingSpiralsWithCircles(); break;
      case 1: animation2_TriangleStarDance(); break;
      case 2: animation3_HexagonalWaves(); break;
      case 3: animation4_DiamondSpiral(); break;
      case 4: animation5_MultiShapeCollision(); break;
      case 5: animation6_OozingCircles(); break;
      case 6: animation7_StarburstTriangles(); break;
      case 7: animation8_WavyHexagons(); break;
      case 8: animation9_SpiralingDiamonds(); break;
      case 9: animation10_GeometricChaos(); break;
    }
  }

  display.show();
  animationStep++;
  delay(20); // 50fps
}

// Generate random colors for strips
void generateRandomColors() {
  for (int i = 0; i < 11; i++) {
    randomStripColors[i][0] = random(100, 255);
    randomStripColors[i][1] = random(100, 255);
    randomStripColors[i][2] = random(100, 255);
  }
}

// Generate random colors for geometric objects
void generateRandomObjectColors() {
  for (int i = 0; i < 10; i++) {
    objectColors[i][0] = random(50, 255);
    objectColors[i][1] = random(50, 255);
    objectColors[i][2] = random(50, 255);
  }
}

// Display animation number
void displayAnimationNumber(int num) {
  display.clear();

  // Center the number
  int startX = (GRID_WIDTH / 2) - 3;
  int startY = (GRID_HEIGHT / 2) - 3;

  // Use bright white for number
  NumberDisplay::renderDigit(num % 10, startX, startY, 255, 255, 255, display);
}

// ========================================
// ANIMATION 1: Rotating Spirals with Pulsing Circles
// Objects: 2 spirals (rotating opposite directions), 3 pulsing circles
// ========================================
void animation1_RotatingSpiralsWithCircles() {
  display.clear();

  static Spiral spiral1(45, 5.5, 1.2, 3);
  static Spiral spiral2(45, 5.5, 1.0, 2);
  static Circle circle1(20, 5.5, 3, objectColors[0][0], objectColors[0][1], objectColors[0][2], true);
  static Circle circle2(45, 5.5, 2, objectColors[1][0], objectColors[1][1], objectColors[1][2], true);
  static Circle circle3(70, 5.5, 3, objectColors[2][0], objectColors[2][1], objectColors[2][2], true);

  spiral1.setColor(objectColors[3][0], objectColors[3][1], objectColors[3][2]);
  spiral2.setColor(objectColors[4][0], objectColors[4][1], objectColors[4][2]);

  spiral1.renderGradient(display, GRID_WIDTH, GRID_HEIGHT);
  spiral2.renderGradient(display, GRID_WIDTH, GRID_HEIGHT);

  // Render pulsing circles
  float r1 = circle1.getPulsingRadius();
  float r2 = circle2.getPulsingRadius();
  float r3 = circle3.getPulsingRadius();

  Circle c1(circle1.centerX, circle1.centerY, r1, circle1.r, circle1.g, circle1.b, true);
  Circle c2(circle2.centerX, circle2.centerY, r2, circle2.r, circle2.g, circle2.b, true);
  Circle c3(circle3.centerX, circle3.centerY, r3, circle3.r, circle3.g, circle3.b, true);

  c1.render(display, GRID_WIDTH, GRID_HEIGHT);
  c2.render(display, GRID_WIDTH, GRID_HEIGHT);
  c3.render(display, GRID_WIDTH, GRID_HEIGHT);

  spiral1.update(0.08, 0.15);
  spiral2.update(-0.06, 0.12);
  circle1.update();
  circle2.update();
  circle3.update();
}

// ========================================
// ANIMATION 2: Triangle and Star Dance
// Objects: 2 rotating triangles, 1 multicolor star
// ========================================
void animation2_TriangleStarDance() {
  display.clear();

  static Triangle tri1(30, 5.5, 5, objectColors[0][0], objectColors[0][1], objectColors[0][2], false);
  static Triangle tri2(60, 5.5, 4, objectColors[1][0], objectColors[1][1], objectColors[1][2], true);
  static Star star(45, 5.5, 6, 3, 5);

  // Setup multicolor star
  uint8_t c1[] = {255, 0, 0};
  uint8_t c2[] = {0, 255, 0};
  uint8_t c3[] = {0, 0, 255};
  uint8_t c4[] = {255, 255, 0};
  uint8_t c5[] = {255, 0, 255};
  star.setMulticolor(c1, c2, c3, c4, c5);

  tri1.render(display, GRID_WIDTH, GRID_HEIGHT);
  tri2.render(display, GRID_WIDTH, GRID_HEIGHT);
  star.render(display, GRID_WIDTH, GRID_HEIGHT);

  tri1.update(0.1);
  tri2.update(-0.08);
  star.update(0.05);
}

// ========================================
// ANIMATION 3: Hexagonal Waves
// Objects: 3 rotating hexagons, 2 waves
// ========================================
void animation3_HexagonalWaves() {
  display.clear();

  static Hexagon hex1(25, 5.5, 4, objectColors[0][0], objectColors[0][1], objectColors[0][2], false);
  static Hexagon hex2(45, 5.5, 5, objectColors[1][0], objectColors[1][1], objectColors[1][2], true);
  static Hexagon hex3(65, 5.5, 3, objectColors[2][0], objectColors[2][1], objectColors[2][2], false);
  static Wave wave1(0.3, 2.5, objectColors[3][0], objectColors[3][1], objectColors[3][2], true, 2);
  static Wave wave2(0.25, 2.0, objectColors[4][0], objectColors[4][1], objectColors[4][2], false, 2);

  wave1.renderGradient(display, GRID_WIDTH, GRID_HEIGHT);
  wave2.renderGradient(display, GRID_WIDTH, GRID_HEIGHT);
  hex1.render(display, GRID_WIDTH, GRID_HEIGHT);
  hex2.render(display, GRID_WIDTH, GRID_HEIGHT);
  hex3.render(display, GRID_WIDTH, GRID_HEIGHT);

  hex1.update(0.06);
  hex2.update(-0.05);
  hex3.update(0.08);
  wave1.update(0.12);
  wave2.update(0.1);
}

// ========================================
// ANIMATION 4: Diamond Spiral Fusion
// Objects: 3 diamonds, 1 rainbow spiral
// ========================================
void animation4_DiamondSpiral() {
  display.clear();

  static Diamond dia1(20, 5.5, 8, 4, objectColors[0][0], objectColors[0][1], objectColors[0][2], true);
  static Diamond dia2(45, 5.5, 10, 5, objectColors[1][0], objectColors[1][1], objectColors[1][2], false);
  static Diamond dia3(70, 5.5, 6, 3, objectColors[2][0], objectColors[2][1], objectColors[2][2], true);
  static Spiral spiral(45, 5.5, 1.5, 4);

  spiral.enableRainbow();

  spiral.renderGradient(display, GRID_WIDTH, GRID_HEIGHT);
  dia1.render(display, GRID_WIDTH, GRID_HEIGHT);
  dia2.render(display, GRID_WIDTH, GRID_HEIGHT);
  dia3.render(display, GRID_WIDTH, GRID_HEIGHT);

  dia1.update(0.07, 0.1);
  dia2.update(-0.06, 0.12);
  dia3.update(0.09, 0.08);
  spiral.update(0.1, 0.2);
}

// ========================================
// ANIMATION 5: Multi-Shape Collision
// Objects: circle, triangle, hexagon, star all moving
// ========================================
void animation5_MultiShapeCollision() {
  display.clear();

  static float circle_x = 20;
  static float triangle_x = 90;
  static float hex_x = 45;
  static float star_y = 0;
  static float dx_circle = 0.3;
  static float dx_triangle = -0.25;
  static float dx_hex = 0.2;
  static float dy_star = 0.15;

  Circle circ(circle_x, 5.5, 3, objectColors[0][0], objectColors[0][1], objectColors[0][2], true);
  Triangle tri(triangle_x, 5.5, 4, objectColors[1][0], objectColors[1][1], objectColors[1][2], true);
  Hexagon hex(hex_x, 5.5, 3, objectColors[2][0], objectColors[2][1], objectColors[2][2], false);
  Star star(45, star_y, 4, 2, 5);

  circ.render(display, GRID_WIDTH, GRID_HEIGHT);
  tri.render(display, GRID_WIDTH, GRID_HEIGHT);
  hex.render(display, GRID_WIDTH, GRID_HEIGHT);
  star.render(display, GRID_WIDTH, GRID_HEIGHT);

  // Bounce logic
  circle_x += dx_circle;
  if (circle_x > 85 || circle_x < 5) dx_circle *= -1;

  triangle_x += dx_triangle;
  if (triangle_x > 85 || triangle_x < 5) dx_triangle *= -1;

  hex_x += dx_hex;
  if (hex_x > 85 || hex_x < 5) dx_hex *= -1;

  star_y += dy_star;
  if (star_y > 10 || star_y < 0) dy_star *= -1;
}

// ========================================
// ANIMATION 6: Oozing Circles
// Objects: 5 circles with changing sizes (oozing effect)
// ========================================
void animation6_OozingCircles() {
  display.clear();

  static Circle circles[5] = {
    Circle(15, 5.5, 3, objectColors[0][0], objectColors[0][1], objectColors[0][2], true),
    Circle(30, 5.5, 2.5, objectColors[1][0], objectColors[1][1], objectColors[1][2], true),
    Circle(45, 5.5, 4, objectColors[2][0], objectColors[2][1], objectColors[2][2], true),
    Circle(60, 5.5, 3.5, objectColors[3][0], objectColors[3][1], objectColors[3][2], true),
    Circle(75, 5.5, 2, objectColors[4][0], objectColors[4][1], objectColors[4][2], true)
  };

  for (int i = 0; i < 5; i++) {
    float r = circles[i].getPulsingRadius();
    Circle c(circles[i].centerX, circles[i].centerY, r, circles[i].r, circles[i].g, circles[i].b, true);
    c.render(display, GRID_WIDTH, GRID_HEIGHT);
    circles[i].update(0, 0.05 + i * 0.02); // Different pulse speeds
  }
}

// ========================================
// ANIMATION 7: Starburst Triangles
// Objects: 1 central star, 4 rotating triangles around it
// ========================================
void animation7_StarburstTriangles() {
  display.clear();

  static Star star(45, 5.5, 8, 4, 6);
  static float angle = 0;

  star.renderFilled(display, GRID_WIDTH, GRID_HEIGHT);

  // 4 triangles orbiting
  for (int i = 0; i < 4; i++) {
    float orbitAngle = angle + (i * PI / 2);
    float x = 45 + 25 * cos(orbitAngle);
    float y = 5.5 + 4 * sin(orbitAngle);

    Triangle tri(x, y, 3, objectColors[i][0], objectColors[i][1], objectColors[i][2], true);
    tri.rotation = -orbitAngle;
    tri.render(display, GRID_WIDTH, GRID_HEIGHT);
  }

  star.update(0.03);
  angle += 0.05;
}

// ========================================
// ANIMATION 8: Wavy Hexagons
// Objects: 6 hexagons arranged in wave pattern
// ========================================
void animation8_WavyHexagons() {
  display.clear();

  static float phase = 0;

  for (int i = 0; i < 6; i++) {
    float x = 15 + i * 13;
    float y = 5.5 + 3 * sin(phase + i * 0.5);

    Hexagon hex(x, y, 3, objectColors[i % 5][0], objectColors[i % 5][1], objectColors[i % 5][2], true);
    hex.rotation = phase + i;
    hex.render(display, GRID_WIDTH, GRID_HEIGHT);
  }

  phase += 0.08;
}

// ========================================
// ANIMATION 9: Spiraling Diamonds
// Objects: 3 diamonds following spiral path
// ========================================
void animation9_SpiralingDiamonds() {
  display.clear();

  static float spiral_phase = 0;

  for (int i = 0; i < 3; i++) {
    float angle = spiral_phase + (i * 2 * PI / 3);
    float radius = 15 + 10 * sin(spiral_phase * 0.5);
    float x = 45 + radius * cos(angle);
    float y = 5.5 + (radius * 0.3) * sin(angle);

    Diamond dia(x, y, 6, 4, objectColors[i][0], objectColors[i][1], objectColors[i][2], true);
    dia.rotation = angle;
    dia.render(display, GRID_WIDTH, GRID_HEIGHT);
  }

  spiral_phase += 0.1;
}

// ========================================
// ANIMATION 10: Geometric Chaos
// Objects: ALL shapes at once, total madness
// ========================================
void animation10_GeometricChaos() {
  display.clear();

  static Circle c1(20, 3, 2, objectColors[0][0], objectColors[0][1], objectColors[0][2], false);
  static Triangle t1(40, 7, 3, objectColors[1][0], objectColors[1][1], objectColors[1][2], true);
  static Star s1(60, 4, 4, 2, 5);
  static Hexagon h1(30, 8, 2.5, objectColors[2][0], objectColors[2][1], objectColors[2][2], false);
  static Diamond d1(70, 6, 5, 3, objectColors[3][0], objectColors[3][1], objectColors[3][2], true);
  static Spiral sp1(45, 5.5, 0.8, 2);
  static Wave w1(0.4, 1.5, objectColors[4][0], objectColors[4][1], objectColors[4][2], true, 3);

  w1.renderInterference(display, GRID_WIDTH, GRID_HEIGHT);
  sp1.renderGradient(display, GRID_WIDTH, GRID_HEIGHT);
  c1.renderRotating(display, GRID_WIDTH, GRID_HEIGHT, 12);
  t1.render(display, GRID_WIDTH, GRID_HEIGHT);
  s1.render(display, GRID_WIDTH, GRID_HEIGHT);
  h1.render(display, GRID_WIDTH, GRID_HEIGHT);
  d1.render(display, GRID_WIDTH, GRID_HEIGHT);

  c1.update(0.15);
  t1.update(0.1);
  s1.update(0.08);
  h1.update(-0.12);
  d1.update(0.09, 0.15);
  sp1.update(0.12, 0.18);
  w1.update(0.1);
}
