#include <Wire.h>
#include <Adafruit_PN532.h>
// I2C setup for PN532
#define SDA_PIN 21
#define SCL_PIN 22
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);  // I2C
// Relay connected to GPIO 15
#define RELAY_PIN 15
void setup(void) {
  Serial.begin(115200);
  Serial.println("RFID Relay Trigger with ESP32 & PN532");
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Relay off initially
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1); // halt
  }
  nfc.SAMConfig();  // Configure board to read RFID tags
  Serial.println("Waiting for RFID tag...");
}
void loop(void) {
  uint8_t success;
  uint8_t uid[7];  // Buffer to store UID
  uint8_t uidLength;
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    Serial.print("Detected UID: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print("0x"); Serial.print(uid[i], HEX); Serial.print(" ");
    }
    Serial.println();
    // Relay ON for any card detected
    Serial.println("Any card detected. Relay ON.");
    digitalWrite(RELAY_PIN, HIGH); // Turn relay ON
    delay(3000);                   // Keep relay ON for 3 seconds
    digitalWrite(RELAY_PIN, LOW);  // Turn relay OFF
    delay(1000); // Prevent double reads
  }
}