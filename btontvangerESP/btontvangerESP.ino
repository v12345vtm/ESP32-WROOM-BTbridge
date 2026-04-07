 

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
  getBTmacForMaster();//needed to link the master the this esp32
  Serial.println("--------------------------------------------------");
  Serial.println("SLAVE ACTIVE: Waiting for Master connection...");
  Serial.println("--------------------------------------------------");
}





void getBTmacForMaster() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Bridge_Slave");

 

  String macStr = SerialBT.getBtAddressString();

  uint8_t mac[6];

  // Parse string to byte array
  sscanf(macStr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
         &mac[0], &mac[1], &mac[2],
         &mac[3], &mac[4], &mac[5]);

  // Print in your exact format
  Serial.print("uint8_t address[6] = {");
  for (int i = 0; i < 6; i++) {
    if (i > 0) Serial.print(", ");
    Serial.print("0x");
    if (mac[i] < 16) Serial.print("0"); // leading zero
    Serial.print(mac[i], HEX);
  }
  Serial.println("};");
   Serial.println("THIS LINE MUST BE KNOWN BY THE MASTER");
}



void loop() {
  // --- CONNECTION WATCHDOG ---
  // hasClient() returns true if the Master is currently connected
  bool currentState = SerialBT.hasClient();

  // If we just lost the connection
  if (lastState && !currentState) {
    Serial.println("\n[!] Connection Lost. Slave is rebooting ...and wait to the Master finds me");
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