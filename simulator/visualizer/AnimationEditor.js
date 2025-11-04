/**
 * Animation Editor
 * 
 * Provides a code editor for creating animations in JavaScript
 * and converting them to Arduino code
 */

import { TestLEDDisplay, colorWheel, sin8, XY } from './TestLEDDisplay.js';
import { CodeConverter } from './CodeConverter.js';
import { exampleAnimations, getExampleNames, getExample } from './ExampleAnimations.js';

export class AnimationEditor {
    constructor(renderer) {
        this.renderer = renderer;
        this.display = new TestLEDDisplay(renderer);
        this.converter = new CodeConverter();
        this.currentCode = '';
        this.animationFunction = null;
        this.isRunning = false;
        this.animationFrameId = null;
        this.frameDelay = 20; // milliseconds between frames
        
        this.initializeElements();
        this.initializeEventListeners();
        this.loadDefaultExample();
    }

    initializeElements() {
        this.elements = {
            codeEditor: document.getElementById('codeEditor'),
            exampleSelect: document.getElementById('exampleSelect'),
            runBtn: document.getElementById('runAnimationBtn'),
            stopBtn: document.getElementById('stopAnimationBtn'),
            exportBtn: document.getElementById('exportArduinoBtn'),
            copyBtn: document.getElementById('copyArduinoBtn'),
            arduinoOutput: document.getElementById('arduinoOutput'),
            errorDisplay: document.getElementById('editorError'),
            statusDisplay: document.getElementById('editorStatus')
        };

        // Populate example select
        this.populateExamples();
    }

    populateExamples() {
        const examples = getExampleNames();
        this.elements.exampleSelect.innerHTML = '';
        
        examples.forEach(({ key, name, description }) => {
            const option = document.createElement('option');
            option.value = key;
            option.textContent = name;
            option.title = description;
            this.elements.exampleSelect.appendChild(option);
        });
    }

    initializeEventListeners() {
        this.elements.exampleSelect.addEventListener('change', (e) => {
            this.loadExample(e.target.value);
        });

        this.elements.runBtn.addEventListener('click', () => {
            this.runAnimation();
        });

        this.elements.stopBtn.addEventListener('click', () => {
            this.stopAnimation();
        });

        this.elements.exportBtn.addEventListener('click', () => {
            this.exportToArduino();
        });

        this.elements.copyBtn.addEventListener('click', () => {
            this.copyArduinoCode();
        });

        // Auto-save code on change (with debounce)
        let saveTimeout;
        this.elements.codeEditor.addEventListener('input', () => {
            clearTimeout(saveTimeout);
            saveTimeout = setTimeout(() => {
                this.currentCode = this.elements.codeEditor.value;
            }, 500);
        });
    }

    loadDefaultExample() {
        this.loadExample('rainbow');
    }

    loadExample(key) {
        const example = getExample(key);
        if (example) {
            this.elements.codeEditor.value = example.code;
            this.currentCode = example.code;
            this.clearError();
            this.setStatus('Example loaded: ' + example.name);
        }
    }

    runAnimation() {
        try {
            this.clearError();
            this.stopAnimation();
            
            // Get the code from editor
            const code = this.elements.codeEditor.value;
            
            // Create a safe execution environment
            const animationCode = this.prepareAnimationCode(code);
            
            // Create the animation function
            this.animationFunction = new Function(
                'display', 
                'colorWheel', 
                'sin8', 
                'XY',
                'Math',
                animationCode
            );
            
            // Configure display to match current layout
            this.configureDisplay();
            
            // Start animation loop
            this.isRunning = true;
            this.elements.runBtn.disabled = true;
            this.elements.stopBtn.disabled = false;
            this.setStatus('✅ Animation running');
            
            this.animationLoop();
            
        } catch (error) {
            this.showError('Error running animation: ' + error.message);
            console.error('Animation error:', error);
        }
    }

    prepareAnimationCode(code) {
        // Wrap the code to make it executable
        // Extract the myAnimation function and call it
        let wrappedCode = code;
        
        // If code doesn't have myAnimation function, assume entire code is the function body
        if (!code.includes('function myAnimation')) {
            wrappedCode = `function myAnimation() {\n${code}\n}`;
        }
        
        // Add the function call
        wrappedCode += '\nmyAnimation();';
        
        return wrappedCode;
    }

    configureDisplay() {
        // Match display configuration to current layout
        const layout = this.renderer.layout;
        
        if (layout.type === 'parallel-strips') {
            this.display.setStripCount(layout.stripCount);
        } else {
            this.display.setPixelCount(layout.count);
        }
    }

    animationLoop() {
        if (!this.isRunning) return;
        
        try {
            // Execute the animation function
            this.animationFunction(
                this.display,
                colorWheel,
                sin8,
                XY,
                Math
            );
            
            // Schedule next frame
            setTimeout(() => {
                this.animationLoop();
            }, this.frameDelay);
            
        } catch (error) {
            this.showError('Runtime error: ' + error.message);
            this.stopAnimation();
        }
    }

    stopAnimation() {
        this.isRunning = false;
        this.elements.runBtn.disabled = false;
        this.elements.stopBtn.disabled = true;
        this.display.clear();
        this.display.show();
        this.setStatus('Animation stopped');
    }

    exportToArduino() {
        try {
            this.clearError();
            
            const code = this.elements.codeEditor.value;
            const layout = this.renderer.layout;
            
            const config = {
                ledCount: layout.count,
                stripCount: layout.stripCount || 1,
                dataPin: 5,
                useHardware: false
            };
            
            const arduinoCode = this.converter.createCompleteSketch(
                code,
                'CustomAnimation',
                config
            );
            
            this.elements.arduinoOutput.value = arduinoCode;
            this.elements.arduinoOutput.style.display = 'block';
            this.elements.copyBtn.style.display = 'inline-block';
            
            // Show conversion notes
            const notes = this.converter.getConversionNotes(code);
            if (notes.length > 0) {
                this.setStatus('✅ Exported! Notes:\n' + notes.join('\n'));
            } else {
                this.setStatus('✅ Arduino code exported successfully!');
            }
            
            // Scroll to output
            this.elements.arduinoOutput.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
            
        } catch (error) {
            this.showError('Export error: ' + error.message);
            console.error('Export error:', error);
        }
    }

    copyArduinoCode() {
        const code = this.elements.arduinoOutput.value;
        
        navigator.clipboard.writeText(code).then(() => {
            this.setStatus('✅ Arduino code copied to clipboard!');
            
            // Flash the button to show success
            const originalText = this.elements.copyBtn.textContent;
            this.elements.copyBtn.textContent = '✓ Copied!';
            setTimeout(() => {
                this.elements.copyBtn.textContent = originalText;
            }, 2000);
        }).catch(err => {
            this.showError('Failed to copy: ' + err.message);
        });
    }

    showError(message) {
        this.elements.errorDisplay.textContent = message;
        this.elements.errorDisplay.style.display = 'block';
    }

    clearError() {
        this.elements.errorDisplay.textContent = '';
        this.elements.errorDisplay.style.display = 'none';
    }

    setStatus(message) {
        this.elements.statusDisplay.textContent = message;
    }

    getFrameDelay() {
        return this.frameDelay;
    }

    setFrameDelay(delay) {
        this.frameDelay = Math.max(1, Math.min(1000, delay));
    }
}

