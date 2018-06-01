#include <RFID.h>
#include <EEPROM.h>     // We are going to read and write PICC's UIDs from/to EEPROM
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include <Arduino.h>

constexpr uint8_t wipeB = 3;     // Button pin for WipeMode
bool programMode = false;  // initialize programming mode to false
bool successRead;    // Variable integer to keep if we have Successful Read from Reader
byte cardID[4];   // Stores scanned ID read from RFID Module
RFID rfid;

void setup() {
  //Arduino Pin Configuration
  pinMode(wipeB, INPUT_PULLUP);   // Enable pin's pull up resistor
  rfid.initialize();
  //Protocol Configuration
  Serial.begin(9600);  // Initialize serial communications with PC
  //If you set Antenna Gain to Max it will increase reading distance
  //mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  Serial.println(F("Access Control Example v0.1"));
  rfid.showReaderDetails();  // Show details of Card Reader
  rfid.toggleDeleteAllRecords(wipeB);
  bool masterDefined = rfid.isMasterDefined();
  if(!masterDefined) {
    rfid.defineMasterCard();
  }
  rfid.printInitMessage();
}

void loop() {
  successRead = rfid.foundID();
  // rfid.getCardID(cardID);

  rfid.toggleDeleteMasterCard(wipeB);
  if(!successRead)
  {
    return;
  }
  // mode for adding and deleting RFID tags
  if (programMode)
  {
    // When in program mode check First If master card scanned again to exit program mode
    if ( rfid.isMaster(cardID) )
    {
      Serial.println(F("Master Card Scanned"));
      Serial.println(F("Exiting Program Mode"));
      Serial.println(F("-----------------------------"));
      programMode = false;
      return;
    }
    else
    {
      if ( rfid.findID(cardID) )
      { // If scanned card is known delete it
        Serial.println(F("I know this ID, removing..."));
        rfid.deleteID(cardID);
        Serial.println(F("Succesfully removed ID"));
        Serial.println("-----------------------------");
        Serial.println(F("Scan an RFID tag to ADD or REMOVE"));
      }
      else
      {                    // If scanned card is not known add it
        Serial.println(F("I do not know this ID, adding..."));
        rfid.writeID(cardID);
        Serial.println(F("Succesfully added ID"));
        Serial.println(F("-----------------------------"));
        Serial.println(F("Scan an RFID tag to ADD or REMOVE"));
      }
    }
  }

  // mode for checking IDs to see if they match existing records
  else {
    if ( rfid.isMaster(cardID)) {    // If scanned card's ID matches Master Card's ID - enter program mode
      programMode = true;
      Serial.println(F("Hello Master - Entered Program Mode"));
      Serial.println();
      Serial.println(F("Scan an RFID tag to ADD or REMOVE"));
      Serial.println(F("Scan Master Card again to Exit Program Mode"));
      Serial.println(F("-----------------------------"));
    }
    else {
      if ( rfid.findID(cardID) ) { // If not, see if the card is in the records
        Serial.println(F("Welcome, You shall pass"));
      }
      else {      // If not, show that the ID was not valid
        Serial.println(F("You shall not pass"));
      }
    }
  }
}
