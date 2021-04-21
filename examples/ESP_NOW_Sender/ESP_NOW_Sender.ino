//  Reads an analog input on GPIO 34, sends the results to master board using ESP-NOW protocol.
//  2021-04-21 by ELEMYO (https://github.com/ELEMYO/Elemyo-library)
//
//  Changelog:
//  2021-04-21 - initial release
//  

/* ============================================
Substantial portions of the Software prowided by:
Rui Santos
Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/

ELEMYO library code is placed under the MIT license
Copyright (c) 2021 ELEMYO

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
  OUT1     pin 34
  СS       pin 10
  MOSI     pin 23 
  SCK      pin 18

*/

#include <ELEMYO.h>
#include <esp_now.h>
#include <WiFi.h>

#define   CSpin         10
#define   sensorInPin   34      // analog input pin that the sensor is attached to

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x7C, 0x9E, 0xBD, 0xF0, 0x30, 0xF8};

short signalValue[125];

ELEMYO MyoSensor(CSpin);

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  MyoSensor.gain(1);    // Initial value of gain
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_MINUS_1dBm);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  pinMode(sensorInPin, INPUT);  // initialize sensorInPin
}
 
void loop() {
  // Set values to send
  for (int i =0; i < 125; i++)
  {
    signalValue[i] = analogRead(sensorInPin);
    delayMicroseconds(1000);
  }
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &signalValue, sizeof(signalValue));
}
