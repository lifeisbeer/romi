#include "lineSensor.h"
#include "motor.h"
#include "encoders.h"
#include "kinematics.h"
#include "pid.h"
#include "helper_functions.h"

unsigned int count = 0;
unsigned int calTime = 50;
int turn = 1350;
int tPower = 25;
int state = 0;

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

  pR = tPower;
  pL = tPower;

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
  // calibration procedure
  if ( count < calTime ) {
    // red led
    led_on(RED);
    adjSpeeds(tPower);
    m.move2speed(pL, pR);
    // run callibration procedures for each
    ll.cal();
    lc.cal();
    lr.cal();
    count++;
  }
  // move forward for a bit to connect to line
  else if ( count < calTime + 1 ) { 
    // find line
    led_on(YELLOW);
    if ( ll.onLine() || lc.onLine() || lr.onLine() ) {
      buzz_on();
      count++;
    }
    else {
      // move straight until you find line
      adjSpeeds(tPower);
      m.move2speed(pL, pR);
    }
  }
  // normal functionality
  else if ( state == 0 ){
    if ( count_left - turn < count_right ){
      // turn left
      m.turnOnSpot(tPower, LEFT);
    }
    else {
      state++;
    }
  }
  else if ( state == 1 ) {
    int leftPow = (1-ll.norm())*20 + 15;
    int rightPow = (1-lr.norm())*20 + 15;
    m.move2speed(leftPow, rightPow);
  }

  // short delay
  delay( 25 );
  // turn off leds and buzzer
  led_off(YELLOW);
  led_off(GREEN);
  led_off(RED);
  buzz_off();
  // short delay
  delay( 25 );
}
