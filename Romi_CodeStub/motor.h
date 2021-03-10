#ifndef _MOTOR_H
#define _MOTOR_H

#define L_PWM_PIN 10
#define L_DIR_PIN 16
#define R_PWM_PIN  9
#define R_DIR_PIN 15
#define DIR_BWD HIGH // HIGH: backwards
#define DIR_FWD LOW // LOW: forward
#define LEFT true
#define RIGHT false

// A class to neatly contain commands for the 
// motors, to take care of +/- values, a min/max
// power value, & pin setup.

// move(int power), move2speed(int pLeft, int pRight),
// turn(int power, bool dir), turnOnSpot(int power, bool dir)
class motors_c {
  private:
    // Accepts two integers (motor and power) 
    // and sets the power for that mortor 
    void setMotorPower(int motor, int power) {
      if ( power >= 0 ) {
        if ( power > 255 ) { power = 255; }
        // pin for direction is +6 from pin for power
        digitalWrite( motor + 6, DIR_FWD );
        analogWrite( motor, power );
      }
      else {
        if ( power < -255 ) { power = -255; }
        digitalWrite( motor + 6, DIR_BWD );
        analogWrite( motor, -power );    
      }
    }
    
  public:
    motors_c() {
      pinMode( L_PWM_PIN, OUTPUT );
      pinMode( L_DIR_PIN, OUTPUT );
      pinMode( R_PWM_PIN, OUTPUT );
      pinMode( R_DIR_PIN, OUTPUT );
    } 

    void move(int power) {
      // left motor
      setMotorPower(L_PWM_PIN, power);
      // right motor
      setMotorPower(R_PWM_PIN, power);
    }

    void move2speed(int pLeft, int pRight) {
      // left motor
      setMotorPower(L_PWM_PIN, pLeft);
      // right motor
      setMotorPower(R_PWM_PIN, pRight);
    }
    
    void turn(int power, bool dir) {
      if (dir) {
        // use left as pivot
        // left motor
        setMotorPower(L_PWM_PIN, 0);
        // right motor
        setMotorPower(R_PWM_PIN, power);
      }
      else {
        // use right as pivot
        // left motor
        setMotorPower(L_PWM_PIN, power);
        // right motor
        setMotorPower(R_PWM_PIN, 0);
      }
    }

    void turnOnSpot(int power, bool dir) {
      if (dir) {
        // towards left
        // left motor
        setMotorPower(L_PWM_PIN, -power);
        // right motor
        setMotorPower(R_PWM_PIN, power);
      }
      else {
        // towards right
        // left motor
        setMotorPower(L_PWM_PIN, power);
        // right motor
        setMotorPower(R_PWM_PIN, -power);
      }
    }    
};

#endif
