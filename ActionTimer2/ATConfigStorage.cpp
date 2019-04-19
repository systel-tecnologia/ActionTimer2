/*
 File  : ATConfigStorage.cpp
 Version : 1.0
 Date  : 13/03/2019
 Project : Systel Storage EEPROM Devices Configuration Data Support Arduino Library
 Author  : Daniel Valentin - dtvalentin@gmail.com
 
 Storage EEPROM Devices Configuration Data
 
 */

#include "ATConfigStorage.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <Equino.h>
#include <stdint.h>

void ATConfigStorage::start(void) {
#if(DEBUG_LEVEL >= 2)
	DBG_PRINTLN_LEVEL(
			"Starting AT Devices Configuration EEPROM Data Storage...");
#endif	
	setup();
#if(DEBUG_LEVEL >= 2)
	DBG_PRINTLN_LEVEL("\tEEPROM Data Storage Started...");
#endif
}

ATProgram ATConfigStorage::load(int programaIndex) {
	ATProgram data;
	int eeAddress = DATA_STORE_ADDRESS + (sizeof(data) * programaIndex);
	EEPROM.get(eeAddress, data);
	delay(100);
#if(DEBUG_LEVEL >= 3)
	DBG_PRINT_LEVEL("\tEEPROM Data loadded... [");
	DBG_PRINTLN_LEVEL("]");
#endif
	return data;
}

void ATConfigStorage::save(ATProgram data, int programaIndex) {
	if (data.modified != 0) {
#if(DEBUG_LEVEL >= 4)
		DBG_PRINTLN_LEVEL("\tEEPROM Data saved...");
#endif
		int eeAddress = DATA_STORE_ADDRESS + (sizeof(data) * programaIndex);
		data.modified = 0;
		EEPROM.put(eeAddress, data);
		delay(100);
	}
}

void ATConfigStorage::setup(void) {
	int eeAddress = INIT_DATA_FLAG_ADDRESS;
	uint8_t flag = 0;
	EEPROM.get(eeAddress, flag);
	delay(100);
	if (flag != INIT_DATA_FLAG) {
#if(DEBUG_LEVEL >= 3)
		DBG_PRINTLN_LEVEL("\tStartig Configuration Database...");
#endif
		reset();
		flag = INIT_DATA_FLAG;
		EEPROM.put(eeAddress, flag);
		delay(100);
	}
}

void ATConfigStorage::reset(void) {
#if(DEBUG_LEVEL >= 4)
	DBG_PRINTLN_LEVEL("\tReset Configuration Database...");
#endif
	for (int i = 0; i < 9; ++i) {
		ATProgram program;
		program.modified = 1;
		save(program, i);
	}
}
