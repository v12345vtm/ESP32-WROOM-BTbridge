#include <WiFi.h>
#include <esp_now.h>

uint8_t slaveAddress[6] = {0x10, 0x06, 0x1C, 0x80, 0x94, 0x70}; //10:06:1C:80:94:70<C

#define ESPNOW_CHANNEL 1
#define BUF_SIZE 240

// ================= CALLBACKS =================

void onDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  if (status != ESP_NOW_SEND_SUCCESS) {
    Serial.println("[MASTER] Send FAIL");
  }
}

void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  // ESP-NOW → PC
  Serial.write(data, len);
}

// ================= SETUP =================

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_CHANNEL);
  WiFi.disconnect();

  Serial.println("\n[MASTER] Starting...");
  Serial.println("masterMAC: " + WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    ESP.restart();
  }

  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataRecv);

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, slaveAddress, 6);
  peer.channel = ESPNOW_CHANNEL;
  peer.encrypt = false;
  peer.ifidx = WIFI_IF_STA;

  if (esp_now_add_peer(&peer) != ESP_OK) {
    Serial.println("Peer add failed");
    ESP.restart();
  }

  Serial.println("[MASTER] READY");
}

// ================= LOOP =================

void loop() {
  if (Serial.available()) {
    uint8_t buf[BUF_SIZE];
    int len = Serial.readBytes(buf, BUF_SIZE);

    esp_err_t result = esp_now_send(slaveAddress, buf, len);

    if (result != ESP_OK) {
      Serial.println("[MASTER] Send error");
    }
  }
}