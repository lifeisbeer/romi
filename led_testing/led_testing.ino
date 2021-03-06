#define YELLOW LED_BUILTIN
#define RED LED_BUILTIN_RX
#define GREEN LED_BUILTIN_TX
#define OFF LOW
#define ON HIGH

void blink(int col) {
  digitalWrite(col, ON);   // turn the LED on (HIGH is the voltage level)
  //delay(50);                 // wait
  //digitalWrite(col, OFF);    // turn the LED off by making the voltage LOW
}

void leds_off() {
  digitalWrite(YELLOW, OFF);
  digitalWrite(RED, ON);
  digitalWrite(GREEN, ON);
}

void leds_on() {
  digitalWrite(YELLOW, ON);
  digitalWrite(RED, OFF);
  digitalWrite(GREEN, OFF);
}

int count = 0;

void setup() {
  // set leds as output
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (count == 0) { 
    leds_on();
    count++;
  }
  else if (count == 1) { 
    leds_off();
    count++;
  }
  delay(3000);
}
