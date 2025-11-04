// TEST: This should appear immediately
console.log('🔵 app.js is loading...');

import { LEDRenderer } from './renderer.js';
import { SerialBridge } from './serial-bridge.js';

console.log('🔵 Basic imports loaded, trying AnimationEditor...');

// Don't import AnimationEditor at the top - we'll import it dynamically later
// import { AnimationEditor } from './AnimationEditor.js';

class LEDSimulatorApp {
    constructor() {
        this.renderer = new LEDRenderer(document.getElementById('ledCanvas'));
        this.serialBridge = null;
        this.isConnected = false;
        this.animationEditor = null;
        this.editorOpen = false;

        this.initElements();
        this.initEventListeners();
        this.startRenderLoop();
    }

    initElements() {
        this.elements = {
            connectBtn: document.getElementById('connectBtn'),
            connectionStatus: document.getElementById('connectionStatus'),
            fpsDisplay: document.getElementById('fps'),
            overlay: document.getElementById('canvasOverlay'),

            layoutType: document.getElementById('layoutType'),
            ledCount: document.getElementById('ledCount'),
            orientation: document.getElementById('orientation'),
            matrixWidth: document.getElementById('matrixWidth'),
            matrixHeight: document.getElementById('matrixHeight'),
            serpentine: document.getElementById('serpentine'),
            ringRadius: document.getElementById('ringRadius'),
            applyLayoutBtn: document.getElementById('applyLayoutBtn'),

            stripControls: document.getElementById('stripControls'),
            matrixControls: document.getElementById('matrixControls'),
            ringControls: document.getElementById('ringControls'),
            parallelStripControls: document.getElementById('parallelStripControls'),
            stripCount: document.getElementById('stripCount'),
            stripSpacing: document.getElementById('stripSpacing'),
            stripSpacingValue: document.getElementById('stripSpacingValue'),
            parallelOrientation: document.getElementById('parallelOrientation'),

            ledSize: document.getElementById('ledSize'),
            ledSizeValue: document.getElementById('ledSizeValue'),
            ledSpacing: document.getElementById('ledSpacing'),
            ledSpacingValue: document.getElementById('ledSpacingValue'),
            glowIntensity: document.getElementById('glowIntensity'),
            glowIntensityValue: document.getElementById('glowIntensityValue'),
            showIndices: document.getElementById('showIndices'),
            backgroundColor: document.getElementById('backgroundColor'),

            testRainbow: document.getElementById('testRainbow'),
            testChase: document.getElementById('testChase'),
            testRandom: document.getElementById('testRandom'),
            testClear: document.getElementById('testClear'),

            toggleEditorBtn: document.getElementById('toggleEditorBtn'),
            closeEditorBtn: document.getElementById('closeEditorBtn'),
            editorPanel: document.getElementById('editorPanel')
        };
    }

    initEventListeners() {
        this.elements.connectBtn.addEventListener('click', () => this.toggleConnection());

        this.elements.layoutType.addEventListener('change', () => this.updateLayoutControls());
        this.elements.applyLayoutBtn.addEventListener('click', () => this.applyLayout());

        this.elements.ledSize.addEventListener('input', (e) => {
            this.elements.ledSizeValue.textContent = e.target.value;
            this.updateAppearance();
        });

        this.elements.ledSpacing.addEventListener('input', (e) => {
            this.elements.ledSpacingValue.textContent = e.target.value;
            this.updateAppearance();
        });

        this.elements.glowIntensity.addEventListener('input', (e) => {
            this.elements.glowIntensityValue.textContent = e.target.value;
            this.updateAppearance();
        });

        // Add strip spacing listener
        if (this.elements.stripSpacing) {
            this.elements.stripSpacing.addEventListener('input', (e) => {
                this.elements.stripSpacingValue.textContent = e.target.value;
                this.applyLayout();
            });
        }

        this.elements.showIndices.addEventListener('change', () => this.updateAppearance());
        this.elements.backgroundColor.addEventListener('change', () => this.updateAppearance());

        this.elements.testRainbow.addEventListener('click', () => this.renderer.testPattern('rainbow'));
        this.elements.testChase.addEventListener('click', () => this.renderer.testPattern('chase'));
        this.elements.testRandom.addEventListener('click', () => this.renderer.testPattern('random'));
        this.elements.testClear.addEventListener('click', () => this.renderer.clear());

        // Animation Editor controls
        console.log('Setting up editor button:', this.elements.toggleEditorBtn);
        if (this.elements.toggleEditorBtn) {
            this.elements.toggleEditorBtn.addEventListener('click', () => {
                console.log('Open Editor clicked!');
                this.toggleEditor();
            });
        } else {
            console.error('toggleEditorBtn not found!');
        }
        
        if (this.elements.closeEditorBtn) {
            this.elements.closeEditorBtn.addEventListener('click', () => this.closeEditor());
        }

        window.addEventListener('resize', () => {
            this.renderer.resizeCanvas();
            this.renderer.calculatePositions();
            this.renderer.render();
        });
    }

    toggleEditor() {
        console.log('toggleEditor called, editorOpen:', this.editorOpen);
        if (this.editorOpen) {
            this.closeEditor();
        } else {
            this.openEditor();
        }
    }

    async openEditor() {
        console.log('openEditor called');
        console.log('editorPanel element:', this.elements.editorPanel);
        
        // Initialize editor on first open (lazy loading with dynamic import)
        if (!this.animationEditor) {
            console.log('Initializing AnimationEditor...');
            try {
                // Dynamic import to avoid loading errors at startup
                const { AnimationEditor } = await import('./AnimationEditor.js');
                console.log('AnimationEditor module loaded');
                
                this.animationEditor = new AnimationEditor(this.renderer);
                console.log('AnimationEditor initialized successfully');
            } catch (error) {
                console.error('Error initializing AnimationEditor:', error);
                alert('Error loading Animation Editor: ' + error.message);
                return;
            }
        }
        
        if (this.elements.editorPanel) {
            this.elements.editorPanel.style.display = 'block';
            this.editorOpen = true;
            this.elements.toggleEditorBtn.textContent = 'Close Editor';
            console.log('Editor panel should now be visible');
        } else {
            console.error('editorPanel element not found!');
        }
    }

    closeEditor() {
        this.elements.editorPanel.style.display = 'none';
        this.editorOpen = false;
        this.elements.toggleEditorBtn.textContent = 'Open Editor';
        
        // Stop any running animation
        if (this.animationEditor && this.animationEditor.isRunning) {
            this.animationEditor.stopAnimation();
        }
    }

    updateLayoutControls() {
        const type = this.elements.layoutType.value;

        this.elements.stripControls.style.display = type === 'strip' ? 'block' : 'none';
        this.elements.matrixControls.style.display = type === 'matrix' ? 'block' : 'none';
        this.elements.ringControls.style.display = type === 'ring' ? 'block' : 'none';
        this.elements.parallelStripControls.style.display = type === 'parallel-strips' ? 'block' : 'none';

        if (type === 'matrix') {
            const width = parseInt(this.elements.matrixWidth.value);
            const height = parseInt(this.elements.matrixHeight.value);
            this.elements.ledCount.value = width * height;
        }
    }

    applyLayout() {
        const type = this.elements.layoutType.value;
        const count = parseInt(this.elements.ledCount.value);

        const layout = {
            type,
            count
        };

        switch (type) {
            case 'strip':
                layout.orientation = this.elements.orientation.value;
                break;

            case 'parallel-strips':
                layout.stripCount = parseInt(this.elements.stripCount.value);
                layout.stripSpacing = parseInt(this.elements.stripSpacing.value);
                layout.orientation = this.elements.parallelOrientation.value;
                break;

            case 'matrix':
                layout.width = parseInt(this.elements.matrixWidth.value);
                layout.height = parseInt(this.elements.matrixHeight.value);
                layout.serpentine = this.elements.serpentine.checked;
                layout.count = layout.width * layout.height;
                this.elements.ledCount.value = layout.count;
                break;

            case 'ring':
                layout.radius = parseInt(this.elements.ringRadius.value);
                break;
        }

        this.renderer.setLayout(layout);
        this.renderer.render();
    }

    updateAppearance() {
        const config = {
            ledSize: parseInt(this.elements.ledSize.value),
            ledSpacing: parseInt(this.elements.ledSpacing.value),
            glowIntensity: parseInt(this.elements.glowIntensity.value) / 100,
            showIndices: this.elements.showIndices.checked,
            backgroundColor: this.elements.backgroundColor.value
        };

        this.renderer.setConfig(config);
        this.renderer.render();
    }

    async toggleConnection() {
        if (this.isConnected) {
            await this.disconnect();
        } else {
            await this.connect();
        }
    }

    async connect() {
        try {
            this.serialBridge = new SerialBridge(
                (leds, fps) => this.onFrame(leds, fps),
                (status, message) => this.onStatusChange(status, message)
            );

            await this.serialBridge.connect();
        } catch (error) {
            console.error('Connection failed:', error);
            alert(`Connection failed: ${error.message}`);
        }
    }

    async disconnect() {
        if (this.serialBridge) {
            await this.serialBridge.disconnect();
            this.serialBridge = null;
        }
    }

    onFrame(leds, fps) {
        this.renderer.updateLEDs(leds);
        this.elements.fpsDisplay.textContent = `FPS: ${Math.round(fps)}`;
    }

    onStatusChange(status, message) {
        this.isConnected = status === 'connected';

        if (status === 'connected') {
            this.elements.connectionStatus.textContent = 'Connected';
            this.elements.connectionStatus.className = 'status status-connected';
            this.elements.connectBtn.textContent = 'Disconnect';
            this.elements.overlay.classList.add('hidden');
        } else {
            this.elements.connectionStatus.textContent = status === 'error'
                ? `Error: ${message}`
                : 'Disconnected';
            this.elements.connectionStatus.className = 'status status-disconnected';
            this.elements.connectBtn.textContent = 'Connect to Serial';
            this.elements.overlay.classList.remove('hidden');
            this.elements.fpsDisplay.textContent = 'FPS: 0';
        }
    }

    startRenderLoop() {
        const render = () => {
            this.renderer.render();
            requestAnimationFrame(render);
        };
        render();
    }
}

document.addEventListener('DOMContentLoaded', () => {
    new LEDSimulatorApp();
});