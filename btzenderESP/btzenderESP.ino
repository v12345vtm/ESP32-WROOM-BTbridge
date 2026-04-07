#include "BluetoothSerial.h"
 
//https://github.com/v12345vtm/ESP32-WROOM-BTbridge/blob/main/btzenderESP/btzenderESP.ino
 

BluetoothSerial SerialBT;

// HARDCODED SLAVE MAC ADDRESS
// Replace this if you change your Slave hardware
//uint8_t address[6] = {0xE8, 0x9F, 0x6D, 0xA8, 0x4E, 0xFE}; 
uint8_t address[6] = {0x10, 0x06, 0x1C, 0x80, 0x94, 0x72}; //esp met aangelijmde rs485


void setup() {
  Serial.begin(115200);


//printSLavetoConnectto();

 



    // Use Pins 16 (RX) and 17 (TX) for the actual data
  //Serial2.begin(115200, SERIAL_8N1, 16, 17); 
  
  // Start Bluetooth in Master Mode
  SerialBT.begin("ESP32_Bridge_Master", true); 

 

  Serial.println("Master Radio Initialized.Keep this ESP32 with your logging PC");
  delay(2000); // Warm-up delay for voltage stability

  bool connected = false;
  int attempt = 0;

  Serial.println("Start Searching for my Slave...");

  // SEARCH LOOP: Will not leave setup() until Slave is found
  while (!connected) {
    attempt++;
    Serial.print("Master Connection Attempt #");
    Serial.print(attempt);
   // Serial.print(" to slave... ");
    printSLavetoConnectto();

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




void printSLavetoConnectto() {
  
  Serial.println();
  Serial.print("to slave = {");
  for (int i = 0; i < 6; i++) {
    if (i > 0) Serial.print(", ");
    Serial.print("0x");
    if (address[i] < 16) Serial.print("0"); // leading zero
    Serial.print(address[i], HEX);
  }
  Serial.println("};");
   // Serial.println("Looking for slave with this BTadress");
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