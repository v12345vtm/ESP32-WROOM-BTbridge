#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200); // For your PC Monitor
  
  // Use Pins 16 (RX) and 17 (TX) for the actual data
  Serial2.begin(115200, SERIAL_8N1, 16, 17); 
  
  SerialBT.begin("ESP32_Bridge_Slave"); 
  Serial.println("Slave Started. Waiting for Master...");
}

void loop() {
  // 1. Bluetooth to USB (PC)
  if (SerialBT.available()) {
    while (SerialBT.available()) {
      Serial.write(SerialBT.read());
    }
    Serial.flush(); // Ensure USB buffer is pushed
  }

  // 2. USB (PC) to Bluetooth
  if (Serial.available()) {
    while (Serial.available()) {
      SerialBT.write(Serial.read());
    }
    // No flush needed for BT, it sends automatically after the while loop
  }
}