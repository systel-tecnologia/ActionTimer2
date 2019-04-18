/*
 File: ActionTimer2.cpp
 Version: 2.0
 Date: 16/04/2019
 Project: Systel Action Timer 2 Application
 Author: Daniel Valentin - dtvalentin@gmail.com

 */

#include "ActionTimer2.h"

#include <AudioMessageDevice.h>
#include <Equino.h>
#include <StateDevice.h>

ActionTimer2 timer;

void setup() {
#if(DEBUG_LEVEL >= 1)
	Serial.begin(SERIAL_BOUND);
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}
	if (Serial) {
		Serial.println("Serial Communication Started...");
	}
#endif

	// Action Timer Start Point
	timer.setup();
}

void loop() {
	timer.run();
}

void rtcIsr() {
	timer.update();
}

void ActionTimer2::setup(void) {

#if(DEBUG_LEVEL >= 1)
	DBG_PRINTLN_LEVEL("Starting Action Timer Devices and System...");
#endif

	// Starting Output Audio Device
	audio.start(SOUND_OUT_PIN, SOUND_ADJ_PIN);

	// Starting RTC
	rtc.start();

	// Starting Storage Configuration
	configStorage.start();
	programaDataBase = configStorage.load();

	// Starting Display user interface
	userInterface.start();

	// Starting IR Receiver Data
	irr.start(IR_REC_DATA_PIN);

	// Starting State Devices
	divisorLed.start(DIVISOR_PIN);
	pilotLed.start(STANDBY_PIN);
	relay.start(BELL_PIN);

	// Starting Keypad Device
	keypad.start();

	// Start Interrupts
	attachInterrupt(digitalPinToInterrupt(RTC_INT_PIN), rtcIsr, CHANGE);
#if(DEBUG_LEVEL >= 2)
	DBG_PRINTLN_LEVEL("All Interrupts Started...");
#endif
	state = STOPPED;
	loadProgram(0);
	updateDiplayValue();
	userInterface.updateDisplay();

#if(DEBUG_LEVEL >= 1)
	DBG_PRINTLN_LEVEL("All Devices Started...");
	DBG_PRINTLN_LEVEL("System Running...");
#endif
	gotoPage(&mainPage);
	pilotLed.write(STATE_ON);
	divisorLed.write(STATE_OFF);
}

void ActionTimer2::gotoPage(ATPage *page) {
	userInterface.showPage(page);
}

void ActionTimer2::changeProgram(int programIndex, ATPage *page) {
	loadProgram(programIndex);
	value.action = 1;
	updateDiplayValue();
	gotoPage(page);
}

void ActionTimer2::run(void) {
	// Exec Count
	if (isRunning() && updateDisplay) {
		DateTime now = rtc.now();
		calculateElapsedTime(now);
		userInterface.updateDisplay();
		updateDisplay = false;
	} else if (updateDisplay) {
		// Update Display Data
		userInterface.updateDisplay();
		updateDisplay = false;
	}

	char code = irr.read();
	if (code == EMPTY_DATA) {
		code = keypad.read();
	}
	userInterface.read(code);
}

void ActionTimer2::calculateElapsedTime(DateTime now) {
	if (programaDataBase.progs[program].mode == UP) {
		upTo(now);
	} else {
		downTo(now);
	}
	updateDiplayValue();
}

void ActionTimer2::upTo(DateTime now) {
	if (elapsedTime.totalseconds() >= endTime.totalseconds()) {
		elapsedTime = endTime;
		done();
	} else {
		elapsedTime = (now - startTime);
	}
}

void ActionTimer2::downTo(DateTime now) {
	if (elapsedTime.totalseconds() <= 0) {
		elapsedTime = TimeSpan(0);
		done();
	} else {
		elapsedTime = (now - startTime);
		elapsedTime = (endTime - elapsedTime);
	}
}

void ActionTimer2::updateDiplayValue(void) {
	value.minute = elapsedTime.minutes();
	value.second = elapsedTime.seconds();
}

void ActionTimer2::start(void) {
	loadProgram(program);
	value.action = 1;
	updateDiplayValue();
	userInterface.updateDisplay();
	delay(500);
	alarm(PHASE_1);
	startTime = rtc.now();
	state = RUNNING;
}

void ActionTimer2::done(void) {
	if (value.action >= programaDataBase.progs[program].actions) {
		state = DONE;
		alarm(PHASE_3);
	} else {
		// Done
		state = DONE;
		alarm(PHASE_2);
		delay(500);

		// Restart
		loadProgram(program);
		value.action++;
		updateDiplayValue();
		userInterface.updateDisplay();
		alarm(PHASE_1);
		delay(500);
		startTime = rtc.now();
		state = RUNNING;
	}
}

void ActionTimer2::update(void) {
	if (!updateDisplay) {
		updateDisplay = true;
	}
}

void ActionTimer2::stop(void) {
	state = STOPPED;
	alarm(PHASE_5);
}

void ActionTimer2::resume(void) {
	if (programaDataBase.progs[program].mode == UP) {
		startTime = (rtc.now() - elapsedTime);
	}
	state = RUNNING;
}

void ActionTimer2::pause(void) {
	state = PAUSED;
}

void ActionTimer2::config(void) {
	state = STOPPED;
	loadProgram(program);
	value.action = 1;
	gotoPage(&setupPage);
}

boolean ActionTimer2::isRunning(void) {
	return (state == RUNNING);
}

boolean ActionTimer2::isPaused(void) {
	return (state == PAUSED);
}

DisplayValue ActionTimer2::getValue(void) {
	return value;
}

void ActionTimer2::loadProgram(int programIndex) {
	program = programIndex;
	elapsedTime = TimeSpan(0);
	if (programaDataBase.progs[program].mode == DOWN) {
		elapsedTime = TimeSpan(programaDataBase.progs[program].action);
	}
	endTime = TimeSpan(programaDataBase.progs[program].action);
}

void ActionTimer2::alarm(int phase) {
	if (programaDataBase.progs[program].sound == LIGHT) {
		audio.error(LEVEL_100, phase);
	} else if (programaDataBase.progs[program].sound == HEAVE) {
		audio.fatal(LEVEL_100, phase);
	} else if (programaDataBase.progs[program].sound == HARD) {
		for (int i = 0; i < phase; ++i) {
			relay.write(STATE_ON);
			delay(250);
			relay.write(STATE_OFF);
			delay(250);
		}
	}
}
