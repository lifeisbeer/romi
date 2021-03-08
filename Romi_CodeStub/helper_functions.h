#define YELLOW LED_BUILTIN
#define RED LED_BUILTIN_RX
#define GREEN LED_BUILTIN_TX
#define OFF LOW
#define ON HIGH
#define BUZZ 6

// instantiate classes
motors_c m;
lineSensor_c ll(LINE_LEFT_PIN);
lineSensor_c lc(LINE_CENTRE_PIN);
lineSensor_c lr(LINE_RIGHT_PIN);
// global variables
int pL;
int pR;

void adjSpeeds(int target) {
  if ( count_left > count_right ) {
    pL = target*1.05;
    pR = target*0.95;
  } else if ( count_right > count_left ) {
    pL = target*0.95;
    pR = target*1.05;
  } else {
    pL = target;
    pR = target;
  }
}

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
