// an adc conversion appears to take about 90 us
#include <arduino.h>

unsigned long start, end;
uint32_t result;

void setup(){
    Serial.begin(115200);
    analogReadResolution(12);
    analogSetPinAttenuation(GPIO_NUM_34, ADC_11db);
    pinMode(GPIO_NUM_34, INPUT);
}

void loop() {
    start = micros();
    result = analogRead(GPIO_NUM_34);
    end = micros();
    Serial.println(end - start);
}