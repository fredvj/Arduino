#include <LEDlib.h>

// Create a global LED object on Pin 13 with a standard blink interval

LED TestLed(13, LED_MODE_BLINK);

// Lets assume we have a dual channel relay board on pins 7 & 8

LED Relay1(7);
LED Relay2(8);

void setup() {
	Relay1.blink(BLINK_SLOW);
	Relay2.holdOn(BLINK_ULTRASLOW);
}

void loop() {
  // We just call the clk() member function of our global variables

  TestLed.clk();
  Relay1.clk();
  Relay2.clk();

  // Sleep a little bit to allow debug output to contain not just clk() output in case you defined _DEBUG

  // delay(100);
}
