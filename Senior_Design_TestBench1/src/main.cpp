// #include <Arduino.h>
// #include <arduinoFFT.h>


// // Define the ADC pin, number of samples, and sampling frequency
// #define ADC_PIN 34
// #define SAMPLES 1024
// #define SAMPLING_FREQUENCY 42100

// // Flags to indicate when the ADC buffer is full and when the FFT is ready
// volatile bool bufferReady = false;
// volatile bool fftReady = false;

// // Buffers for ADC samples and FFT magnitudes
// double adcBuffer[SAMPLES];
// double fftBuffer[SAMPLES / 2];

// // Arrays for the FFT input and output data
// double vReal[SAMPLES];
// double vImag[SAMPLES];

// // Initialize the arduinoFFT library with the input/output arrays, number of samples, and sampling frequency
// arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

// // Structure to store musical notes and their frequencies
// struct Note {
//   const char* name;
//   double frequency;
// };

// // Lookup table of musical notes and their frequencies
// const Note noteTable[] = {
//   {"C0", 16.35},
//   {"C#0/Db0", 17.32},
//   {"D0", 18.35},
//   {"D#0/Eb0", 19.45},
//   {"E0", 20.60},
//   {"F0", 21.83},
//   {"F#0/Gb0", 23.12},
//   {"G0", 24.50},
//   {"G#0/Ab0", 25.96},
//   {"A0", 27.50},
//   {"A#0/Bb0", 29.14},
//   {"B0", 30.87},
//   {"C1", 32.70},
//   {"C#1/Db1", 34.65},
//   {"D1", 36.71},
//   {"D#1/Eb1", 38.89},
//   {"E1", 41.20},
//   {"F1", 43.65},
//   {"F#1/Gb1", 46.25},
//   {"G1", 49.00},
//   {"G#1/Ab1", 51.91},
//   {"A1", 55.00},
//   {"A#1/Bb1", 58.27},
//   {"B1", 61.74},
//   {"C2", 65.41},
//   {"C#2/Db2", 69.30},
//   {"D2", 73.42},
//   {"D#2/Eb2", 77.78},
//   {"E2", 82.41},
//   {"F2", 87.31},
//   {"F#2/Gb2", 92.50},
//   {"G2", 98.00},
//   {"G#2/Ab2", 103.83},
//   {"A2", 110.00},
//   {"A#2/Bb2", 116.54},
//   {"B2", 123.47},
//   {"C3", 130.81},
//   {"C#3/Db3", 138.59},
//   {"D3", 146.83},
//   {"D#3/Eb3", 155.56},
//   {"E3", 164.81},
//   {"F3", 174.61},
//   {"F#3/Gb3", 185.00},
//   {"G3", 196.00},
//   {"G#3/Ab3", 207.65},
//   {"A3", 220.00},
//   {"A#3/Bb3", 233.08},
//   {"B3", 246.94},
//   {"C4", 261.63},
//   {"C#4/Db4", 277.18},
//   {"D4", 293.66},
//   {"D#4/Eb4", 311.13},
//   {"E4", 329.63},
//   {"F4", 349.23},
//   {"F#4/Gb4", 369.99},
//   {"G4", 392.00},
//   {"G#4/Ab4", 415.30},
//   {"A4", 440.00},
//   {"A#4/Bb4", 466.16},
//   {"B4", 493.88},
//   {"C5", 523.25},
//   {"C#5/Db5", 554.37},
//   {"D5", 587.33},
//   {"D#5/Eb5", 622.25},
//   {"E5", 659.26},
//   {"F5", 698.46},
//   {"F#5/Gb5", 739.99},
//   {"G5", 783.99},
//   {"G#5/Ab5", 830.61},
//   {"A5", 880.00},
//   {"A#5/Bb5", 932.33},
//   {"B5", 987.77},
//   {"C6", 1046.50},
//   {"C#6/Db6", 1108.73},
//   {"D6", 1174.66},
//   {"D#6/Eb6", 1244.51},
//   {"E6", 1318.51},
//   {"F6", 1396.91},
//   {"F#6/Gb6", 1479.98},
//   {"G6", 1567.98},
//   {"G#6/Ab6", 1661.22},
//   {"A6", 1760.00},
//   {"A#6/Bb6", 1864.66},
//   {"B6", 1975.53},
//   {"C7", 2093.00},
//   {"C#7/Db7", 2217.46},
//   {"D7", 2349.32},
//   {"D#7/Eb7", 2489.02},
//   {"E7", 2637.02},
//   {"F7", 2793.83},
//   {"F#7/Gb7", 2959.96},
//   {"G7", 3135.96},
//   {"G#7/Ab7", 3322.44},
//   {"A7", 3520.00},
//   {"A#7/Bb7", 3729.31},
//   {"B7", 3951.07},
//   {"C8", 4186.01},
//   {"C#8/Db8", 4434.92},
//   {"D8", 4698.64},
//   {"D#8/Eb8", 4978.03},
//   {"E8", 5274.04},
//   {"F8", 5587.65},
//   {"F#8/Gb8", 5919.91},
//   {"G8", 6271.93},
//   {"G#8/Ab8", 6644.88},
//   {"A8", 7040.00},
//   {"A#8/Bb8", 7458.62},
//   {"B8", 7902.13},
// };

// const size_t noteTableSize = sizeof(noteTable) / sizeof(Note);

// // Function to find the closest musical note and percentage error for a given frequency
// Note findClosestNote(double frequency, double& percentError) {
//   Note closestNote = noteTable[0];
//   double minDifference = fabs(frequency - noteTable[0].frequency);
//   percentError = (minDifference / closestNote.frequency) * 100;

//   for (size_t i = 1; i < noteTableSize; i++) {
//     double difference = fabs(frequency - noteTable[i].frequency);
//     double currentPercentError = (difference / noteTable[i].frequency) * 100;

//     if (difference < minDifference) {
//       closestNote = noteTable[i];
//       minDifference = difference;
//       percentError = currentPercentError;
//     }
//   }

//   return closestNote;
// }

// // Interrupt function to fill the ADC buffer
// void fillBuffer() {
//   for (int i = 0; i < SAMPLES; i++) {
//     adcBuffer[i] = analogRead(ADC_PIN);
//   }

//   bufferReady = true;
// }

// void setup() {
//   //Enable ADC input on pin 34
//   analogReadResolution(12); // Set the ADC resolution to 12 bits
//   analogSetPinAttenuation(ADC_PIN, ADC_11db); // Set the input attenuation to 11 dB
//   pinMode(ADC_PIN, INPUT); // Set the pin as an input

//   // Set up the serial port
//   Serial.begin(9600);
//   while (!Serial) {}

//   // Attach the interrupt to fill the ADC buffer
//   attachInterrupt(ADC_PIN, fillBuffer, RISING);
// }


// void loop() {

//   // If the ADC buffer is full and the FFT is not already in progress
//   if (bufferReady && !fftReady) {
//     // Perform the FFT on the ADC buffer
//     for (int i = 0; i < SAMPLES; i++) {
//       vReal[i] = adcBuffer[i];
//       vImag[i] = 0;
//     }

//     FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
//     FFT.Compute(FFT_FORWARD);
//     FFT.ComplexToMagnitude();

//     // Copy the magnitude data to the fftBuffer
//     for (int i = 0; i < SAMPLES / 2; i++) {
//       double real = vReal[i];
//       double imag = vImag[i];
//       fftBuffer[i] = sqrt(real * real + imag * imag);
// }

// // Set the fftReady flag to indicate that the FFT is complete
// fftReady = true;
// bufferReady = false; // Reset the bufferReady flag

// }

// // If the FFT is ready
// if (fftReady) {
// // Find and print the top 5 signals and their closest musical notes
// FFT.MajorPeak();
// for (int i = 1; i <= 5; i++) {
// int index = FFT.MajorPeak() / (SAMPLING_FREQUENCY / SAMPLES) * i;
// double frequency = index * (SAMPLING_FREQUENCY / SAMPLES);
// double magnitude = fftBuffer[index];

//   double percentError;
//   Note closestNote = findClosestNote(frequency, percentError);

//   Serial.print("Peak ");
//   Serial.print(i);
//   Serial.print(": ");
//   Serial.print(frequency);
//   Serial.print(" Hz, Magnitude: ");
//   Serial.print(magnitude);
//   Serial.print(", Closest Note: ");
//   Serial.print(closestNote.name);
//   Serial.print(" (");
//   Serial.print(closestNote.frequency);
//   Serial.print(" Hz), Percent Error: ");
//   Serial.print(percentError);
//   Serial.println("%");
// }

// fftReady = false; // Reset the fftReady flag
// }
// }