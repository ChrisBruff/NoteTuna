#include <Arduino.h>
#include <arduinoFFT.h>
//#include <DacESP32.h>

#define SAMPLES 4096 // need to lower N samples if increasing frequency
#define SAMPLING_FREQUENCY 20000 // set in conjunction with timerAlarmWrite() tick rate

// ADC Buffer and index
double adcBuffer[SAMPLES];
int adcBufferIndex = 0;
bool adcBufferFull = false;
volatile bool sample_adc = false;

// Start FFT flag, real and imag arrays
bool start_FFT = false;
double vReal[SAMPLES];
double vImag[SAMPLES];

// Create DAC object
//DacESP32 dac1(GPIO_NUM_25);

// Initialize the arduinoFFT
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

// Initialize timer
hw_timer_t *Timer0 = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


// timer ISR samples ADC every interrupt until adcBuffer is full
void IRAM_ATTR timer_ISR(){
  portENTER_CRITICAL_ISR(&timerMux);
  sample_adc = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void sample_ADC(){
  if(adcBufferIndex < SAMPLES){
    adcBuffer[adcBufferIndex++] = analogRead(GPIO_NUM_34); 
  }else{
    adcBufferFull = true;
  }
}

void loadBuffer_FFT(){
  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = adcBuffer[i];
    vImag[i] = 0;
  }
  adcBufferFull = false; // Reset the start_FFT flag
  adcBufferIndex = 0; // reset adcBuffer index  
  start_FFT = true;
}

// compute and print fft
void compute_FFT(){
  FFT.Windowing(FFT_WIN_TYP_RECTANGLE, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  Serial.print(FFT.MajorPeak());
  Serial.println(" Hz");
  start_FFT = false; // Reset the print_FFT flag
}

void setup(){
  Serial.begin(115200); // set serial at 115200 baud rate
  //dac1.outputCW(440); // output cosine wave at parameter Hz  
  analogReadResolution(12); // 12-bit ADC resolution
  analogSetPinAttenuation(GPIO_NUM_34, ADC_11db); // 11dB attenuation
  pinMode(GPIO_NUM_34, INPUT); // set ADC pin as input

  /*Configure Timer to sample ADC
    
    PRESCALER = [2 - 65355]
        CLOCK = 80 Mhz / PRESCALER
         TICK = 1 / CLOCK

    PRESCALER :  CLOCK :   TICK : TICKS =  PERIOD  = FREQ
            2 : 40 Mhz :  25 ns :   906 = 22650 ns = 44150 Hz
              :        :        :   907 = 22675 ns = 44101 Hz WORKS N = 64
              :        :        :   908 = 22700 ns = 44053 Hz
              :        :        :  1000 = 25000 ns = 40000 Hz
           80 :  1 Mhz :   1 us :    20 =    20 us = 50000 Hz
              :        :        :    22 =    22 us = 45455 Hz
              :        :        :   100 =   100 us = 10000 Hz N = 2048, inaccurate
              :        :        :   125 =   125 us =  8000 Hz
              :        :        :   200 =   200 us =  5000 Hz N = 2048, inaccurate
              :        :        :   250 =   250 us =  4000 Hz WORKS N = 4096 inaccurate?
              :        :        :   300 =   300 us =  3333 Hz WORKS N = 4096             
              :        :        :   500 =   500 us =  2000 Hz WORKS N = 4096
              :        :        :  1000 =  1000 us =  1000 Hz WORKS N = 4096
  */
  // Timer0
  Timer0 = timerBegin(0, 80, true); // Timer 0 is configured to count up with a prescaler of 80
  timerAttachInterrupt(Timer0, &timer_ISR, RISING); // Attaches timer interrupt to the timer_ISR ISR
  timerAlarmWrite(Timer0, 50, true); // interrupts after 100 ticks and resets for repeated interrupts
  timerAlarmEnable(Timer0); // enable Timer 0
}

void loop(){
  if(sample_adc){
    portENTER_CRITICAL(&timerMux);
    sample_adc = false;
    portEXIT_CRITICAL(&timerMux);
    sample_ADC();
  }

  if(adcBufferFull){
    loadBuffer_FFT();
  }

  if(start_FFT){
    compute_FFT();
  }
}
