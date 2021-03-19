//  Reads an analog input on pin 11, prints the result to the Serial Monitor.
//  2021-03-19 by ELEMYO (https://github.com/ELEMYO/Elemyo-library)
//
//  Changelog:
//  2021-03-19 - initial release

/* ============================================
ELEMYO library code is placed under the MIT license
Copyright (c) 2018 ELEMYO

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================


Wiring the ELEMYO sensor to an ESP32

ELEMYO --> ESP32
  +        5V
  -        GND
  OUT1     pin 13
  СS       pin 10
  MOSI     pin 23 
  SCK      pin 18

*/

#include <ELEMYO.h>

#define   CSpin         10
#define   sensorInPin   13     // analog input pin that the sensor is attached to

int signalValue = 0;           // value read from the sensor
int bandStopValue = 0;         // result of filtration by BandStop filter
int bandPassValue = 0;         // result of filtration by BandPass filter
int lowPassValue = 0;          // result of filtration by LowPass filter
int highPassValue = 0;         // result of filtration by HighPass filter

ELEMYO MyoSensor(CSpin);

void setup() { 
  Serial.begin(115200);          // initialize serial communications at 115200 bps
  MyoSensor.gain(1);             // initial value of gain: x1; x2; x4; x5; x8; x10; x16; x32
  pinMode(sensorInPin, INPUT);   // initialisation of sensorInPin
}

void loop() {
  
  // read the analog input value:
  signalValue = analogRead(sensorInPin);
  
  // notch 50 Hz filter with band window 4 Hz.
  bandStopValue = MyoSensor.BandStop(signalValue, 50, 4);

  // Chebyshev 6th order 1dB ripple low pass filter for 80 Hz. 
  // Sampling (loop) frequency must be more then 2* cutoff frequency.
  // Cutoff frequency should be more than 30 Hz.
  lowPassValue = MyoSensor.LowPass(signalValue, 80, 2);

  // Chebyshev 4th order 1dB ripple bandpass filter for 30 - 80 Hz. 
  // Sampling (loop) frequency must be more then 2* upper cutoff frequency.
  // Lower cutoff frequency should be more than 30 Hz.
  // Difference between a lower and an upper cutoff frequencies should be more than 10 Hz
  bandPassValue = MyoSensor.BandPass(signalValue, 30, 80, 2);

  // Butterworth 1th order highpass filter for 10 Hz
  // Cutoff frequency should be more than 1 Hz.
  highPassValue = MyoSensor.HighPass(signalValue, 10);

  //print the results to the Serial Monitor:
  Serial.print(signalValue);
  Serial.print(" ");
  Serial.println(bandStopValue);
  //Serial.println(lowPassValue);
  //Serial.println(bandPassValue);
  //Serial.println(highPassValue);



  // wait before the next loop
  // delay more than 4ms can broke BandPass and LowPass filters with 80 Hz cutoff frequency.
  delay(1);
}
