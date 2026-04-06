#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  // Name your Slave device
  SerialBT.begin("ESP32_Bridge_Slave");
  
  // Wait a moment for the radio to initialize
  delay(1000);

  // Get the raw MAC address
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_BT);

  Serial.println("\n--- COPY THE LINE BELOW INTO YOUR MASTER SKETCH ---");
  
  Serial.print("uint8_t address[6] = {");
  for (int i = 0; i < 6; i++) {
    Serial.print("0x");
    if (mac[i] < 0x10) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(", ");
  }
  Serial.println("};");
  
  Serial.println("--- END OF COPY ---");
  Serial.println("\nSlave is now running and discoverable.");
}

void loop() {
  // Simple USB to Bluetooth bridge
  if (SerialBT.available()) { Serial.write(SerialBT.read()); }
  if (Serial.available()) { SerialBT.write(Serial.read()); }
}
