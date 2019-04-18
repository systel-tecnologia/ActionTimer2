/*
	File  : InfraredTSOP4838.h
	Version : 1.0
	Date  : 05/03/2019
	Project : Systel Equino Net e Amplimatic Receiver InfraRed Support Arduino Library
	Author  : Daniel Valentin - dtvalentin@gmail.com
	
	Infrared Receiver
		DATA    connect to analog pin 6 //	ATMEGA_8 PIN 12
	
*/

#ifndef _InfraredTSOP4838_H_
	#define _InfraredTSOP4838_H_
	
	#include <avr/pgmspace.h>
	#include <Arduino.h>
	#include <IRremote.h>
	#include <DataReceiverDevice.h>
	
	// Receive pin
	#define IR_REC_DATA_PIN 6
	
// IR Codes																
	const unsigned long IRCodes_NEC[24] PROGMEM = {
		// 	Decimal		Code	
		3782869247,		// CODE_0
		3782901887,		// CODE_1
		3782885567,		// CODE_2
		3782918207,		// CODE_3
		3782877407,		// CODE_4
		3782910047,		// CODE_5
		3782893727,		// CODE_6
		3782926367,		// CODE_7
		3782873327,		// CODE_8
		3782905967,		// CODE_9
		3782916167,		// CODE_A
		3782919227,		// CODE_B
		3782887607,		// CODE_C
		0,				// CODE_D
		3782912087,		// CODE_E
		3782883527,		// CODE_F
		3782881487,		// CODE_G
		3782924327,		// CODE_H
		3782922287,		// CODE_I
		3782895767,		// CODE_J
		3782928407,		// CODE_K
		3782899847,		// CODE_L
		3782875367,		// CODE_M
		3782908007		// CODE_N
	};
	
	const unsigned long IRCodes_AMPL[24] PROGMEM = {
		// 	Decimal		Code	
		2155833495,		// CODE_0
		2155866135,		// CODE_1
		2155813095,		// CODE_2
		2155845735,		// CODE_3
		2155829415,		// CODE_4
		2155862055,		// CODE_5
		2155821255,		// CODE_6
		2155853895,		// CODE_7
		2155837575,		// CODE_8
		2155870215,		// CODE_9
		2155847775,		// CODE_A
		2155815135,		// CODE_B
		2155839615,		// CODE_C
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0		
	};
	
	
	// library interface description
	class InfraredTSOP4838 : public DataReceiverDevice {
		// user-accessible "public" interface
		public:

		void start(uint8_t data_PIN);
		
		uint8_t read(void);
		
		int read(decode_results &results);
		
		// library-accessible "protected" interface
		protected:
		
		uint8_t getIRCodesLen(){return(lenIRCodes);}
		
		void setup(void);
		
		// library-accessible "private" interface
		private:
		
		IRrecv* irrecv;
		
		const uint8_t lenIRCodes = 24;
		
		volatile uint8_t dataPin = IR_REC_DATA_PIN;
		
	};
	
#endif
