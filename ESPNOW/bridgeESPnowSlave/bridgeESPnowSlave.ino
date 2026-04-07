#include <WiFi.h>
#include <esp_now.h>

bool dataReceivedRecently = true;
unsigned long lastReceiveTime = 0;

// Callback when data received from Master
void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  lastReceiveTime = millis();

  for (int i = 0; i < len; i++) {
    Serial.write(incomingData[i]);
  }
  Serial.flush();
}

void onDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  if (status != ESP_NOW_SEND_SUCCESS) {
    Serial.println("[!] Send Failed → Restarting...");
    ESP.restart();
  }
}

uint8_t masterAddress[6]; // learned automatically

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println("ESP-NOW SLAVE STARTING...");

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed → Restarting...");
    ESP.restart();
  }

  esp_now_register_recv_cb(onDataRecv);
  esp_now_register_send_cb(onDataSent);

  Serial.println("--------------------------------------------------");
  Serial.println("SLAVE ACTIVE: Waiting for Master...");
  Serial.println("--------------------------------------------------");

  // Print own MAC (like your BT version)
  Serial.print("My MAC: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // --- WATCHDOG (if no data for 10 sec → restart) ---
  if (millis() - lastReceiveTime > 10000) {
    Serial.println("[!] No Master activity → Restarting...");
    delay(100);
    ESP.restart();
  }
lastReceiveTime = millis();
  // --- UART → ESP-NOW ---
  if (Serial.available()) {
    uint8_t buffer[250];
    int len = Serial.readBytes(buffer, sizeof(buffer));

    // Send back to last sender (auto-learn)
    esp_now_send(NULL, buffer, len); // broadcast or last peer
  }
}