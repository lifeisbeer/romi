// code easier to read.
#define L_PWM_PIN 10
#define L_DIR_PIN 16
#define R_PWM_PIN  9
#define R_DIR_PIN 15
#define DIR_BWD HIGH // HIGH: backwards
#define DIR_FWD LOW // LOW: forward
#define LEFT true
#define RIGHT false

int power, mTimer, tTimer;
// FORWARD: 
// 10 is the min power for some movement (on desk)
// left wheel turns at: 11
// right wheel turns at: 8
// BACKWARD: 
// 12 is the min power for some movement (on desk)
// left wheel turns at: 12
// right wheel turns at: 11
//Deadband: 1-9 (FWD), 1-11 (BWD)

// Setup, only runs once when powered on.
void setup() {
  // Set our motor driver pins as outputs.
  pinMode( L_PWM_PIN, OUTPUT );
  pinMode( L_DIR_PIN, OUTPUT );
  pinMode( R_PWM_PIN, OUTPUT );
  pinMode( R_DIR_PIN, OUTPUT );
  // Set timers
  mTimer = 0;
  tTimer = 0;
  // Start up the Serial for debugging.
  Serial.begin(9600);
  delay(1000);
  // Print reset so we can catch any reset error.
  Serial.println(" ***Reset*** ");
}

// Accepts two integers (motor and power) 
// and sets the power for that mortor 
void setMotorPower(int motor, int power) {
  if ( motor == L_PWM_PIN || motor == R_PWM_PIN ) {
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
  else {
    // error wrong motor given
    Serial.println(" ***Wrong motor passed to function*** ");
  }
}

void move(int power) {
  // left motor
  setMotorPower(L_PWM_PIN, power);
  // right motor
  setMotorPower(R_PWM_PIN, power);
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

// put your main code here, to run repeatedly:
void loop() {
  power = 50;
  if (mTimer < 2) { 
    // move forward
    move(power);
    mTimer++;
  }
  else if (tTimer < 6) {
    // turn left (use left as pivot)
    turn(power, LEFT);
    tTimer++;
  }
  else {
    move(power);
  }
  // Brief pause
  delay(250);
}
