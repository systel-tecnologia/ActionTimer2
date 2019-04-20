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
static const char* DSP_FORMAT2 = "%c%2d%.2d";
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

void ATPage::blink(void) {
	// Update Status Led
	if (timer.isRunning()) {
		timer.divisorLed.write(STATE_ON);
	} else {
		if ((timer.pilotLed.read() == STATE_OFF)) {
			if (timer.divisorLed.read() == STATE_OFF) {
				timer.divisorLed.write(STATE_ON);
			} else {
				timer.divisorLed.write(STATE_OFF);
			}
		}
	}
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
		if (!timer.isDisplayActions()) {
			display.print((char*) DSP_FORMAT2, BUFFER_SIZE, 32, value.minute,
					value.second);
		} else {
			display.print((char*) DSP_FORMAT1, BUFFER_SIZE, value.action,
					value.minute, value.second);
		}
	}
#endif
#if(TIMER_MODE == 2)
	display.print((char*) DSP_FORMAT1, BUFFER_SIZE, value.action, value.minute, value.second);
#endif
	blink();
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
	if (phase == SELECT_PROGRAM) {
		display.print((char*) (DSP_TM_MODE_FMT), BUFFER_SIZE, value0, 'R', 32,
				32, (char) (((timer.program + 1) + 48)));
	}

	// Mode Select
	if (phase == SELECT_MODE) {
		if (timer.dataProgram.mode == UP) {
			display.print((char*) DSP_TM_MODE_FMT, BUFFER_SIZE, value0, 32, 32,
					'C', 'R');
		} else {
			display.print((char*) DSP_TM_MODE_FMT, BUFFER_SIZE, value0, 32, 32,
					'D', 'c');
		}
	}

	// Set Times
	if (phase == SET_ACTION_TIME || phase == SET_PAUSE_TIME
			|| phase == SET_INTERVAL_TIME) {
		display.print((char*) DSP_FORMAT2, BUFFER_SIZE, value0, value1, value2);
	}

	if (phase == SET_AL_INITIAL_TIME || phase == SET_AL_FINAL_TIME) {
		display.print("A%c%1d%.2d", BUFFER_SIZE, value0, value1, value2);
	}

	// Cycles Select
	if (phase == SELECT_CYCLES) {
		display.print("C%4d", BUFFER_SIZE, value2);
	}

	// Sound Select
	if (phase == SELECT_SOUND) {
		switch (timer.dataProgram.sound) {
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

	if (!((phase == SELECT_PROGRAM) || (phase == SELECT_MODE)
			|| (phase == SELECT_CYCLES) || (phase == SELECT_SOUND))) {
		blink();
	} else {
		timer.divisorLed.write(STATE_OFF);
	}

}

void ATSetupPage::input(char code) {
	ATPage::input(code);
	if (code >= CODE_0 && code <= CODE_9) {
		if ((code >= CODE_1 && code <= CODE_9) && (phase == SELECT_PROGRAM)) {
			loadProgram(code);
		} else
		// Action, Pause, Interval Inputs
		if ((phase == SET_ACTION_TIME) || (phase == SET_PAUSE_TIME)
				|| (phase == SET_INTERVAL_TIME)) {
			inputTime(shift, ((uint8_t) code - 48));
			shift += 1;
			if (shift == 4) {
				shift = 0;
			}
		} else
		// Alert Inputs
		if ((phase == SET_AL_INITIAL_TIME) || (phase == SET_AL_FINAL_TIME)) {
			inputTime(shift, ((uint8_t) code - 48));
			shift += 1;
			if (shift == 3) {
				shift = 0;
			}
		} else
		// Cycles Input
		if ((phase == SELECT_CYCLES)) {
			inputTime(shift, ((uint8_t) code - 48));
			shift += 1;
			if (shift == 2) {
				shift = 0;
			}
		}
	}

	if (code == CODE_B) {
		switch (phase) {
		case SELECT_MODE:
			inputMode();
			break;
		case SELECT_SOUND:
			inputSound();
			break;
		}
	}

	if (code == CODE_A) {
		inputValue();
		phase++;
		switch (phase) {
		case SELECT_MODE:
			loadMode();
			break;
		case SET_ACTION_TIME:
			loadAction();
			break;
		case SET_PAUSE_TIME:
			loadPause();
			break;
		case SELECT_CYCLES:
			loadCycles();
			break;
		case SET_INTERVAL_TIME:
			loadInterval();
			break;
		case SET_AL_INITIAL_TIME:
			loadALInitial();
			break;
		case SET_AL_FINAL_TIME:
			loadALFinal();
			break;
		case SELECT_SOUND:
			loadSound();
			break;
		case SAVE_DATA:
			saveData();
			break;
		}
	}
}

void ATSetupPage::saveData(void) {
	phase = SELECT_PROGRAM;
	value0 = 'P';
	if (timer.dataProgram.modified) {
		timer.audio.info(LEVEL_100, 4);
	}
	timer.configStorage.save(timer.dataProgram, timer.program);
	timer.changeProgram(timer.program, &runPage);
}

void ATSetupPage::loadAction(void) {
	value0 = 'A';
	value1 = (timer.dataProgram.action / 60) % 60;
	value2 = (timer.dataProgram.action % 60);
}

void ATSetupPage::loadPause(void) {
	value0 = 'P';
	value1 = (timer.dataProgram.pause / 60) % 60;
	value2 = (timer.dataProgram.pause % 60);
}

void ATSetupPage::loadInterval(void) {
	value0 = 'F';
	value1 = (timer.dataProgram.interval / 60) % 60;
	value2 = (timer.dataProgram.interval % 60);
}

void ATSetupPage::loadALInitial(void) {
	value0 = 'I';
	value1 = (timer.dataProgram.alertInitial / 60) % 60;
	value2 = (timer.dataProgram.alertInitial % 60);
}

void ATSetupPage::loadALFinal(void) {
	value0 = 'F';
	value1 = (timer.dataProgram.alertFinal / 60) % 60;
	value2 = (timer.dataProgram.alertFinal % 60);
}

void ATSetupPage::loadCycles(void) {
	value0 = 'C';
	value2 = timer.dataProgram.cycles;
}

void ATSetupPage::loadMode(void) {
	value0 = 'T';
}

void ATSetupPage::loadSound(void) {
	value0 = 'B';
}

void ATSetupPage::loadProgram(char code) {
	value0 = 'P';
	timer.changeProgram(code - 49, this);
}

void ATSetupPage::inputMode(void) {
	if (timer.dataProgram.mode == UP) {
		timer.dataProgram.mode = DOWN;
	} else {
		timer.dataProgram.mode = UP;
	}
	timer.dataProgram.modified = 1;
}

void ATSetupPage::inputSound(void) {
	if (timer.dataProgram.sound == LIGHT) {
		timer.dataProgram.sound = HEAVE;
	} else if (timer.dataProgram.sound == HEAVE) {
		timer.dataProgram.sound = HARD;
	} else if (timer.dataProgram.sound == HARD) {
		timer.dataProgram.sound = LIGHT;
	}
	timer.dataProgram.modified = 1;
	timer.alarm(PHASE_2);
}

void ATSetupPage::inputValue(void) {
	if ((value3 > 0) || (value3 = 0 && shift > 0)) {
		timer.dataProgram.modified = 1;
		switch (phase) {
		case SET_ACTION_TIME:
			timer.dataProgram.action = value3;
			break;
		case SET_PAUSE_TIME:
			timer.dataProgram.pause = value3;
			break;
		case SELECT_CYCLES:
			timer.dataProgram.cycles = value3;
			break;
		case SET_INTERVAL_TIME:
			timer.dataProgram.interval = value3;
			break;
		case SET_AL_INITIAL_TIME:
			timer.dataProgram.alertInitial = value3;
			break;
		case SET_AL_FINAL_TIME:
			timer.dataProgram.alertFinal = value3;
			break;
		}
	}
	value3 = 0;
	shift = 0;
}

void ATSetupPage::inputTime(uint8_t shift, uint8_t value) {
	switch (shift) {
	case 0:
		value1 = 0;
		value2 = value;
		break;

	case 1:
		value1 = 0;
		value2 = (value2 * 10) + value;
		break;

	case 2:
		value1 = (value2 / 10);
		value2 = ((value2 - (value1 * 10)) * 10) + value;
		break;

	case 3:
		value1 = ((value1 * 1000) + (value2 * 10)) / 100;
		if (((value2 - ((value2 / 10) * 10)) * 10) > 59) {
			value2 = 50 + value;
		} else {
			value2 = ((value2 - ((value2 / 10) * 10)) * 10) + value;
		}
		break;
	}

	value3 = ((value1 * 60) + value2);
}
