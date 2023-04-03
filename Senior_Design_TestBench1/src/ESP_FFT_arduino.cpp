// #include <Arduino.h>
// #include <arduinoFFT.h> 


// void setup() {
//     // Initialize Serial Monitor
//   Serial.begin(9600);

//   // Configure ADC
//   analogReadResolution(12); // Set ADC resolution to 12 bits (0-4095)
//   analogSetAttenuation(ADC_11db); // Set input attenuation to 11dB to allow for a maximum voltage of 3.6V

// }

// void loop() {
  
//   // Read ADC value from pin 36
//   int adcValue = analogRead(34);

//   // Convert ADC value to voltage (assuming 3.6V max input voltage)
//   float voltage = adcValue * (3.6 / 4095.0);

//   // Print ADC value and voltage to Serial Monitor
//   Serial.print("ADC Value: ");
//   Serial.print(adcValue);
//   Serial.print(", Voltage: ");
//   Serial.print(voltage, 2); // Print voltage with 2 decimal places
//   Serial.println("V");

//   // Wait for 1 second before taking another reading
//   delay(100);

// }