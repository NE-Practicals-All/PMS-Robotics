#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  Serial.println("Place your card to reset...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  // Blocks to reset
  byte blocks[] = {2, 4}; // plate and balance
  byte emptyBlock[16] = {0};

  for (byte i = 0; i < sizeof(blocks); i++) {
    byte block = blocks[i];
    // Authenticate
    MFRC522::StatusCode status = mfrc522.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print("Auth failed for block ");
      Serial.print(block);
      Serial.print(": ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      continue;
    }

    // Write empty data
    status = mfrc522.MIFARE_Write(block, emptyBlock, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print("Write failed for block ");
      Serial.print(block);
      Serial.print(": ");
      Serial.println(mfrc522.GetStatusCodeName(status));
    } else {
      Serial.print("Block ");
      Serial.print(block);
      Serial.println(" reset successfully.");
    }
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(2000);
}
