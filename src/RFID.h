#ifndef RFID_h
#define RFID_h

#include "require_cpp11.h"
#include "deprecated.h"
// Enable integer limits
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>
#include <MFRC522.h>
//fdsaf
constexpr uint8_t RST_PIN = 9;
constexpr uint8_t SS_PIN = 10;
#define MASTER_CARD_SET 143

class RFID {
public:
	RFID();  // default RST and SS pins are 9 and 10
  RFID(uint8_t _RST_PIN, uint8_t _SS_PIN);

	// Not in original AccessControl.ino:
	/* ------------------------------------------------ */
	void initialize();				// initialize SPI and RFID module hardware
	void printInitMessage();	// print to Serial that RFID is initialized
	void defineMasterCard();	// defines master ID through RFID scanning
	bool isMasterDefined();		// returns whether or not master ID has been defined
	// if wipeB is pressed for 10 seconds, all records are wiped
	void toggleDeleteAllRecords(uint8_t wipeB);
  void getCardID(byte *_cardID);		// sets _cardID to be the last read ID
	// if wipeB is pressed for 10 seconds, master ID is wiped
	void toggleDeleteMasterCard(uint8_t wipeB);
	void deleteID(byte cardID[4]);	// removes ID from list of passable IDs
	void writeID(byte cardID[4]);		// adds ID to list of passable IDs
  void maxRangeOn();		// turns on max range detection for RFID module
	/* ------------------------------------------------ */

	// In original AccessControl.ino
	/* ------------------------------------------------ */
	bool foundID();			// returns true if any ID is read and false otherwise
	void showReaderDetails();	// prints RFID module data to Serial
	bool findID(byte find[]);	// finds specific ID
	bool isMaster(byte test[]);		// returns whether ID is master key or not
	/* ------------------------------------------------ */

private:
	void readID(uint8_t number);
	bool monitorWipeButton(uint32_t interval, uint8_t wipeB);
  void writeIDTag(byte a[]);
	void deleteIDTag(byte a[]);
	bool checkTwo(byte a[], byte b[]);
	uint8_t findIDSLOT(byte find[]);
	byte storedCard[4];   // Stores an ID read from EEPROM
	byte cardID[4];   // Stores scanned ID read from RFID Module
	byte masterCard[4];   // Stores master card's ID read from EEPROM
  MFRC522 mfrc522;   // RFID reader/writer module
};

#endif
