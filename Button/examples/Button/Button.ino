#include <Buttonlib.h>

// Create a global BUTTON object on Pin 2

BUTTON TestButton(2, BUTTON_MODE_HOLD);

void setup() {
	// Define pin 13 as an output using the Led connected on lots of boards

	pinMode(13, OUTPUT);
}

void loop() {
	// We just call the clk() member function of our global variable

	TestButton.clk();
  
	// Use the button state to set the led
  
	if(TestButton.isPressed())
		digitalWrite(13, HIGH);
	else
		digitalWrite(13, LOW);

  // Sleep a little bit to allow debug output to contain not just clk() output in case you defined _DEBUG

  // delay(100);
}
