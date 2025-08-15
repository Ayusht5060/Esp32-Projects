ESP-NOW Receiver 
#include <WiFi.h>
#include <esp_now.h>
// Define the structure of incoming data
typedef struct struct_message {
  char uid[20];
  char name[30];
  char room[20];
} struct_message;
struct_message incomingData;
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingDataBytes, int len) {
  Serial.println("📡 Data received via ESP-NOW");
  memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));
  Serial.print("UID: ");
  Serial.println(incomingData.uid);
  Serial.print("Name: ");
  Serial.println(incomingData.name);
  Serial.print("Room: ");
  Serial.println(incomingData.room);
  Serial.println("-----------------------");
}
void setup() {
  Serial.begin(115200);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ Error initializing ESP-NOW");
    return;
  }
  Serial.println("✅ ESP-NOW Initialized");
  esp_now_register_recv_cb(OnDataRecv);
}
void loop(){
}void loop() {
}