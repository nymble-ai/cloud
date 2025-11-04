/**
 * CodeConverter - Convert JavaScript animations to Arduino C++ code
 * 
 * Handles the mechanical translation of JS syntax to Arduino syntax
 * while preserving the animation logic
 */

export class CodeConverter {
    constructor() {
        this.indentLevel = 0;
        this.indentSize = 2;
    }

    /**
     * Main conversion function
     * Takes JavaScript code and returns Arduino C++ code
     */
    convertToArduino(jsCode, config = {}) {
        const {
            ledCount = 90,
            stripCount = 1,
            dataPin = 5,
            useHardware = false
        } = config;

        // Generate the complete Arduino sketch
        let arduinoCode = this.generateHeader(ledCount, stripCount, dataPin, useHardware);
        arduinoCode += '\n';
        arduinoCode += this.convertAnimationCode(jsCode);
        arduinoCode += '\n';
        arduinoCode += this.generateSetupLoop();

        return arduinoCode;
    }

    /**
     * Generate Arduino sketch header with includes and display setup
     */
    generateHeader(ledCount, stripCount, dataPin, useHardware) {
        const template = `#include <LEDDisplay.h>

#ifdef HARDWARE_MODE
  #include <LEDDisplayHardware.h>
  LEDDisplayHardware<${ledCount}, ${dataPin}> display;
#else
  #include <LEDDisplaySimulator.h>
  LEDDisplaySimulator<${ledCount}> display;
#endif

// Animation variables (define your globals here)`;

        return template;
    }

    /**
     * Convert animation function code
     */
    convertAnimationCode(jsCode) {
        let arduinoCode = jsCode;

        // Convert variable declarations
        arduinoCode = this.convertVariableDeclarations(arduinoCode);
        
        // Convert function declarations
        arduinoCode = this.convertFunctionDeclarations(arduinoCode);
        
        // Convert Math functions
        arduinoCode = this.convertMathFunctions(arduinoCode);
        
        // Convert integer operations
        arduinoCode = this.convertIntegerOperations(arduinoCode);
        
        // Convert comments
        arduinoCode = this.convertComments(arduinoCode);

        return arduinoCode;
    }

    /**
     * Convert variable declarations (let, const, var → typed)
     */
    convertVariableDeclarations(code) {
        // Convert let/const with number literals to appropriate types
        code = code.replace(/\b(let|const|var)\s+(\w+)\s*=\s*(\d+);/g, (match, keyword, varName, value) => {
            const numValue = parseInt(value);
            let type;
            
            if (numValue < 0) {
                type = numValue > -128 ? 'int8_t' : 'int16_t';
            } else {
                if (numValue <= 255) type = 'uint8_t';
                else if (numValue <= 65535) type = 'uint16_t';
                else type = 'uint32_t';
            }
            
            return `${type} ${varName} = ${value};`;
        });

        // Convert let/const with float literals
        code = code.replace(/\b(let|const|var)\s+(\w+)\s*=\s*(\d+\.\d+);/g, 'float $2 = $3;');

        // Convert let/const without initialization
        code = code.replace(/\b(let|const|var)\s+(\w+);/g, 'uint16_t $2;');

        return code;
    }

    /**
     * Convert function declarations
     */
    convertFunctionDeclarations(code) {
        // Convert function declarations to void
        code = code.replace(/function\s+(\w+)\s*\(/g, 'void $1(');
        
        // Convert arrow functions (simple cases)
        code = code.replace(/const\s+(\w+)\s*=\s*\([^)]*\)\s*=>\s*{/g, 'void $1() {');
        
        return code;
    }

    /**
     * Convert Math functions
     */
    convertMathFunctions(code) {
        code = code.replace(/Math\.sin\(/g, 'sin(');
        code = code.replace(/Math\.cos\(/g, 'cos(');
        code = code.replace(/Math\.abs\(/g, 'abs(');
        code = code.replace(/Math\.sqrt\(/g, 'sqrt(');
        code = code.replace(/Math\.floor\(/g, '(int)(');
        code = code.replace(/Math\.ceil\(/g, '(int)(');
        code = code.replace(/Math\.round\(/g, '(int)(');
        code = code.replace(/Math\.min\(/g, 'min(');
        code = code.replace(/Math\.max\(/g, 'max(');
        code = code.replace(/Math\.pow\(/g, 'pow(');
        
        return code;
    }

    /**
     * Convert integer operations
     */
    convertIntegerOperations(code) {
        // Integer division is the same, but mark it clearly
        // No changes needed, but could add comments
        
        return code;
    }

    /**
     * Convert comments
     */
    convertComments(code) {
        // Single-line comments are the same
        // Multi-line comments are the same
        // No changes needed
        
        return code;
    }

    /**
     * Generate setup() and loop() functions
     */
    generateSetupLoop() {
        return `
void setup() {
  display.begin();
  display.setBrightness(128);
  
  // Add your initialization code here
}

void loop() {
  // Call your animation function
  myAnimation();
  
  delay(20);  // Adjust timing as needed
}`;
    }

    /**
     * Generate helper functions (colorWheel, XY, etc.)
     */
    generateHelperFunctions() {
        return `
// Color wheel helper (0-255 input)
uint32_t colorWheel(uint8_t wheelPos) {
  wheelPos = 255 - wheelPos;
  if (wheelPos < 85) {
    return LEDDisplay::Color(255 - wheelPos * 3, 0, wheelPos * 3);
  }
  if (wheelPos < 170) {
    wheelPos -= 85;
    return LEDDisplay::Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
  wheelPos -= 170;
  return LEDDisplay::Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}

// Matrix XY helper (serpentine layout)
uint16_t XY(uint8_t x, uint8_t y) {
  const uint8_t WIDTH = 16;
  const uint8_t HEIGHT = 16;
  const bool SERPENTINE = true;
  
  if (y >= HEIGHT || x >= WIDTH) return 0;
  
  if (SERPENTINE && y % 2 == 1) {
    return y * WIDTH + (WIDTH - 1 - x);
  }
  return y * WIDTH + x;
}

// Fast 8-bit sine approximation
uint8_t sin8(uint8_t theta) {
  uint16_t angle = theta;
  uint8_t result;
  
  if (angle < 64) {
    result = angle * 4;
  } else if (angle < 128) {
    result = 255 - ((angle - 64) * 4);
  } else if (angle < 192) {
    result = 0;
  } else {
    result = 0;
  }
  
  return (result / 2) + 64;
}`;
    }

    /**
     * Create a complete, ready-to-upload Arduino sketch
     */
    createCompleteSketch(jsCode, animationName = 'CustomAnimation', config = {}) {
        const arduinoCode = this.convertToArduino(jsCode, config);
        const helpers = this.generateHelperFunctions();
        
        const sketch = `/*
 * ${animationName}
 * 
 * Auto-generated from JavaScript animation
 * Tested in LED Simulator visualizer
 * 
 * Upload with:
 * - Simulator: pio run -e esp32_simulator -t upload
 * - Hardware:  pio run -e esp32_hardware -t upload
 */

${arduinoCode}

${helpers}
`;

        return sketch;
    }

    /**
     * Provide conversion tips and warnings
     */
    getConversionNotes(jsCode) {
        const notes = [];
        
        if (jsCode.includes('setTimeout') || jsCode.includes('setInterval')) {
            notes.push('⚠️ Warning: setTimeout/setInterval need manual conversion to timing logic');
        }
        
        if (jsCode.includes('console.log')) {
            notes.push('💡 Tip: Replace console.log() with Serial.println()');
        }
        
        if (jsCode.includes('.forEach') || jsCode.includes('.map')) {
            notes.push('⚠️ Warning: Array methods need conversion to for loops');
        }
        
        if (jsCode.includes('=>')) {
            notes.push('💡 Tip: Arrow functions converted to standard functions');
        }
        
        return notes;
    }
}

/**
 * Quick convert function for simple use
 */
export function convertJStoArduino(jsCode, config = {}) {
    const converter = new CodeConverter();
    return converter.createCompleteSketch(jsCode, 'CustomAnimation', config);
}

