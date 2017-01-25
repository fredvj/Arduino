// Include header files for the watchdog timer

#include <avr/wdt.h>

// Include header files for the DS1307 real time clock and the i2c bus

#include <Wire.h>
#include "RTClib.h"

// Include header of the LED library

#include <LEDlib.h>

// Include header of the button library

#include <Buttonlib.h>

// You can turn on debugging at different stages. Move it above the other include files to see even more debugging output on the serial line.
// Use _RESET_UP_DOWN to reset the time for the up & down movement to the defaults

// #define _DEBUG  1
// #define _RESET_UP_DOWN  1
// #define _WRITE_TIME_DATE  1

// Define the time it takes to move the shutters up or down

#define HOLD_UP_DOWN  20000

// Define the digital I/Os

#define STATUS_LED    2

#define RELAY_UP      3
#define RELAY_DOWN    4

#define BUTTON_STATE  5
#define BUTTON_UP     6
#define BUTTON_DOWN   7

// Create global button, LED & relay objects

// Create a global LED object on Pin 2

LED StatusLED(STATUS_LED, LED_MODE_OFF);

// Create a global relay object for moving the shades up

LED RelayUp(RELAY_UP, LED_MODE_ON);

// Create a global relay object for moving the shades down

LED RelayDown(RELAY_DOWN, LED_MODE_ON);

// Create a global button object for the state

BUTTON ButtonState(BUTTON_STATE, BUTTON_MODE_HOLD | BUTTON_MODE_PULLUP);

// Create a global button object for moving the shades up

BUTTON ButtonUp(BUTTON_UP, BUTTON_MODE_DEBOUNCE | BUTTON_MODE_PULLUP);

// Create a global button object for moving the shades down

BUTTON ButtonDown(BUTTON_DOWN, BUTTON_MODE_DEBOUNCE | BUTTON_MODE_PULLUP);

// Create the global object for the real time clock module connected via I2C

RTC_DS1307 rtc;

// Create global variables for the time to move up & down

unsigned long UpTime = 27900L; // 7:45 = 7*3600 + 45*60 = 27900
unsigned long DownTime = 72900L; // 20:00 = 20*3600 +15*60 = 72900
unsigned long DaySeconds = 0L;

// Functions to read & store times in / from NVRAM

unsigned long readUpTime(void) {
  unsigned long Up[4];
  unsigned char a;
  unsigned char store = 0;
  unsigned long value = 0L;

  #ifdef _DEBUG
    Serial.print("readUpTime: ");
  #endif

  for(a=0; a<4; a++) {
    store = rtc.readnvram(a);
    #ifdef _DEBUG
      Serial.print(store, HEX);
      Serial.print(" ");
    #endif
    Up[a] = (unsigned long)store;
  }

  value = Up[0] + (Up[1]<<8) + (Up[2]<<16);

  #ifdef _DEBUG
    Serial.print(" := ");
    Serial.print(value, DEC);
    Serial.println("");
  #endif

  return(value);
}

unsigned long readDownTime(void) {
  unsigned long Down[4];
  unsigned char a;
  unsigned char store = 0;
  unsigned long value = 0L;

  #ifdef _DEBUG
    Serial.print("readDownTime: ");
  #endif

  for(a=0; a<4; a++) {
    store = rtc.readnvram(a+4);
    #ifdef _DEBUG
      Serial.print(store, HEX);
      Serial.print(" ");
    #endif
    Down[a] = (unsigned long)store;
  }

  value = Down[0] + (Down[1]<<8) + (Down[2]<<16);

  #ifdef _DEBUG
    Serial.print(" := ");
    Serial.print(value, DEC);
    Serial.println("");
  #endif

  return(value);
}

boolean writeUpTime(unsigned long up) {
  unsigned char store=0;

  #ifdef _DEBUG
    Serial.print("writeUpTime: ");
  #endif

  store = (unsigned char)(up & 0x000000ff);
  rtc.writenvram(0, store);
  #ifdef _DEBUG
    Serial.print(store, HEX);
    Serial.print(" ");
  #endif
  
  store = (unsigned char)((up & 0x0000ff00)>>8);
  rtc.writenvram(1, store);
  #ifdef _DEBUG
    Serial.print(store, HEX);
    Serial.print(" ");
  #endif
  
  store = (unsigned char)((up & 0x00ff0000)>>16);
  rtc.writenvram(2, store);
  #ifdef _DEBUG
    Serial.print(store, HEX);
    Serial.print(" ");
  #endif

  store = (unsigned char)((up & 0xff000000)>>24);
  rtc.writenvram(3, store);
  #ifdef _DEBUG
    Serial.print(store, HEX);
    Serial.println("");
  #endif

  return(true);
}

boolean writeDownTime(unsigned long down) {
  unsigned char store;

  #ifdef _DEBUG
    Serial.print("writeDownTime: ");
  #endif

  store = (unsigned char)(down & 0x000000ff);
  rtc.writenvram(4, store);
  #ifdef _DEBUG
    Serial.print(store, HEX);
    Serial.print(" ");
  #endif

  store = (unsigned char)((down & 0x0000ff00)>>8);
  rtc.writenvram(5, store);
  #ifdef _DEBUG
    Serial.print(store, HEX);
    Serial.print(" ");
  #endif

  store = (unsigned char)((down & 0x00ff0000)>>16);
  rtc.writenvram(6, store);
  #ifdef _DEBUG
    Serial.print(store, HEX);
    Serial.print(" ");
  #endif

  store = (unsigned char)((down & 0xff000000)>>24);
  rtc.writenvram(7, store);
  #ifdef _DEBUG
    Serial.print(store, HEX);
    Serial.println("");
  #endif

  return(true);
}

void serialPrintUpDownTime(void) {
  #ifdef _DEBUG
    Serial.print("Up: ");
    Serial.print(UpTime, DEC);
    Serial.print("; Down: ");
    Serial.print(DownTime, DEC);
    Serial.println("");
  #endif
}

void serialPrintTime(void) {
  #ifdef _DEBUG
    DateTime now = rtc.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");

    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
  #endif
}

unsigned long getDaySeconds(void) {
  DateTime now = rtc.now();

  return((unsigned long)now.hour() * 3600L + (unsigned long)now.minute() * 60L + (unsigned long)now.second());
}

void setup() {
  // Enable the watchdog timer - 1 second

  wdt_enable(WDTO_1S);
  
  // Set up the serial port

  #ifdef _DEBUG
    Serial.begin(57600);
    Serial.println("Shutter control ...");
  #endif

  // Setup the LED for some kind of "I am still alive" signal - very slow blinking

  StatusLED.setLowInterval(BLINK_EXTRASLOW);
  StatusLED.setHighInterval(BLINK_EXTRAFAST);
  StatusLED.setMode(LED_MODE_BLINK_A);

  if(!rtc.begin()) {
    #ifdef _DEBUG
      Serial.println("DS1307 real time clock module not found.");
    #endif
  }
  else {
    if(!rtc.isrunning()) {
      #ifdef _DEBUG
        Serial.print("DS1307 is not running. Initializing: ");
      #endif

      // Use date & time of compile time
      
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      #ifdef _DEBUG
        Serial.print(__DATE__);
        Serial.print(" ");
        Serial.print(__TIME__);
        Serial.println("");
      #endif

      // Reset the up & down time

      writeUpTime(UpTime);
      writeDownTime(DownTime);
    }
    else {
      #ifdef _RESET_UP_DOWN
      #ifdef _DEBUG
        Serial.println("Resetting up & down time to defaults.");
      #endif
        writeUpTime(UpTime);
        writeDownTime(DownTime);
      #endif

      #ifdef _WRITE_TIME_DATE
        // Use date & time of compile time
      
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      #ifdef _DEBUG
        Serial.print(__DATE__);
        Serial.print(" ");
        Serial.print(__TIME__);
        Serial.println("");
      #endif
      #endif

      // Read the up & down time from NVRAM

      UpTime = readUpTime();
      DownTime = readDownTime();

      #ifdef _DEBUG
        Serial.print("DS1307 running: ");
        serialPrintTime();
        serialPrintUpDownTime();
      #endif
    }
  }
}

void loop() {
  // Let us start by resetting the watchdog timer

  wdt_reset();
  
  // We call the clk() member functions of our global variables to keep them happy

  StatusLED.clk();
  RelayUp.clk();
  RelayDown.clk();
  ButtonState.clk();
  ButtonUp.clk();
  ButtonDown.clk();

  // If either one of the relays or the LED is running a holding pattern, do not execute the rest
  // Should be a good way to give the user some time to release the button and not writting values to NVRAM repeatedly

  if(RelayUp.isHolding() ||
     RelayDown.isHolding() ||
     StatusLED.isHolding()) return;

  if(ButtonState.isPressed()) {
    // This is automatic mode
    
    DaySeconds = getDaySeconds();

    // Check if we reached the time to move things up

    if((UpTime != 0L) && (UpTime == DaySeconds)) {
      #ifdef _DEBUG
        Serial.println("Automatic mode: Time to move things UP.");
        Serial.print("Seconds since midnight: ");
        Serial.print(DaySeconds, DEC);
        Serial.println("");
        serialPrintUpDownTime();
        serialPrintTime();
      #endif

      // The rest is the same as for manual mode
      
      StatusLED.blink(BLINK_VERYFAST);
      RelayDown.turnOn();
      RelayUp.holdOff(HOLD_UP_DOWN);
      return;
    }

    // Check if we reached the time to move things down

    if((DownTime != 0L) && (DownTime == DaySeconds)) {
      #ifdef _DEBUG
        Serial.println("Automatic mode: Time to move things DOWN.");
        Serial.print("Seconds since midnight: ");
        Serial.print(DaySeconds, DEC);
        Serial.println("");
        serialPrintUpDownTime();
        serialPrintTime();
      #endif

      // The rest is the same as for manual mode
      
      StatusLED.blink(BLINK_VERYFAST);
      RelayUp.turnOn();
      RelayDown.holdOff(HOLD_UP_DOWN);
      return;
    }

    // Check if the user wants to set a new time for moving things up

    if(ButtonUp.isPressed(HOLD_TWOSECONDS)) {
      // Store the new time for moving things up
      
      UpTime = getDaySeconds();
      writeUpTime(UpTime);
      #ifdef _DEBUG
        serialPrintUpDownTime();
        serialPrintTime();
      #endif
      
      // The rest is the same as for manual mode
      
      StatusLED.blink(BLINK_VERYFAST);
      RelayDown.turnOn();
      RelayUp.holdOff(HOLD_UP_DOWN);
      return;
    }

    // Check if the user wants to set a new time for moving things down

    if(ButtonDown.isPressed(HOLD_TWOSECONDS)) {
      // Store the new time for moving things down
      
      DownTime = getDaySeconds();
      writeDownTime(DownTime);
      #ifdef _DEBUG
        serialPrintUpDownTime();
        serialPrintTime();
      #endif
      
      // The rest is the same as for manual mode
      
      StatusLED.blink(BLINK_VERYFAST);
      RelayUp.turnOn();
      RelayDown.holdOff(HOLD_UP_DOWN);
      return;
    }

    // This is the default pattern for the status LED in automatic mode - on most of the time; just a short low interval to show that we are still alive

    StatusLED.setHighInterval(BLINK_EXTRASLOW);
    StatusLED.setLowInterval(BLINK_EXTRAFAST);

    StatusLED.setMode(LED_MODE_BLINK_A);
    return;
  }

  // The rest of the code will handle manual mode

  if(ButtonUp.isPressed(HOLD_TWOSECONDS)) {
    #ifdef _DEBUG
      Serial.println("");
      Serial.println("Starting holding pattern to move things UP ...");
    #endif
    
    StatusLED.blink(BLINK_VERYFAST);
    RelayDown.turnOn();
    RelayUp.holdOff(HOLD_UP_DOWN);
    return;
  }

  if(ButtonDown.isPressed(HOLD_TWOSECONDS)) {
    #ifdef _DEBUG
      Serial.println("");
      Serial.println("Starting holding pattern to move things DOWN ...");
    #endif
    
    StatusLED.blink(BLINK_VERYFAST);
    RelayUp.turnOn();
    RelayDown.holdOff(HOLD_UP_DOWN);
    return;
  }

  if(ButtonUp.isPressed()) {
    #ifdef _DEBUG
      Serial.print("UP ");
    #endif
    
    StatusLED.turnOn();
    RelayDown.turnOn();
    RelayUp.turnOff();
    return;
  }

  if(ButtonDown.isPressed()) {
    #ifdef _DEBUG
      Serial.print("DOWN ");
    #endif
    
    StatusLED.turnOn();
    RelayUp.turnOn();
    RelayDown.turnOff();
    return;
  }

  RelayUp.turnOn();
  RelayDown.turnOn();

  StatusLED.setLowInterval(BLINK_EXTRASLOW);
  StatusLED.setHighInterval(BLINK_EXTRAFAST);
  StatusLED.setMode(LED_MODE_BLINK_A);
}
