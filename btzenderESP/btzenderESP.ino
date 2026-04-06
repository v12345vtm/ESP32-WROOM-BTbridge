#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// REPLACE THESE HEX VALUES WITH YOUR SLAVE'S MAC ADDRESS
uint8_t address[6]  = {0xE8,0x9F,0x6D,0xA8,0x4E,0xFE};  //E8:9F:6D:A8:4E:FE

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17); // Your data pins
  
  SerialBT.begin("ESP32_Bridge_Master", true); 
  Serial.println("Attempting direct connection via MAC...");

  // Connect using the MAC address array
  bool connected = SerialBT.connect(address);

  if (connected) {
    Serial.println("CONNECTED SUCCESSFULLY!");
  } else {
    Serial.println("Connection failed. Check power or distance.");
  }
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