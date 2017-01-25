#include <LEDlib.h>

// Create a global LED object on Pin 13 with a standard blink interval

LED TestLed(13, LED_MODE_BLINK);

void setup() {
  // No need for any setup code
  // The constructor of our global variable will take care of it
}

void loop() {
  // We just call the clk() member function of our global variable

  TestLed.clk();

  // Sleep a little bit to allow debug output to contain not just clk() output in case you defined _DEBUG

  // delay(100);
}
