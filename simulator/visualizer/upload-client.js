/**
 * Upload Client - Handles code upload from browser to ESP32
 */

class UploadClient {
    constructor() {
        this.serverUrl = 'http://localhost:3001';
        this.wsUrl = 'ws://localhost:3002';
        this.ws = null;
        this.statusCallback = null;
        this.selectedPort = null;

        this.connectWebSocket();
    }

    connectWebSocket() {
        try {
            this.ws = new WebSocket(this.wsUrl);

            this.ws.onopen = () => {
                console.log('Connected to upload server');
                this.updateStatus('connected', 'Connected to upload server');
            };

            this.ws.onmessage = (event) => {
                const data = JSON.parse(event.data);
                this.handleMessage(data);
            };

            this.ws.onerror = (error) => {
                console.error('WebSocket error:', error);
                this.updateStatus('error', 'Connection error - is upload server running?');
            };

            this.ws.onclose = () => {
                console.log('Disconnected from upload server');
                this.updateStatus('disconnected', 'Disconnected from upload server');
                // Try to reconnect after 5 seconds
                setTimeout(() => this.connectWebSocket(), 5000);
            };
        } catch (error) {
            console.error('Failed to connect to upload server:', error);
            this.updateStatus('error', 'Upload server not available');
        }
    }

    handleMessage(data) {
        switch (data.type) {
            case 'status':
                this.updateStatus('info', data.message);
                break;
            case 'progress':
                this.updateStatus('progress', data.message, data.detail);
                break;
            case 'warning':
                this.updateStatus('warning', data.message, data.detail);
                break;
            case 'success':
                this.updateStatus('success', data.message);
                break;
            case 'error':
                this.updateStatus('error', data.message, data.details || data.detail);
                break;
            case 'connected':
                this.updateStatus('connected', data.message);
                break;
        }
    }

    updateStatus(type, message, details = '') {
        if (this.statusCallback) {
            this.statusCallback(type, message, details);
        }
    }

    setStatusCallback(callback) {
        this.statusCallback = callback;
    }

    async checkServerStatus() {
        try {
            const response = await fetch(`${this.serverUrl}/api/status`);
            const data = await response.json();
            return data.status === 'ready';
        } catch (error) {
            return false;
        }
    }

    async getPorts() {
        try {
            const response = await fetch(`${this.serverUrl}/api/ports`);
            const data = await response.json();
            return data.ports || [];
        } catch (error) {
            console.error('Failed to get ports:', error);
            return [];
        }
    }

    async compileCode(code, environment = 'esp32_simulator') {
        try {
            this.updateStatus('info', 'Starting compilation...');

            const response = await fetch(`${this.serverUrl}/api/compile`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ code, environment })
            });

            const result = await response.json();

            if (!response.ok) {
                throw new Error(result.error || 'Compilation failed');
            }

            return result;
        } catch (error) {
            this.updateStatus('error', 'Compilation failed', error.message);
            throw error;
        }
    }

    async uploadCode(code, environment = 'esp32_simulator', port = 'auto') {
        try {
            this.updateStatus('info', 'Starting upload process...');

            const response = await fetch(`${this.serverUrl}/api/upload`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ code, environment, port })
            });

            const result = await response.json();

            if (!response.ok) {
                throw new Error(result.error || 'Upload failed');
            }

            return result;
        } catch (error) {
            this.updateStatus('error', 'Upload failed', error.message);
            throw error;
        }
    }

    async compileAndUpload(code, environment = 'esp32_simulator', port = 'auto') {
        try {
            // First compile
            await this.compileCode(code, environment);

            // Then upload
            const result = await this.uploadCode(code, environment, port);

            this.updateStatus('success', '✅ Code uploaded successfully!');
            return result;
        } catch (error) {
            this.updateStatus('error', 'Process failed', error.message);
            throw error;
        }
    }
}

// Export for use
export default UploadClient;