// Includes, etc.

// void ConstantVoltage(float);
// float ReadOutputVoltage(void);

// #define _DEBUG_    1

// Define the input & output pins we are going to use

#define DIGITAL_PIN_PWM_OUT           6
#define ANALOG_PIN_FEEDBACK_VOLTAGE   0

// It is a good idea to limit the duty cycle that is driving the transistor charging the inductor
// A duty cycle of 100% (255) would create a short circuit
// We are going to limit at around 80%: 255/100*80 = 204
//
// 100% -> 255
// 90%  -> 229.5
// 80%  -> 204
// 70%  -> 178.5
// 60%  -> 153
// 50%  -> 127.5
// ...

#define PWM_MAX      204

// ... and a value to start with

#define PWM_START    0

// The constant voltage we want to see

#define TARGET_VOLTAGE       12.0

// The output voltage is measured using a 15:1 voltage divider feeding back data to analog pin 0
// It does not hurt to specify the exact resistor values, if you want to sync theory and real world data coming from your multimeter

#define VOLTAGE_DIVIDER_R1    140000.0    // 150,000
#define VOLTAGE_DIVIDER_R2    9840.0      // 10,000

#define VOLTAGE_MULTIPLIER (VOLTAGE_DIVIDER_R1 / VOLTAGE_DIVIDER_R2)

// Maximum voltage we can feed to our analog inputs ...

#define MAXIMUM_ADC_VOLTAGE  5.0

// ... and the maximum value from the ADC

#define MAXIMUM_ADC_VALUE    1023.0

// Minimum voltage delta we can detect at the ADC

#define MINIMUM_ADC_VOLTAGE_DELTA  (MAXIMUM_ADC_VOLTAGE / MAXIMUM_ADC_VALUE)

// And the minimum voltage delta we are able to detect before it goes through the voltage divider

#define MINIMUM_VOLTAGE_DELTA (MINIMUM_ADC_VOLTAGE_DELTA * VOLTAGE_MULTIPLIER)

// If we are off by x * MINIMUM_VOLTAGE_DELTA let us rush the adjustment

#define SPIKE_FACTOR          20

// We have to adjust the PWM prescaler to increase the frequency

void setup() {
  // Setup PWM frequency - we want it to be as fast as possible
  // See here for more details:
  // http://arduino-info.wikispaces.com/Arduino-PWM-Frequency
  
  TCCR0B = TCCR0B & (0b11111000 | 0b00000001);
  
#ifdef _DEBUG_
  // Setup serial console
  
  Serial.begin(9600);
  Serial.println("Serial console initialized.");
#endif
  
  // Setup PWM output
  
  pinMode(DIGITAL_PIN_PWM_OUT, OUTPUT);
  
  // Start with the requested value for the duty cycle to start with - 0 if unchanged
  
  analogWrite(DIGITAL_PIN_PWM_OUT, PWM_START);
}

void loop() {
  // All we have to do here is calling the ConstantVoltage function
  // If anything goes wrong the function will return
  
  ConstantVoltage(TARGET_VOLTAGE);
  
  // ... and something went wrong ?!

#ifdef _DEBUG_
  Serial.println("How did we get here: Target voltage either to high or to low?");
  Serial.println("Giving up ...");
#endif
}

// Function to adjust the PWM duty cycle

void ConstantVoltage(float TargetVoltage) {
  float Voltage = 0.0;
  int pwm = PWM_START;
  int LoopCounter = 0;
  
  // Loop forever
  
  while(true) {
    // Let us start by reading the current voltage
    
    Voltage = ReadOutputVoltage();
    
#ifdef _DEBUG_
    // Show what we have
    
    Serial.print("Target: ");
    Serial.print(TargetVoltage);
    Serial.print("; Actual: ");
    Serial.print(Voltage);
    Serial.print("; PWM: ");
    Serial.print(pwm);
    Serial.print("; min. delta: ");
    Serial.print(MINIMUM_VOLTAGE_DELTA);
    Serial.print("; ");
#endif
    
    // Decide what to do
    
    if((LoopCounter % 2) == 0) {
      // Way too low ?
    
      if((Voltage + (SPIKE_FACTOR * MINIMUM_VOLTAGE_DELTA)) < TargetVoltage) {
        // Increase the PWM duty cycle
      
        pwm += (SPIKE_FACTOR/2);
      
        if(pwm > PWM_MAX) pwm = PWM_MAX;
        
#ifdef _DEBUG_
        Serial.print("too low; ");
#endif
      }

      // Spike ?
    
      if((Voltage - (SPIKE_FACTOR * MINIMUM_VOLTAGE_DELTA)) > TargetVoltage) {
        // Decrease the PWM duty cycle
      
        pwm -= (SPIKE_FACTOR/2);
      
        if(pwm < PWM_START) pwm = PWM_START;
        
#ifdef _DEBUG_
        Serial.print("spike; ");
#endif
      }
    }
    
    if((LoopCounter % 8) == 0) {
      // Increase duty cycle ?
    
      if((Voltage + MINIMUM_VOLTAGE_DELTA) < TargetVoltage) {
        // Check for error condition
      
        if(pwm >= PWM_MAX) {
          Serial.print("Error: Reached maximum duty cycle (");
          Serial.print(PWM_MAX);
          Serial.print("); ");
        }
      
        // Increase the PWM duty cycle
      
        if(pwm < PWM_MAX) pwm++;
        
#ifdef _DEBUG_
        Serial.print("increase");
#endif
      }

      // Decrease duty cycle ?
    
      if((Voltage - MINIMUM_VOLTAGE_DELTA) > TargetVoltage) {
        // Check for error condition
      
        if(pwm <= PWM_START) {
          Serial.print("Error: Reached minimum duty cycle (");
          Serial.print(PWM_START);
          Serial.print("); ");
        }
      
        // Decrease the PWM duty cycle
      
        if(pwm > PWM_START) pwm--;
        
#ifdef _DEBUG_
        Serial.print("decrease");
#endif
      }
    }

    // Set the new value of the duty cycle
    
    analogWrite(DIGITAL_PIN_PWM_OUT, pwm);
    
    // Increase the loop counter
    
    LoopCounter++;
    Serial.println();
    
  } // End of while(true)
  
#ifdef _DEBUG_
  Serial.println("Error: How did we break out of an infinite loop? (ConstantVoltage)");
#endif
  
  return;
}

// Function to read the real voltage taking into account the voltage divider being used

float ReadOutputVoltage(void) {
  float U = 0.0;
  
  U = (float)analogRead(ANALOG_PIN_FEEDBACK_VOLTAGE) * MINIMUM_VOLTAGE_DELTA;
  
  return(U);
}
