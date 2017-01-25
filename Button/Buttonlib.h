// LEDlib for Arduino
// http://www.github.com/fredvj/Buttonlib

#ifndef _BUTTONLIB_H_
#define _BUTTONLIB_H_	1

// Include for the Arduino platform

#include <Arduino.h>

// Uncomment line below to enable debugging at 57600 baud on the serial line
// #define _DEBUG				1
// #define _DEBUG_INIT_SERIAL	1

#define DEFAULT_BUTTON_PIN		2

// Define different modes

#define BUTTON_MODE_NONE		0
#define BUTTON_MODE_DEBOUNCE	1
#define BUTTON_MODE_HOLD		2

#define BUTTON_MODE_PULLUP		64

// Define some standards for hold times

#define HOLD_DEBOUNCE		50
#define HOLD_LONG			250
#define HOLD_SECOND			1000
#define HOLD_TWOSECONDS		2000
#define HOLD_FIVESECONDS	5000
#define HOLD_RESET			10000


class BUTTON {
	public:
		// Constructor variants
		
		BUTTON(void);
		BUTTON(int);
		BUTTON(int, unsigned int);
		
		// Clock function to be called in loop
		
		void clk(void);
		void clk(unsigned long);
		
		// Other calls

		boolean isPressed(void);
		boolean isPressed(unsigned int);
		
		void setMode(unsigned int);
		
		void reset(void);
		
	protected:
		int Pin = DEFAULT_BUTTON_PIN;

		boolean Pullup = false;
		boolean Pressed = false;
		boolean Input = false;
		boolean Triggered = false;

		unsigned int Mode = BUTTON_MODE_NONE;

		unsigned long StateChange = 0;
		unsigned long LastClock = 0;
		
	private:
		// Private init function used by constructor
		
		void init(void);
		void setPin(int);
};

#endif // _BUTTONLIB_H_
