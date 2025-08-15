ESP-NOW Sender 
#include <esp_now.h>
#include <WiFi.h>
typedef struct {
  int command;
} Message;
Message msg;
uint8_t targetMAC[] = {0x74, 0x4D, 0xBD, 0xB0, 0x60, 0xB4};
void initESPNow() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("⚠️ ESPNow Init Failed");
    return;
  }
  Serial.println("✅ ESPNow Init Success");
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, targetMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("⚠️ Failed to add peer");
  } else {
    Serial.println("✅ Peer added successfully");
  }
}
void triggerSlot(uint8_t *macAddr) {
  msg.command = 1;
  esp_err_t result = esp_now_send(macAddr, (uint8_t *)&msg, sizeof(msg));
  if (result == ESP_OK) {
    Serial.println("📤 Trigger sent");
  } else {
    Serial.println("⚠️ Failed to send trigger");
  }
}
void setup() {
  Serial.begin(115200);
  initESPNow();
  triggerSlot(targetMAC);
}
void loop() {
}