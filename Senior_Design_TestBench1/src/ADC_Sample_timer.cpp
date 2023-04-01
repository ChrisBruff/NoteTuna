#include <Arduino.h>
#include <arduinoFFT.h>
#include <DacESP32.h> // library to output cosine wave

#define SAMPLES 1024
#define SAMPLING_FREQUENCY 43478 // set in conjunction with timerAlarmWrite() tick rate

/*  frequency period pairs
    1/f = T us
    1/43478 = 23 us
    1/44100 = 22.67 us
    1/45455 = 22 us
    1/47619 = 21 us
    1/50000 = 20 us
*/

// Index counter for timer
volatile int timerIndex = 0;

// Flags for ADC buffer and FFT ready
volatile bool bufferReady = false;
volatile bool fftReady = false;

// ADC Buffer
double adcBuffer[SAMPLES];

// FFT
double vReal[SAMPLES];
double vImag[SAMPLES];
volatile double peak = 0;

// Create DAC object
DacESP32 dac1(GPIO_NUM_25);

// Initialize the arduinoFFT
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

// Initialize timers
hw_timer_t *Timer0 = NULL;
hw_timer_t *Timer1 = NULL;

// timer ISR to sample from ADC
void IRAM_ATTR ADC_Sample(){
  if((timerIndex < SAMPLES)){
    adcBuffer[timerIndex] = analogRead(GPIO_NUM_34);
    timerIndex++;
  }else if(timerIndex == SAMPLES){
    bufferReady = true; 
  }
}

void IRAM_ATTR FFT_Compute(){
  if (bufferReady && !fftReady){
    // pass adcbuffer to fft arrays
    for (int i = 0; i < SAMPLES; i++) {
      vReal[i] = adcBuffer[i];
      vImag[i] = 0;
    }

    // Compute FFT
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    peak = FFT.MajorPeak();

    // Set the fftReady flag to indicate that the FFT is complete
    fftReady = true;
    bufferReady = false; // Reset the bufferReady flag
    timerIndex = 0; // reset adc sample timer index
  }
}

void setup(){
  dac1.outputCW(440); // output cosine wave at parameter Hz

  analogReadResolution(12); // 12-bit ADC resolution
  analogSetPinAttenuation(GPIO_NUM_34, ADC_11db); // 11dB attenuation
  pinMode(GPIO_NUM_34, INPUT); // set ADC pin as input

  // timer for adc sampling every 23 us or 43478 hz
  Timer0 = timerBegin(0, 80, true); // 80 prescaler for 1MHZ clock from 80MHZ/80
  timerAttachInterrupt(Timer0, &ADC_Sample, RISING);
  timerAlarmWrite(Timer0, 23, true); // 20 ticks at 1MHZ clock = 20 us
  timerAlarmEnable(Timer0);

  // timer for fft after 1024 samples (23 x 1024 = 23552 ticks)
  Timer1 = timerBegin(1, 80, true); // 80 prescaler for 1MHZ clock from 80MHZ/80
  timerAttachInterrupt(Timer1, &FFT_Compute, RISING);
  timerAlarmWrite(Timer1, 23552, true);
  timerAlarmEnable(Timer1);
 
  Serial.begin(9600); // set serial at 9600 baud rate
}

void loop(){
  // If the fft was computed, print frequency and reset flags
  if(fftReady){
    Serial.print(peak);
    Serial.println(" Hz");

    bufferReady = false; // Reset the bufferReady flag 
    fftReady = false; // Reset the fftReady flag
  }
}