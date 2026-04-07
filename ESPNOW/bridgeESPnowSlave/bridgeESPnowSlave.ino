#include <WiFi.h>
#include <esp_now.h>

uint8_t masterAddress[6] = {0x94, 0x51, 0xDC, 0x4B, 0x61, 0xF4};

#define ESPNOW_CHANNEL 1
#define BUF_SIZE 240

unsigned long lastPacketTime = 0;

// ================= CALLBACKS =================

void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  lastPacketTime = millis();

  // ESP-NOW → UART
  Serial.write(data, len);
}

void onDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  if (status != ESP_NOW_SEND_SUCCESS) {
    Serial.println("[SLAVE] Send FAIL");
  }
}

// ================= SETUP =================

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_CHANNEL);
  WiFi.disconnect();

  Serial.println("\n[SLAVE] Starting...");
  Serial.println("slaveMAC: " + WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    ESP.restart();
  }

  esp_now_register_recv_cb(onDataRecv);
  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, masterAddress, 6);
  peer.channel = ESPNOW_CHANNEL;
  peer.encrypt = false;
  peer.ifidx = WIFI_IF_STA;

  if (esp_now_add_peer(&peer) != ESP_OK) {
    Serial.println("Peer add failed");
    ESP.restart();
  }

  lastPacketTime = millis();

  Serial.println("[SLAVE] READY");
}

// ================= LOOP =================

void loop() {
  // UART → ESP-NOW
  if (Serial.available()) {
    uint8_t buf[BUF_SIZE];
    int len = Serial.readBytes(buf, BUF_SIZE);

    esp_err_t result = esp_now_send(masterAddress, buf, len);

    if (result != ESP_OK) {
      Serial.println("[SLAVE] Send error");
    }
  }

  // Optional watchdog (safe, no bootloop)
  if (millis() - lastPacketTime > 30000) {
    Serial.println("[SLAVE] No traffic...");
    lastPacketTime = millis();
  }
}