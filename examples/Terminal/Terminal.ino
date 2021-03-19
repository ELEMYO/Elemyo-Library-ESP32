//  Reads an analog input on pin A0, prints the result to the Serial Monitor.
//  The sensor gain is controlled by the program.
//  Graphical representation is available using Elemyo_GUI, Python_GUI and Matlab_GUI.
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
#define   sensorInPin   13      // analog input pin that the sensor is attached to
#define   timePeriod    0.7     // frequency of signal update (time in ms)

int signalValue = 0;            // value read from the sensor
long loopTime=0, sendTime=0;    // values for calculating and sending a time period of one loop

ELEMYO MyoSensor(CSpin);

void setup() {
  Serial.begin(115200);		// initialize serial communications at 115200 bps
  MyoSensor.gain(1);		// Initial value of gain
  pinMode(sensorInPin, INPUT);	// initialize sensorInPin
}

void loop() {
  
  signalValue = analogRead(sensorInPin);  // read the analog in value

  Serial.println(signalValue);            // print the results to the Serial Monitor
  
  if (Serial.available() > 0)             // check for byte in buffer
    MyoSensor.gain(Serial.read());        // Read gain value from serial buffer

  if ((micros() - sendTime > 10000) && (micros() < 100000))
  {
    sendTime = micros();
    Serial.print("T");
    Serial.println(sendTime - loopTime);
  }
  loopTime = micros();
  
  delayMicroseconds(timePeriod*1000);     // wait before the next loop
}
