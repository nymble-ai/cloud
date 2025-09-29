#ifndef STRIP_CONFIG_H
#define STRIP_CONFIG_H

#include <Arduino.h>
#include "StripManager.h"
#include "Effects.h"

// Predefined color palettes
namespace Colors {
    const uint32_t RED = 0xFF0000;
    const uint32_t GREEN = 0x00FF00;
    const uint32_t BLUE = 0x0000FF;
    const uint32_t WHITE = 0xFFFFFF;
    const uint32_t YELLOW = 0xFFFF00;
    const uint32_t CYAN = 0x00FFFF;
    const uint32_t MAGENTA = 0xFF00FF;
    const uint32_t ORANGE = 0xFF8000;
    const uint32_t PURPLE = 0x8000FF;
    const uint32_t PINK = 0xFF1493;
    const uint32_t WARM_WHITE = 0xFFD700;
    const uint32_t COOL_WHITE = 0xE0FFFF;
}

// Configuration builder for easy setup
class StripConfigBuilder {
private:
    StripManager* manager;
    Effects* effects;

public:
    StripConfigBuilder(StripManager* mgr, Effects* fx)
        : manager(mgr), effects(fx) {}

    // Quick configuration methods
    uint8_t addRainbowStrip(uint8_t pin, uint16_t numLEDs, const String& name = "Rainbow") {
        uint8_t index = manager->addStrip(pin, numLEDs, name);
        manager->setEffect(index, Effects::rainbow(5));
        return index;
    }

    uint8_t addSolidStrip(uint8_t pin, uint16_t numLEDs, uint32_t color, const String& name = "Solid") {
        uint8_t index = manager->addStrip(pin, numLEDs, name);
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        manager->setEffect(index, Effects::solidColor(r, g, b));
        return index;
    }

    uint8_t addChaseStrip(uint8_t pin, uint16_t numLEDs, uint32_t color, const String& name = "Chase") {
        uint8_t index = manager->addStrip(pin, numLEDs, name);
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        manager->setEffect(index, Effects::chase(r, g, b, 5, 50));
        return index;
    }

    uint8_t addBreathingStrip(uint8_t pin, uint16_t numLEDs, uint32_t color, const String& name = "Breathing") {
        uint8_t index = manager->addStrip(pin, numLEDs, name);
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        manager->setEffect(index, Effects::breathing(r, g, b, 10));
        return index;
    }

    uint8_t addSparkleStrip(uint8_t pin, uint16_t numLEDs, uint32_t color, const String& name = "Sparkle") {
        uint8_t index = manager->addStrip(pin, numLEDs, name);
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        manager->setEffect(index, Effects::sparkle(r, g, b, 5, 50));
        return index;
    }

    uint8_t addFireStrip(uint8_t pin, uint16_t numLEDs, const String& name = "Fire") {
        uint8_t index = manager->addStrip(pin, numLEDs, name);
        manager->setEffect(index, Effects::fire(55, 120));
        return index;
    }

    uint8_t addWaveStrip(uint8_t pin, uint16_t numLEDs, uint32_t color, const String& name = "Wave") {
        uint8_t index = manager->addStrip(pin, numLEDs, name);
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        manager->setEffect(index, Effects::wave(r, g, b, 20, 30));
        return index;
    }

    uint8_t addGradientStrip(uint8_t pin, uint16_t numLEDs, uint32_t color1, uint32_t color2, const String& name = "Gradient") {
        uint8_t index = manager->addStrip(pin, numLEDs, name);
        uint8_t r1 = (color1 >> 16) & 0xFF;
        uint8_t g1 = (color1 >> 8) & 0xFF;
        uint8_t b1 = color1 & 0xFF;
        uint8_t r2 = (color2 >> 16) & 0xFF;
        uint8_t g2 = (color2 >> 8) & 0xFF;
        uint8_t b2 = color2 & 0xFF;
        manager->setEffect(index, Effects::gradient(r1, g1, b1, r2, g2, b2));
        return index;
    }

    // Preset configurations
    void setupPartyMode() {
        // Multiple colorful strips with different effects
        addRainbowStrip(5, 30, "Front Rainbow");
        addChaseStrip(18, 60, Colors::MAGENTA, "Side Chase");
        addSparkleStrip(19, 45, Colors::CYAN, "Back Sparkle");
        addStrobeStrip(21, 30, Colors::WHITE, "Strobe");
    }

    void setupAmbientMode() {
        // Calm, relaxing strips
        addBreathingStrip(5, 30, Colors::WARM_WHITE, "Front Breathing");
        addWaveStrip(18, 60, Colors::BLUE, "Side Wave");
        addGradientStrip(19, 45, Colors::PURPLE, Colors::PINK, "Back Gradient");
    }

    void setupChristmasMode() {
        // Christmas themed
        addChaseStrip(5, 30, Colors::RED, "Red Chase");
        addChaseStrip(18, 60, Colors::GREEN, "Green Chase");
        addSparkleStrip(19, 45, Colors::WHITE, "White Sparkle");
    }

    void setupFireMode() {
        // Multiple fire effects
        addFireStrip(5, 30, "Fire 1");
        addFireStrip(18, 60, "Fire 2");
        addFireStrip(19, 45, "Fire 3");
    }

    // Helper to add strobe
    uint8_t addStrobeStrip(uint8_t pin, uint16_t numLEDs, uint32_t color, const String& name = "Strobe") {
        uint8_t index = manager->addStrip(pin, numLEDs, name);
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        manager->setEffect(index, Effects::strobe(r, g, b, 50, 100));
        return index;
    }
};

// Macro for easy configuration
#define SETUP_STRIP(manager, effects, config_func) \
    do { \
        StripConfigBuilder builder(&manager, &effects); \
        builder.config_func(); \
    } while(0)

#endif