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

constexpr uint8_t RST_PIN = 9;
constexpr uint8_t SS_PIN = 10;
#define MASTER_CARD_SET 143

class RFID {
public:
	RFID();  // default RST and SS pins are 9 and 10
  RFID(uint8_t _RST_PIN, uint8_t _SS_PIN);

	// Not in original AccessControl.ino:
	/* ------------------------------------------------ */
	void initialize();
	void printInitMessage();
	void defineMasterCard();
	bool isMasterDefined();
	void toggleDeleteAllRecords(int wipeB);
  void getCardID(byte *_cardID);
	void toggleDeleteMasterCard(int wipeB);
	void deleteID(byte cardID[4]);
	void writeID(byte cardID[4]);
  void maxRangeOn();
	/* ------------------------------------------------ */

	// In original AccessControl.ino
	/* ------------------------------------------------ */
	bool foundID();
	void showReaderDetails();
	bool findID(byte find[]);
	bool isMaster(byte test[]);
	/* ------------------------------------------------ */

private:
	void readID(uint8_t number);
	bool monitorWipeButton(uint32_t interval, int wipeB);
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
