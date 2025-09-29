/**
 * Code Generator App - Main application logic
 */

import CodeGenerator from './code-generator.js';
import UploadClient from './upload-client.js';

class CodeGeneratorApp {
    constructor() {
        this.generator = new CodeGenerator();
        this.uploadClient = new UploadClient();
        this.currentEditIndex = -1;
        this.previewAnimation = null;
        this.generatedCode = null;

        this.initializeUI();
        this.attachEventListeners();
        this.updateStripsList();
        this.initializeUploadUI();
    }

    initializeUI() {
        // Set default project name
        const projectName = document.getElementById('projectName');
        projectName.value = `LED_Project_${new Date().toISOString().split('T')[0]}`;

        // Initialize preview canvas
        this.previewCanvas = document.getElementById('previewCanvas');
        this.previewCtx = this.previewCanvas.getContext('2d');
        this.resizeCanvas();
    }

    attachEventListeners() {
        // Preset buttons
        document.querySelectorAll('.preset-btn').forEach(btn => {
            btn.addEventListener('click', (e) => {
                const preset = e.target.dataset.preset;
                this.loadPreset(preset);
            });
        });

        // Add strip button
        document.getElementById('addStripBtn').addEventListener('click', () => {
            this.showStripModal();
        });

        // Clear all button
        document.getElementById('clearAllBtn').addEventListener('click', () => {
            if (confirm('Clear all strips?')) {
                this.generator.clearStrips();
                this.updateStripsList();
                this.updatePreview();
            }
        });

        // Generate code button
        document.getElementById('generateBtn').addEventListener('click', () => {
            this.generateCode();
        });

        // Download button
        document.getElementById('downloadBtn').addEventListener('click', () => {
            this.downloadSketch();
        });

        // Copy button
        document.getElementById('copyBtn').addEventListener('click', () => {
            this.copyToClipboard();
        });

        // Export/Import config
        document.getElementById('exportConfigBtn').addEventListener('click', () => {
            this.exportConfiguration();
        });

        document.getElementById('importConfigBtn').addEventListener('click', () => {
            document.getElementById('importFile').click();
        });

        document.getElementById('importFile').addEventListener('change', (e) => {
            this.importConfiguration(e.target.files[0]);
        });

        // Modal controls
        document.getElementById('modalSave').addEventListener('click', () => {
            this.saveStripModal();
        });

        document.getElementById('modalCancel').addEventListener('click', () => {
            this.hideStripModal();
        });

        // Effect change handler
        document.getElementById('modalEffect').addEventListener('change', (e) => {
            this.updateEffectParams(e.target.value);
        });

        // Brightness slider
        document.getElementById('modalBrightness').addEventListener('input', (e) => {
            document.getElementById('brightnessValue').textContent = e.target.value;
        });

        // Code tabs
        document.querySelectorAll('.code-tab').forEach(tab => {
            tab.addEventListener('click', (e) => {
                this.switchCodeTab(e.target.dataset.tab);
            });
        });

        // Preview controls
        document.getElementById('previewBtn').addEventListener('click', () => {
            this.startPreview();
        });

        document.getElementById('stopPreviewBtn').addEventListener('click', () => {
            this.stopPreview();
        });

        // Window resize
        window.addEventListener('resize', () => {
            this.resizeCanvas();
        });
    }

    loadPreset(presetName) {
        this.generator.loadPreset(presetName);
        this.updateStripsList();
        this.updatePreview();

        // Show notification
        this.showNotification(`Loaded ${presetName} preset`);
    }

    updateStripsList() {
        const container = document.getElementById('stripsList');
        container.innerHTML = '';

        this.generator.strips.forEach((strip, index) => {
            const stripEl = document.createElement('div');
            stripEl.className = 'strip-item';
            stripEl.innerHTML = `
                <div class="strip-info">
                    <div class="strip-name">${strip.name}</div>
                    <div class="strip-details">
                        Pin ${strip.pin} | ${strip.numLeds} LEDs | ${strip.effect}
                    </div>
                </div>
                <div class="strip-actions">
                    <button class="strip-btn edit" data-index="${index}">Edit</button>
                    <button class="strip-btn delete" data-index="${index}">×</button>
                </div>
            `;

            // Add event listeners
            stripEl.querySelector('.edit').addEventListener('click', (e) => {
                e.stopPropagation();
                this.editStrip(index);
            });

            stripEl.querySelector('.delete').addEventListener('click', (e) => {
                e.stopPropagation();
                this.deleteStrip(index);
            });

            stripEl.addEventListener('click', () => {
                this.editStrip(index);
            });

            container.appendChild(stripEl);
        });

        // Update counts
        document.getElementById('totalLeds').textContent =
            `Total LEDs: ${this.generator.strips.reduce((sum, s) => sum + s.numLeds, 0)}`;
        document.getElementById('totalStrips').textContent =
            `Strips: ${this.generator.strips.length}`;
    }

    showStripModal(stripIndex = -1) {
        const modal = document.getElementById('stripModal');
        this.currentEditIndex = stripIndex;

        if (stripIndex >= 0) {
            // Edit mode
            const strip = this.generator.strips[stripIndex];
            document.getElementById('modalStripName').value = strip.name;
            document.getElementById('modalPin').value = strip.pin;
            document.getElementById('modalNumLeds').value = strip.numLeds;
            document.getElementById('modalEffect').value = strip.effect;
            document.getElementById('modalBrightness').value = strip.brightness;
            document.getElementById('brightnessValue').textContent = strip.brightness;
            this.updateEffectParams(strip.effect, strip.effectParams);
        } else {
            // Add mode
            document.getElementById('modalStripName').value = `Strip ${this.generator.strips.length + 1}`;
            document.getElementById('modalPin').value = '5';
            document.getElementById('modalNumLeds').value = '30';
            document.getElementById('modalEffect').value = 'rainbow';
            document.getElementById('modalBrightness').value = '255';
            document.getElementById('brightnessValue').textContent = '255';
            this.updateEffectParams('rainbow');
        }

        modal.style.display = 'flex';
    }

    hideStripModal() {
        document.getElementById('stripModal').style.display = 'none';
        this.currentEditIndex = -1;
    }

    saveStripModal() {
        const config = {
            name: document.getElementById('modalStripName').value || 'Unnamed Strip',
            pin: parseInt(document.getElementById('modalPin').value),
            numLeds: parseInt(document.getElementById('modalNumLeds').value),
            effect: document.getElementById('modalEffect').value,
            brightness: parseInt(document.getElementById('modalBrightness').value),
            effectParams: this.getEffectParams()
        };

        if (this.currentEditIndex >= 0) {
            this.generator.updateStrip(this.currentEditIndex, config);
        } else {
            this.generator.addStrip(config);
        }

        this.updateStripsList();
        this.updatePreview();
        this.hideStripModal();
    }

    editStrip(index) {
        this.showStripModal(index);
    }

    deleteStrip(index) {
        if (confirm(`Delete ${this.generator.strips[index].name}?`)) {
            this.generator.removeStrip(index);
            this.updateStripsList();
            this.updatePreview();
        }
    }

    updateEffectParams(effect, existingParams = {}) {
        const container = document.getElementById('effectParams');
        container.innerHTML = '';

        switch (effect) {
            case 'solid':
            case 'chase':
            case 'breathing':
            case 'sparkle':
            case 'wave':
            case 'strobe':
            case 'theater':
                container.innerHTML = `
                    <label>
                        Color:
                        <div class="color-input-group">
                            <input type="color" id="paramColor" value="${existingParams.color || '#FFFFFF'}">
                            <div class="color-preview" id="colorPreview"></div>
                        </div>
                    </label>
                `;
                document.getElementById('paramColor').addEventListener('input', (e) => {
                    document.getElementById('colorPreview').style.backgroundColor = e.target.value;
                });
                document.getElementById('colorPreview').style.backgroundColor = existingParams.color || '#FFFFFF';
                break;

            case 'gradient':
                container.innerHTML = `
                    <label>
                        Start Color:
                        <input type="color" id="paramColor1" value="${existingParams.color1 || '#0000FF'}">
                    </label>
                    <label>
                        End Color:
                        <input type="color" id="paramColor2" value="${existingParams.color2 || '#FF0000'}">
                    </label>
                `;
                break;

            case 'fire':
                container.innerHTML = `<p style="color: #888;">Fire effect uses default parameters</p>`;
                break;

            case 'rainbow':
                container.innerHTML = `<p style="color: #888;">Rainbow effect cycles through all colors</p>`;
                break;
        }
    }

    getEffectParams() {
        const effect = document.getElementById('modalEffect').value;
        const params = {};

        switch (effect) {
            case 'solid':
            case 'chase':
            case 'breathing':
            case 'sparkle':
            case 'wave':
            case 'strobe':
            case 'theater':
                const colorInput = document.getElementById('paramColor');
                if (colorInput) params.color = colorInput.value;
                break;

            case 'gradient':
                const color1Input = document.getElementById('paramColor1');
                const color2Input = document.getElementById('paramColor2');
                if (color1Input) params.color1 = color1Input.value;
                if (color2Input) params.color2 = color2Input.value;
                break;
        }

        return params;
    }

    generateCode() {
        const projectName = document.getElementById('projectName').value || 'MyLEDProject';
        const code = this.generator.generateSketch(projectName);
        this.generatedCode = code;  // Store for upload

        document.getElementById('codeOutput').innerHTML = `<code>${this.escapeHtml(code)}</code>`;

        // Enable download, copy, and upload buttons
        document.getElementById('downloadBtn').disabled = false;
        document.getElementById('copyBtn').disabled = false;
        document.getElementById('compileBtn').disabled = false;
        document.getElementById('uploadBtn').disabled = false;

        // Show notification
        this.showNotification('Code generated successfully!');

        // Add animation
        document.getElementById('generateBtn').classList.add('generating');
        setTimeout(() => {
            document.getElementById('generateBtn').classList.remove('generating');
        }, 1000);
    }

    initializeUploadUI() {
        // Set up upload status callback
        this.uploadClient.setStatusCallback((type, message, details) => {
            this.updateUploadStatus(type, message, details);
        });

        // Load available ports
        this.refreshPorts();

        // Attach upload event listeners
        document.getElementById('refreshPortsBtn').addEventListener('click', () => {
            this.refreshPorts();
        });

        document.getElementById('compileBtn').addEventListener('click', () => {
            this.compileCode();
        });

        document.getElementById('uploadBtn').addEventListener('click', () => {
            this.uploadCode();
        });

        // Check server status
        this.checkUploadServer();
    }

    async checkUploadServer() {
        const isReady = await this.uploadClient.checkServerStatus();
        if (!isReady) {
            this.updateUploadStatus('error', 'Upload server not running. Start server with: npm run upload-server');
        }
    }

    async refreshPorts() {
        const portSelect = document.getElementById('portSelect');
        const ports = await this.uploadClient.getPorts();

        // Clear existing options except auto
        portSelect.innerHTML = '<option value="auto">Auto-detect</option>';

        // Add available ports
        if (Array.isArray(ports) && ports.length > 0) {
            ports.forEach(port => {
                const option = document.createElement('option');
                option.value = typeof port === 'string' ? port : port.port;
                option.textContent = typeof port === 'string' ? port : `${port.port} - ${port.description || ''}`;
                portSelect.appendChild(option);
            });
            this.showNotification(`Found ${ports.length} port(s)`);
        }
    }

    async compileCode() {
        if (!this.generatedCode) {
            this.showNotification('Generate code first!', 'error');
            return;
        }

        const environment = document.getElementById('environmentSelect').value;

        try {
            document.getElementById('uploadProgress').style.display = 'block';
            document.getElementById('compileBtn').disabled = true;

            const result = await this.uploadClient.compileCode(this.generatedCode, environment);

            if (result.success) {
                this.showNotification('Compilation successful!');
                document.getElementById('uploadOutput').textContent = result.output || 'Compiled successfully';
            }
        } catch (error) {
            this.showNotification('Compilation failed', 'error');
            document.getElementById('uploadOutput').textContent = error.message;
        } finally {
            document.getElementById('compileBtn').disabled = false;
        }
    }

    async uploadCode() {
        if (!this.generatedCode) {
            this.showNotification('Generate code first!', 'error');
            return;
        }

        const environment = document.getElementById('environmentSelect').value;
        const port = document.getElementById('portSelect').value;

        try {
            document.getElementById('uploadProgress').style.display = 'block';
            document.getElementById('uploadBtn').disabled = true;
            document.getElementById('progressFill').style.width = '50%';

            const result = await this.uploadClient.uploadCode(this.generatedCode, environment, port);

            if (result.success) {
                document.getElementById('progressFill').style.width = '100%';
                this.showNotification('✅ Code uploaded to ESP32!');
                document.getElementById('uploadOutput').textContent = result.output || 'Upload complete!';

                // Auto-open terminal after successful upload
                setTimeout(() => {
                    if (confirm('Upload successful! Open terminal to monitor ESP32?')) {
                        window.location.href = 'index.html';
                    }
                }, 1000);
            }
        } catch (error) {
            this.showNotification('Upload failed', 'error');
            document.getElementById('uploadOutput').textContent = error.message;
            document.getElementById('progressFill').style.width = '0%';
        } finally {
            document.getElementById('uploadBtn').disabled = false;
        }
    }

    updateUploadStatus(type, message, details) {
        const statusDiv = document.getElementById('uploadStatus');
        const statusText = document.getElementById('statusText');

        statusDiv.className = `upload-status ${type}`;
        statusText.textContent = message;

        // Update icon based on status
        const statusIcon = statusDiv.querySelector('.status-icon');
        switch (type) {
            case 'success':
                statusIcon.textContent = '✅';
                break;
            case 'error':
                statusIcon.textContent = '❌';
                break;
            case 'warning':
                statusIcon.textContent = '⚠️';
                break;
            case 'progress':
                statusIcon.textContent = '⏳';
                // Extract percentage from message if present
                const match = message.match(/(\d+)%/);
                if (match) {
                    const percentage = match[1];
                    document.getElementById('progressFill').style.width = `${percentage}%`;
                }
                break;
            case 'info':
                statusIcon.textContent = '📡';
                break;
            case 'connected':
                statusIcon.textContent = '🔗';
                break;
            default:
                statusIcon.textContent = '⚡';
        }

        if (details) {
            // Append to output instead of replacing to show log
            const outputEl = document.getElementById('uploadOutput');
            outputEl.textContent = (outputEl.textContent + '\n' + details).trim();
            // Auto-scroll to bottom
            outputEl.scrollTop = outputEl.scrollHeight;
        }
    }

    switchCodeTab(tab) {
        // Update active tab
        document.querySelectorAll('.code-tab').forEach(t => {
            t.classList.toggle('active', t.dataset.tab === tab);
        });

        const codeOutput = document.getElementById('codeOutput');
        const instructionsPanel = document.getElementById('instructionsPanel');

        switch (tab) {
            case 'sketch':
                codeOutput.style.display = 'block';
                instructionsPanel.style.display = 'none';
                break;

            case 'platformio':
                codeOutput.style.display = 'block';
                instructionsPanel.style.display = 'none';
                const platformioConfig = this.generator.generatePlatformioConfig();
                codeOutput.innerHTML = `<code>${this.escapeHtml(platformioConfig)}</code>`;
                break;

            case 'instructions':
                codeOutput.style.display = 'none';
                instructionsPanel.style.display = 'block';
                break;
        }
    }

    downloadSketch() {
        const projectName = document.getElementById('projectName').value || 'MyLEDProject';
        const code = this.generator.generateSketch(projectName);

        const blob = new Blob([code], { type: 'text/plain' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = `${projectName}.ino`;
        a.click();
        URL.revokeObjectURL(url);

        this.showNotification('Sketch downloaded!');
    }

    copyToClipboard() {
        const code = this.generator.generateSketch(
            document.getElementById('projectName').value || 'MyLEDProject'
        );

        navigator.clipboard.writeText(code).then(() => {
            this.showNotification('Code copied to clipboard!');
        }).catch(() => {
            // Fallback for older browsers
            const textarea = document.createElement('textarea');
            textarea.value = code;
            document.body.appendChild(textarea);
            textarea.select();
            document.execCommand('copy');
            document.body.removeChild(textarea);
            this.showNotification('Code copied to clipboard!');
        });
    }

    exportConfiguration() {
        const config = this.generator.exportConfig();
        const blob = new Blob([config], { type: 'application/json' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = 'led-config.json';
        a.click();
        URL.revokeObjectURL(url);

        this.showNotification('Configuration exported!');
    }

    importConfiguration(file) {
        if (!file) return;

        const reader = new FileReader();
        reader.onload = (e) => {
            if (this.generator.importConfig(e.target.result)) {
                this.updateStripsList();
                this.updatePreview();
                this.showNotification('Configuration imported!');
            } else {
                this.showNotification('Failed to import configuration', 'error');
            }
        };
        reader.readAsText(file);
    }

    // Preview functionality
    resizeCanvas() {
        const container = this.previewCanvas.parentElement;
        this.previewCanvas.width = container.clientWidth - 40;
        this.previewCanvas.height = 200;
    }

    updatePreview() {
        if (!this.previewAnimation) return;

        const ctx = this.previewCtx;
        const width = this.previewCanvas.width;
        const height = this.previewCanvas.height;

        // Clear canvas
        ctx.fillStyle = '#1a1a1a';
        ctx.fillRect(0, 0, width, height);

        // Draw each strip
        let yOffset = 20;
        this.generator.strips.forEach((strip, stripIndex) => {
            const ledSize = Math.min(20, width / strip.numLeds);
            const spacing = 2;

            // Draw strip label
            ctx.fillStyle = '#888';
            ctx.font = '12px Arial';
            ctx.fillText(strip.name, 10, yOffset - 5);

            // Draw LEDs
            for (let i = 0; i < strip.numLeds; i++) {
                const x = 10 + i * (ledSize + spacing);
                const color = this.getPreviewColor(strip, i);

                // Draw LED
                ctx.fillStyle = color;
                ctx.fillRect(x, yOffset, ledSize, ledSize);

                // Add glow effect
                ctx.shadowColor = color;
                ctx.shadowBlur = 10;
                ctx.fillRect(x, yOffset, ledSize, ledSize);
                ctx.shadowBlur = 0;
            }

            yOffset += ledSize + 20;
        });
    }

    getPreviewColor(strip, ledIndex) {
        // Simple preview colors based on effect
        switch (strip.effect) {
            case 'rainbow':
                const hue = (ledIndex * 360 / strip.numLeds + Date.now() / 10) % 360;
                return `hsl(${hue}, 100%, 50%)`;
            case 'solid':
                return strip.effectParams.color || '#FFFFFF';
            case 'fire':
                const intensity = Math.random() * 100;
                return `hsl(${10 + Math.random() * 20}, 100%, ${intensity}%)`;
            default:
                return '#FFFFFF';
        }
    }

    startPreview() {
        if (this.previewAnimation) return;

        this.previewAnimation = setInterval(() => {
            this.updatePreview();
        }, 50);

        this.showNotification('Preview started');
    }

    stopPreview() {
        if (this.previewAnimation) {
            clearInterval(this.previewAnimation);
            this.previewAnimation = null;

            // Clear canvas
            const ctx = this.previewCtx;
            ctx.fillStyle = '#1a1a1a';
            ctx.fillRect(0, 0, this.previewCanvas.width, this.previewCanvas.height);

            this.showNotification('Preview stopped');
        }
    }

    showNotification(message, type = 'success') {
        // Simple notification (could be enhanced with a proper notification system)
        console.log(`[${type.toUpperCase()}] ${message}`);

        // Create temporary notification element
        const notification = document.createElement('div');
        notification.style.cssText = `
            position: fixed;
            top: 20px;
            right: 20px;
            background: ${type === 'error' ? '#ff4444' : '#00ff88'};
            color: ${type === 'error' ? 'white' : 'black'};
            padding: 15px 20px;
            border-radius: 5px;
            font-weight: bold;
            z-index: 10000;
            animation: slideIn 0.3s ease;
        `;
        notification.textContent = message;
        document.body.appendChild(notification);

        setTimeout(() => {
            notification.style.animation = 'slideOut 0.3s ease';
            setTimeout(() => {
                document.body.removeChild(notification);
            }, 300);
        }, 3000);
    }

    escapeHtml(text) {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }
}

// Add animations CSS
const style = document.createElement('style');
style.textContent = `
    @keyframes slideIn {
        from {
            transform: translateX(100%);
            opacity: 0;
        }
        to {
            transform: translateX(0);
            opacity: 1;
        }
    }

    @keyframes slideOut {
        from {
            transform: translateX(0);
            opacity: 1;
        }
        to {
            transform: translateX(100%);
            opacity: 0;
        }
    }
`;
document.head.appendChild(style);

// Initialize app when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    window.codeGeneratorApp = new CodeGeneratorApp();
});