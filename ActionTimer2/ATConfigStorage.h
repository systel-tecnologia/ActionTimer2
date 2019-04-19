/*
 File  : ATConfigStorage.h
 Version : 1.0
 Date  : 13/03/2019
 Project : Systel Storage EEPROM Devices Configuration Data Support Arduino Library
 Author  : Daniel Valentin - dtvalentin@gmail.com
 
 Storage EEPROM Devices Configuration Data
 
 */

#ifndef _ATConfigStorage_H_
#define _ATConfigStorage_H_

#define INIT_DATA_FLAG					61

#define INIT_DATA_FLAG_ADDRESS			1

#define DATA_STORE_ADDRESS				10

enum Mode {
	UP, DOWN
};

enum Sound {
	LIGHT, HEAVE, HARD
};

struct ATProgram {
	Mode mode = DOWN;
	Sound sound = LIGHT;
	int cycles = 3; // Count
	int action = 60; // Seconds
	int pause = 30; // Seconds
	int interval = 60; // Seconds
	int alertInitial = 10;
	int alertFinal = 15;
	int modified = 0;
};

// library interface description
class ATConfigStorage {
	// user-accessible "public" interface
public:

	void start(void);

	ATProgram load(int programaIndex);

	void save(ATProgram data, int programaIndex);

	void reset(void);

	// library-accessible "protected" interface
protected:

	void setup(void);

	// library-accessible "private" interface
private:
};

#endif
