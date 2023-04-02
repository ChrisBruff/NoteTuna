#include <Arduino.h>
#include <arduinoFFT.h>
#include <DacESP32.h> // library to output cosine wave

#define SAMPLES 2048
#define SAMPLING_FREQUENCY 44101 // set in conjunction with timerAlarmWrite() tick rate

/*// frequency period pairs
  1/f = T us
  1/43478 = 23 us
  1/44099 = 22.6760 us
  1/44100 = 22.6757369... us
  1/44101 = 22.6750 us
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

// Create DAC object
DacESP32 dac1(GPIO_NUM_25);

// Initialize the arduinoFFT
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

// Initialize timers
hw_timer_t *Timer0 = NULL;

// timer ISR to sample from ADC
void IRAM_ATTR sample_ADC(){
  if((timerIndex < SAMPLES) && !bufferReady){
    adcBuffer[timerIndex] = analogRead(GPIO_NUM_34);
    timerIndex++;
  }else if(timerIndex == SAMPLES){
    bufferReady = true; 
  }
}

void compute_FFT(){
  if (bufferReady && !fftReady){
    // pass adcbuffer to fft arrays
    for (int i = 0; i < SAMPLES; i++) {
      vReal[i] = adcBuffer[i];
      vImag[i] = 0;
    }

    // Compute FFT
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);

    // Set the fftReady flag to indicate that the FFT is complete
    fftReady = true;
    bufferReady = false; // Reset the bufferReady flag
  }

  // If the fft was computed, print frequency and reset flags
  if(fftReady){   
    Serial.print(FFT.MajorPeak());
    Serial.println(" Hz");

    bufferReady = false; // Reset the bufferReady flag 
    fftReady = false; // Reset the fftReady flag
    timerIndex = 0; // reset adc sample timer index 
    delay(1000);
  }
}

void setup(){
  Serial.begin(9600); // set serial at 115200 baud rate

  dac1.outputCW(440); // output cosine wave at parameter Hz  

  analogReadResolution(12); // 12-bit ADC resolution
  analogSetPinAttenuation(GPIO_NUM_34, ADC_11db); // 11dB attenuation
  pinMode(GPIO_NUM_34, INPUT); // set ADC pin as input

  // Timer0 with 1Mhz Clock or 1 us ticks
  /*
  // 50Khz sampling rate or 20 us sampling period with prescaler of 80 and 20 ticks
  Timer0 = timerBegin(0, 80, true); // prescaler
  timerAttachInterrupt(Timer0, &sample_ADC, RISING);
  timerAlarmWrite(Timer0, 20, true); // ticks
  timerAlarmEnable(Timer0);
  */

  /*
    Configure Timer to sample ADC
    44101.433296 Hz sampling rate, or 22.675 us sampling period
    
    Timer Clock = 80Mhz / Prescaler
    set Prescaler from [2 - 65355]
    
    Prescaler of 2 : sets a 40 Mhz Clock that ticks every 25 ns
    A sampling period of 22.675 us requires 907 ticks with a 40 Mhz clock
  */
  Timer0 = timerBegin(0, 2, true); // Timer 0 is configured to count up with a prescaler of 2
  timerAttachInterrupt(Timer0, &sample_ADC, RISING); // Attaches timer interrupt to the sample_ADC ISR
  timerAlarmWrite(Timer0, 907, true); // interrupts after 907 ticks and resets for repeated interrupts
  timerAlarmEnable(Timer0);
}

void loop(){
  compute_FFT();
}