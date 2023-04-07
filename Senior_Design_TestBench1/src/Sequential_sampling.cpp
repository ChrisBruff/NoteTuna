#include <arduinoFFT.h>
//#include <DacESP32.h>

#define SAMPLES 4096
#define SAMPLING_FREQUENCY 3000

unsigned int sampling_period_us;
unsigned long newTime;
double vReal[SAMPLES];
double vImag[SAMPLES];

//DacESP32 dac1(GPIO_NUM_25);
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

void setup(){
    Serial.begin(115200);
    analogReadResolution(12);
    analogSetPinAttenuation(GPIO_NUM_34, ADC_11db);
    pinMode(GPIO_NUM_34, INPUT);
    sampling_period_us = round(1000000 * 1 / SAMPLING_FREQUENCY);
    //dac1.outputCW(440);
}

void loop(){
    for (int i = 0; i < SAMPLES; i++) {
        newTime = micros();
        vReal[i] = analogRead(GPIO_NUM_34); // A conversion takes about 9.7uS on an ESP32
        vImag[i] = 0;
        while ((micros() - newTime) < sampling_period_us){}
    }

    FFT.Windowing(FFT_WIN_TYP_RECTANGLE, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);

    Serial.print(FFT.MajorPeak());
    Serial.println(" Hz");
}