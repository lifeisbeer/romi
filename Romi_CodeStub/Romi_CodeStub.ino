#include "lineSensor.h"
#include "motor.h"
#include "encoders.h"
//#include "kinematics.h"
//#include "pid.h"
#include "helper_functions.h"

unsigned int count = 0;
unsigned int calTime = 50;
int turn = 1000;
int fullTurn = 1350;
int tPower = 25;
int state = 0;
int pState = 0;

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

  switch ( state ) 
  {
    // calibration procedure
    case 0:
      if ( pState != state ) { Serial.println(state); }
      else { pState = state; }
      if ( count < calTime ) {
        led_on(RED);
        adjSpeeds(tPower);
        m.move2speed(pL, pR);
        // run callibration procedures for each
        ll.cal();
        lc.cal();
        lr.cal();
        count++;
      }
      else {
        buzz_on();
        state = 1;
      }
      break;
 
    // move forward for a bit to connect to line
    case 1: 
      if ( pState != state ) { Serial.println(state); }
      else { pState = state; }
      led_on(YELLOW);
      if ( ll.onLine() || lc.onLine() || lr.onLine() ) {
        buzz_on();
        state = 2;
      }
      else {
        adjSpeeds(tPower);
        m.move2speed(pL, pR);
      }
      break;
    
    // turn left 90 deg, then go to controller(3)
    case 2:
      if ( pState != state ) { Serial.println(state); }
      else { pState = state; }
      if ( count_left - turn < count_right ){
        m.turnOnSpot(tPower, LEFT);
      }
      else {
        count_left = 0;
        count_right = 0;
        state = 3;
      }
      break;
    
    // normal functionality 
    case 3:
      if ( pState != state ) { Serial.println(state); }
      else { pState = state; }
      if (        /*111*/ ll.onLine()    && lc.onLine()    && lr.onLine() ) {
        // all on line => move straight
        led_on(YELLOW);
        adjSpeeds(tPower);
        m.move2speed(pL, pR);
      } else if ( /*110*/ ll.onLine()    && lc.onLine()    && lr.notOnLine() ) {
        // left & centre on, right off => turn using left as pivot
        m.turn(tPower, LEFT);
        count_left = 0;
        count_right = 0;
      } else if ( /*101*/ ll.onLine()    && lc.notOnLine() && lr.onLine() ) {
        // shouldn't be possible => do nothing?
        led_on(RED);
        led_on(GREEN);
        led_on(YELLOW);
      } else if ( /*100*/ ll.onLine()    && lc.notOnLine() && lr.notOnLine() ) {
        // left on, centre & right off => turn using left as pivot
        m.turn(tPower, LEFT);
        count_left = 0;
        count_right = 0;
      } else if ( /*011*/ ll.notOnLine() && lc.onLine()    && lr.onLine() ) {
        // centre & right on, left off => turn using right as pivot
        m.turn(tPower, RIGHT);
        count_left = 0;
        count_right = 0;
      } else if ( /*010*/ ll.notOnLine() && lc.onLine()    && lr.notOnLine() ) {
        // shouldn't be possible => do nothing?
        led_on(RED);
        led_on(GREEN);
        led_on(YELLOW);
      } else if ( /*001*/ ll.notOnLine() && lc.notOnLine() && lr.onLine() ) {
        // right on, centre & left off => turn using right as pivot
        m.turn(tPower, RIGHT);
        count_left = 0;
        count_right = 0;
      } else if ( /*000*/ ll.notOnLine() && lc.notOnLine() && lr.notOnLine() ) {
        // nothing on => left 90 deg and check
        led_on(GREEN);
        buzz_on();
        state = 4;
        m.move(0);
        count_left = 0;
        count_right = 0;
      }
      break;
 
    // turn left 90 deg, then check if on line
    case 4:
      if ( pState != state ) { Serial.println(state); }
      else { pState = state; }
      led_on(GREEN);
      if ( count_left - turn < count_right ){
        m.turnOnSpot(tPower, LEFT);
      }
      else {
        m.move(0);
        count_left = 0;
        count_right = 0;
        // check if any on line and go to controller
        if ( ll.onLine() || lc.onLine() || lr.onLine() ) {
          buzz_on();
          state = 3;
        }
        // not on line, turn right 180 deg and check
        else {
          buzz_on();
          state = 5;
        }
      }
      break;
    
    // turn right 180 deg, then check if on line
    case 5:
      if ( pState != state ) { Serial.println(state); }
      else { pState = state; }
      led_on(GREEN);
      if ( count_right - 2*turn < count_left ){
        m.turnOnSpot(tPower, RIGHT);
      }
      else {
        m.move(0);
        count_left = 0;
        count_right = 0;
        // check if any on line and go to controller
        if ( ll.onLine() || lc.onLine() || lr.onLine() ) {
          buzz_on();
          state = 3;
        }
        // not on line, turn left 90 deg and move straight
        else {
          buzz_on();
          state = 6;
        }
      }
      break;
    
    // turn left 90 deg, then move straight for a bit
    case 6:
      if ( pState != state ) { Serial.println(state); }
      else { pState = state; }
      if ( count_left - turn < count_right ){
        led_on(GREEN);
        m.turnOnSpot(tPower, LEFT);
      }
      else {
        m.move(0);
        count_left = 0;
        count_right = 0;
        count = 0;
        state = 7;
      }
      break;

    // move straight for a bit, then check for the line
    case 7:
      if ( pState != state ) { Serial.println(state); }
      else { pState = state; }
      adjSpeeds(tPower);
      m.move2speed(pL, pR);
      if ( count > 20 ) {
        m.move(0);
        // check if any on line and go to controller
        if ( ll.onLine() || lc.onLine() || lr.onLine() ) {
          buzz_on();
          state = 3;
        }
        // not on line, then turn back
        else {
          buzz_on();
          state = 6;
        }
      }
      else {
        count++;
      }
      break;

    // at the end of the line => go home
    case 8:
      break;
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
