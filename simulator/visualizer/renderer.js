export class LEDRenderer {
    constructor(canvas) {
        this.canvas = canvas;
        this.ctx = canvas.getContext('2d');
        this.leds = new Array(90).fill(null).map(() => ({ r: 0, g: 0, b: 0 }));
        this.config = {
            ledSize: 20,
            ledSpacing: 5,
            glowIntensity: 0.3,
            glowRadius: 10,
            backgroundColor: '#1a1a1a',
            offLedColor: '#222222',
            showIndices: false
        };
        this.layout = {
            type: 'strip',
            count: 90,
            orientation: 'horizontal',
            width: 8,
            height: 8,
            serpentine: true,
            radius: 150
        };
        this.positions = [];
        this.resizeCanvas();
        this.calculatePositions();
    }

    setConfig(config) {
        Object.assign(this.config, config);
        this.resizeCanvas();
        this.calculatePositions();
    }

    setLayout(layout) {
        Object.assign(this.layout, layout);
        this.leds = new Array(this.layout.count).fill(null).map(() => ({ r: 0, g: 0, b: 0 }));
        this.resizeCanvas();
        this.calculatePositions();
    }

    resizeCanvas() {
        const container = this.canvas.parentElement;
        const containerWidth = container.clientWidth - 40;
        const containerHeight = container.clientHeight - 40;

        let requiredWidth, requiredHeight;

        switch (this.layout.type) {
            case 'strip':
                if (this.layout.orientation === 'horizontal') {
                    requiredWidth = this.layout.count * (this.config.ledSize + this.config.ledSpacing);
                    requiredHeight = this.config.ledSize + 40;
                } else {
                    requiredWidth = this.config.ledSize + 40;
                    requiredHeight = this.layout.count * (this.config.ledSize + this.config.ledSpacing);
                }
                break;

            case 'matrix':
                requiredWidth = this.layout.width * (this.config.ledSize + this.config.ledSpacing);
                requiredHeight = this.layout.height * (this.config.ledSize + this.config.ledSpacing);
                break;

            case 'ring':
                const diameter = this.layout.radius * 2 + this.config.ledSize + 40;
                requiredWidth = diameter;
                requiredHeight = diameter;
                break;

            default:
                requiredWidth = 800;
                requiredHeight = 600;
        }

        const scale = Math.min(
            containerWidth / requiredWidth,
            containerHeight / requiredHeight,
            1
        );

        this.canvas.width = requiredWidth * scale;
        this.canvas.height = requiredHeight * scale;
        this.scale = scale;
    }

    calculatePositions() {
        this.positions = [];
        const padding = 20 * this.scale;

        switch (this.layout.type) {
            case 'strip':
                this.calculateStripPositions(padding);
                break;
            case 'matrix':
                this.calculateMatrixPositions(padding);
                break;
            case 'ring':
                this.calculateRingPositions();
                break;
        }
    }

    calculateStripPositions(padding) {
        const ledSize = this.config.ledSize * this.scale;
        const ledSpacing = this.config.ledSpacing * this.scale;

        for (let i = 0; i < this.layout.count; i++) {
            if (this.layout.orientation === 'horizontal') {
                this.positions.push({
                    x: padding + i * (ledSize + ledSpacing),
                    y: this.canvas.height / 2
                });
            } else {
                this.positions.push({
                    x: this.canvas.width / 2,
                    y: padding + i * (ledSize + ledSpacing)
                });
            }
        }
    }

    calculateMatrixPositions(padding) {
        const ledSize = this.config.ledSize * this.scale;
        const ledSpacing = this.config.ledSpacing * this.scale;

        for (let y = 0; y < this.layout.height; y++) {
            for (let x = 0; x < this.layout.width; x++) {
                let actualX = x;

                if (this.layout.serpentine && y % 2 === 1) {
                    actualX = this.layout.width - 1 - x;
                }

                this.positions.push({
                    x: padding + actualX * (ledSize + ledSpacing),
                    y: padding + y * (ledSize + ledSpacing)
                });
            }
        }
    }

    calculateRingPositions() {
        const centerX = this.canvas.width / 2;
        const centerY = this.canvas.height / 2;
        const radius = this.layout.radius * this.scale;

        for (let i = 0; i < this.layout.count; i++) {
            const angle = (i / this.layout.count) * Math.PI * 2 - Math.PI / 2;
            this.positions.push({
                x: centerX + Math.cos(angle) * radius,
                y: centerY + Math.sin(angle) * radius
            });
        }
    }

    updateLEDs(leds) {
        if (leds.length !== this.leds.length) {
            console.warn(`LED count mismatch: received ${leds.length}, expected ${this.leds.length}`);
            return;
        }
        this.leds = leds;
    }

    render() {
        this.ctx.fillStyle = this.config.backgroundColor;
        this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);

        const ledSize = this.config.ledSize * this.scale;

        for (let i = 0; i < this.leds.length; i++) {
            if (i >= this.positions.length) break;

            const led = this.leds[i];
            const pos = this.positions[i];
            const isOff = led.r === 0 && led.g === 0 && led.b === 0;

            if (!isOff && this.config.glowIntensity > 0) {
                const glowRadius = this.config.glowRadius * this.scale;
                const gradient = this.ctx.createRadialGradient(
                    pos.x, pos.y, ledSize / 4,
                    pos.x, pos.y, glowRadius
                );
                gradient.addColorStop(0, `rgba(${led.r}, ${led.g}, ${led.b}, ${this.config.glowIntensity})`);
                gradient.addColorStop(1, 'rgba(0, 0, 0, 0)');

                this.ctx.fillStyle = gradient;
                this.ctx.fillRect(
                    pos.x - glowRadius,
                    pos.y - glowRadius,
                    glowRadius * 2,
                    glowRadius * 2
                );
            }

            this.ctx.beginPath();
            this.ctx.arc(pos.x, pos.y, ledSize / 2, 0, Math.PI * 2);

            if (isOff) {
                this.ctx.fillStyle = this.config.offLedColor;
            } else {
                this.ctx.fillStyle = `rgb(${led.r}, ${led.g}, ${led.b})`;
            }
            this.ctx.fill();

            this.ctx.strokeStyle = '#000';
            this.ctx.lineWidth = 1;
            this.ctx.stroke();

            if (this.config.showIndices) {
                this.ctx.fillStyle = isOff ? '#666' : '#fff';
                this.ctx.font = `${Math.max(8, ledSize / 3)}px monospace`;
                this.ctx.textAlign = 'center';
                this.ctx.textBaseline = 'middle';
                this.ctx.fillText(i, pos.x, pos.y);
            }
        }
    }

    clear() {
        this.leds = new Array(this.layout.count).fill(null).map(() => ({ r: 0, g: 0, b: 0 }));
        this.render();
    }

    testPattern(pattern) {
        switch (pattern) {
            case 'rainbow':
                this.testRainbow();
                break;
            case 'chase':
                this.testChase();
                break;
            case 'random':
                this.testRandom();
                break;
        }
    }

    testRainbow() {
        for (let i = 0; i < this.leds.length; i++) {
            const hue = (i / this.leds.length) * 360;
            const rgb = this.hsvToRgb(hue, 100, 100);
            this.leds[i] = rgb;
        }
        this.render();
    }

    testChase() {
        let position = 0;
        const interval = setInterval(() => {
            this.clear();
            for (let i = 0; i < 5; i++) {
                const idx = (position + i) % this.leds.length;
                this.leds[idx] = { r: 255, g: 255, b: 255 };
            }
            this.render();
            position = (position + 1) % this.leds.length;
        }, 50);

        setTimeout(() => clearInterval(interval), 5000);
    }

    testRandom() {
        const interval = setInterval(() => {
            for (let i = 0; i < this.leds.length; i++) {
                this.leds[i] = {
                    r: Math.floor(Math.random() * 256),
                    g: Math.floor(Math.random() * 256),
                    b: Math.floor(Math.random() * 256)
                };
            }
            this.render();
        }, 100);

        setTimeout(() => clearInterval(interval), 5000);
    }

    hsvToRgb(h, s, v) {
        s /= 100;
        v /= 100;
        const c = v * s;
        const x = c * (1 - Math.abs(((h / 60) % 2) - 1));
        const m = v - c;
        let r, g, b;

        if (h < 60) { r = c; g = x; b = 0; }
        else if (h < 120) { r = x; g = c; b = 0; }
        else if (h < 180) { r = 0; g = c; b = x; }
        else if (h < 240) { r = 0; g = x; b = c; }
        else if (h < 300) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }

        return {
            r: Math.round((r + m) * 255),
            g: Math.round((g + m) * 255),
            b: Math.round((b + m) * 255)
        };
    }
}