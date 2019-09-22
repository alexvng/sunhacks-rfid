#include <SPI.h>
#include <MFRC522.h>

// pin constants required for board to send data
constexpr uint8_t SS_PIN = 10;
constexpr uint8_t RST_PIN = 9;

const int Bt4 = 3;
const int Bt3 = 4;
const int Bt2 = 5;
const int Bt1 = 6;
const int Bt0 = 7;

const char user0[16] = "ehfxrxnubluvrsvx";
const char user1[16] = "awhgzmmticgesttp";
const char user2[16] = "xfctpqcdpssssgqx";
const char user3[16] = "zrlaomqssoiensbc";
const char user4[16] = "fqerjbwupdegfwxf";

char dataBlock[16];

int whichUser;

MFRC522 mfrc522(SS_PIN, RST_PIN); // orange, white

MFRC522::MIFARE_Key key; // set by factory
MFRC522::StatusCode status; // checks for failed operations

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(Bt4, INPUT);
  pinMode(Bt3, INPUT);
  pinMode(Bt2, INPUT);
  pinMode(Bt1, INPUT);
  pinMode(Bt0, INPUT);

  whichUser = 0;

  // required to authenticate data gathering
  // default FF FF FF FF FF FF
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  for (int i = 0; i < 16; i++) {
    dataBlock[i] = user0[i];
  }

  Serial.println("Ready to use!");
}

void loop() {

  trackLastButtonPressed();

  if (whichUser == 0) {
    for (int i = 0; i < 16; i++) {
      dataBlock[i] = user0[i];
    }
  } else if (whichUser == 1) {
    for (int i = 0; i < 16; i++) {
      dataBlock[i] = user1[i];
    }
  } else if (whichUser == 2) {
    for (int i = 0; i < 16; i++) {
      dataBlock[i] = user2[i];
    }
  } else if (whichUser == 3) {
    for (int i = 0; i < 16; i++) {
      dataBlock[i] = user3[i];
    }
  } else if (whichUser == 4) {
    for (int i = 0; i < 16; i++) {
      dataBlock[i] = user4[i];
    }
  }

  // breaks loop if no card present
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // queues card-reading functionality
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  /* REMEMBER: NO CODE BELOW RUNS UNLESS NEW CARD IS PRESENT */

  byte blockAddress = 1;
  byte buffer[18]; // 18, even though a data block is 16 long?
  byte size = sizeof(buffer); // used as a counter variable

  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(
             MFRC522::PICC_CMD_MF_AUTH_KEY_A,
             blockAddress, // block being read (0-15)
             &key, // auth key A
             & (mfrc522.uid)); // pointer to rfid data

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Operation failed #1: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(
             blockAddress, // block being read (0-15)
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
             MFRC522::PICC_CMD_MF_AUTH_KEY_B,
             blockAddress, // block being read (0-15)
             &key, // auth key A
             & (mfrc522.uid)); // pointer to rfid data

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Operation failed #3: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(
             blockAddress,
             dataBlock,
             16);

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Operation failed #4: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  }

  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(
             blockAddress, // block being read (0-15)
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

void trackLastButtonPressed() {
  if (digitalRead(Bt0) == HIGH) {
    whichUser = 0;
  } else if (digitalRead(Bt1) == HIGH) {
    whichUser = 1;
  } else if (digitalRead(Bt2) == HIGH) {
    whichUser = 2;
  } else if (digitalRead(Bt3) == HIGH) {
    whichUser = 3;
  } else if (digitalRead(Bt4) == HIGH) {
    whichUser = 4;
  }
}
