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

    void move2speed(int left, int right) {
      // left motor
      setMotorPower(L_PWM_PIN, left);
      // right motor
      setMotorPower(R_PWM_PIN, right);
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
    float minVal = 1024.0;
    float maxVal = 0.0;
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
    float val() {
      return analogRead( pin );
    }

    // return normilised sensor output
    float norm() {
      float value = analogRead( pin );
      if ( value > maxVal ) { value = maxVal; }
      if ( value < minVal ) { value = minVal; }
      return 1024*(value - minVal)/(maxVal - minVal);
    }

    // return sensor limit
    float lim() {
      return limit;
    }
    
    // Write your calibration routine here
    // to remove bias offset
    void cal() {
      float value = analogRead( pin ); // Do an analogRead
      if ( value > maxVal ) { maxVal = value; }
      if ( value < minVal ) { minVal = value; }
      limit = (maxVal + minVal)/2.0;
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
bool norm_verbose;
bool m_verbose;

void blink(int col) {
  if (col == YELLOW) {
    digitalWrite(col, ON);
    delay(10);
    digitalWrite(col, OFF);
  }
  else {
    digitalWrite(col, OFF);
    delay(10);
    digitalWrite(col, ON);
  }
}

void buzz() {
  analogWrite(6, 100);
  delay(50);
  analogWrite(6, 0);
}

float weighted() {
  // if nothing on line => M = 0
  // if only left on line => M = 1
  // if only right on line => M = -1
  // if left and centre on line => M = 0.5
  // if right and centre on line => M = -0.5
  // if all 3 on line => M = 0
  
  // weighted line sensng
  float M = 0.0;
  float Ir = lr.norm();
  float Ic = lc.norm();
  float Il = ll.norm();
  float Itotal = Ir + Ic + Il;
  if ( Itotal == 0 ) {
    M = 0.0;
  }
  else {
    float Pr = Ir/Itotal;
    float Pl = Il/Itotal;
    M = Pl-Pr;
  }
  return M;
}

void BangBang() {
  // variables for slow and fast speeds
  int p = 20;
  int pM = 40;
  int half = 100;
  int full = 20;
  int count = 0;
  int single = 100;
  float M = weighted();

  if ( M < 0.1 || ( M < 0 && M > -0.1 ) ) {
    // either all on line or none on line
    if ( lc.notOnLine() ) {
      // none on line
      count = 0;
      while ( count < half && lc.notOnLine() ) {
        // blink green
        blink(GREEN);
        // turn left
        m.turnOnSpot(p, LEFT);
        delay(single);
        count++;
      }
      // and check if there is a line
      if ( ll.notOnLine() && lc.notOnLine() && lr.notOnLine() ) {
        // not on line turn right 180 degrees
        count = 0;
        while ( count < full && lc.notOnLine() ) {
          // blink green
          blink(GREEN);
          // turn right
          m.turnOnSpot(p, RIGHT);
          delay(single);
          count++;
        }
        // check if there is a line
        if ( ll.notOnLine() && lc.notOnLine() && lr.notOnLine() ) {
          // not on line
          // blink green
          blink(GREEN);
          // turn left 90 degrees
          m.turnOnSpot(p, LEFT);
          delay(half*single);
          // and just move straight slowly
          m.move(p);
          delay(1000);
        }
      }
    }
    else {
      // all on line
      // move straight slowly
      m.move(p);
    }
  }
  else if ( M < 0 ) {
    if( M*pM*-1 > 30) {
      m.move2speed(30, -30);
    }
    else if ( M*pM*-1 < 15 ) {
      m.move2speed(15, -15);
    }
    else {
      m.move2speed(round(M*pM*-1), round(M*pM));
    }
  }
  else {
    if( M*pM > 30) {
      m.move2speed(-30, 30);
    }
    else if ( M*pM < 15 ) {
      m.move2speed(-15, 15);
    }
    else {
      m.move2speed(round(M*pM*-1), round(M*pM));
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
    m.move(35); 
    count++;
  }
  // normal functionality
  else {
    // using weighted method to move
    BangBang();
    // OUTPUT METHODS
    if (plot_verbose) {
      Serial.print( ll.norm() );
      Serial.print( ", " );
      Serial.print( ll.val() );
      Serial.print( ", " );
      Serial.print( lc.norm() );
      Serial.print( ", " );
      Serial.print( lc.val() );
      Serial.print( ", " );
      Serial.print( lr.norm() );
      Serial.print( ", " );
      Serial.print( lr.val() );
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
