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

#define INIT_DATA_FLAG					62

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
	int actions = 3; // Count
	int action = 10; // Seconds
	int pause = 10; // Seconds
	int interval = 10; // Seconds
};

struct ATProgramDataBase {
	ATProgram progs[9];
	int modified = 0;
};

// library interface description
class ATConfigStorage {
	// user-accessible "public" interface
public:

	void start(void);

	ATProgramDataBase load(void);

	void save(ATProgramDataBase data);

	void reset(void);

	// library-accessible "protected" interface
protected:

	void setup(void);

	// library-accessible "private" interface
private:
};

#endif
