/*
 File	: InfraredTSOP4838.cpp
 Version : 1.0
 Date  : 12/07/2016
 Project : Systel Babuino Net e Amplimatic Receiver InfraRed Support Arduino Library
 Author  : Daniel Valentin - dtvalentin@gmail.com

 Descrição:
 1.0 - 12/07/2015 - Programação Básica

 */

#include "InfraredTSOP4838.h"

#include <Equino.h>

decode_results results;

void InfraredTSOP4838::start(uint8_t data_PIN) {
	dataPin = data_PIN;
	setup();
	irrecv = &IRrecv(dataPin);
	irrecv->enableIRIn(); // Start the receiver
#if(DEBUG_LEVEL >= 3)
	DBG_PRINT_LEVEL("TSOP4838 Infrared Receiver Device Driver Started (PIN: ");
	DBG_PRINT_LEVEL(dataPin);
	DBG_PRINTLN_LEVEL(")...");
#endif
}

uint8_t InfraredTSOP4838::read(void) {
	// Verifica entrada por Infra-vermelho
	uint8_t value = EMPTY_DATA;

	if (irrecv->decode(&results)) {
#if(DEBUG_LEVEL >= 4)
		DBG_PRINT_LEVEL("\nFind Key code received -> ");
		DBG_PRINT_LEVEL(results.value);
		DBG_PRINT_LEVEL("\tDecoder -> ");
		if (results.decode_type == NEC) {
			DBG_PRINTLN_LEVEL("NEC");
		} else {
			DBG_PRINTLN_LEVEL("Key code " + String(results.decode_type));
		}
#endif

		unsigned long key = 0;

		for (byte k = 0; k < getIRCodesLen(); k++) {
			key = (unsigned long) pgm_read_dword(&IRCodes_NEC[k]);

#if(DEBUG_LEVEL >= 4)
			DBG_PRINT_LEVEL("\tFind key[" + String(k) + "] -> ");
			DBG_PRINT_LEVEL(key);
#endif

			if (results.value == key) {
				value = (k + 48);
#if(DEBUG_LEVEL >= 3)
				DBG_PRINTLN_LEVEL(
						"\tIR Receive NEC Key code found -> [" + String(k) + "]");
#endif
				break;
			} else if (results.value
					== (unsigned long) pgm_read_dword(&IRCodes_AMPL[k])) {
				value = (k + 48);
#if(DEBUG_LEVEL >= 3)
				DBG_PRINTLN_LEVEL(
						"\tIR Receive Amplimatic Key code found -> [" + String(k)
								+ "]");
#endif
				break;
			}

#if(DEBUG_LEVEL >= 4)
			DBG_PRINTLN_LEVEL("\t IR Receive Key not Found... ");
#endif
		}
		irrecv->resume(); // Receive the next value
	}
	return value;
}

int InfraredTSOP4838::read(decode_results &results) {
	int i = irrecv->decode(&results);
	irrecv->resume();
	return i;
}

void InfraredTSOP4838::setup(void) {
	pinMode(dataPin, INPUT);
}
