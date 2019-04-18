/*
 File  : ATUserInterface.h
 Version : 1.0
 Date  : 16/04/2019
 Project : Systel Action Timer User Interface Support Arduino Library
 Author  : Daniel Valentin - dtvalentin@gmail.com
 
 
 */

#ifndef _ATUserInterface_H_
#define _ATUserInterface_H_

#include <DisplayDevice7Seg.h>

#define LATCH_PIN			A2
#define DATA_PIN			A1
#define CLOCK_PIN			A0
#define ENABLE_PIN			3

#define BUFFER_SIZE			8

// library interface description
class ATPage {
	// user-accessible "public" interface
public:
	virtual ~ATPage();

	virtual void show(void);

	virtual void input(char code);

	virtual void navigateTo(ATPage *page);

	virtual void refresh(void);

	// library-accessible "protected" interface
protected:

	// library-accessible "private" interface
private:

};

// library interface description
class ATUserInterface {
	// user-accessible "public" interface
public:

	void start(void);

	void updateDisplay(void);

	void read(char code);

	void showPage(ATPage *page);

	// library-accessible "protected" interface
protected:

	void setup(void);

	// library-accessible "private" interface
private:

	ATPage *activePage;

};

// Pages

// library interface description
class ATMainPage: public ATPage {
	// user-accessible "public" interface
public:
	void refresh(void);

	void input(char code);

	// library-accessible "protected" interface
protected:

	// library-accessible "private" interface
private:

};

// library interface description
class ATRunPage: public ATPage {
	// user-accessible "public" interface
public:

	void refresh(void);

	void input(char code);

	// library-accessible "protected" interface
protected:

	// library-accessible "private" interface
private:

};

// library interface description
class ATProgramPage: public ATPage {
	// user-accessible "public" interface
public:

	void refresh(void);

	// library-accessible "protected" interface
protected:

	// library-accessible "private" interface
private:
	byte time = 0;

};

// library interface description
class ATSetupPage: public ATPage {
	// user-accessible "public" interface
public:
	void refresh(void);

	void input(char code);

	// library-accessible "protected" interface
protected:
	void loadProgram(char code);

	void saveData(void);

	void loadAction(void);

	void loadPause(void);

	void loadInterval(void);

	void loadCycles(void);

	void loadMode(void);

	void loadSound(void);

	void inputMode(void);

	void inputSound(void);

	// library-accessible "private" interface
private:

	char value0 = 'P';

	int value1 = 0;

	int value2 = 0;

	int phase = 0;

	void print();
};

// Display
extern DisplayDevice7Seg4094 display;
extern ATUserInterface userInterface;

// Pages
extern ATMainPage mainPage;
extern ATRunPage runPage;
extern ATProgramPage programPage;
extern ATSetupPage setupPage;

#endif

