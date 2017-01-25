// LEDlib for Arduino
// http://www.github.com/fredvj/LEDlib

#ifndef _LEDLIB_H_
#define _LEDLIB_H_	1

// Include for the Arduino platform

#include <Arduino.h>

// Uncomment line below to enable debugging at 57600 baud on the serial line
// #define _DEBUG				1
// #define _DEBUG_INIT_SERIAL	1

#define DEFAULT_LED_PIN		13

// Define different modes

#define LED_MODE_OFF		0
#define LED_MODE_BLINK		1
#define LED_MODE_BLINK_A	2
#define LED_MODE_HOLD_ON	3
#define LED_MODE_HOLD_OFF	4
#define LED_MODE_ON			100

// Define some standards for blink modes

#define BLINK_ULTRASLOW		10000
#define BLINK_EXTRASLOW		5000
#define BLINK_VERYSLOW		2000
#define BLINK_SLOW			1000
#define BLINK_STANDARD		500
#define BLINK_FAST			250
#define BLINK_VERYFAST		100
#define BLINK_EXTRAFAST		50
#define BLINK_ULTRAFAST		25


class LED {
	public:
		// Constructor variants
		
		LED(void);
		LED(int);
		LED(int, unsigned int);
		
		// Clock function to be called in loop
		
		void clk(void);
		void clk(unsigned long);
		
		// Other calls
		
		void turnOn(void);
		void turnOff(void);
		
		void setMode(unsigned int);
		void setLowInterval(unsigned long);
		void setHighInterval(unsigned long);
		
		void blink(void);
		void blink(unsigned long);
		void blink(unsigned long, unsigned long);
		
		void holdOn(unsigned long);
		void holdOff(unsigned long);
		
		boolean isHolding(void);
		
	protected:
		int Pin = DEFAULT_LED_PIN;

		boolean On = false;

		unsigned int Mode = LED_MODE_OFF;
		unsigned long Low = BLINK_STANDARD;
		unsigned long High = BLINK_STANDARD;

		unsigned long StateChange = 0;
		// unsigned long LastClock = 0;
		
	private:
		// Private init function used by constructor
		
		void init(void);
		void setPin(int);

		void on(void);
		void off(void);
};

#endif // _LEDLIB_H_
