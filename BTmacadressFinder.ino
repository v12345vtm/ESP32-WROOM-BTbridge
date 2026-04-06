void macStringToArray(const char* macStr, uint8_t* macArray) {
  sscanf(macStr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
         &macArray[0], &macArray[1], &macArray[2],
         &macArray[3], &macArray[4], &macArray[5]);
}

void setup() {
  Serial.begin(115200);

  const char* macStr = "E8:9F:6D:A8:4E:FE";
  uint8_t address[6];

  macStringToArray(macStr, address);

  // Print nicely
  Serial.print("uint8_t address[6] = {");
  for (int i = 0; i < 6; i++) {
    if (i > 0) Serial.print(", ");
    Serial.print("0x");
    if (address[i] < 16) Serial.print("0"); // leading zero
    Serial.print(address[i], HEX);
  }
  Serial.println("};");
}

void loop() {}
