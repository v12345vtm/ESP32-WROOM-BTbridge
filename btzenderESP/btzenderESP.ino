#include "BluetoothSerial.h"
//https://github.com/v12345vtm/ESP32-WROOM-BTbridge/blob/main/btzenderESP/btzenderESP.ino


BluetoothSerial SerialBT;

// HARDCODED SLAVE MAC ADDRESS
// Replace this if you change your Slave hardware
uint8_t address[6] = {0xE8, 0x9F, 0x6D, 0xA8, 0x4E, 0xFE}; 

void setup() {
  Serial.begin(115200);

    // Use Pins 16 (RX) and 17 (TX) for the actual data
  //Serial2.begin(115200, SERIAL_8N1, 16, 17); 
  
  // Start Bluetooth in Master Mode
  SerialBT.begin("ESP32_Bridge_Master", true); 
  Serial.println("Master Radio Initialized.Keep this ESP32 with your logging PC");
  delay(2000); // Warm-up delay for voltage stability

  bool connected = false;
  int attempt = 0;

  Serial.println("Starting Persistent Search for Slave...");

  // SEARCH LOOP: Will not leave setup() until Slave is found
  while (!connected) {
    attempt++;
    Serial.print("Connection Attempt #");
    Serial.print(attempt);
    Serial.print(" to [E8:9F:6D:A8:4E:FE]... ");

    connected = SerialBT.connect(address);

    if (connected) {
      Serial.println("SUCCESS!");
      Serial.println("--------------------------------------------------");
      Serial.println("BRIDGE ACTIVE: PC with MASTER <~~BT~~> SLAVE into the FIELD");
      Serial.println("--------------------------------------------------");
    } else {
      Serial.println("Failed. Retrying in 1 seconds...");
      delay(1000); // Wait 3s between scans to prevent radio hangs
    }
  }
}

void loop() {
  // 1. CONNECTION WATCHDOG
  // If the physical link drops, we reboot to clear the stack and start over
  if (!SerialBT.connected()) {
    Serial.println("\n[!] CRITICAL: BT Link Lost. Rebooting system...");
    delay(100); 
    ESP.restart(); 
  }

  // 2. DATA BRIDGE: Bluetooth -> USB (PC)
  if (SerialBT.available()) {
    while (SerialBT.available()) {
      // NOTE: Use Serial.write(~SerialBT.read()); if your field data is inverted
      Serial.write(SerialBT.read()); 
    }
    Serial.flush(); 
  }

  // 3. DATA BRIDGE: USB (PC) -> Bluetooth
  if (Serial.available()) {
    while (Serial.available()) {
      SerialBT.write(Serial.read());
    }
  }
}
