/*
 File  : AudioDevTDA7056B.cpp
 Version : 1.0
 Date  : 05/03/2019
 Project : Systel Equino TDA7056B Audio Driver Device Support Arduino Library
 Author  : Daniel Valentin - dtvalentin@gmail.com

 */
#include <Arduino.h>
#include "AudioDevTDA7056B.h"

AudioDevTDA7056B::AudioDevTDA7056B() {

}

void AudioDevTDA7056B::start(uint8_t sound_PIN, uint8_t adj_PIN) {
	soundPin = sound_PIN;
	adjPin = adj_PIN;
	setup();
#if(DEBUG_LEVEL >= 2)
	DBG_PRINT_LEVEL("TDA7056A/B Audio Device Driver Started (PINS: ");
	DBG_PRINT_LEVEL(soundPin);
	DBG_PRINT_LEVEL(", ");
	DBG_PRINT_LEVEL(adjPin);
	DBG_PRINTLN_LEVEL(")...");
#endif
}

void AudioDevTDA7056B::info(Level level, uint8_t repetitions) {
#if(DEBUG_LEVEL >= 3)
	DBG_PRINT_LEVEL("\tINFO Sound Message ");
	DBG_PRINT_LEVEL(repetitions);
	DBG_PRINTLN_LEVEL(" Repetitions ");
#endif
	volume(level);
	for (int r = 0; r < repetitions; r++) {
		tone(soundPin, BEEP_TONE_FREQ, BEEP_TIME);
		delay(BEEP_TIME * 10);
	}
	volume(LEVEL_0);
}

void AudioDevTDA7056B::warn(Level level, uint8_t repetitions) {
#if(DEBUG_LEVEL >= 3)
	DBG_PRINT_LEVEL("\tWARN Sound Message ");
	DBG_PRINT_LEVEL(repetitions);
	DBG_PRINTLN_LEVEL(" Repetitions ");
#endif
	volume(level);
	for (int r = 0; r < repetitions; r++) {
		for (int i = (FREQ_INIT_WARN); i < (FREQ_FINAL_WARN); i++) {
			tone(soundPin, i, TONE_TIME_WARN);
		}
		delay(TONE_TIME_WARN * 5);
	}
	volume(LEVEL_0);
}

void AudioDevTDA7056B::error(Level level, uint8_t repetitions) {
#if(DEBUG_LEVEL >= 3)
	DBG_PRINT_LEVEL("\tERROR Sound Message ");
	DBG_PRINT_LEVEL(repetitions);
	DBG_PRINTLN_LEVEL(" Repetitions ");
#endif
	volume(level);
	for (int r = 0; r < repetitions; r++) {
		for (int j = 0; j < TONE_TIME_ERROR; j++) {
			for (int i = FREQ_INIT_ERROR; i < FREQ_FINAL_ERROR; i++) {
				// Som Continuo
				tone(soundPin, FREQ_FINAL_ERROR, TONE_TIME_ERROR);
			}
		}
		compensation(repetitions);
	}
	volume(LEVEL_0);
}

void AudioDevTDA7056B::fatal(Level level, uint8_t repetitions) {
#if(DEBUG_LEVEL >= 3)
	DBG_PRINT_LEVEL("\tFATAL Sound Message ");
	DBG_PRINT_LEVEL(repetitions);
	DBG_PRINTLN_LEVEL(" Repetitions ");
#endif
	volume(level);
	for (int r = 0; r < repetitions; r++) {
		for (int j = 0; j < TONE_TIME_FATAL; j++) {
			for (int i = FREQ_INIT_FATAL; i < FREQ_FINAL_FATAL; i++) {
				// Som oscilante
				tone(soundPin, i, TONE_TIME_FATAL);
			}
		}
		compensation(repetitions);
	}
	volume(LEVEL_0);
}

void AudioDevTDA7056B::signal(Level level, int frequency,
		unsigned long duration) {
#if(DEBUG_LEVEL >= 4)
		DBG_PRINT_LEVEL("\t\tSound Output Tone Frequency: ");
		DBG_PRINT_LEVEL(frequency);
		DBG_PRINT_LEVEL(" Duration:  ");
		DBG_PRINTLN_LEVEL(duration);
	#endif		
	volume(level);
	tone(soundPin, frequency, duration);
}

void AudioDevTDA7056B::mute(void) {
	volume(LEVEL_0);
}

void AudioDevTDA7056B::compensation(uint8_t repetitions) {
	if (repetitions == 2) {
		delay(50);
	} else if (repetitions >= 3) {
		delay(40);
	}
}

void AudioDevTDA7056B::setup(void) {

	// digital Pins
	pinMode(soundPin, OUTPUT);

	// Analog Pins 
	pinMode(adjPin, OUTPUT);

	// Preset
	digitalWrite(soundPin, 0);
	analogWrite(adjPin, LEVEL_0);
}

void AudioDevTDA7056B::volume(Level level) {
	analogWrite(adjPin, level * 2);
#if(DEBUG_LEVEL >= 4)
		DBG_PRINT_LEVEL("\t\t(Adj. Level to : ");
		DBG_PRINT_LEVEL((level % 255) * 100);
		DBG_PRINTLN_LEVEL("%) ");
		if(level == LEVEL_0){
			DBG_PRINT_LEVEL("\n");
			noTone(soundPin);
		}
	#endif	
}
