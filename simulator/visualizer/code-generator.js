/**
 * Arduino Code Generator for LED Strips
 * Generates complete Arduino sketches from visual configuration
 */

class CodeGenerator {
    constructor() {
        this.strips = [];
        this.effects = {
            rainbow: { name: 'Rainbow', params: [] },
            solid: { name: 'Solid Color', params: ['color'] },
            chase: { name: 'Chase', params: ['color', 'tail_length', 'speed'] },
            breathing: { name: 'Breathing', params: ['color', 'speed'] },
            sparkle: { name: 'Sparkle', params: ['color', 'density'] },
            fire: { name: 'Fire', params: ['cooling', 'sparking'] },
            wave: { name: 'Wave', params: ['color', 'wavelength', 'speed'] },
            gradient: { name: 'Gradient', params: ['color1', 'color2'] },
            strobe: { name: 'Strobe', params: ['color', 'on_time', 'off_time'] },
            theater: { name: 'Theater Chase', params: ['color', 'speed'] },
            custom: { name: 'Custom', params: ['code'] }
        };

        this.colors = {
            RED: '#FF0000',
            GREEN: '#00FF00',
            BLUE: '#0000FF',
            WHITE: '#FFFFFF',
            YELLOW: '#FFFF00',
            CYAN: '#00FFFF',
            MAGENTA: '#FF00FF',
            ORANGE: '#FF8000',
            PURPLE: '#8000FF',
            PINK: '#FF1493',
            WARM_WHITE: '#FFD700',
            COOL_WHITE: '#E0FFFF'
        };
    }

    addStrip(config) {
        this.strips.push({
            pin: config.pin || 5,
            numLeds: config.numLeds || 30,
            name: config.name || `Strip ${this.strips.length + 1}`,
            effect: config.effect || 'rainbow',
            effectParams: config.effectParams || {},
            enabled: config.enabled !== false,
            brightness: config.brightness || 255
        });
        return this.strips.length - 1;
    }

    removeStrip(index) {
        if (index >= 0 && index < this.strips.length) {
            this.strips.splice(index, 1);
        }
    }

    updateStrip(index, config) {
        if (index >= 0 && index < this.strips.length) {
            Object.assign(this.strips[index], config);
        }
    }

    clearStrips() {
        this.strips = [];
    }

    // Convert hex color to RGB
    hexToRgb(hex) {
        const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
        return result ? {
            r: parseInt(result[1], 16),
            g: parseInt(result[2], 16),
            b: parseInt(result[3], 16)
        } : { r: 255, g: 255, b: 255 };
    }

    // Find closest named color
    getClosestNamedColor(hex) {
        for (const [name, color] of Object.entries(this.colors)) {
            if (color.toLowerCase() === hex.toLowerCase()) {
                return `Colors::${name}`;
            }
        }
        return hex;
    }

    // Generate effect function call
    generateEffectCode(strip) {
        const { effect, effectParams } = strip;

        switch (effect) {
            case 'rainbow':
                return `config.addRainbowStrip(${strip.pin}, ${strip.numLeds}, "${strip.name}")`;

            case 'solid':
                const solidColor = this.getClosestNamedColor(effectParams.color || '#FFFFFF');
                return `config.addSolidStrip(${strip.pin}, ${strip.numLeds}, ${solidColor}, "${strip.name}")`;

            case 'chase':
                const chaseColor = this.getClosestNamedColor(effectParams.color || '#FFFFFF');
                return `config.addChaseStrip(${strip.pin}, ${strip.numLeds}, ${chaseColor}, "${strip.name}")`;

            case 'breathing':
                const breathColor = this.getClosestNamedColor(effectParams.color || '#FFFFFF');
                return `config.addBreathingStrip(${strip.pin}, ${strip.numLeds}, ${breathColor}, "${strip.name}")`;

            case 'sparkle':
                const sparkleColor = this.getClosestNamedColor(effectParams.color || '#FFFFFF');
                return `config.addSparkleStrip(${strip.pin}, ${strip.numLeds}, ${sparkleColor}, "${strip.name}")`;

            case 'fire':
                return `config.addFireStrip(${strip.pin}, ${strip.numLeds}, "${strip.name}")`;

            case 'wave':
                const waveColor = this.getClosestNamedColor(effectParams.color || '#0000FF');
                return `config.addWaveStrip(${strip.pin}, ${strip.numLeds}, ${waveColor}, "${strip.name}")`;

            case 'gradient':
                const color1 = this.getClosestNamedColor(effectParams.color1 || '#0000FF');
                const color2 = this.getClosestNamedColor(effectParams.color2 || '#FF0000');
                return `config.addGradientStrip(${strip.pin}, ${strip.numLeds}, ${color1}, ${color2}, "${strip.name}")`;

            case 'strobe':
                const strobeColor = this.getClosestNamedColor(effectParams.color || '#FFFFFF');
                return `config.addStrobeStrip(${strip.pin}, ${strip.numLeds}, ${strobeColor}, "${strip.name}")`;

            case 'theater':
                const theaterColor = this.getClosestNamedColor(effectParams.color || '#FF00FF');
                return `  // Theater Chase effect
  uint8_t stripIndex = stripManager.addStrip(${strip.pin}, ${strip.numLeds}, "${strip.name}");
  stripManager.setEffect(stripIndex, Effects::theaterChase(${this.hexToRgb(theaterColor).r}, ${this.hexToRgb(theaterColor).g}, ${this.hexToRgb(theaterColor).b}, 100))`;

            case 'custom':
                return `  // Custom effect for ${strip.name}
  ${effectParams.code || '// Add your custom code here'}`;

            default:
                return `config.addRainbowStrip(${strip.pin}, ${strip.numLeds}, "${strip.name}")`;
        }
    }

    // Generate complete Arduino sketch
    generateSketch(projectName = "MyLEDProject") {
        const timestamp = new Date().toISOString().split('T')[0];

        const sketch = `/*
 * ${projectName}
 * Generated by LED Strip Code Generator
 * Date: ${timestamp}
 *
 * This sketch controls ${this.strips.length} LED strip${this.strips.length !== 1 ? 's' : ''} with different effects.
 * Total LEDs: ${this.strips.reduce((sum, s) => sum + s.numLeds, 0)}
 *
 * Hardware Setup:
${this.strips.map((s, i) => ` * - Strip ${i + 1}: ${s.numLeds} LEDs on GPIO ${s.pin} ("${s.name}")`).join('\n')}
 */

#include <LEDDisplay.h>
#include <StripManager.h>
#include <Effects.h>
#include <StripConfig.h>

// Display setup (automatically works with hardware or simulator)
#ifdef HARDWARE_MODE
  #include <MultiStripHardware.h>
  MultiStripHardware display;
#else
  #include <MultiStripSimulator.h>
  MultiStripSimulator display;
#endif

// Create managers
StripManager stripManager(&display);
Effects effects(&stripManager);
StripConfigBuilder config(&stripManager, &effects);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("${projectName} Starting...");

  // ============================================================
  // LED STRIP CONFIGURATION
  // ============================================================

${this.strips.map(strip => '  ' + this.generateEffectCode(strip) + ';').join('\n')}

  // ============================================================

  // Initialize the display with configured strips
  for (uint8_t i = 0; i < stripManager.getStripCount(); i++) {
    ManagedStrip& strip = stripManager.getStrip(i);
    display.addStrip(strip.pin, strip.numLEDs, strip.name);
  }

  display.begin();

  // Set individual strip brightness if needed
${this.strips.map((strip, i) => strip.brightness < 255 ? `  stripManager.setStripBrightness(${i}, ${strip.brightness});` : '').filter(s => s).join('\n') || '  // All strips at full brightness'}

  // Print configuration
  printConfiguration();
}

void loop() {
  // Update all strips with their assigned effects
  stripManager.update();

  // Optional: Add serial control
  checkForSerialCommands();

  delay(10); // Small delay for smooth animation
}

void printConfiguration() {
  Serial.println("\\n=== LED Strip Configuration ===");
  Serial.print("Total strips: ");
  Serial.println(stripManager.getStripCount());

  for (uint8_t i = 0; i < stripManager.getStripCount(); i++) {
    ManagedStrip& strip = stripManager.getStrip(i);
    Serial.print("  [");
    Serial.print(i);
    Serial.print("] Pin ");
    Serial.print(strip.pin);
    Serial.print(": ");
    Serial.print(strip.numLEDs);
    Serial.print(" LEDs - \\"");
    Serial.print(strip.name);
    Serial.println("\\"");
  }
  Serial.println("==============================\\n");
}

void checkForSerialCommands() {
  if (Serial.available()) {
    char cmd = Serial.read();

    switch(cmd) {
      case 'i': // Info
        printConfiguration();
        break;

      case 'r': // Reset/restart
        ESP.restart();
        break;

      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        // Toggle strip on/off
        {
          uint8_t stripIdx = cmd - '1';
          if (stripIdx < stripManager.getStripCount()) {
            static bool stripStates[9] = {true, true, true, true, true, true, true, true, true};
            stripStates[stripIdx] = !stripStates[stripIdx];
            stripManager.enableStrip(stripIdx, stripStates[stripIdx]);
            Serial.print("Strip ");
            Serial.print(stripIdx);
            Serial.println(stripStates[stripIdx] ? " enabled" : " disabled");
          }
        }
        break;

      case 'h': // Help
        Serial.println("\\n=== Serial Commands ===");
        Serial.println("i - Show configuration info");
        Serial.println("r - Reset/restart ESP32");
        Serial.println("1-9 - Toggle strip 1-9 on/off");
        Serial.println("h - Show this help");
        Serial.println("======================\\n");
        break;
    }
  }
}`;

        return sketch;
    }

    // Generate platformio.ini configuration
    generatePlatformioConfig() {
        return `[platformio]
default_envs = esp32_simulator

[env:esp32_hardware]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
    adafruit/Adafruit NeoPixel@^1.12.0
build_flags =
    -DHARDWARE_MODE=1
monitor_speed = 115200
upload_speed = 921600

[env:esp32_simulator]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
    adafruit/Adafruit NeoPixel@^1.12.0
build_flags =
    -DSIMULATOR_MODE=1
monitor_speed = 115200
upload_speed = 921600`;
    }

    // Export configuration as JSON
    exportConfig() {
        return JSON.stringify({
            strips: this.strips,
            generated: new Date().toISOString(),
            version: '1.0'
        }, null, 2);
    }

    // Import configuration from JSON
    importConfig(jsonStr) {
        try {
            const config = JSON.parse(jsonStr);
            this.strips = config.strips || [];
            return true;
        } catch (e) {
            console.error('Failed to import configuration:', e);
            return false;
        }
    }

    // Load preset configuration
    loadPreset(presetName) {
        this.clearStrips();

        switch (presetName) {
            case 'party':
                this.addStrip({ pin: 5, numLeds: 30, name: 'Front Rainbow', effect: 'rainbow' });
                this.addStrip({ pin: 18, numLeds: 60, name: 'Side Chase', effect: 'chase', effectParams: { color: '#FF00FF' } });
                this.addStrip({ pin: 19, numLeds: 45, name: 'Back Sparkle', effect: 'sparkle', effectParams: { color: '#00FFFF' } });
                this.addStrip({ pin: 21, numLeds: 30, name: 'Strobe', effect: 'strobe', effectParams: { color: '#FFFFFF' } });
                break;

            case 'ambient':
                this.addStrip({ pin: 5, numLeds: 30, name: 'Breathing', effect: 'breathing', effectParams: { color: '#FFD700' } });
                this.addStrip({ pin: 18, numLeds: 60, name: 'Ocean Wave', effect: 'wave', effectParams: { color: '#0000FF' } });
                this.addStrip({ pin: 19, numLeds: 45, name: 'Sunset', effect: 'gradient', effectParams: { color1: '#8000FF', color2: '#FF1493' } });
                break;

            case 'christmas':
                this.addStrip({ pin: 5, numLeds: 30, name: 'Red Chase', effect: 'chase', effectParams: { color: '#FF0000' } });
                this.addStrip({ pin: 18, numLeds: 60, name: 'Green Chase', effect: 'chase', effectParams: { color: '#00FF00' } });
                this.addStrip({ pin: 19, numLeds: 45, name: 'White Sparkle', effect: 'sparkle', effectParams: { color: '#FFFFFF' } });
                break;

            case 'fire':
                this.addStrip({ pin: 5, numLeds: 30, name: 'Fire Left', effect: 'fire' });
                this.addStrip({ pin: 18, numLeds: 60, name: 'Fire Center', effect: 'fire' });
                this.addStrip({ pin: 19, numLeds: 45, name: 'Fire Right', effect: 'fire' });
                break;

            case 'simple':
                this.addStrip({ pin: 5, numLeds: 90, name: 'Main Strip', effect: 'rainbow' });
                break;

            default:
                // Empty configuration
                break;
        }
    }
}

// Export for use in other modules
export default CodeGenerator;