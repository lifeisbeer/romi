#include "lineSensor.h"
#include "encoders.h"
#include "kinematics.h"
#include "motor.h"
#include "pid.h"

#define YELLOW LED_BUILTIN
#define RED LED_BUILTIN_RX
#define GREEN LED_BUILTIN_TX
#define OFF LOW
#define ON HIGH
#define BUZZ 6

void led_on(int col) {
  if (col == YELLOW) {
    digitalWrite(col, ON);
  }
  else {
    digitalWrite(col, OFF);
  }
}

void led_off(int col) {
  if (col == YELLOW) {
    digitalWrite(col, OFF);
  }
  else {
    digitalWrite(col, ON);
  }
}

void buzz_on() {
  analogWrite(BUZZ, 100);
}

void buzz_off() {
  analogWrite(BUZZ, 0);
}

//instantiate classes
motors_c m;
lineSensor_c ll(LINE_LEFT_PIN);
lineSensor_c lc(LINE_CENTRE_PIN);
lineSensor_c lr(LINE_RIGHT_PIN);

// Setup, only runs once when the power
// is turned on.  However, if your Romi
// gets reset, it will run again.
void setup() {

  // encoder setup
  setupEncoder0();
  setupEncoder1();

  // set leds as output
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);

  // Start up the serial port.
  Serial.begin(9600);
  // Delay to connect properly.
  delay(1000);
  // Print a debug, so we can see a reset on monitor.
  Serial.println("***RESET***");

}


// The main loop of execution.  This loop()
// function is automatically called every
// time it finishes.  You should try to write
// your code to take advantage of this looping
// behaviour.  
void loop() {
  
  Serial.print( count_left );
  Serial.print( ", ");
  Serial.println( count_right );

  // short delay so that our plotter graph keeps
  // some history.
  delay( 2 );
  
}
