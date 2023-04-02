#include <Arduino.h>
#include <arduinoFFT.h>
#include <DacESP32.h> // library to output cosine wave

#define SAMPLES 1024
#define SAMPLING_FREQUENCY 1000 // set in conjunction with timerAlarmWrite() tick rate

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

// timer ISR samples ADC every interrupt until adcBuffer is full
void IRAM_ATTR sample_ADC(){
  portENTER_CRITICAL_ISR(&timerMux);
  if(adcBufferIndex < SAMPLES){
    adcBuffer[adcBufferIndex++] = analogRead(GPIO_NUM_34); 
  }else{
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
    FFT.Windowing(FFT_WIN_TYP_RECTANGLE, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);

    Serial.print(FFT.MajorPeak());
    Serial.println(" Hz");
}

void setup(){
  Serial.begin(115200); // set serial at 115200 baud rate

  dac1.outputCW(16); // output cosine wave at parameter Hz  

  analogReadResolution(12); // 12-bit ADC resolution
  analogSetPinAttenuation(GPIO_NUM_34, ADC_11db); // 11dB attenuation
  pinMode(GPIO_NUM_34, INPUT); // set ADC pin as input

  /*Configure Timer to sample ADC
    
    PRESCALER = [2 - 65355]
        CLOCK = 80 Mhz / PRESCALER

    PRESCALER :  CLOCK :   TICK : CALCULATIONS
            2 : 40 Mhz :  25 ns :   906 Ticks = 22650 ns = 44150 Hz
            2 : 40 Mhz :  25 ns :   907 Ticks = 22675 ns = 44101 Hz
            2 : 40 Mhz :  25 ns :   908 Ticks = 22700 ns = 44053 Hz
           80 :  1 Mhz :   1 us :    20 Ticks =    20 us = 50000 Hz
           80 :  1 Mhz :   1 us :   100 Ticks =   100 us = 10000 Hz           
           80 :  1 Mhz :   1 us :  1000 Ticks =  1000 us =  1000 Hz
  */
  Timer0 = timerBegin(0, 80, true); // Timer 0 is configured to count up with a prescaler of 80
  timerAttachInterrupt(Timer0, &sample_ADC, RISING); // Attaches timer interrupt to the sample_ADC ISR
  timerAlarmWrite(Timer0, 1000, true); // interrupts after 1000 ticks and resets for repeated interrupts
  timerAlarmEnable(Timer0);
}

void loop(){  
  while(start_FFT){  
    portENTER_CRITICAL(&timerMux);
    start_FFT = false; // Reset the start_FFT flag
    adcBufferIndex = 0; // reset adc sample timer index 
    compute_FFT();    
    portEXIT_CRITICAL(&timerMux); 
  }
}