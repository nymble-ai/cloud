#include "driver/i2s.h"
#include "arduinoFFT.h"   // We'll start with this for simplicity
#include <FastLED.h>

// LED Configuration
#define LED_PIN 5
#define NUM_ROWS 16
#define NUM_COLS 16
#define NUM_LEDS 256
#define BRIGHTNESS 20

CRGB leds[NUM_ROWS * NUM_COLS];

// I2S Configuration
#define I2S_WS 25
#define I2S_SD 32
#define I2S_SCK 33
#define SAMPLE_RATE 16000
#define SAMPLES 512
#define BANDS 16   // Start simple with 8 bands

// FFT
double vReal[SAMPLES];
double vImag[SAMPLES];
int bandValues[BANDS];
int oldBarHeights[BANDS];
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SAMPLE_RATE);


void setup() {
 Serial.begin(115200);
   
   // Initialize FastLED
   FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
   FastLED.setBrightness(BRIGHTNESS);
   FastLED.clear();
   FastLED.show();
   
   // Configure I2S
   setupI2S();
   
   Serial.println("ESP32 Spectrum Analyzer MVP Ready!");
   Serial.println("Play some music and watch the LEDs dance!");
}

void loop() {
   // Sample audio
   sampleAudio();
   
   // Perform FFT
   FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
   FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
   FFT.complexToMagnitude(vReal, vImag, SAMPLES);
   
   // Calculate 8 frequency bands
   calculateBands();
   
   // Update LED strip
   updateLEDs();
   
   FastLED.show();
   delay(10); //  FPS
}

void setupI2S() {
   i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 4,
      .dma_buf_len = 64
   };
   
   i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_SCK,
      .ws_io_num = I2S_WS,
      .data_out_num = I2S_PIN_NO_CHANGE,
      .data_in_num = I2S_SD
   };
   
   i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
   i2s_set_pin(I2S_NUM_0, &pin_config);
   i2s_start(I2S_NUM_0);
}

void sampleAudio() {
   size_t bytes_read;
   int32_t samples[SAMPLES];
   
   i2s_read(I2S_NUM_0, samples, sizeof(samples), &bytes_read, portMAX_DELAY);
   
   for(int i = 0; i < SAMPLES; i++) {
      vReal[i] = samples[i] >> 16; // Convert 32-bit to 16-bit
      vImag[i] = 0.0;
   }
}

void calculateBands() {
   // Simple 8-band mapping (each band gets multiple FFT bins)
   int bins_per_band = (SAMPLES / 2) / BANDS;
   
   for(int band = 0; band < BANDS; band++) {
      float band_sum = 0;
      int start_bin = band * bins_per_band;
      int end_bin = start_bin + bins_per_band;
      
      for(int bin = start_bin; bin < end_bin; bin++) {
         band_sum += vReal[bin];
      }
      
      // Scale to LED count per band (256 LEDs / 16 bands = 16 LEDs per band)
      bandValues[band] = map(band_sum / bins_per_band, 0, 2000, 0, 15);
      bandValues[band] = constrain(bandValues[band], 0, 15);
   }
}

void updateLEDs() {
   FastLED.clear();
      
   int col;
   int row;
   int pix;
   int rowPix;


   for(col = 0; col < BANDS; col++)
   {
      // Color mapping: Red (bass) → Blue (treble)
      CHSV band_color = CHSV(
                              map(col, 0, BANDS-1, 0, 160),            // Hue: Red to Blue
                                                           255,        // Full saturation
                                                               255 );  // Full brightness
     

      for(int row = 0; row < bandValues[col]; row++)
      {                  
         // Dimmer LEDs at the bottom, brighter at top
         CHSV led_color = band_color;
         led_color.val = map(col, 0, 15, 80, 255);
                  
         rowPix = (row * 16);
         pix = rowPix + col;
         if(row%2 != 0)                   
         {
           pix = rowPix + 15 - col;
         }
         
                   
                        
         leds[pix] = led_color;
      }     
   }
}

