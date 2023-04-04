//  Controlling a servo position using EMG signal.
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
#include <ESP32Servo.h>

#define   CSpin         10
#define   sensorInPin   13	// analog input pin that the sensor is attached to

ELEMYO MyoSensor(CSpin);      // create ELEMYO object to work with signal
Servo myservo;                // create servo object to control a servo

void setup() {
  Serial.begin(115200);           // initialize serial communications at 115200 bps
  MyoSensor.gain(2);             // initial value of gain
  pinMode(sensorInPin, INPUT);    // initialize sensorInPin
  myservo.attach(5);              // attaches the servo on pin 5 to the servo object
}

void loop() {
  int signalValue = analogRead(sensorInPin);                // read the analog in value
  signalValue = MyoSensor.BandStop(signalValue, 50, 4);     // notch 50 Hz filter with band window 4 Hz.  
  signalValue = MyoSensor.BandStop(signalValue, 100, 6);    // notch 100 Hz (one of 50 Hz mode) filter with band window 6 Hz
  signalValue = MyoSensor.HighPass(signalValue, 10);        // 1th order Butterworth highpass filter, with 10 cutoff frequency in Hz (≥1 Hz)

  int signalValueMA = MyoSensor.movingAverage(signalValue,  0.98); // moving average transformation with 0.98 smoothing constant

  Serial.print(signalValue);                          // print the signal to the Serial Monitor:
  Serial.print(" ");
  Serial.println(signalValueMA);    // print the signal value after moving average transformation to the Serial Monitor:

  int val = map(signalValueMA, 0, 4095, 0, 180);      // scale signalValueMA to use it with the servo (value between 0 and 180)
  myservo.write(val);                                 // sets the servo position according to the scaled value
  
  delay(1);      // wait before the next loop
}
