/*
 File  : AudioDevTDA7056B.h
 Version : 1.0
 Date : 05/03/2019
 Project : Systel Equino TDA7056B Audio Driver Device Support Arduino Library
 Author  : Daniel Valentin - dtvalentin@gmail.com

 For uno:
 SOUND_OUT_PIN    connect to digital pin 5	ATMEGA_8 PIN 11
 SOUND_ADJ_PIN    connect to analog  pin A3	ATMEGA_8 PIN 26 (Esta Porta não é PWM não suporta ajuste variável)

 For Mega:
 SOUND_OUT_PIN    connect to digital MEGA 2560 PIN
 SOUND_ADJ_PIN    connect to analog  MEGA 2560 PIN (Esta Porta não é PWM não suporta ajuste variável)

 */

#ifndef _AudioDevTDA7056B_h_
#define _AudioDevTDA7056B_h_

#include "AudioMessageDevice.h"

#if defined(__AVR_ATmega2560__)
#define SOUND_OUT_PIN   18  // connect to digital MEGA 2560 PIN
#define SOUND_ADJ_PIN   A7 // connect to analog  MEGA 2560 PIN (Esta Porta não é PWM não suporta ajuste variável)
#else
		#define SOUND_OUT_PIN   5  // connect to digital pin 5	ATMEGA_8 PIN 11
		#define SOUND_ADJ_PIN   A3 // connect to analog  pin A3	ATMEGA_8 PIN 26 (Esta Porta não é PWM não suporta ajuste variável)
	#endif

// library interface description
class AudioDevTDA7056B: public AudioMessageDevice {
	// user-accessible "public" interface
public:

	AudioDevTDA7056B();

	void start(uint8_t sound_PIN, uint8_t adj_PIN);

	void signal(Level level, int frequency, unsigned long duration);

	void info(Level level, uint8_t repetitions = 1);

	void warn(Level level, uint8_t repetitions = 1);

	void error(Level level, uint8_t repetitions = 1);

	virtual void fatal(Level level, uint8_t repetitions = 1);

	virtual void mute();

	// library-accessible "private" interface
protected:

	void setup(void);

	void volume(Level level);

	// library-accessible "private" interface
private:

	void compensation(uint8_t repetitions);

	volatile uint8_t soundPin = SOUND_OUT_PIN;

	volatile uint8_t adjPin = SOUND_ADJ_PIN;
};

#endif
