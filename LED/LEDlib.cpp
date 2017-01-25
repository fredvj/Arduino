// LEDlib for Arduino
// http://www.github.com/fredvj/LEDlib

#include "LEDlib.h"
#include <Arduino.h>

LED::LED() {
	setPin(DEFAULT_LED_PIN);
	init();
}

LED::LED(int LedPin) {
	setPin(LedPin);
	init();
}

LED::LED(int LedPin, unsigned int Mode) {
	setPin(LedPin);
	init();
	setMode(Mode);
}

void LED::init(void) {
	StateChange = millis();
	Mode = LED_MODE_OFF;
	On = false;
	
	// Enable serial port for debugging
	
	#ifdef _DEBUG
		#ifdef _DEBUG_INIT_SERIAL
		Serial.begin(57600);
		#endif
	#endif
}

void LED::setPin(int LedPin) {
	// Remember the pinMode
	
	Pin = LedPin;
	
	// Set the pin to output mode
	
	pinMode(Pin, OUTPUT);
}

void LED::setMode(unsigned int NewMode) {
	// Set new mode
	
	switch(NewMode) {
		case LED_MODE_HOLD_ON:
		case LED_MODE_HOLD_OFF:
			// Remember time of state
	
			StateChange = millis();
			break;
			
		case LED_MODE_ON:
		case LED_MODE_OFF:
		case LED_MODE_BLINK:
		case LED_MODE_BLINK_A:
			// Just set the new mode
			break;
			
		default:
			// Unknown mode; set to off
			NewMode = LED_MODE_OFF;
			break;
	}
	
	// Set new mode if we are not in hold mode
	
	Mode = NewMode;
}

void LED::setLowInterval(unsigned long newLow) {
	Low = newLow;
}

void LED::setHighInterval(unsigned long newHigh) {
	High = newHigh;
}

void LED::blink(void) {
	blink(BLINK_STANDARD, BLINK_STANDARD);
}

void LED::blink(unsigned long Interval) {
	blink(Interval, Interval);
}

void LED::blink(unsigned long newLow, unsigned long newHigh) {
	setLowInterval(newLow);
	setHighInterval(newHigh);
	setMode(LED_MODE_BLINK_A);
}

void LED::holdOn(unsigned long Interval) {
	setHighInterval(Interval);
	setMode(LED_MODE_HOLD_ON);
}

void LED::holdOff(unsigned long Interval) {
	setLowInterval(Interval);
	setMode(LED_MODE_HOLD_OFF);
}

boolean LED::isHolding(void) {
	if((Mode == LED_MODE_HOLD_ON) || (Mode == LED_MODE_HOLD_OFF)) return(true);
	
	return(false);
}

void LED::turnOn(void) {
	setMode(LED_MODE_ON);
}

void LED::turnOff(void) {
	setMode(LED_MODE_OFF);
}

void LED::on(void) {
	#ifdef _DEBUG
		Serial.print("LED(");
		Serial.print(Pin, DEC);
		Serial.println(").on()");
	#endif
	
	On = true;
	
	digitalWrite(Pin, HIGH);
	
	StateChange = millis();
}

void LED::off(void) {
	#ifdef _DEBUG
		Serial.print("LED(");
		Serial.print(Pin, DEC);
		Serial.println(").off()");
	#endif
	
	On = false;
	
	digitalWrite(Pin, LOW);
	
	StateChange = millis();
}

void LED::clk(void) {
	// Get the milliseconds of the system running
	
	unsigned long now = millis();
	
	// Call other member function an pass along millis()
	
	clk(now);
}
	
void LED::clk(unsigned long now) {
	#ifdef _DEBUG
		Serial.print("LED(");
		Serial.print(Pin, DEC);
		Serial.print(").clk(");
		Serial.print(now, DEC);
		Serial.println(")");
	#endif
	
	switch(Mode) {
		case LED_MODE_ON:
			// Turn the LED on just in case it is off
			
			if(!On) on();
			break;
			
		case LED_MODE_OFF:
			// Turn the LED off just in case it is on
			
			if(On) off();
			break;
			
		case LED_MODE_BLINK:
		case LED_MODE_BLINK_A:
			// Symetric blinking is just a special case of asymetrical blinking
			
			if(On) {
				if(now >= (StateChange + High)) off();
			}
			else {
				if(now >= (StateChange + Low)) on();
			}
			break;
			
		case LED_MODE_HOLD_ON:
			// Once we exceed the hold time, we change the mode to off
			
			if(now >= (StateChange + High)) {
				setMode(LED_MODE_OFF);
			}
			else {
				if(!On) on();
			}
			break;
			
		case LED_MODE_HOLD_OFF:
			// Once we exceed the hold time, we change the mode to on
			
			if(now >= (StateChange + Low)) {
				setMode(LED_MODE_ON);
			}
			else {
				if(On) off();
			}
			break;
			
		default:
			// Unknown mode - Set mode to off
			
			setMode(LED_MODE_OFF);
			break;
	}
	
	// Remember time of clock cycle for the next run
	// Should prevent time warp issues, but we do not compensate - See https://www.arduino.cc/en/Reference/Millis for details
	
	if(StateChange > now) {
		StateChange = now;
	}
}