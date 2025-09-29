/**
 * Embedded Terminal for LED Visualizer
 * Provides serial communication interface within the visualizer
 */

class EmbeddedTerminal {
    constructor(container, serialPort = null) {
        this.container = container;
        this.serialPort = serialPort;
        this.isConnected = false;
        this.commandHistory = [];
        this.historyIndex = -1;
        this.maxHistorySize = 50;

        this.createTerminalUI();
        this.attachEventListeners();
    }

    createTerminalUI() {
        this.container.innerHTML = `
            <div class="terminal-wrapper">
                <div class="terminal-header">
                    <span class="terminal-title">Serial Terminal - 115200 baud</span>
                    <div class="terminal-controls">
                        <button id="terminalClear" class="terminal-btn">Clear</button>
                        <button id="terminalConnect" class="terminal-btn">Connect</button>
                    </div>
                </div>
                <div id="terminalOutput" class="terminal-output"></div>
                <div class="terminal-input-wrapper">
                    <span class="terminal-prompt">&gt;</span>
                    <input type="text" id="terminalInput" class="terminal-input" placeholder="Type command and press Enter">
                    <button id="terminalSend" class="terminal-btn">Send</button>
                </div>
                <div class="terminal-shortcuts">
                    <button class="shortcut-btn" data-cmd="i">Info</button>
                    <button class="shortcut-btn" data-cmd="h">Help</button>
                    <button class="shortcut-btn" data-cmd="r">Reset</button>
                    <button class="shortcut-btn" data-cmd="1">Toggle 1</button>
                    <button class="shortcut-btn" data-cmd="2">Toggle 2</button>
                    <button class="shortcut-btn" data-cmd="3">Toggle 3</button>
                </div>
            </div>
        `;

        this.output = document.getElementById('terminalOutput');
        this.input = document.getElementById('terminalInput');
        this.connectBtn = document.getElementById('terminalConnect');
    }

    attachEventListeners() {
        // Send button
        document.getElementById('terminalSend').addEventListener('click', () => {
            this.sendCommand();
        });

        // Input field
        this.input.addEventListener('keydown', (e) => {
            if (e.key === 'Enter') {
                this.sendCommand();
            } else if (e.key === 'ArrowUp') {
                this.navigateHistory(-1);
                e.preventDefault();
            } else if (e.key === 'ArrowDown') {
                this.navigateHistory(1);
                e.preventDefault();
            }
        });

        // Clear button
        document.getElementById('terminalClear').addEventListener('click', () => {
            this.clearTerminal();
        });

        // Connect button
        this.connectBtn.addEventListener('click', () => {
            this.toggleConnection();
        });

        // Shortcut buttons
        document.querySelectorAll('.shortcut-btn').forEach(btn => {
            btn.addEventListener('click', (e) => {
                const cmd = e.target.dataset.cmd;
                if (cmd && this.isConnected) {
                    this.sendRawCommand(cmd);
                }
            });
        });
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
            if (!this.serialPort) {
                // Request port selection from user
                this.serialPort = await navigator.serial.requestPort();
            }

            // Open the serial port
            await this.serialPort.open({ baudRate: 115200 });

            this.isConnected = true;
            this.connectBtn.textContent = 'Disconnect';
            this.connectBtn.classList.add('connected');

            this.appendOutput('Connected to serial port at 115200 baud', 'system');

            // Start reading from serial
            this.readSerial();

        } catch (error) {
            this.appendOutput(`Connection failed: ${error.message}`, 'error');
            console.error('Serial connection error:', error);
        }
    }

    async disconnect() {
        try {
            if (this.serialPort && this.serialPort.readable) {
                await this.serialPort.close();
            }

            this.isConnected = false;
            this.connectBtn.textContent = 'Connect';
            this.connectBtn.classList.remove('connected');

            this.appendOutput('Disconnected from serial port', 'system');

        } catch (error) {
            this.appendOutput(`Disconnect error: ${error.message}`, 'error');
            console.error('Serial disconnect error:', error);
        }
    }

    async readSerial() {
        if (!this.serialPort || !this.serialPort.readable) return;

        const reader = this.serialPort.readable.getReader();

        try {
            while (true) {
                const { value, done } = await reader.read();
                if (done) break;

                // Convert the received data to string
                const text = new TextDecoder().decode(value);
                this.appendOutput(text, 'received');
            }
        } catch (error) {
            console.error('Read error:', error);
        } finally {
            reader.releaseLock();
        }
    }

    async sendCommand() {
        const command = this.input.value.trim();
        if (!command) return;

        // Add to history
        this.addToHistory(command);

        // Display in terminal
        this.appendOutput(`> ${command}`, 'sent');

        // Clear input
        this.input.value = '';

        // Send if connected
        if (this.isConnected) {
            await this.sendRawCommand(command);
        } else {
            this.appendOutput('Not connected. Click Connect first.', 'error');
        }
    }

    async sendRawCommand(command) {
        if (!this.serialPort || !this.serialPort.writable) {
            this.appendOutput('Serial port not writable', 'error');
            return;
        }

        try {
            const writer = this.serialPort.writable.getWriter();
            const encoder = new TextEncoder();
            const data = encoder.encode(command + '\n');
            await writer.write(data);
            writer.releaseLock();

            // Show what was sent
            this.appendOutput(`Sent: ${command}`, 'info');

        } catch (error) {
            this.appendOutput(`Send error: ${error.message}`, 'error');
            console.error('Send error:', error);
        }
    }

    appendOutput(text, className = '') {
        const timestamp = new Date().toLocaleTimeString();
        const line = document.createElement('div');
        line.className = `terminal-line ${className}`;

        // Add timestamp for non-empty lines
        if (text.trim()) {
            line.innerHTML = `<span class="timestamp">[${timestamp}]</span> ${this.escapeHtml(text)}`;
        } else {
            line.innerHTML = text;
        }

        this.output.appendChild(line);

        // Auto-scroll to bottom
        this.output.scrollTop = this.output.scrollHeight;

        // Limit output lines
        while (this.output.children.length > 500) {
            this.output.removeChild(this.output.firstChild);
        }
    }

    clearTerminal() {
        this.output.innerHTML = '';
        this.appendOutput('Terminal cleared', 'system');
    }

    addToHistory(command) {
        // Remove if already exists
        const index = this.commandHistory.indexOf(command);
        if (index > -1) {
            this.commandHistory.splice(index, 1);
        }

        // Add to end
        this.commandHistory.push(command);

        // Limit history size
        if (this.commandHistory.length > this.maxHistorySize) {
            this.commandHistory.shift();
        }

        // Reset index
        this.historyIndex = this.commandHistory.length;
    }

    navigateHistory(direction) {
        if (this.commandHistory.length === 0) return;

        this.historyIndex += direction;

        if (this.historyIndex < 0) {
            this.historyIndex = 0;
        } else if (this.historyIndex >= this.commandHistory.length) {
            this.historyIndex = this.commandHistory.length;
            this.input.value = '';
            return;
        }

        this.input.value = this.commandHistory[this.historyIndex];
    }

    escapeHtml(text) {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }

    // Set the serial port (for when already connected from visualizer)
    setSerialPort(port) {
        this.serialPort = port;
        if (port && port.readable) {
            this.isConnected = true;
            this.connectBtn.textContent = 'Disconnect';
            this.connectBtn.classList.add('connected');
            this.readSerial();
        }
    }
}

// Terminal styles
const terminalStyles = `
<style>
.terminal-wrapper {
    background: #0a0a0a;
    border: 1px solid #333;
    border-radius: 5px;
    font-family: 'Courier New', monospace;
    font-size: 12px;
    height: 100%;
    display: flex;
    flex-direction: column;
}

.terminal-header {
    background: #1a1a1a;
    padding: 8px 10px;
    display: flex;
    justify-content: space-between;
    align-items: center;
    border-bottom: 1px solid #333;
}

.terminal-title {
    color: #00ff88;
    font-weight: bold;
}

.terminal-controls {
    display: flex;
    gap: 10px;
}

.terminal-btn {
    padding: 4px 10px;
    background: #333;
    color: #fff;
    border: 1px solid #555;
    border-radius: 3px;
    cursor: pointer;
    font-size: 11px;
}

.terminal-btn:hover {
    background: #444;
}

.terminal-btn.connected {
    background: #00ff88;
    color: black;
}

.terminal-output {
    flex: 1;
    padding: 10px;
    overflow-y: auto;
    background: #0a0a0a;
    color: #fff;
}

.terminal-line {
    margin: 2px 0;
    line-height: 1.4;
    word-wrap: break-word;
}

.terminal-line.sent {
    color: #00ff88;
}

.terminal-line.received {
    color: #ffffff;
}

.terminal-line.error {
    color: #ff4444;
}

.terminal-line.system {
    color: #ffaa00;
    font-style: italic;
}

.terminal-line.info {
    color: #8888ff;
}

.timestamp {
    color: #666;
    font-size: 10px;
}

.terminal-input-wrapper {
    display: flex;
    padding: 10px;
    background: #1a1a1a;
    border-top: 1px solid #333;
    align-items: center;
}

.terminal-prompt {
    color: #00ff88;
    margin-right: 8px;
}

.terminal-input {
    flex: 1;
    background: #0a0a0a;
    color: #fff;
    border: 1px solid #333;
    padding: 5px;
    font-family: 'Courier New', monospace;
    font-size: 12px;
}

.terminal-input:focus {
    outline: none;
    border-color: #00ff88;
}

.terminal-shortcuts {
    display: flex;
    gap: 5px;
    padding: 5px 10px;
    background: #1a1a1a;
    border-top: 1px solid #222;
}

.shortcut-btn {
    padding: 3px 8px;
    background: #2a2a2a;
    color: #888;
    border: 1px solid #333;
    border-radius: 3px;
    cursor: pointer;
    font-size: 10px;
}

.shortcut-btn:hover {
    background: #333;
    color: #fff;
}

/* Terminal in modal */
.terminal-modal {
    position: fixed;
    bottom: 20px;
    right: 20px;
    width: 600px;
    height: 400px;
    z-index: 1000;
    box-shadow: 0 4px 20px rgba(0, 255, 136, 0.3);
    resize: both;
    overflow: auto;
}
</style>
`;

// Add styles to document
if (!document.getElementById('terminal-styles')) {
    const styleElement = document.createElement('div');
    styleElement.id = 'terminal-styles';
    styleElement.innerHTML = terminalStyles;
    document.head.appendChild(styleElement.firstElementChild);
}

// Export for use
export default EmbeddedTerminal;