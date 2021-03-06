// code easier to read.
#define YELLOW LED_BUILTIN
#define RED LED_BUILTIN_RX
#define GREEN LED_BUILTIN_TX
#define OFF LOW
#define ON HIGH

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
      int offset = 0;
      if ( power < 0 ) { offset = -offset; }
      setMotorPower(L_PWM_PIN, power+offset);
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

class lineSensor_c {
  private:
    int pin;
    float minVal = 1000.0;
    float maxVal = 0.0;
    // default value if not calibrated
    float limit = 500.0; //more is dark, less is light
  
  public:  
    // Constructor, accepts a pin number as
    // argument and sets this as input
    lineSensor_c(int _pin) {
       pin = _pin; // Record which pin we used
       pinMode( pin, INPUT ); // Set this pin to input
    };

    // return sensor output
    float val() {
      return analogRead( pin );
    }

    // return normilised sensor output
    float norm() {
      float value = analogRead( pin );
      if ( value > maxVal ) { value = maxVal; }
      if ( value < minVal ) { value = minVal; }
      return (value - minVal)/(maxVal - minVal);
    }

    // return sensor limit
    float lim() {
      return limit;
    }
    
    // Write your calibration routine here
    // to remove bias offset
    void cal() {
      float value;
      value = analogRead( pin ); // Do an analogRead
      if ( value > maxVal ) { maxVal = value; }
      if ( value < minVal ) { minVal = value; }
      limit = (maxVal + minVal)/2;
    }
    
    // Write a routine here to check if your
    // sensor is on a line (true or false)
    boolean notOnLine() {
      float value = analogRead( pin ); // Do an analogRead
      if ( value > limit ) { return false; }
      else { return true; }
    }
};

//instantiate classes
motors_c m;
lineSensor_c ll(LINE_LEFT_PIN);
lineSensor_c lc(LINE_CENTRE_PIN);
lineSensor_c lr(LINE_RIGHT_PIN);

// global variables
unsigned int count = 0;
unsigned int nonce = 0;
unsigned int calTime;
bool plot_verbose;
bool msg_verbose;

void blink(int col) {
  if (col == YELLOW) {
    digitalWrite(col, ON);
    delay(50);
    digitalWrite(col, OFF);
  }
  else {
    digitalWrite(col, OFF);
    delay(50);
    digitalWrite(col, ON);
  }
}

void buzz() {
  analogWrite(6, 100);
  delay(50);
  analogWrite(6, 0);
}

void BangBang() {
  // variables for slow and fast speeds
  int pLow = 20;
  int pHigh = 25;
  // bang-bang conditions
  if ( lc.notOnLine() ) {
    // centre not on line, check left and right 
    if ( ll.notOnLine() ) {
      // left not on line either, check right
      if ( lr.notOnLine() ) {
        // nothing on line
        // blink green
        blink(GREEN);
        // turn left 90 degrees
        m.turnOnSpot(pLow, LEFT);
        delay(2000);
        // and check if there is a line
        if ( ll.notOnLine() && lc.notOnLine() && lr.notOnLine() ) {
          // not on line turn right 180 degrees
          // and check if there is a line
          m.turnOnSpot(pLow, RIGHT);
          delay(4000);
          // heck if there is a line
          if ( ll.notOnLine() && lc.notOnLine() && lr.notOnLine() ) {
            // not on line
            // turn left 90 degrees
            m.turnOnSpot(pLow, LEFT);
            delay(2000);
            // and just move straight slowly
            m.move(pLow);
            delay(1000);
          }
        }
      }
      else {
        // only right on line
        // turn fast using right as pivot
        m.turn(pHigh, RIGHT);
      }
    }
    else if ( lr.notOnLine() ) {
      // left on line, centre not on line 
      // => right not on line either 
      // turn fast using left as pivot
      m.turn(pHigh, LEFT);
    }
  }
  else {
    // centre is on line, check left and right
    // (not possible to only have centre on line)
    if ( ll.notOnLine() ) { 
      // left not on line
      // turn slowly using right as pivot
      m.turn(pLow, RIGHT);
    }
    else if ( lr.notOnLine() ) { 
      // right not on line
      // turn slowly using left as pivot
      m.turn(pLow, LEFT);
    }
    else {
      // all 3 on line, move straight fast
      m.move(pHigh);
    }
  }
}

// Setup, only runs once when powered on.
void setup() {
  // variables for output
  plot_verbose = false;
  msg_verbose = false;
  // set leds as output
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  // time for calibration
  calTime = 40;
  // Start up the Serial for debugging
  Serial.begin(9600);
  delay(1000);
  // Print reset so we can catch any reset error
  Serial.println(" ***Reset*** ");
}

// Remmeber, loop is called again and again
void loop() 
{
  // initial back and forth for calibration
  // this will happen when count is at zero & calTime
  if ( count % calTime == 0 ) { m.move(20); }
  // this will happen when count is at half calTime
  else if ( count % (calTime/2) == 0 ) { m.move(-20); }

  // calibration procedure
  if ( count <= calTime ) {
    // red led
    blink(RED);
    // half calibration done, play sound
    if ( count == calTime / 2 ) { buzz(); }
    // calibration done, play sound
    if ( count == calTime ) { buzz(); }
    // run callibration procedures for each
    ll.cal();
    lc.cal();
    lr.cal();
    count++;
  }
  // move forward for a bit to connect to line
  else if ( count < calTime + 15 ) { 
    blink(YELLOW);
    m.move(25); 
    count++;
  }
  // normal functionality
  else {
    // using bang-bang method to move
    BangBang();
    // OUTPUT METHODS
    if (plot_verbose) {
      Serial.print( ll.norm() );
      Serial.print( ", " );
      Serial.print( lc.norm() );
      Serial.print( ", " );
      Serial.print( lr.norm() );
      Serial.print( "\n" );
    }
    if (msg_verbose) {
      Serial.println(nonce); 
      nonce++;
      if ( ll.norm() > 0.5 ) { Serial.println("left over line"); }
      if ( lc.norm() > 0.5 ) { Serial.println("centre over line"); }
      if ( lr.norm() > 0.5 ) { Serial.println("right over line"); }
    }
  }
  // short delay
  delay(50);
}
