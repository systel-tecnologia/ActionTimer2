/*
 File  : ATUserInterface.cpp
 Version : 1.0
 Date  : 13/03/2019
 Project : Systel BPM Data Logger Support Arduino Library
 Author  : Daniel Valentin - dtvalentin@gmail.com
 
 */

#include "ATUserInterface.h"

#include <DisplayDevice.h>
#include <Equino.h>
#include "ActionTimer2.h"

DisplayDevice7Seg4094 display;
ATUserInterface userInterface;

void ATUserInterface::start(void) {
#if(DEBUG_LEVEL >= 2)
	DBG_PRINTLN_LEVEL("Starting Action Timer User Interface...");
#endif
	setup();
#if(DEBUG_LEVEL >= 2)
	DBG_PRINTLN_LEVEL("\tDisplay User Interface Started...");
#endif
}

void ATUserInterface::setup(void) {
#if(DEBUG_LEVEL >= 3)
	DBG_PRINTLN_LEVEL("\t\tDisplay 7 Segments Initialized...");
#endif
#if(TIMER_MODE == 1)
	display.start(ENABLE_PIN, CLOCK_PIN, DATA_PIN, LATCH_PIN, 5);
#endif
#if(TIMER_MODE == 2)
	display.start(ENABLE_PIN, CLOCK_PIN, DATA_PIN, LATCH_PIN, 6);
#endif
	display.setEnable(true);
}

void ATUserInterface::updateDisplay(void) {
	if (activePage) {
		activePage->refresh();
	}
#if(DEBUG_LEVEL >= 4)
	DBG_PRINTLN_LEVEL("\t\t\tUpdating Display...");
#endif	
}

void ATUserInterface::showPage(ATPage *page) {
	activePage = page;
	if (activePage) {
		display.clear();
		activePage->show();
	}
}

void ATUserInterface::read(char code) {
#if(DEBUG_LEVEL >= 4)
	DBG_PRINTLN_LEVEL("\t\t\tInput Data Received...");
#endif
	if (activePage) {
		if (code != EMPTY_DATA) {
			timer.audio.info(LEVEL_100, 1);
			activePage->input(code);
		}
	}

}

