#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
const char* ssid = "Vivo";
const char* password = "9090909090";
const char* serverURL = "http://192.168.224.210:5000/log";
#define SDA_PIN 8
#define SCL_PIN 9
#define PN532_IRQ   (2)
#define PN532_RESET (3)
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
#define MOTOR_DIR   1
#define MOTOR_PWM   0
void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);
  analogWrite(MOTOR_PWM, 0);
  WiFi.begin(ssid, password);
  Serial.print("🔌 Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to WiFi");
  Serial.print("🌐 IP: "); Serial.println(WiFi.localIP());
  nfc.begin();
  uint32_t version = nfc.getFirmwareVersion();
  if (!version) {
    Serial.println("❌ PN532 not found.");
    while (1);
  }
  nfc.SAMConfig();
  Serial.println("🎯 Waiting for card...");
}
void loop() {
  uint8_t uid[7];
  uint8_t uidLen;
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen)) {
    String uidStr = "";
    for (uint8_t i = 0; i < uidLen; i++) {
      uidStr += String(uid[i], HEX);
    }
    uidStr.toUpperCase();
    Serial.print("🏷 UID: "); Serial.println(uidStr);
    Serial.println("➡ Forward...");
    digitalWrite(MOTOR_DIR, HIGH);
    analogWrite(MOTOR_PWM, 200);
    delay(1000);
    analogWrite(MOTOR_PWM, 0);
    delay(2000);
    Serial.println("⬅ Reverse...");
    digitalWrite(MOTOR_DIR, LOW);
    analogWrite(MOTOR_PWM, 200);
    delay(2000);
    analogWrite(MOTOR_PWM, 0);
    Serial.println("⛔ Stop");
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverURL);
      http.addHeader("Content-Type", "application/json");
      String user = "Unknown";
      if (uidStr == "A1B2C3D4") user = "John Doe";
      else if (uidStr == "11223344") user = "Jane Smith";
      String jsonPayload = "{\"uid\": \"" + uidStr + "\", \"name\": \"" + user + "\"}";
      int httpResponseCode = http.POST(jsonPayload);
      if (httpResponseCode > 0) {
        Serial.print("📤 Sent log. Status code: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("❌ Failed to send log. Error: ");
        Serial.println(http.errorToString(httpResponseCode).c_str());
      }
      http.end();
    }
    delay(3000);
    Serial.println("🎯 Waiting for card...");
  }
}void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
