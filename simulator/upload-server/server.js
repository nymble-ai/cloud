/**
 * Upload Server - Compiles and uploads Arduino code to ESP32
 * Provides a web API for the visualizer to upload code directly
 */

const express = require('express');
const cors = require('cors');
const { exec } = require('child_process');
const fs = require('fs').promises;
const path = require('path');
const WebSocket = require('ws');

const app = express();
const PORT = 3001;

// Middleware
app.use(cors());
app.use(express.json({ limit: '10mb' }));
app.use(express.text({ limit: '10mb' }));

// WebSocket server for real-time updates
const wss = new WebSocket.Server({ port: 3002 });

// Broadcast to all connected clients
function broadcast(data) {
    wss.clients.forEach(client => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(JSON.stringify(data));
        }
    });
}

// Firmware directory path
const FIRMWARE_DIR = path.join(__dirname, '../../firmware');
const MAIN_FILE = path.join(FIRMWARE_DIR, 'src', 'main.cpp');

/**
 * Save and compile Arduino code
 */
app.post('/api/compile', async (req, res) => {
    try {
        const { code, environment = 'esp32_simulator' } = req.body;

        if (!code) {
            return res.status(400).json({ error: 'No code provided' });
        }

        broadcast({ type: 'status', message: 'Saving code...' });

        // Save the code to main.cpp
        await fs.writeFile(MAIN_FILE, code);

        broadcast({ type: 'status', message: 'Compiling...' });

        // Compile the code
        exec(`pio run -e ${environment}`, { cwd: FIRMWARE_DIR }, (error, stdout, stderr) => {
            if (error) {
                console.error('Compile error:', error);
                broadcast({ type: 'error', message: 'Compilation failed', details: stderr });
                return res.status(500).json({
                    success: false,
                    error: 'Compilation failed',
                    details: stderr
                });
            }

            broadcast({ type: 'success', message: 'Compilation successful!' });
            res.json({
                success: true,
                message: 'Code compiled successfully',
                output: stdout
            });
        });

    } catch (error) {
        console.error('Server error:', error);
        broadcast({ type: 'error', message: error.message });
        res.status(500).json({ error: error.message });
    }
});

/**
 * Upload compiled code to ESP32
 */
app.post('/api/upload', async (req, res) => {
    try {
        const { code, environment = 'esp32_simulator', port = 'auto' } = req.body;

        broadcast({ type: 'status', message: 'Preparing upload...' });

        // Save the code if provided
        if (code) {
            await fs.writeFile(MAIN_FILE, code);
        }

        // Build upload command
        let uploadCmd = `pio run -e ${environment} -t upload`;
        if (port !== 'auto') {
            uploadCmd += ` --upload-port ${port}`;
        }

        broadcast({ type: 'status', message: 'Starting compilation and upload...' });

        // Execute upload with real-time output streaming
        const { spawn } = require('child_process');
        const args = ['run', '-e', environment, '-t', 'upload'];
        if (port !== 'auto') {
            args.push('--upload-port', port);
        }

        const uploadProcess = spawn('pio', args, {
            cwd: FIRMWARE_DIR,
            shell: false  // Use false for proper argument handling
        });

        let output = '';
        let hasError = false;

        // Stream stdout in real-time
        uploadProcess.stdout.on('data', (data) => {
            const text = data.toString();
            output += text;

            // Parse and send progress updates
            const lines = text.split('\n').filter(line => line.trim());
            lines.forEach(line => {
                if (line.includes('Building')) {
                    broadcast({ type: 'progress', message: '🔨 Building...', detail: line });
                } else if (line.includes('Compiling')) {
                    broadcast({ type: 'progress', message: '⚙️ Compiling...', detail: line });
                } else if (line.includes('Linking')) {
                    broadcast({ type: 'progress', message: '🔗 Linking...', detail: line });
                } else if (line.includes('Uploading')) {
                    broadcast({ type: 'progress', message: '📤 Uploading to ESP32...', detail: line });
                } else if (line.includes('Writing at')) {
                    // Extract progress percentage
                    const match = line.match(/(\d+)%/);
                    if (match) {
                        broadcast({ type: 'progress', message: `📤 Uploading... ${match[1]}%`, detail: line });
                    }
                } else if (line.includes('SUCCESS')) {
                    broadcast({ type: 'progress', message: '✅ Success!', detail: line });
                }
            });
        });

        // Stream stderr
        uploadProcess.stderr.on('data', (data) => {
            const text = data.toString();
            output += text;

            if (text.includes('error') || text.includes('Error')) {
                hasError = true;
                broadcast({ type: 'error', message: '❌ Error detected', detail: text });
            } else {
                broadcast({ type: 'warning', message: '⚠️ Warning', detail: text });
            }
        });

        // Handle completion
        uploadProcess.on('close', (code) => {
            if (code !== 0 || hasError) {
                broadcast({ type: 'error', message: 'Upload failed', details: output });
                return res.status(500).json({
                    success: false,
                    error: 'Upload failed',
                    details: output
                });
            }

            broadcast({ type: 'success', message: '✅ Upload complete! ESP32 is ready.' });
            res.json({
                success: true,
                message: 'Code uploaded successfully',
                output: output
            });
        });

    } catch (error) {
        console.error('Server error:', error);
        broadcast({ type: 'error', message: error.message });
        res.status(500).json({ error: error.message });
    }
});

/**
 * Get available serial ports
 */
app.get('/api/ports', (req, res) => {
    exec('pio device list --json-output', (error, stdout, stderr) => {
        if (error) {
            // Fallback to simpler method
            exec('ls /dev/tty.* | grep -i usb', (error2, stdout2, stderr2) => {
                if (error2) {
                    return res.json({ ports: [] });
                }
                const ports = stdout2.trim().split('\n').filter(p => p);
                res.json({ ports });
            });
            return;
        }

        try {
            const data = JSON.parse(stdout);
            res.json({ ports: data });
        } catch (e) {
            res.json({ ports: [] });
        }
    });
});

/**
 * Get compilation status
 */
app.get('/api/status', (req, res) => {
    res.json({
        status: 'ready',
        firmwareDir: FIRMWARE_DIR,
        mainFile: MAIN_FILE
    });
});

/**
 * Clean build files
 */
app.post('/api/clean', (req, res) => {
    exec('pio run -t clean', { cwd: FIRMWARE_DIR }, (error, stdout, stderr) => {
        if (error) {
            return res.status(500).json({ error: 'Clean failed' });
        }
        res.json({ success: true, message: 'Build files cleaned' });
    });
});

// WebSocket connection handler
wss.on('connection', (ws) => {
    console.log('WebSocket client connected');
    ws.send(JSON.stringify({ type: 'connected', message: 'Connected to upload server' }));

    ws.on('close', () => {
        console.log('WebSocket client disconnected');
    });
});

// Start server
app.listen(PORT, () => {
    console.log(`Upload server running on http://localhost:${PORT}`);
    console.log(`WebSocket server running on ws://localhost:3002`);
    console.log(`Firmware directory: ${FIRMWARE_DIR}`);
});

// Error handling
process.on('unhandledRejection', (reason, promise) => {
    console.error('Unhandled Rejection at:', promise, 'reason:', reason);
});

module.exports = app;