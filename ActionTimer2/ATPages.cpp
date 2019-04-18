/*
 File  : ATPages.cpp
 Version : 1.0
 Date  : 16/04/2019
 Project : Systel Action Timer Pages Implementation Support Arduino Library
 Author  : Daniel Valentin - dtvalentin@gmail.com

 GUI Pages
 */

#include <DisplayDevice7Seg.h>
#include <Keys.h>
#include <Led.h>
#include <StateDevice.h>

#include "ActionTimer2.h"
#include "ATUserInterface.h"

// Formats
#if(TIMER_MODE == 1)
static const char* DSP_FORMAT1 = "%.2d%.1d%.2d";
static const char* DSP_FORMAT2 = "%c%.2d%.2d";
#endif

#if(TIMER_MODE == 2)
static const char* DSP_FORMAT1 = "%.2d%.2d%.2d";
#endif

static const char* DSP_TM_MODE_FMT = "%c%c%c%c%c";

ATMainPage mainPage;
ATRunPage runPage;
ATProgramPage programPage;
ATSetupPage setupPage;

// Generic Page
ATPage::~ATPage() {

}

void ATPage::show(void) {

}

void ATPage::input(char code) {
	// Power ON/OFF Timer
	if (code == CODE_C) {
		if (!timer.isRunning()) {
#if(TIMER_MODE == 1)
			timer.divisorLed.write(STATE_OFF);
#endif
#if(TIMER_MODE == 2)
			timer.divisorLed.write(STATE_ON);
#endif
			timer.pilotLed.write(STATE_ON);
			navigateTo(&mainPage);
		}
	}
}

void ATPage::navigateTo(ATPage *page) {
	timer.gotoPage(page);
}

void ATPage::refresh(void) {

}

// Main page
void ATMainPage::refresh(void) {
#if(TIMER_MODE == 2)
	DateTime now = timer.rtc.now();
	display.print((char*) DSP_FORMAT1, BUFFER_SIZE, now.hour(), now.minute(), now.second());
#endif
}

void ATMainPage::input(char code) {
	if (code == CODE_C) {
		timer.divisorLed.write(STATE_ON);
#if(TIMER_MODE == 1)
		timer.pilotLed.write(STATE_OFF);
#endif
#if(TIMER_MODE == 2)
		timer.pilotLed.write(STATE_ON);
#endif
		navigateTo(&runPage);
	}
}

// Run Page
void ATRunPage::refresh(void) {
	DisplayValue value = timer.getValue();
#if(TIMER_MODE == 1)
	if (value.minute > 9) {
		display.print((char*) DSP_FORMAT2, BUFFER_SIZE, 32, value.minute,
				value.second);
	} else {
		display.print((char*) DSP_FORMAT1, BUFFER_SIZE, value.action,
				value.minute, value.second);
	}
#endif
#if(TIMER_MODE == 2)
	display.print((char*) DSP_FORMAT1, BUFFER_SIZE, value.action, value.minute, value.second);
#endif
}

void ATRunPage::input(char code) {
	ATPage::input(code);

	// STOP Action
	if (code == CODE_0) {
		if (timer.isRunning()) {
			timer.stop();
		}
	}

	// Load New Program
	if (code >= CODE_1 && code <= CODE_9) {
		if (!timer.isRunning()) {
			timer.changeProgram(code - 49, &programPage);
		}
	}

	// Setup Program
	if (code == CODE_A) {
		if (!timer.isRunning()) {
			timer.config();
		}
	}

	// START / PAUSE Action
	if (code == CODE_B) {
		if (timer.isRunning()) {
			timer.pause();
		} else if (timer.isPaused()) {
			timer.resume();
		} else {
			timer.start();
		}
	}
}

// Display Program Page
void ATProgramPage::refresh(void) {
	if (time % 2 == 0) {
		display.print("PR%3d", BUFFER_SIZE, timer.program + 1);
	} else {
		display.clear();
	}
	time++;
	if (time >= 4) {
		time = 0;
		navigateTo(&runPage);
	}
}

// Setup Page
void ATSetupPage::refresh(void) {
	// Program Select
	if (phase == 0) {
		display.print((char*) (DSP_TM_MODE_FMT), BUFFER_SIZE, value0, 'R', 32,
				32, (char) (((timer.program + 1) + 48)));
	}

	// Mode Select
	if (phase == 1) {
		if (timer.programaDataBase.progs[timer.program].mode == UP) {
			display.print((char*) DSP_TM_MODE_FMT, BUFFER_SIZE, value0, 32, 32,
					'C', 'R');
		} else {
			display.print((char*) DSP_TM_MODE_FMT, BUFFER_SIZE, value0, 32, 32,
					'D', 'c');
		}
	}

	// Time Select
	if (phase == 2 || phase == 3 || phase == 5) {
		display.print("%c%2d%.2d", BUFFER_SIZE, value0, value1, value2);
	}

	// Cycles Select
	if (phase == 4) {
		display.print("C%4d", BUFFER_SIZE,
				timer.programaDataBase.progs[timer.program].actions);
	}

	// Sound Select
	if (phase == 6) {
		switch (timer.programaDataBase.progs[timer.program].sound) {
		case LIGHT:
			display.print((char*) DSP_TM_MODE_FMT, BUFFER_SIZE, value0, 32, '_',
					32, 32);
			break;
		case HEAVE:
			display.print((char*) DSP_TM_MODE_FMT, BUFFER_SIZE, value0, 32, '_',
					'=', 32);
			break;
		case HARD:
			display.print((char*) DSP_TM_MODE_FMT, BUFFER_SIZE, value0, 32, '_',
					'=', 'x');
			break;
		}
	}
}

void ATSetupPage::input(char code) {
	ATPage::input(code);
	if (code >= CODE_1 && code <= CODE_9) {
		if (phase == 0) {
			loadProgram(code);
		}
	}

	if (code == CODE_B) {
		switch (phase) {
		case 1:
			inputMode();
			break;
		case 6:
			inputSound();
			break;
		}
	}

	if (code == CODE_A) {
		phase++;
		switch (phase) {
		case 1:
			loadMode();
			break;
		case 2:
			loadAction();
			break;
		case 3:
			loadPause();
			break;
		case 4:
			loadCycles();
			break;
		case 5:
			loadInterval();
			break;
		case 6:
			loadSound();
			break;
		case 7:
			saveData();
			break;
		}
	}
}

void ATSetupPage::saveData(void) {
	phase = 0;
	value0 = 'P';
	timer.programaDataBase.modified = 1;
	timer.configStorage.save(timer.programaDataBase);
	timer.audio.info(LEVEL_100, 4);
	timer.changeProgram(timer.program, &runPage);
}

void ATSetupPage::loadAction() {
	value0 = 'A';
	value1 = (timer.programaDataBase.progs[timer.program].action / 60) % 60;
	value2 = (timer.programaDataBase.progs[timer.program].action % 60);
}

void ATSetupPage::loadPause() {
	value0 = 'P';
	value1 = (timer.programaDataBase.progs[timer.program].pause / 60) % 60;
	value2 = (timer.programaDataBase.progs[timer.program].pause % 60);
}

void ATSetupPage::loadInterval() {
	value0 = 'F';
	value1 = (timer.programaDataBase.progs[timer.program].interval / 60) % 60;
	value2 = (timer.programaDataBase.progs[timer.program].interval % 60);
}

void ATSetupPage::loadCycles() {
	value0 = 'C';
}

void ATSetupPage::loadMode() {
	value0 = 'T';
}

void ATSetupPage::loadSound() {
	value0 = 'B';
	timer.alarm(PHASE_2);
}

void ATSetupPage::loadProgram(char code) {
	value0 = 'P';
	timer.changeProgram(code - 49, this);
}

void ATSetupPage::inputMode() {
	if (timer.programaDataBase.progs[timer.program].mode == UP) {
		timer.programaDataBase.progs[timer.program].mode = DOWN;
	} else {
		timer.programaDataBase.progs[timer.program].mode = UP;
	}
}

void ATSetupPage::inputSound() {
	if (timer.programaDataBase.progs[timer.program].sound == LIGHT) {
		timer.programaDataBase.progs[timer.program].sound = HEAVE;
	} else if (timer.programaDataBase.progs[timer.program].sound == HEAVE) {
		timer.programaDataBase.progs[timer.program].sound = HARD;
	} else if (timer.programaDataBase.progs[timer.program].sound == HARD) {
		timer.programaDataBase.progs[timer.program].sound = LIGHT;
	}
	timer.alarm(PHASE_2);
}
