#include <LEDDisplay.h>

#ifdef HARDWARE_MODE
  #include <MultiStripHardware.h>
  MultiStripHardware display;
#else
  #include <MultiStripSimulator.h>
  MultiStripSimulator display;
#endif

// Your 12 GPIO pins for LED strips
const uint8_t LED_PINS[] = {0, 2, 4, 16, 17, 18, 19, 23, 14, 25, 26, 27};
const uint16_t LEDS_PER_STRIP = 90;
const uint8_t NUM_STRIPS = 12;

// 🎨 EACH STRIP GETS ITS OWN COLOR - You'll define what each represents!
// Format: {R, G, B} - feel free to change these!
uint8_t stripColors[12][3] = {
  {255, 50, 50},    // Strip 1: Deep Red
  {255, 100, 0},    // Strip 2: Orange
  {255, 200, 0},    // Strip 3: Golden Yellow
  {100, 255, 50},   // Strip 4: Spring Green
  {0, 255, 150},    // Strip 5: Cyan
  {0, 200, 255},    // Strip 6: Sky Blue
  {100, 100, 255},  // Strip 7: Soft Blue
  {180, 100, 255},  // Strip 8: Purple
  {255, 50, 200},   // Strip 9: Magenta
  {255, 100, 150},  // Strip 10: Pink
  {255, 255, 255},  // Strip 11: Pure White
  {200, 150, 255}   // Strip 12: Lavender
};

// Animation control
uint8_t currentEffect = 0;
unsigned long lastEffectChange = 0;
const unsigned long EFFECT_DURATION = 12000; // 12 seconds - faster transitions!
uint16_t animationStep = 0;

// Particle system for fireworks!
struct Particle {
  float pos;
  float velocity;
  uint8_t life;
  uint8_t brightness;
};

// Forward declarations - EXPLOSIVE CHAOS MODE
void fireworksExplode();
void chaosLightning();
void randomBurst();
void particleStorm();
void waveCrash();
void energyPulse();
void quantumFlicker();
void supernovaBlast();
void digitalRain();
void cosmicChaos();
uint32_t getStripColor(uint8_t strip, uint8_t brightness);
float easeInOutCubic(float t);
float randomFloat(float min, float max);

void setup() {
  // Configure all 12 LED strips
  for (uint8_t i = 0; i < NUM_STRIPS; i++) {
    String stripName = "Strip " + String(i + 1);
    display.addStrip(LED_PINS[i], LEDS_PER_STRIP, stripName);
  }

  display.begin();
  display.setBrightness(26);  // 10% brightness - perfect for vibes

  #ifndef HARDWARE_MODE
    Serial.println("💥 CLOUD DJ - FIREWORKS MODE ACTIVATED 💥");
    Serial.println("🎆 EXPLOSIVE. CHAOTIC. ELECTRIC. 🎆");
    Serial.println("Breaking symmetry. Finding flow. LET'S GOOO!");
  #endif

  randomSeed(analogRead(0));
}

void loop() {
  // Faster, more chaotic transitions
  if (millis() - lastEffectChange > EFFECT_DURATION) {
    lastEffectChange = millis();
    currentEffect = (currentEffect + 1) % 10; // 10 explosive effects
    animationStep = 0;

    #ifndef HARDWARE_MODE
      const char* effectNames[] = {
        "💥 FIREWORKS EXPLODE", "⚡ CHAOS LIGHTNING", "🎇 RANDOM BURST",
        "🌪️  PARTICLE STORM", "🌊 WAVE CRASH", "⚡ ENERGY PULSE",
        "✨ QUANTUM FLICKER", "💫 SUPERNOVA BLAST", "🌧️  DIGITAL RAIN", "🌌 COSMIC CHAOS"
      };
      Serial.print("🔥 UNLEASHING: ");
      Serial.println(effectNames[currentEffect]);
    #endif
  }

  // UNLEASH THE CHAOS
  switch (currentEffect) {
    case 0: fireworksExplode(); break;
    case 1: chaosLightning(); break;
    case 2: randomBurst(); break;
    case 3: particleStorm(); break;
    case 4: waveCrash(); break;
    case 5: energyPulse(); break;
    case 6: quantumFlicker(); break;
    case 7: supernovaBlast(); break;
    case 8: digitalRain(); break;
    case 9: cosmicChaos(); break;
  }

  animationStep++;
  delay(20); // 50fps chaos
}

// ═══════════════════════════════════════════════════════════════
// 💥 EFFECT 1: FIREWORKS EXPLODE - Particles shoot out in all directions!
// ═══════════════════════════════════════════════════════════════
void fireworksExplode() {
  static Particle particles[12][20];  // 20 particles per strip
  static bool initialized = false;

  if (!initialized) {
    for (uint8_t s = 0; s < NUM_STRIPS; s++) {
      for (uint8_t p = 0; p < 20; p++) {
        particles[s][p].life = 0;
      }
    }
    initialized = true;
  }

  // Random new explosions!
  if (random(100) < 15) {
    uint8_t strip = random(NUM_STRIPS);
    uint8_t center = random(20, LEDS_PER_STRIP - 20);

    // Launch particles in random directions
    for (uint8_t p = 0; p < 20; p++) {
      if (particles[strip][p].life == 0) {
        particles[strip][p].pos = center;
        particles[strip][p].velocity = randomFloat(-3.5, 3.5);
        particles[strip][p].life = random(30, 80);
        particles[strip][p].brightness = 255;
        break;
      }
    }
  }

  // Clear all
  display.clear();

  // Update and draw particles
  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    for (uint8_t p = 0; p < 20; p++) {
      if (particles[strip][p].life > 0) {
        particles[strip][p].pos += particles[strip][p].velocity;
        particles[strip][p].life--;
        particles[strip][p].brightness = (particles[strip][p].life * 255) / 80;

        if (particles[strip][p].pos >= 0 && particles[strip][p].pos < LEDS_PER_STRIP) {
          uint32_t color = getStripColor(strip, particles[strip][p].brightness);
          display.setPixelOnStrip(strip, (uint8_t)particles[strip][p].pos,
            (uint8_t)((color >> 16) & 0xFF),
            (uint8_t)((color >> 8) & 0xFF),
            (uint8_t)(color & 0xFF));
        }
      }
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// ⚡ EFFECT 2: CHAOS LIGHTNING - Random strikes across strips!
// ═══════════════════════════════════════════════════════════════
void chaosLightning() {
  static uint8_t lightningBuffer[12][90];

  // Fade everything
  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    for (uint16_t pixel = 0; pixel < LEDS_PER_STRIP; pixel++) {
      lightningBuffer[strip][pixel] = lightningBuffer[strip][pixel] * 0.85;
    }
  }

  // Random STRIKES!
  if (random(100) < 25) {
    uint8_t strip = random(NUM_STRIPS);
    uint8_t strikePoint = random(LEDS_PER_STRIP);
    uint8_t strikeLength = random(10, 30);

    for (int i = 0; i < strikeLength; i++) {
      int pos = strikePoint + i - strikeLength/2;
      if (pos >= 0 && pos < LEDS_PER_STRIP) {
        lightningBuffer[strip][pos] = 255;
      }
    }
  }

  // Render
  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    for (uint16_t pixel = 0; pixel < LEDS_PER_STRIP; pixel++) {
      uint32_t color = getStripColor(strip, lightningBuffer[strip][pixel]);
      display.setPixelOnStrip(strip, pixel,
        (uint8_t)((color >> 16) & 0xFF),
        (uint8_t)((color >> 8) & 0xFF),
        (uint8_t)(color & 0xFF));
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 🎇 EFFECT 3: RANDOM BURST - Strips randomly explode to full brightness!
// ═══════════════════════════════════════════════════════════════
void randomBurst() {
  static uint8_t stripBrightness[12];
  static uint8_t burstDecay[12];

  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    // Random BURST!
    if (random(100) < 4 && stripBrightness[strip] == 0) {
      stripBrightness[strip] = 255;
      burstDecay[strip] = random(5, 15);  // How fast it fades
    }

    // Decay
    if (stripBrightness[strip] > 0) {
      int newBright = stripBrightness[strip] - burstDecay[strip];
      stripBrightness[strip] = (newBright > 0) ? newBright : 0;
    }

    uint32_t color = getStripColor(strip, stripBrightness[strip]);
    for (uint16_t pixel = 0; pixel < LEDS_PER_STRIP; pixel++) {
      display.setPixelOnStrip(strip, pixel,
        (uint8_t)((color >> 16) & 0xFF),
        (uint8_t)((color >> 8) & 0xFF),
        (uint8_t)(color & 0xFF));
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 🌪️ EFFECT 4: PARTICLE STORM - Chaotic particles everywhere!
// ═══════════════════════════════════════════════════════════════
void particleStorm() {
  static uint8_t stormBuffer[12][90];

  // Fade
  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    for (uint16_t pixel = 0; pixel < LEDS_PER_STRIP; pixel++) {
      stormBuffer[strip][pixel] = stormBuffer[strip][pixel] * 0.88;
    }
  }

  // MASSIVE CHAOS - tons of random particles
  for (int i = 0; i < 25; i++) {
    uint8_t strip = random(NUM_STRIPS);
    uint8_t pixel = random(LEDS_PER_STRIP);
    stormBuffer[strip][pixel] = random(150, 255);
  }

  // Render
  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    for (uint16_t pixel = 0; pixel < LEDS_PER_STRIP; pixel++) {
      uint32_t color = getStripColor(strip, stormBuffer[strip][pixel]);
      display.setPixelOnStrip(strip, pixel,
        (uint8_t)((color >> 16) & 0xFF),
        (uint8_t)((color >> 8) & 0xFF),
        (uint8_t)(color & 0xFF));
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 🌊 EFFECT 5: WAVE CRASH - Breaking waves, asymmetric chaos!
// ═══════════════════════════════════════════════════════════════
void waveCrash() {
  static float wavePositions[12];
  static float waveVelocities[12];
  static bool initialized = false;

  if (!initialized) {
    for (uint8_t s = 0; s < NUM_STRIPS; s++) {
      wavePositions[s] = random(LEDS_PER_STRIP);
      waveVelocities[s] = randomFloat(1.5, 4.5);
    }
    initialized = true;
  }

  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    // Move wave
    wavePositions[strip] += waveVelocities[strip];
    if (wavePositions[strip] >= LEDS_PER_STRIP) {
      wavePositions[strip] = 0;
      waveVelocities[strip] = randomFloat(1.5, 4.5);
    }

    for (uint16_t pixel = 0; pixel < LEDS_PER_STRIP; pixel++) {
      float distance = abs((float)pixel - wavePositions[strip]);
      uint8_t brightness = (distance < 15) ? (255 - (distance * 17)) : 0;

      uint32_t color = getStripColor(strip, brightness);
      display.setPixelOnStrip(strip, pixel,
        (uint8_t)((color >> 16) & 0xFF),
        (uint8_t)((color >> 8) & 0xFF),
        (uint8_t)(color & 0xFF));
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// ⚡ EFFECT 6: ENERGY PULSE - Random strips pulse at different times!
// ═══════════════════════════════════════════════════════════════
void energyPulse() {
  static float pulsePhases[12];
  static float pulseRates[12];
  static bool initialized = false;

  if (!initialized) {
    for (uint8_t s = 0; s < NUM_STRIPS; s++) {
      pulsePhases[s] = randomFloat(0, 6.28);
      pulseRates[s] = randomFloat(0.05, 0.15);
    }
    initialized = true;
  }

  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    pulsePhases[strip] += pulseRates[strip];
    float brightness = (sin(pulsePhases[strip]) + 1.0) * 127.5;

    uint32_t color = getStripColor(strip, (uint8_t)brightness);
    for (uint16_t pixel = 0; pixel < LEDS_PER_STRIP; pixel++) {
      display.setPixelOnStrip(strip, pixel,
        (uint8_t)((color >> 16) & 0xFF),
        (uint8_t)((color >> 8) & 0xFF),
        (uint8_t)(color & 0xFF));
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// ✨ EFFECT 7: QUANTUM FLICKER - Unpredictable micro-fluctuations!
// ═══════════════════════════════════════════════════════════════
void quantumFlicker() {
  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    for (uint16_t pixel = 0; pixel < LEDS_PER_STRIP; pixel++) {
      // Each pixel randomly flickers
      uint8_t baseB = 100;
      uint8_t flicker = random(155);
      uint8_t brightness = baseB + flicker;

      uint32_t color = getStripColor(strip, brightness);
      display.setPixelOnStrip(strip, pixel,
        (uint8_t)((color >> 16) & 0xFF),
        (uint8_t)((color >> 8) & 0xFF),
        (uint8_t)(color & 0xFF));
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 💫 EFFECT 8: SUPERNOVA BLAST - Massive explosions from strip centers!
// ═══════════════════════════════════════════════════════════════
void supernovaBlast() {
  static uint16_t blastTime[12];
  static bool blasting[12];

  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    // Random supernova trigger
    if (!blasting[strip] && random(100) < 2) {
      blasting[strip] = true;
      blastTime[strip] = 0;
    }

    for (uint16_t pixel = 0; pixel < LEDS_PER_STRIP; pixel++) {
      uint8_t brightness = 0;

      if (blasting[strip]) {
        float center = LEDS_PER_STRIP / 2.0;
        float distance = abs((float)pixel - center);
        float radius = blastTime[strip] * 1.5;

        if (abs(distance - radius) < 10) {
          float fade = 1.0 - (blastTime[strip] / 60.0);
          brightness = (uint8_t)(fade * 255);
        }
      }

      uint32_t color = getStripColor(strip, brightness);
      display.setPixelOnStrip(strip, pixel,
        (uint8_t)((color >> 16) & 0xFF),
        (uint8_t)((color >> 8) & 0xFF),
        (uint8_t)(color & 0xFF));
    }

    if (blasting[strip]) {
      blastTime[strip]++;
      if (blastTime[strip] > 60) blasting[strip] = false;
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 🌧️ EFFECT 9: DIGITAL RAIN - Random cascading data!
// ═══════════════════════════════════════════════════════════════
void digitalRain() {
  static uint8_t rainBuffer[12][90];

  // Shift everything down
  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    for (int pixel = LEDS_PER_STRIP - 1; pixel > 0; pixel--) {
      rainBuffer[strip][pixel] = rainBuffer[strip][pixel - 1] * 0.92;
    }

    // Random new drops at top
    rainBuffer[strip][0] = (random(100) < 20) ? random(200, 255) : 0;
  }

  // Render
  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    for (uint16_t pixel = 0; pixel < LEDS_PER_STRIP; pixel++) {
      uint32_t color = getStripColor(strip, rainBuffer[strip][pixel]);
      display.setPixelOnStrip(strip, pixel,
        (uint8_t)((color >> 16) & 0xFF),
        (uint8_t)((color >> 8) & 0xFF),
        (uint8_t)(color & 0xFF));
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 🌌 EFFECT 10: COSMIC CHAOS - Everything at once. MAXIMUM ENERGY!
// ═══════════════════════════════════════════════════════════════
void cosmicChaos() {
  for (uint8_t strip = 0; strip < NUM_STRIPS; strip++) {
    // Each strip gets random chaos style
    uint8_t chaosType = (strip + (animationStep / 20)) % 4;

    for (uint16_t pixel = 0; pixel < LEDS_PER_STRIP; pixel++) {
      uint8_t brightness = 0;

      switch (chaosType) {
        case 0: // Random flicker
          brightness = random(50, 255);
          break;
        case 1: // Wave
          brightness = (uint8_t)((sin((pixel + animationStep) * 0.2) + 1.0) * 127.5);
          break;
        case 2: // Pulse
          brightness = (uint8_t)((sin(animationStep * 0.1) + 1.0) * 127.5);
          break;
        case 3: // Chase
          brightness = ((pixel + animationStep * 2) % 20 < 5) ? 255 : 50;
          break;
      }

      uint32_t color = getStripColor(strip, brightness);
      display.setPixelOnStrip(strip, pixel,
        (uint8_t)((color >> 16) & 0xFF),
        (uint8_t)((color >> 8) & 0xFF),
        (uint8_t)(color & 0xFF));
    }
  }
  display.show();
}

// ═══════════════════════════════════════════════════════════════
// 🎨 HELPER FUNCTIONS - The magic behind the scenes
// ═══════════════════════════════════════════════════════════════

// Get strip's base color with brightness applied
uint32_t getStripColor(uint8_t strip, uint8_t brightness) {
  if (strip >= NUM_STRIPS) strip = 0;

  uint8_t r = (stripColors[strip][0] * brightness) / 255;
  uint8_t g = (stripColors[strip][1] * brightness) / 255;
  uint8_t b = (stripColors[strip][2] * brightness) / 255;

  return LEDDisplay::Color(r, g, b);
}

// Smooth easing function for organic movement
float easeInOutCubic(float t) {
  if (t < 0.5) {
    return 4 * t * t * t;
  } else {
    float f = (2 * t - 2);
    return 0.5 * f * f * f + 1;
  }
}

// Random float helper - for MAXIMUM CHAOS
float randomFloat(float min, float max) {
  return min + (random(10000) / 10000.0) * (max - min);
}

// ═══════════════════════════════════════════════════════════════
// 💥 END OF EXPLOSIVE FIREWORKS PERFORMANCE 💥
// ═══════════════════════════════════════════════════════════════