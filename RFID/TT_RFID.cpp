#include <Arduino.h>
#include <TT_RFID.h>

TT_RFID::TT_RFID() : mfrc522(MFRC522(SS_PIN, RST_PIN)) { }

void TT_RFID::initialize() {
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
}

TT_RFID::TT_RFID(uint8_t _RST_PIN, uint8_t _SS_PIN)
  : mfrc522(MFRC522(_SS_PIN, _RST_PIN))
  {
    SPI.begin();           // MFRC522 Hardware uses SPI protocol
    mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  }

void TT_RFID::maxRangeOn() {
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
}

void TT_RFID::printInitMessage() {
  Serial.println(F("-------------------"));
  Serial.println(F("Master Card's ID"));
  for ( uint8_t i = 0; i < 4; i++ ) {          // Read Master Card's UID from EEPROM
    masterCard[i] = EEPROM.read(2 + i);    // Write it to masterCard
    Serial.print(masterCard[i], HEX);
  }
  Serial.println("");
  Serial.println(F("-------------------"));
  Serial.println(F("Everything is ready"));
  Serial.println(F("Waiting for IDs to be scanned"));
}

void TT_RFID::defineMasterCard() {
  Serial.println(F("No Master Card Defined"));
  Serial.println(F("Scan an ID to Define as Master Card"));
  bool successRead = false;
  do {
    successRead = foundID();            // sets successRead to 1 when we get read from reader otherwise 0
  }
  while (!successRead);                  // Program will not go further while you not get a successful read
  for ( uint8_t j = 0; j < 4; j++ ) {        // Loop 4 times
    EEPROM.write( 2 + j, cardID[j] );  // Write scanned PICC's UID to EEPROM, start from address 3
  }
  EEPROM.write(1, 143);                  // Write to EEPROM we defined Master Card.
  Serial.println(F("Master Card Defined"));
}

bool TT_RFID::isMasterDefined() {
  // Check if master card defined, if not let user choose a master card
  // This also useful to just redefine the Master Card
  // You can keep other EEPROM records just write other than 143 to EEPROM address 1
  // EEPROM address 1 should hold magical number which is '143'
  if (EEPROM.read(1) != MASTER_CARD_SET) return false;
  return true;
}

void TT_RFID::toggleDeleteAllRecords(uint8_t wipeB) {
  //Wipe Code - If the Button (wipeB) Pressed while setup run (powered on) it wipes EEPROM
  if (digitalRead(wipeB) == LOW) {  // when button pressed pin should get low, button connected to ground
    Serial.println(F("Wipe Button Pressed"));
    Serial.println(F("You have 10 seconds to Cancel"));
    Serial.println(F("This will be remove all records and cannot be undone"));
    bool buttonState = monitorWipeButton(10000, wipeB); // Give user enough time to cancel operation
    if (buttonState == true && digitalRead(wipeB) == LOW) {    // If button still be pressed, wipe EEPROM
      Serial.println(F("Starting Wiping EEPROM"));
      for (uint16_t x = 0; x < EEPROM.length(); x = x + 1) {    //Loop end of EEPROM address
        if (EEPROM.read(x) == 0) {              //If EEPROM address 0
          // do nothing, already clear, go to the next address in order to save time and reduce writes to EEPROM
        }
        else {
          EEPROM.write(x, 0);       // if not write 0 to clear, it takes 3.3mS
        }
      }
      Serial.println(F("Records Successfully Wiped"));
    }
    else {
      Serial.println(F("Wiping Cancelled")); // Show some feedback that the wipe button did not pressed for 15 seconds
    }
  }
}

void TT_RFID::toggleDeleteMasterCard(uint8_t wipeB) {
  // When device is in use if wipe button pressed for 10 seconds initialize Master Card wiping
  if (digitalRead(wipeB) == LOW) { // Check if button is pressed
    // Give some feedback
    Serial.println(F("Wipe Button Pressed"));
    Serial.println(F("Master Card will be Erased! in 10 seconds"));
    bool buttonState = monitorWipeButton(10000, wipeB); // Give user enough time to cancel operation
    if (buttonState == true && digitalRead(wipeB) == LOW) {    // If button still be pressed, wipe EEPROM
      EEPROM.write(1, 0);                  // Reset Magic Number.
      Serial.println(F("Master Card Erased from device"));
      Serial.println(F("Please reset to re-program Master Card"));
      while (1);
    }
    Serial.println(F("Master Card Erase Cancelled"));
  }
}

void TT_RFID::getCardID(byte *_cardID) {
  for(uint8_t i = 0; i < 4; i++) {
    _cardID[i] = cardID[i];
  }
}

void TT_RFID::deleteID(byte cardID[4]) {
  deleteIDTag(cardID);
  deleteIDTag(cardID);
}

void TT_RFID::writeID(byte cardID[4]) {
  writeIDTag(cardID);
  writeIDTag(cardID);
}

///////////////////////////////////////// Get PICC's UID ///////////////////////////////////
bool TT_RFID::foundID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return false;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  Serial.println(F("Scanned ID:"));
  for ( uint8_t i = 0; i < 4; i++) {  //
    cardID[i] = mfrc522.uid.uidByte[i];
    Serial.print(cardID[i], HEX);
  }
  Serial.println("");
  mfrc522.PICC_HaltA(); // Stop reading
  return true;
}

void TT_RFID::showReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown),probably a chinese clone?"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
    Serial.println(F("SYSTEM HALTED: Check connections."));
    while (true); // do not go further
  }
}

//////////////////////////////////////// Read an ID from EEPROM //////////////////////////////
void TT_RFID::readID( uint8_t number ) {
  uint8_t start = (number * 4 ) + 2;    // Figure out starting position
  for ( uint8_t i = 0; i < 4; i++ ) {     // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i);   // Assign values read from EEPROM to array
  }
}

///////////////////////////////////////// Add ID to EEPROM   ///////////////////////////////////
void TT_RFID::writeIDTag( byte a[] ) {
  if ( !findID( a ) ) {     // Before we write to the EEPROM, check to see if we have seen this card before!
    uint8_t num = EEPROM.read(0);     // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t start = ( num * 4 ) + 6;  // Figure out where the next slot starts
    num++;                // Increment the counter by one
    EEPROM.write( 0, num );     // Write the new count to the counter
    for ( uint8_t j = 0; j < 4; j++ ) {   // Loop 4 times
      EEPROM.write( start + j, a[j] );  // Write the array values to EEPROM in the right position
    }
  }
}

///////////////////////////////////////// Remove ID from EEPROM   ///////////////////////////////////
void TT_RFID::deleteIDTag( byte a[] ) {
  if ( !findID( a ) ) {     // Before we delete from the EEPROM, check to see if we have this card!
    // if not, don't do anything (to remove duplicate function call double delete error)
  }
  else {
    uint8_t num = EEPROM.read(0);   // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t slot;       // Figure out the slot number of the card
    uint8_t start;      // = ( num * 4 ) + 6; // Figure out where the next slot starts
    uint8_t looping;    // The number of times the loop repeats
    uint8_t j;
    uint8_t count = EEPROM.read(0); // Read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT( a );   // Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--;      // Decrement the counter by one
    EEPROM.write( 0, num );   // Write the new count to the counter
    for ( j = 0; j < looping; j++ ) {         // Loop the card shift times
      EEPROM.write( start + j, EEPROM.read(start + 4 + j));   // Shift the array values to 4 places earlier in the EEPROM
    }
    for ( uint8_t k = 0; k < 4; k++ ) {         // Shifting loop
      EEPROM.write( start + j + k, 0);
    }
  }
}

///////////////////////////////////////// Check Bytes   ///////////////////////////////////
bool TT_RFID::checkTwo ( byte a[], byte b[] ) {
  for ( uint8_t k = 0; k < 4; k++ ) {   // Loop 4 times
    if ( a[k] != b[k] ) {     // IF a != b then false, because: one fails, all fail
       return false;
    }
  }
  return true;
}

///////////////////////////////////////// Find Slot   ///////////////////////////////////
uint8_t TT_RFID::findIDSLOT( byte find[] ) {
  uint8_t count = EEPROM.read(0);       // Read the first Byte of EEPROM that
  for ( uint8_t i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    readID(i);                // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      // is the same as the find[] ID card passed
      return i;         // The slot number of the card
    }
  }
}

///////////////////////////////////////// Find ID From EEPROM   ///////////////////////////////////
bool TT_RFID::findID( byte find[] ) {
  uint8_t count = EEPROM.read(0);     // Read the first Byte of EEPROM that
  for ( uint8_t i = 1; i < count; i++ ) {    // Loop once for each EEPROM entry
    readID(i);          // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      return true;
    }
    else {    // If not, return false
    }
  }
  return false;
}

////////////////////// Check cardID IF is masterCard   ///////////////////////////////////
// Check to see if the ID passed is the master programing card
bool TT_RFID::isMaster( byte test[] ) {
	return checkTwo(test, masterCard);
}

bool TT_RFID::monitorWipeButton(uint32_t interval, uint8_t wipeB) {
  uint32_t now = (uint32_t)millis();
  while ((uint32_t)millis() - now < interval)  {
    // check on every half a second
    if (((uint32_t)millis() % 500) == 0) {
      if (digitalRead(wipeB) != LOW)
        return false;
    }
  }
  return true;
}
