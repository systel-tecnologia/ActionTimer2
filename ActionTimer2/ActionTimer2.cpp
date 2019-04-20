/*
 File: ActionTimer2.cpp
 Version: 2.0
 Date: 16/04/2019
 Project: Systel Action Timer 2 Application
 Author: Daniel Valentin - dtvalentin@gmail.com

 */

#include "ActionTimer2.h"

#include <AudioMessageDevice.h>
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
	program = 0;
	dataProgram = configStorage.load(program);

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
	loadProgram(0, STEP_ACTION);
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
	loadProgram(programIndex, STEP_ACTION);
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

		// Final Alert
		if (step == STEP_ACTION && dataProgram.alertFinal > 0) {
			if (dataProgram.alertFinal == elapsedTime.totalseconds()) {
				audio.signal(LEVEL_100, 1000, 250);
			}
		}

		if (step == STEP_ALERT) {
			audio.signal(LEVEL_100, 1000, 200);
		}
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
	if (dataProgram.mode == UP || step == STEP_ALERT) {
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
	loadProgram(program, STEP_ALERT);

	value.action = 1;
	if (step == STEP_ALERT) {
		value.action = 0;
	}

	updateDiplayValue();
	userInterface.updateDisplay();
	if (step == STEP_ACTION) {
		alarm(PHASE_1);
	}
	delay(500);
	startTime = rtc.now();
	state = RUNNING;
}

void ActionTimer2::done(void) {
	if (value.action >= dataProgram.cycles
			&& (dataProgram.interval == 0 || dataProgram.cycles == 0)) {
		state = DONE;
		alarm(PHASE_3);
	} else {
		// Done
		state = DONE;
		if (step == STEP_ACTION) {
			alarm(PHASE_2);
		}
		delay(500);

		// Restart
		if (value.action >= dataProgram.cycles && (dataProgram.interval > 0)) {
			loadProgram(program, STEP_INTERVAL);
			value.action = 0;
		} else if (dataProgram.pause > 0 && step == STEP_ACTION) {
			loadProgram(program, STEP_PAUSE);
		} else {
			loadProgram(program, STEP_ACTION);
			value.action++;
		}
		updateDiplayValue();
		userInterface.updateDisplay();
		if (step == STEP_ACTION) {
			alarm(PHASE_1);
		}
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
	if (dataProgram.mode == UP) {
		startTime = (rtc.now() - elapsedTime);
	}
	state = RUNNING;
}

void ActionTimer2::pause(void) {
	state = PAUSED;
}

void ActionTimer2::config(void) {
	state = STOPPED;
	loadProgram(program, STEP_ACTION);
	value.action = 1;
	gotoPage(&setupPage);
}

boolean ActionTimer2::isRunning(void) {
	return (state == RUNNING);
}

boolean ActionTimer2::isPaused(void) {
	return (state == PAUSED);
}

boolean ActionTimer2::isDisplayActions(void) {
	return (step == STEP_ACTION) && (dataProgram.cycles > 0);
}

DisplayValue ActionTimer2::getValue(void) {
	return value;
}

void ActionTimer2::loadProgram(int programIndex, TimerStep timeStep) {
	step = timeStep;
	program = programIndex;
	dataProgram = configStorage.load(program);
	elapsedTime = TimeSpan(0);

	// has initial alert
	if (step == STEP_ALERT && dataProgram.alertInitial == 0) {
		step = STEP_ACTION;
	}

	switch (step) {
	case STEP_ALERT:
		endTime = TimeSpan(dataProgram.alertInitial);
		break;
	case STEP_ACTION:
		if (dataProgram.mode == DOWN) {
			elapsedTime = TimeSpan(dataProgram.action);
		}
		endTime = TimeSpan(dataProgram.action);
		break;
	case STEP_PAUSE:
		if (dataProgram.mode == DOWN) {
			elapsedTime = TimeSpan(dataProgram.pause);
		}
		endTime = TimeSpan(dataProgram.pause);
		break;
	case STEP_INTERVAL:
		if (dataProgram.mode == DOWN) {
			elapsedTime = TimeSpan(dataProgram.interval);
		}
		endTime = TimeSpan(dataProgram.interval);
		break;
	}
}

void ActionTimer2::alarm(int phase) {
	if (dataProgram.sound == LIGHT) {
		audio.error(LEVEL_100, phase);
	} else if (dataProgram.sound == HEAVE) {
		audio.fatal(LEVEL_100, phase);
	} else if (dataProgram.sound == HARD) {
		for (int i = 0; i < phase; ++i) {
			relay.write(STATE_ON);
			delay(250);
			relay.write(STATE_OFF);
			delay(250);
		}
	}
}
