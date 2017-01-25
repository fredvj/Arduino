#include <LEDlib.h>

// Create a global LED object on Pin 13

LED TestLed(13);

void setup() {
  // Lets create something like the monitoring LED of a smoke detector

  TestLed.setHigh(BLINK_EXTRAFAST);
  TestLed.setLow(BLINK_ULTRASLOW);
  TestLed.setMode(LED_MODE_BLINK_A);
}

void loop() {
  // We just call the clk() member function of our global variable

  TestLed.clk();

  // Sleep a little bit to allow debug output to contain not just clk() output in case you defined _DEBUG

  // delay(100);
}
