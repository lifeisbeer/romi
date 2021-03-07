#define YELLOW LED_BUILTIN
#define RED LED_BUILTIN_RX
#define GREEN LED_BUILTIN_TX
#define OFF LOW
#define ON HIGH

#define BUZZ 6

#define L_PWM_PIN 10
#define L_DIR_PIN 16
#define R_PWM_PIN  9
#define R_DIR_PIN 15
#define DIR_BWD HIGH // HIGH: backwards
#define DIR_FWD LOW // LOW: forward
#define LEFT true
#define RIGHT false

#define LINE_LEFT_PIN A2
#define LINE_CENTRE_PIN A3
#define LINE_RIGHT_PIN  A4

// move(int power), move2speed(int pLeft, int pRight),
// turn(int power, bool dir), turnOnSpot(int power, bool dir)
class motors_c {
  private:
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

// int val(), float norm(), float lim(), boolean onLine(), cal()
class lineSensor_c {
  private:
    int pin;
    int minVal = 1024.0;
    int maxVal = 0.0;
    // default value if not calibrated
    float limit = 512.0; //more is dark, less is light
  
  public:  
    // Constructor, accepts a pin number as
    // argument and sets this as input
    lineSensor_c(int _pin) {
       pin = _pin; // Record which pin we used
       pinMode( pin, INPUT ); // Set this pin to input
    };

    // return sensor output
    int val() {
      return analogRead( pin );
    }

    // return normilised sensor output
    float norm() {
      float value = analogRead( pin );
      if ( value > maxVal ) { value = maxVal; }
      if ( value < minVal ) { value = minVal; }
      return float(value - minVal)/float(maxVal - minVal);
    }

    // return sensor limit
    float lim() {
      return limit;
    }
    
    // Write your calibration routine here
    // to remove bias offset
    void cal() {
      int value = analogRead( pin ); // Do an analogRead
      if ( value > maxVal ) { maxVal = value; }
      if ( value < minVal ) { minVal = value; }
      limit = float(maxVal + minVal)/2.0;
    }
    
    // Write a routine here to check if your
    // sensor is on a line (true or false)
    boolean onLine() {
      int value = analogRead( pin ); // Do an analogRead
      if ( value > limit ) { return true; }
      else { return false; }
    }

    boolean notOnLine() {
      return !onLine();
    }
};

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

// global variables
unsigned int state = 0;
unsigned int pLow = 20;
unsigned int pHigh = 30;
unsigned int count = 0;
unsigned int calTime = 100;
bool plot_verbose = false;
unsigned int Lturn = 2500;
unsigned int Rturn = 4500;

// Setup, only runs once when powered on.
void setup() {
  // set leds as output
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  // Start up the Serial for debugging
  Serial.begin(9600);
  delay(1000);
  // Print reset so we can catch any reset error
  Serial.println(" ***Reset*** ");
}

// Remmeber, loop is called again and again
void loop() 
{
  if ( state == 0 ) {
    // callibration state
    m.move(pLow); 
    // run callibration procedures for each
    ll.cal();
    lc.cal();
    lr.cal();
    count++;
    if ( count >= calTime ) {
      buzz_on();
      state = 1;
    }
  }
  else if ( state == 1 ) {
    // find line
    led_on(YELLOW);
    if ( ll.onLine() || lc.onLine() || lr.onLine() ) {
      buzz_on();
      state = 2;
    }
    else {
      // move straight until you find line
      m.move(pHigh);
    }
  }
  else if ( state == 2 ) {
    // found line, connect
    led_on(GREEN);
    // turn left 90 degrees
    m.turnOnSpot(pLow, LEFT);
    delay(Lturn);
    buzz_on();
    state = 3;
  }
  else if ( state == 3 ) {
    led_on(RED);
    // bang-bang conditions
    if ( lc.notOnLine() ) {
      // centre not on line, check left and right 
      if ( ll.notOnLine() ) {
        // centre and left not on line, check right
        if ( lr.notOnLine() ) {
          // nothing on line
          buzz_on();
          state = 4;
        }
        else {
          // only right on line
          buzz_on();
          state = 7;
        }
      }
      else if ( lr.notOnLine() ) {
        // only left on line 
        buzz_on();
        state = 8;
      }
    }
    else {
      // centre is on line, check left and right
      // (not possible to only have centre on line)
      if ( ll.notOnLine() ) { 
        // left not on line
        // turn slowly using right as pivot
        buzz_on();
        state = 7;
      }
      else if ( lr.notOnLine() ) { 
        // right not on line
        // turn slowly using left as pivot
        buzz_on();
        state = 8;
      }
      else {
        // all 3 on line, move straight
        buzz_on();
        state = 9;
      }
    }
  }
  else if ( state = 4 ) {
    // turn left 90 degrees
    m.turnOnSpot(pLow, LEFT);
    delay(Lturn);
    // and check if there is a line
    if ( ll.notOnLine() && lc.notOnLine() && lr.notOnLine() ) {
      buzz_on();
      state = 5;
    }
    else {
      buzz_on();
      state = 3;
    }
  }
  else if ( state = 5 ) 
  {
    // turn right 180 degrees
    m.turnOnSpot(pLow, RIGHT);
    delay(Rturn);
    // check if there is a line
    if ( ll.notOnLine() && lc.notOnLine() && lr.notOnLine() ) {
      buzz_on();
      state = 6;
    }
    else {
      buzz_on();
      state = 3;
    }
  }
  else if ( state = 6 ) {
    // turn left 90 degrees
    m.turnOnSpot(pLow, LEFT);
    delay(Lturn);
    // and just move straight slowly
    m.move(pLow);
    delay(1000);
    buzz_on();
    state = 3;
  }
  else if ( state = 7 ) {
    // turn using right as pivot
    m.turn(pLow, RIGHT);
    buzz_on();
    state = 3;
  }
  else if ( state = 8 ) {
    // turn using left as pivot
    m.turn(pHigh, LEFT);
    buzz_on();
    state = 3;
  }
  else if ( state = 9 ) {
    // all 3 on line, move straight
    m.move(pLow);
    buzz_on();
    state = 3;
  }
  
  if (plot_verbose) {
    Serial.print( ll.norm() );
    Serial.print( ", " );
    Serial.print( ll.val() );
    Serial.print( ", " );
    Serial.print( ll.lim() );
    Serial.print( ", " );
    Serial.print( lc.norm() );
    Serial.print( ", " );
    Serial.print( lc.val() );
    Serial.print( ", " );
    Serial.print( lc.lim() );
    Serial.print( ", " );
    Serial.print( lr.norm() );
    Serial.print( ", " );
    Serial.print( lr.val() );
    Serial.print( ", " );
    Serial.print( lr.lim() );
    Serial.print( "\n" );
  }
  // short delay
  delay(25);
  // turn leds and buzzer off
  buzz_off();
  led_off(RED);
  led_off(GREEN);
  led_off(YELLOW);
  // short delay
  delay(25);
}
