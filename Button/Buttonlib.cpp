// Buttonlib for Arduino
// https://github.com/fredvj/Arduino/tree/master/Button

#include "Buttonlib.h"
#include <Arduino.h>

BUTTON::BUTTON() {
	init();
	
	setMode(BUTTON_MODE_DEBOUNCE);
	setPin(DEFAULT_BUTTON_PIN);
}

BUTTON::BUTTON(int ButtonPin) {
	init();

	setMode(BUTTON_MODE_DEBOUNCE);
	setPin(ButtonPin);
}

BUTTON::BUTTON(int ButtonPin, unsigned int Mode) {
	init();

	setMode(Mode);
	setPin(ButtonPin);
}

void BUTTON::init(void) {
	StateChange = millis();
	Mode = BUTTON_MODE_NONE;
	Pullup = false;
	Input = false;
	Pressed = false;
	Triggered = false;
	
	// Enable serial port for debugging
	
	#ifdef _DEBUG
		#ifdef _DEBUG_INIT_SERIAL
		Serial.begin(57600);
		#endif
	#endif
}

void BUTTON::reset(void) {
	StateChange = millis();
	Input = false;
	Pressed = false;
	Triggered = false;

	#ifdef _DEBUG
		Serial.print("BUTTON(");
		Serial.print(Pin, DEC);
		Serial.println(").reset()");
	#endif
}

void BUTTON::setPin(int ButtonPin) {
	Pin = ButtonPin;
	
	// Check if input is using Arduino's internal pullup resistors
	
	if(Pullup) {
		pinMode(Pin, INPUT_PULLUP);
		Input = true;
	}
	else {
		pinMode(Pin, INPUT);
		Input = false;
	}
}

void BUTTON::setMode(unsigned int NewMode) {
	// Check for pullup
	
	if((NewMode & BUTTON_MODE_PULLUP) == BUTTON_MODE_PULLUP) Pullup = true;
	
	// Filter out the pullup flag
	
	NewMode = NewMode ^ BUTTON_MODE_PULLUP;
	
	// Check for valid modes
	
	switch(NewMode) {
		case BUTTON_MODE_NONE:
		case BUTTON_MODE_DEBOUNCE:
		case BUTTON_MODE_HOLD:
			break;
			
		default:
			NewMode = BUTTON_MODE_NONE;
			break;
	}
	
	// Set new mode
	
	Mode = NewMode;
}

boolean BUTTON::isPressed(void) {
	#ifdef _DEBUG
		Serial.print("BUTTON(");
		Serial.print(Pin, DEC);
		Serial.print(").isPressed() := ");
		if(Pressed)
			Serial.println("true");
		else
			Serial.println("false");
	#endif
	
	// Return the state
	
	return(Pressed);
}

boolean BUTTON::isPressed(unsigned int Interval) {
	#ifdef _DEBUG
		Serial.print("BUTTON(");
		Serial.print(Pin, DEC);
		Serial.print(").isPressed(");
		Serial.print(Interval, DEC);
		Serial.print(") := ");
	#endif
	
	// Return the state
	
	if(Pressed) {
		if(millis() >= (StateChange + Interval)) {
			#ifdef _DEBUG
				Serial.println("true");
			#endif
			return(true);
		}
	}
	
	// Not pressed that long
	
	#ifdef _DEBUG
		Serial.println("false");
	#endif
	
	return(false);
}

void BUTTON::clk(void) {
	// Get the milliseconds of the system running
	
	unsigned long now = millis();
	
	// Call other member function and pass along millis()
	
	clk(now);
}
	
void BUTTON::clk(unsigned long now) {
	boolean currentInput = false;
	
	#ifdef _DEBUG
		Serial.print("BUTTON(");
		Serial.print(Pin, DEC);
		Serial.print(").clk(");
		Serial.print(now, DEC);
		Serial.print(") :: ");
	#endif
	
	// Get the current state of the input pin
	
	if(digitalRead(Pin) == HIGH)
		currentInput = true;
	else
		currentInput = false;
	
	#ifdef _DEBUG
		Serial.print("digitalRead(");
		Serial.print(Pin, DEC);
		Serial.print(") := ");
		if(currentInput)
			Serial.print("HIGH");
		else
			Serial.print("LOW");
		Serial.print(" :: ");
	#endif

	// Check for pullup mode; invert input as required
	
	if(Pullup) {
		currentInput = !currentInput;
		#ifdef _DEBUG
			Serial.print("PullUp :: ");
		#endif
	}

	switch(Mode) {
		case BUTTON_MODE_NONE:
			// Check for a state change
			if(Input != currentInput) {
				Pressed = currentInput;
				StateChange = now;
			}
			break;
			
		case BUTTON_MODE_DEBOUNCE:
			// Check for a state change
			if(Input != currentInput) {
				// We see a state change; either Low -> High or High -> Low
				// Remember the new state and the time of the change
				
				StateChange = now;
				
				// High -> Low changes propagate immediately
				
				if(!currentInput) {
					Pressed = false;
					Triggered = false;
				}
			}
			else {
				// We are seeing the same state; check the time we are in this state
				
				if(currentInput) {
					if(now >= (StateChange + HOLD_DEBOUNCE)) {
						Pressed = true;
						Triggered = true;
					}
				}
			}
			break;
			
		case BUTTON_MODE_HOLD:
			// Check for a state change
			if(Input != currentInput) {
				StateChange = now;
				
				// Check for High -> Low transition
				
				if(!currentInput) {
					Triggered = false;
				}
			}
			else {
				// We are seeing the same state; check the time we are in this state
				
				if(currentInput) {
					if(!Triggered) {
						if(now >= (StateChange + HOLD_DEBOUNCE)) {
							// Invert the pressed state
					
							Pressed = !Pressed;
						
							// Set the triggered flag
						
							Triggered = true;
					
							// Remember the time of the state change
					
							StateChange = now;
						}
					}
				}
			}
			break;
			
		default:
			// Unknown mode - Set mode to none
			
			setMode(BUTTON_MODE_NONE);
			break;
	}
	
	// Remember the input state
	
	Input = currentInput;
	
	// Should prevent time warp issues, but we do not compensate - See https://www.arduino.cc/en/Reference/Millis for details
	
	if(StateChange > now) {
		StateChange = now;
	}
	#ifdef _DEBUG
		Serial.println("");
	#endif
}