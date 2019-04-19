/*
 File  : ActionTimer2.h
 Version : 2.0
 Date  : 16/04/2019
 Project : BoxClock Equino Action Timer Aplication
 Author  : Daniel Valentin - dtvalentin@gmail.com

 ActionTimer2 Equino Config

 The circuit:

 Visual Status:
 STANDBY  LED		connect to analog out 0
 DIVISOR  LED		connect to analog out 1

 Display Config:
 LATCH 			connect to analog 	pin A2
 DATA   		connect to analog 	pin A1
 CLOCK  		connect to analog 	pin A0
 ENABLE 		connect to digital 	pin 3

 Mechanic Alarm:
 BELL			connect to digital out 4

 Sound Control:
 SOUND			connect to digital 	out 5
 ADJ_SOUND  	connect to analog 	out A3

 Infra-red
 IR 			connect to digital 	pin 6

 Real Time Clock:
 INT			connect to digital pin 2
 SDA  			connect to analog 	out A4
 SLA  			connect to analog 	out A5

 keypad:
 ROW 0			connect to digital in 7
 ROW 1			connect to digital in 13
 ROW 2			connect to digital in 12
 ROW 3 			connect to digital in 10

 COL 0 			connect to digital in 11
 COL 1			connect to digital in 9
 COL 2			connect to digital in 8
 */

#ifndef _ActionTimer2_H_
#define _ActionTimer2_H_

#include <Arduino.h>
#include <Equino.h>
#include <Keypad13k7i.h>
#include <Led.h>
#include <Relay.h>
#include <RTClib.h>
#include <TimeDevice.h>

#include "ATConfigStorage.h"
#include "ATUserInterface.h"
#include "AudioDevTDA7056B.h"
#include "InfraredTSOP4838.h"

// Funcao Timer Mode (1 - FightStopWatch, 2 - CrossClock)
#define TIMER_MODE 			1

#if(DEBUG_LEVEL >= 1)
#define SERIAL_BOUND		115200
#endif

#define DIVISOR_PIN			0
#define STANDBY_PIN			1

#define RTC_INT_PIN			2

#define BELL_PIN			4

#define SOUND_PIN			5
#define ADJ_SOUND_PIN		A3

#define IR_RECV_PIN 		6

#define KEYPAD_COL_0_PIN	11
#define KEYPAD_COL_1_PIN	9
#define KEYPAD_COL_2_PIN	8

#define KEYPAD_ROW_0_PIN	7
#define KEYPAD_ROW_1_PIN	13
#define KEYPAD_ROW_2_PIN	12
#define KEYPAD_ROW_3_PIN	10

#define PHASE_1				1
#define PHASE_2				2
#define PHASE_3				3
#define PHASE_4				5
#define PHASE_5				4

struct DisplayValue {
	int action = 1;
	int minute = 0;
	int second = 0;
};

enum TimerStep {
	STEP_ALERT, STEP_ACTION, STEP_PAUSE, STEP_INTERVAL
};

enum TimerState {
	STOPPED, RUNNING, PAUSED, CANCELED, DONE
};

// library interface description
class ActionTimer2 {
	// user-accessible "public" interface
public:
	// library-accessible "private" interface

	void setup(void);

	void run(void);

	void update(void);

	void stop(void);

	void start(void);

	void pause(void);

	void resume(void);

	void done(void);

	void config(void);

	void alarm(int phase);

	void changeProgram(int programaIndex, ATPage* page);

	void gotoPage(ATPage* page);

	boolean isRunning(void);

	boolean isPaused(void);

	boolean isDisplayActions(void);

	DisplayValue getValue(void);

	ATProgram dataProgram;

	ATConfigStorage configStorage;

	Keypad13k7i keypad;

	AudioDevTDA7056B audio;

	ATUserInterface userInterface;

	InfraredTSOP4838 irr;

	Led divisorLed;

	Led pilotLed;

	Relay relay;

	RTCDs1307 rtc;

	int program = 0;

protected:
	// library-accessible "private" interface
	void downTo(DateTime now);

	void upTo(DateTime now);

	void loadProgram(int programIndex, TimerStep phase);

	void updateDiplayValue(void);

	void calculateElapsedTime(DateTime now);

private:

	TimerState state = STOPPED;

	TimerStep step = STEP_ACTION;

	DisplayValue value;

	DateTime startTime;

	TimeSpan elapsedTime = TimeSpan(0, 0, 0, 0);

	TimeSpan endTime = TimeSpan(0, 0, 0, 0);

	volatile boolean updateDisplay = false;
};

extern ActionTimer2 timer;

#endif
