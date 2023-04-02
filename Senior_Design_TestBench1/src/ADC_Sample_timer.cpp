#include <Arduino.h>
#include <arduinoFFT.h>
#include <DacESP32.h> // library to output cosine wave

#define SAMPLES 1024
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

// ADC Buffer and index
volatile double adcBuffer[SAMPLES];
volatile int adcBufferIndex = 0;

// Start FFT flag, real and imag arrays
volatile bool start_FFT = false;
double vReal[SAMPLES];
double vImag[SAMPLES];

// Create DAC object
DacESP32 dac1(GPIO_NUM_25);

// Initialize the arduinoFFT
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

// Initialize timer
hw_timer_t *Timer0 = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// timer ISR samples ADC every 22,675 ns
void IRAM_ATTR sample_ADC(){
  portENTER_CRITICAL_ISR(&timerMux);
  if(adcBufferIndex < SAMPLES){
    adcBuffer[adcBufferIndex] = analogRead(GPIO_NUM_34); 
    adcBufferIndex++;
  }else if(adcBufferIndex == SAMPLES){
    start_FFT = true;
  }   
  portEXIT_CRITICAL_ISR(&timerMux);
}

void compute_FFT(){
    // pass adcbuffer to fft arrays
    for (int i = 0; i < SAMPLES; i++) {
      vReal[i] = adcBuffer[i];
      vImag[i] = 0;
    }

    // Compute FFT
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);

    Serial.print(FFT.MajorPeak());
    Serial.println(" Hz");
}
void setup(){
  Serial.begin(115200); // set serial at 115200 baud rate

  dac1.outputCW(440); // output cosine wave at parameter Hz  

  analogReadResolution(12); // 12-bit ADC resolution
  analogSetPinAttenuation(GPIO_NUM_34, ADC_11db); // 11dB attenuation
  pinMode(GPIO_NUM_34, INPUT); // set ADC pin as input

  /*
    Configure Timer to sample ADC
    44101.433296 Hz sampling rate, or 22.675 us sampling period
    
    Timer Clock = 80Mhz / Prescaler
    set Prescaler from [2 - 65355]

    Prescale  2 : 40 Mhz Clock, 1 Tick = 25 ns
    Prescale 80 :  1 Mhz Clock, 1 Tick =  1 us

    A sampling period of 22.675 us requires 907 ticks with a 40 Mhz clock
  */
  Timer0 = timerBegin(0, 2, true); // Timer 0 is configured to count up with a prescaler of 2
  timerAttachInterrupt(Timer0, &sample_ADC, RISING); // Attaches timer interrupt to the sample_ADC ISR
  timerAlarmWrite(Timer0, 907, true); // interrupts after 907 ticks and resets for repeated interrupts
  timerAlarmEnable(Timer0);
}

void loop(){
  if(start_FFT){  
    portENTER_CRITICAL(&timerMux);

    compute_FFT();  
    start_FFT = false; // Reset the start_FFT flag
    adcBufferIndex = 0; // reset adc sample timer index 

    portEXIT_CRITICAL(&timerMux); 
  }
}