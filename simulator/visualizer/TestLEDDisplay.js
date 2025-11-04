/**
 * TestLEDDisplay - JavaScript LED Display API
 * Mirrors the Arduino LEDDisplay API for animation prototyping
 * 
 * This allows you to write animations in JavaScript that can be
 * automatically converted to Arduino code with minimal changes.
 */

export class TestLEDDisplay {
    constructor(renderer) {
        this.renderer = renderer;
        this.ledCount = 90;
        this.stripCount = 1;
        this.brightness = 255;
        this.leds = [];
        this.clear();
    }

    // Initialize the display
    begin() {
        this.clear();
        return this;
    }

    // Set pixel count (for configuration)
    setPixelCount(count) {
        this.ledCount = count;
        this.leds = new Array(count).fill(null).map(() => ({ r: 0, g: 0, b: 0 }));
    }

    // Set number of strips for multi-strip support
    setStripCount(count) {
        this.stripCount = count;
        this.ledCount = this.renderer.layout.count * count;
        this.leds = new Array(this.ledCount).fill(null).map(() => ({ r: 0, g: 0, b: 0 }));
    }

    // Set individual pixel (r, g, b values)
    setPixel(index, r, g, b) {
        if (index >= 0 && index < this.ledCount) {
            const scaledR = Math.floor((r * this.brightness) / 255);
            const scaledG = Math.floor((g * this.brightness) / 255);
            const scaledB = Math.floor((b * this.brightness) / 255);
            
            this.leds[index] = { 
                r: scaledR, 
                g: scaledG, 
                b: scaledB 
            };
        }
    }

    // Set pixel using packed color value
    setPixelColor(index, color) {
        const r = (color >> 16) & 0xFF;
        const g = (color >> 8) & 0xFF;
        const b = color & 0xFF;
        this.setPixel(index, r, g, b);
    }

    // Update the display (render to canvas)
    show() {
        if (this.renderer) {
            this.renderer.updateLEDs(this.leds);
            this.renderer.render();
        }
    }

    // Clear all LEDs
    clear() {
        this.leds = new Array(this.ledCount).fill(null).map(() => ({ r: 0, g: 0, b: 0 }));
    }

    // Set brightness (0-255)
    setBrightness(brightness) {
        this.brightness = Math.max(0, Math.min(255, brightness));
    }

    // Get brightness
    getBrightness() {
        return this.brightness;
    }

    // Get total pixel count
    getPixelCount() {
        return this.ledCount;
    }

    // Get pixel color
    getPixel(index) {
        if (index >= 0 && index < this.ledCount) {
            const led = this.leds[index];
            return this.Color(led.r, led.g, led.b);
        }
        return 0;
    }

    // Fill all LEDs with one color
    fill(r, g, b) {
        for (let i = 0; i < this.ledCount; i++) {
            this.setPixel(i, r, g, b);
        }
    }

    // Fill with packed color
    fillColor(color) {
        const r = (color >> 16) & 0xFF;
        const g = (color >> 8) & 0xFF;
        const b = color & 0xFF;
        this.fill(r, g, b);
    }

    // Multi-strip support
    getStripCount() {
        return this.stripCount;
    }

    getPixelsPerStrip() {
        return Math.floor(this.ledCount / this.stripCount);
    }

    setStripPixel(strip, index, r, g, b) {
        const globalIndex = strip * this.getPixelsPerStrip() + index;
        this.setPixel(globalIndex, r, g, b);
    }

    setStripPixelColor(strip, index, color) {
        const globalIndex = strip * this.getPixelsPerStrip() + index;
        this.setPixelColor(globalIndex, color);
    }

    setAllStrips(index, r, g, b) {
        for (let s = 0; s < this.stripCount; s++) {
            this.setStripPixel(s, index, r, g, b);
        }
    }

    setAllStripsColor(index, color) {
        for (let s = 0; s < this.stripCount; s++) {
            this.setStripPixelColor(s, index, color);
        }
    }

    // Static helper functions (match Arduino API exactly)
    static Color(r, g, b) {
        return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
    }

    static Red(color) {
        return (color >> 16) & 0xFF;
    }

    static Green(color) {
        return (color >> 8) & 0xFF;
    }

    static Blue(color) {
        return color & 0xFF;
    }

    // Convenience method for accessing static functions
    Color(r, g, b) {
        return TestLEDDisplay.Color(r, g, b);
    }

    Red(color) {
        return TestLEDDisplay.Red(color);
    }

    Green(color) {
        return TestLEDDisplay.Green(color);
    }

    Blue(color) {
        return TestLEDDisplay.Blue(color);
    }
}

// Global helper functions (match Arduino utilities)
export function colorWheel(wheelPos) {
    wheelPos = 255 - wheelPos;
    if (wheelPos < 85) {
        return TestLEDDisplay.Color(255 - wheelPos * 3, 0, wheelPos * 3);
    }
    if (wheelPos < 170) {
        wheelPos -= 85;
        return TestLEDDisplay.Color(0, wheelPos * 3, 255 - wheelPos * 3);
    }
    wheelPos -= 170;
    return TestLEDDisplay.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}

export function sin8(theta) {
    const angle = theta;
    let result;
    
    if (angle < 64) {
        result = angle * 4;
    } else if (angle < 128) {
        result = 255 - ((angle - 64) * 4);
    } else if (angle < 192) {
        result = 0;
    } else {
        result = 0;
    }
    
    return Math.floor((result / 2) + 64);
}

// Matrix helper function
export function XY(x, y, width, height, serpentine = true) {
    if (y >= height || x >= width) return 0;
    
    if (serpentine && y % 2 === 1) {
        return y * width + (width - 1 - x);
    }
    return y * width + x;
}

