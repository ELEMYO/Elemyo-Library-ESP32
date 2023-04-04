//  Sketch for interacting with sensors via ELEMYO_GUI
//  2021-03-19 by ELEMYO (https://github.com/ELEMYO/Elemyo-library)
//
//  Changelog:
//  2021-03-19 - initial releas
//  2023-04-04 - 6 sensors data channels were added

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
  S        analog pins 36, 39, 34, 35, 32, 33
  СS       pins        25, 26,  27,  14,  12,  11 (SS pins)
  MOSI     pin 23
  SCK      pin 18

*/

#include <ELEMYO.h>

const int sensorInPin[] = {36, 39, 34, 35, 32, 33};  // analog input pins that the sensors is attached to
const int CSpin[] = {25, 26, 27, 14, 12, 11};
int sensorsNumber = 1;                         // number of sensors attached to ESP32
int timePeriod = 100 + 310*sensorsNumber;      // signal sampling time period (in microseconds)
int sensorGain[] = {1, 2, 4, 5, 8, 10, 16, 32}; // sensor gain coefficients
ELEMYO MyoSensor[6];

void setup() {
  Serial.begin(115200);              // initialize serial communications at 115200 bps
  for (int i = 0; i < 6; i++)
  {
    pinMode(sensorInPin[i], INPUT);  // initialize input pins
    MyoSensor[i] = ELEMYO(CSpin[i]); // initialize sensors and CS pins
    MyoSensor[i].gain(1);           // initial sensor gain
  }
}

long Time = micros();
void loop() {
  byte buf[2];  // data array to send as a series of bytes
  
  // read and write to serial the last sensor signal value
  for (int i = 0; i < sensorsNumber ; i++)
  {  
    int data = analogRead(sensorInPin[i]); // read sensor signal value
    buf[0] = data & 255;
    buf[1] = (data >> 8)  & 255;
    Serial.write(buf, 2); // write to serial sensor signal value
  }

  int data = micros() - Time;
  Time = micros();
  buf[0] = data & 255;
  buf[1] = (data >> 8)  & 255;
  Serial.write(buf, 2);  // write to serial time period between sampling
  
  buf[0] = 0xFF;
  buf[1] = 0xFF; 
  Serial.write(buf, 2); // write to serial two separating bytes

  // checking for input command from ELEMYO GUI
  if (Serial.available() > 0) {
    int data = Serial.read(); // read data from serial buffer
    if (data < 7)
      sensorsNumber = data;   // set sensors number
    else
      MyoSensor[int(data/10) - 1].gain(sensorGain[int(data%10)]); // set new sensor gain
  }
}
