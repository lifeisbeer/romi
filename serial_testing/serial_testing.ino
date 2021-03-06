unsigned int count = 0;
bool mode = false;

void setup() {
  // put your setup code here, to run once:
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  delay(1000);
  Serial.println("***RESET***");
}

void flash_led() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    char inChar = Serial.read();
    if (inChar == 'l') {
      flash_led();
      count++;
      Serial.println(count);
    }
    else if (inChar == 'r') {
      mode = !mode;
      Serial.println("toggle");
    }
  }
  else if (mode) {
    flash_led();
    count++;
    Serial.println(count);
  }
}
