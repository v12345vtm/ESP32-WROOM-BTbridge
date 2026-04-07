#include <WiFi.h>
#include <esp_now.h>

// === SLAVE MAC ADDRESS ===
uint8_t slaveAddress[6] = {0x10, 0x06, 0x1C, 0x80, 0x94, 0x70}; //10:06:1C:80:94:70

bool peerAdded = false;

// NEW send callback
void onDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  if (status != ESP_NOW_SEND_SUCCESS) {
    Serial.println("[!] Send Failed → Restarting...");
    delay(100);
    ESP.restart();
  }
}

// NEW receive callback
void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  for (int i = 0; i < len; i++) {
    Serial.write(incomingData[i]);
  }
  Serial.flush();
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println("ESP-NOW MASTER STARTING...");

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed → Restarting...");
    ESP.restart();
  }

  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataRecv);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, slaveAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer → Restarting...");
    ESP.restart();
  }

  peerAdded = true;

  Serial.println("--------------------------------------------------");
  Serial.println("ESP-NOW BRIDGE ACTIVE (MASTER)");
  Serial.println("PC <~~ESP-NOW~~> FIELD DEVICE");
  Serial.println("--------------------------------------------------");
}

void loop() {
  if (!peerAdded) return;

  // USB → ESP-NOW
  if (Serial.available()) {
    uint8_t buffer[250];
    int len = Serial.readBytes(buffer, sizeof(buffer));

    esp_err_t result = esp_now_send(slaveAddress, buffer, len);

    if (result != ESP_OK) {
      Serial.println("[!] Send error → Restarting...");
      ESP.restart();
    }
  }
}