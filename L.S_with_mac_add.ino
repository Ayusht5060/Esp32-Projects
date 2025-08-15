#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_PN532.h>

// WiFi credentials
const char* ssid = "Vivo";
const char* password = "9090909090";

// Flask server endpoint
const char* serverURL = "http://192.168.224.210:5000/log";

// PN532 setup
#define PN532_IRQ   (2)
#define PN532_RESET (3)
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

// Motor control pins
#define MOTOR_DIR   1
#define MOTOR_PWM   0

String roomName = "Room A"; // Customize this for each door
String deviceMAC;

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

  // Get and format MAC address
  deviceMAC = WiFi.macAddress();
  Serial.print("📡 MAC: "); Serial.println(deviceMAC);

  // NFC setup
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

    // Motor trigger
    Serial.println("➡ Opening...");
    digitalWrite(MOTOR_DIR, HIGH);
    analogWrite(MOTOR_PWM, 200);
    delay(1000);
    analogWrite(MOTOR_PWM, 0);
    delay(2000);
    Serial.println("⬅ Closing...");
    digitalWrite(MOTOR_DIR, LOW);
    analogWrite(MOTOR_PWM, 200);
    delay(1000);
    analogWrite(MOTOR_PWM, 0);

    // Identify user by UID
    String user = "Unknown";
    if (uidStr == "A1B2C3D4") user = "John Doe";
    else if (uidStr == "11223344") user = "Jane Smith";

    // Send data to Flask
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverURL);
      http.addHeader("Content-Type", "application/json");

      String payload = "{\"uid\": \"" + uidStr + "\", \"name\": \"" + user +
                       "\", \"mac\": \"" + deviceMAC + "\", \"room\": \"" + roomName + "\"}";

      int httpResponseCode = http.POST(payload);

      if (httpResponseCode > 0) {
        Serial.print("📤 Sent log. Code: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("❌ Error: ");
        Serial.println(http.errorToString(httpResponseCode));
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
