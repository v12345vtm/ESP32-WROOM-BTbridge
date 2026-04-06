 

//https://github.com/v12345vtm/ESP32-WROOM-BTbridge/blob/main/btontvangerESP/btontvangerESP.ino



#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Track connection state
bool lastState = false;

void setup() {
  Serial.begin(115200); 
  
  // Start Bluetooth as Slave
  // The name must be what the Master is searching for (if not using MAC)
  SerialBT.begin("ESP32_Bridge_Slave"); 
  
  Serial.println("--------------------------------------------------");
  Serial.println("SLAVE ACTIVE: Waiting for Master connection...");
  Serial.println("--------------------------------------------------");
}

void loop() {
  // --- CONNECTION WATCHDOG ---
  // hasClient() returns true if the Master is currently connected
  bool currentState = SerialBT.hasClient();

  // If we just lost the connection
  if (lastState && !currentState) {
    Serial.println("\n[!] Connection Lost. Rebooting Slave to reset Radio...");
    delay(100);
    ESP.restart();
  }
  lastState = currentState;

  // --- DATA BRIDGE ---
  // 1. Bluetooth (from Master) -> USB (to Field Device)
  if (SerialBT.available()) {
    while (SerialBT.available()) {
      Serial.write(SerialBT.read());
    }
    Serial.flush(); 
  }

  // 2. USB (from Field Device) -> Bluetooth (to Master)
  if (Serial.available()) {
    while (Serial.available()) {
      // NOTE: If field data is inverted (84 E4 EC...), 
      // you can use SerialBT.write(~Serial.read()); here instead.
      SerialBT.write(Serial.read());
    }
  }
}
