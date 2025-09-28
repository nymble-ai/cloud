export class SerialBridge {
    constructor(onFrame, onStatusChange) {
        this.port = null;
        this.reader = null;
        this.writer = null;
        this.connected = false;
        this.onFrame = onFrame;
        this.onStatusChange = onStatusChange;
        this.parser = new SerialProtocolParser();
        this.frameCount = 0;
        this.lastFrameTime = 0;
        this.fps = 0;
    }

    async connect() {
        if (!('serial' in navigator)) {
            throw new Error('Web Serial API not supported. Use Chrome, Edge, or Opera.');
        }

        try {
            this.port = await navigator.serial.requestPort();
            await this.port.open({ baudRate: 115200 });

            this.connected = true;
            this.onStatusChange('connected');

            this.startReading();

            return true;
        } catch (error) {
            console.error('Connection failed:', error);
            this.onStatusChange('error', error.message);
            throw error;
        }
    }

    async disconnect() {
        if (this.reader) {
            await this.reader.cancel();
            this.reader = null;
        }

        if (this.port) {
            await this.port.close();
            this.port = null;
        }

        this.connected = false;
        this.onStatusChange('disconnected');
    }

    async startReading() {
        try {
            const decoder = new TextDecoderStream();
            this.port.readable.pipeTo(decoder.writable);
            this.reader = decoder.readable.getReader();

            while (true) {
                const { value, done } = await this.reader.read();
                if (done) {
                    break;
                }

                for (let i = 0; i < value.length; i++) {
                    const byte = value.charCodeAt(i);
                    const leds = this.parser.parse(byte);

                    if (leds) {
                        this.frameCount++;
                        const now = performance.now();
                        if (this.lastFrameTime > 0) {
                            const elapsed = now - this.lastFrameTime;
                            this.fps = 1000 / elapsed;
                        }
                        this.lastFrameTime = now;

                        this.onFrame(leds, this.fps);
                    }
                }
            }
        } catch (error) {
            if (error.name !== 'AbortError') {
                console.error('Reading error:', error);
                this.onStatusChange('error', error.message);
            }
        }
    }

    isConnected() {
        return this.connected;
    }

    getFrameCount() {
        return this.frameCount;
    }

    getFPS() {
        return Math.round(this.fps);
    }
}

class SerialProtocolParser {
    constructor() {
        this.FRAME_START = 0xFF;
        this.FRAME_END = 0xFE;
        this.ESCAPE_BYTE = 0xFD;

        this.state = 'WAITING_START';
        this.buffer = [];
        this.ledCount = 0;
        this.expectedBytes = 0;
        this.escaped = false;
    }

    parse(byte) {
        if (this.escaped) {
            this.buffer.push(byte);
            this.escaped = false;
            this.checkIfComplete();
            return null;
        }

        if (byte === this.ESCAPE_BYTE) {
            this.escaped = true;
            return null;
        }

        if (byte === this.FRAME_START) {
            this.state = 'COUNT_HIGH';
            this.buffer = [];
            this.ledCount = 0;
            this.expectedBytes = 0;
            return null;
        }

        if (byte === this.FRAME_END && this.state === 'LED_DATA') {
            return this.processFrame();
        }

        switch (this.state) {
            case 'COUNT_HIGH':
                this.ledCount = byte << 8;
                this.state = 'COUNT_LOW';
                break;

            case 'COUNT_LOW':
                this.ledCount |= byte;
                this.expectedBytes = this.ledCount * 3;
                this.state = 'LED_DATA';
                break;

            case 'LED_DATA':
                this.buffer.push(byte);
                this.checkIfComplete();
                break;
        }

        return null;
    }

    checkIfComplete() {
        if (this.state === 'LED_DATA' && this.buffer.length === this.expectedBytes) {
        }
    }

    processFrame() {
        if (this.buffer.length !== this.expectedBytes) {
            console.error(`Frame size mismatch: received ${this.buffer.length}, expected ${this.expectedBytes}`);
            this.state = 'WAITING_START';
            return null;
        }

        const leds = [];
        for (let i = 0; i < this.buffer.length; i += 3) {
            leds.push({
                r: this.buffer[i],
                g: this.buffer[i + 1],
                b: this.buffer[i + 2]
            });
        }

        this.state = 'WAITING_START';
        return leds;
    }

    reset() {
        this.state = 'WAITING_START';
        this.buffer = [];
        this.ledCount = 0;
        this.expectedBytes = 0;
        this.escaped = false;
    }
}