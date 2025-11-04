/**
 * Example Animations
 * 
 * These are working examples of animations written in JavaScript
 * using the TestLEDDisplay API. They can be directly converted
 * to Arduino code using the CodeConverter.
 */

export const exampleAnimations = {
    
    // Example 1: Rainbow Cycle
    rainbow: {
        name: 'Rainbow Cycle',
        description: 'Classic rainbow pattern that cycles through the strip',
        code: `// Rainbow Cycle Animation
let hue = 0;

function myAnimation() {
  for (let i = 0; i < display.getPixelCount(); i++) {
    let pixelHue = (hue + (i * 256 / display.getPixelCount())) & 0xFF;
    let color = colorWheel(pixelHue);
    display.setPixelColor(i, color);
  }
  display.show();
  hue = (hue + 2) & 0xFF;
}`,
        globals: 'let hue = 0;'
    },

    // Example 2: Chase Pattern
    chase: {
        name: 'Chase Effect',
        description: 'Moving dot pattern with trail',
        code: `// Chase Effect Animation
let position = 0;

function myAnimation() {
  display.clear();
  
  for (let i = 0; i < 5; i++) {
    let idx = (position + i) % display.getPixelCount();
    let brightness = 255 - (i * 50);
    display.setPixel(idx, brightness, brightness, brightness);
  }
  
  display.show();
  position = (position + 1) % display.getPixelCount();
}`,
        globals: 'let position = 0;'
    },

    // Example 3: Breathing Effect
    breathing: {
        name: 'Breathing Effect',
        description: 'Smooth breathing/pulsing animation',
        code: `// Breathing Effect Animation
let brightness = 0;
let direction = 1;

function myAnimation() {
  display.fill(255, 0, 100);
  display.setBrightness(brightness);
  display.show();
  
  brightness += direction * 5;
  
  if (brightness >= 250) direction = -1;
  if (brightness <= 5) direction = 1;
}`,
        globals: 'let brightness = 0;\nlet direction = 1;'
    },

    // Example 4: Sparkle
    sparkle: {
        name: 'Sparkle',
        description: 'Random twinkling lights',
        code: `// Sparkle Animation
function myAnimation() {
  // Fade all LEDs
  for (let i = 0; i < display.getPixelCount(); i++) {
    let color = display.getPixel(i);
    let r = (color >> 16) & 0xFF;
    let g = (color >> 8) & 0xFF;
    let b = color & 0xFF;
    
    r = r > 10 ? r - 10 : 0;
    g = g > 10 ? g - 10 : 0;
    b = b > 10 ? b - 10 : 0;
    
    display.setPixel(i, r, g, b);
  }
  
  // Add new sparkles
  if (Math.random() < 0.3) {
    let pos = Math.floor(Math.random() * display.getPixelCount());
    display.setPixel(pos, 255, 255, 255);
  }
  
  display.show();
}`,
        globals: ''
    },

    // Example 5: Color Wave
    colorWave: {
        name: 'Color Wave',
        description: 'Smooth wave of colors moving across the strip',
        code: `// Color Wave Animation
let offset = 0;

function myAnimation() {
  for (let i = 0; i < display.getPixelCount(); i++) {
    let hue = ((i * 10) + offset) & 0xFF;
    let brightness = 128 + (Math.sin((i + offset) * 0.2) * 127);
    let color = colorWheel(hue);
    
    let r = ((color >> 16) & 0xFF) * brightness / 255;
    let g = ((color >> 8) & 0xFF) * brightness / 255;
    let b = (color & 0xFF) * brightness / 255;
    
    display.setPixel(i, r, g, b);
  }
  
  display.show();
  offset += 2;
}`,
        globals: 'let offset = 0;'
    },

    // Example 6: Theater Chase
    theaterChase: {
        name: 'Theater Chase',
        description: 'Theater marquee style chase pattern',
        code: `// Theater Chase Animation
let position = 0;

function myAnimation() {
  display.clear();
  
  for (let i = 0; i < display.getPixelCount(); i += 3) {
    let idx = (i + position) % display.getPixelCount();
    let hue = (i + position) & 0xFF;
    let color = colorWheel(hue);
    display.setPixelColor(idx, color);
  }
  
  display.show();
  position = (position + 1) % 3;
}`,
        globals: 'let position = 0;'
    },

    // Example 7: Fire Effect
    fire: {
        name: 'Fire Effect',
        description: 'Simulated fire/flame effect',
        code: `// Fire Effect Animation
function myAnimation() {
  for (let i = 0; i < display.getPixelCount(); i++) {
    let heat = Math.floor(Math.random() * 160) + 96;
    let color = heatToColor(heat);
    display.setPixelColor(i, color);
  }
  display.show();
}

function heatToColor(heat) {
  let r = 255;
  let g = heat;
  let b = 0;
  
  if (heat < 85) {
    r = heat * 3;
    g = 0;
    b = 0;
  } else if (heat < 170) {
    r = 255;
    g = (heat - 85) * 3;
    b = 0;
  }
  
  return display.Color(r, g, b);
}`,
        globals: ''
    },

    // Example 8: Matrix Plasma (requires matrix layout)
    plasma: {
        name: 'Plasma Effect (Matrix)',
        description: 'Plasma effect for matrix displays',
        code: `// Plasma Effect for Matrix
const WIDTH = 16;
const HEIGHT = 16;
let offset = 0;

function myAnimation() {
  for (let y = 0; y < HEIGHT; y++) {
    for (let x = 0; x < WIDTH; x++) {
      let v1 = Math.sin((x + offset) * 0.3);
      let v2 = Math.sin((y + offset) * 0.3);
      let v3 = Math.sin((x + y + offset) * 0.2);
      let v4 = Math.sin(Math.sqrt((x - 8) * (x - 8) + (y - 8) * (y - 8)) * 0.5 + offset * 0.5);
      
      let value = (v1 + v2 + v3 + v4) / 4.0;
      let hue = Math.floor((value + 1.0) * 128 + offset);
      let color = colorWheel(hue & 0xFF);
      
      let index = XY(x, y, WIDTH, HEIGHT);
      display.setPixelColor(index, color);
    }
  }
  
  display.show();
  offset++;
}`,
        globals: 'const WIDTH = 16;\nconst HEIGHT = 16;\nlet offset = 0;'
    },

    // Example 9: Parallel Strips Wave
    parallelWave: {
        name: 'Parallel Strips Wave',
        description: 'Synchronized wave across multiple strips',
        code: `// Parallel Strips Wave Animation
let waveOffset = 0;

function myAnimation() {
  for (let i = 0; i < display.getPixelsPerStrip(); i++) {
    let hue = (i * 256 / display.getPixelsPerStrip() + waveOffset) & 0xFF;
    let color = colorWheel(hue);
    display.setAllStripsColor(i, color);
  }
  
  display.show();
  waveOffset += 2;
}`,
        globals: 'let waveOffset = 0;'
    },

    // Example 10: Custom Template
    template: {
        name: 'Custom Template',
        description: 'Empty template to start your own animation',
        code: `// Custom Animation
// Add your global variables here
let counter = 0;

function myAnimation() {
  // Your animation code here
  
  // Example: Set all LEDs to a color
  display.fill(255, 0, 0);
  display.show();
  
  counter++;
}`,
        globals: 'let counter = 0;'
    }
};

/**
 * Get list of example names
 */
export function getExampleNames() {
    return Object.keys(exampleAnimations).map(key => ({
        key,
        name: exampleAnimations[key].name,
        description: exampleAnimations[key].description
    }));
}

/**
 * Get example by key
 */
export function getExample(key) {
    return exampleAnimations[key] || exampleAnimations.template;
}

