# MFRC522 Access Control Library
Written by *Patrick Neggie*, 2018 <<patmn@umich.edu>>

## Installation
Please [download][download-zip] the the ZIP file to this library and extract it into your `Arduino/libraries` folder. Restart your Arduino IDE and open the **AccessControl** example for inspiration.

## Features
* Add and remove RFID tags from a list of allowed tags
* Use a master RFID tag to control which tags are added/removed
* Delete all allowed tags with a single function
* Set and reset master RFID tag as much as desired
* Crank up the range on RFID module to detect RFID tags from a distance

## Functions
```c++
void initialize();				// initialize SPI and RFID module hardware
void printInitMessage();	// print to Serial that RFID is initialized
void defineMasterCard();	// defines master ID through RFID scanning
bool isMasterDefined();		// returns whether or not master ID has been defined
void toggleDeleteAllRecords(uint8_t wipeB);		// if wipeB is pressed for 10 seconds, all records are wiped
void getCardID(byte *_cardID);		// sets _cardID to be the last read ID
void toggleDeleteMasterCard(uint8_t wipeB);		// if wipeB is pressed for 10 seconds, master ID is wiped
void deleteID(byte cardID[4]);	// removes ID from list of passable IDs
void writeID(byte cardID[4]);		// adds ID to list of passable IDs
void maxRangeOn();		// turns on max range detection for RFID module
```

## Dependencies
##### **Note:** The non-native libraries below (MFRC522 and EEPROM) have been copied into the *AllLibraries* branch for your convenience
* **MFRC522.h**
    * From: [Github Repo][MFRC522-github]
    * License: Unlicense
* **EEPROM.h**
    * From: [Github Repo][EEPROM-lib]
    * Liscense: (target: Arduino) GNU Lesser General Public License 2.1
* **Arduino.h**

  * From: Arduino IDE / target specific
  * License: (target: Arduino) GNU Lesser General Public License 2.1

* **SPI.h**

  * From: Arduino IDE / target specific
  * License: (target: Arduino) GNU Lesser General Public License 2.1

* **stdint.h**

  * From: Arduino IDE / Compiler and target specific
  * License: different


[download-zip]: "https://github.com/TinkerYpsi/TT_RFID_Simple_Access_Control/archive/master.zip"
[MFRC522-github]: "https://github.com/miguelbalboa/rfid"
[EEPROM-lib]: "https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM"
