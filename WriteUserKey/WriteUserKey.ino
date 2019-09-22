#include <SPI.h>
#include <MFRC522.h>

// pin constants required for board to send data
constexpr uint8_t SS_PIN = 10;
constexpr uint8_t RST_PIN = 9;

MFRC522 mfrc522(SS_PIN, RST_PIN); // orange, white

MFRC522::MIFARE_Key key; // set by factory
MFRC522::StatusCode status; // checks for failed operations

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  // required to authenticate data gathering
  // default FF FF FF FF FF FF
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void loop() {

  // breaks loop if no card present
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // queues card-reading functionality
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  /* REMEMBER: NO CODE BELOW RUNS UNLESS NEW CARD IS PRESENT */

  byte buffer[18]; // 18, even though a data block is 16 long?
  byte size = sizeof(buffer); // used as a counter variable
  byte dataBlock[] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
  };

  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(
             MFRC522::PICC_CMD_MF_AUTH_KEY_A,
             1, // block being read (0-15)
             &key, // auth key A
             & (mfrc522.uid)); // pointer to rfid data

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Operation failed #1: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(
             1, // block being read (0-15)
             buffer, // variable where data is sent
             &size); // how far to go

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Operation failed #2: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  }

  Serial.print("Original data: ");
  dump_byte_array(buffer, 16);
  Serial.println();

  /* Write Procedure */

  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(
             MFRC522::PICC_CMD_MF_AUTH_KEY_A,
             1, // block being read (0-15)
             &key, // auth key A
             & (mfrc522.uid)); // pointer to rfid data

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Operation failed #3: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(
             1,
             dataBlock,
             16);

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Operation failed #4: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  }

  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(
             1, // block being read (0-15)
             buffer, // variable where data is sent
             &size); // how far to go

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Operation failed #5: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  }

  Serial.print("Final output:  ");
  dump_byte_array(buffer, 16);
  Serial.println();

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  Serial.println();

}

// reads out bytes spaced nicely
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

//TODO block address var