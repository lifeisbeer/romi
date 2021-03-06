#include <math.h>

float a;
float b;
float c;

void setup() {

  //Start a serial connection
  Serial.begin( 9600 );

  // Wait for stable connection, report reset.
  delay(1000);
  Serial.println("***RESET***");

  a = TWO_PI;
  b = a;
  c = b;

}

void loop() {

  a += 0.01;
  b = sin( a * 20 );
  c = cos( a * 10 );

  Serial.print( a );
  Serial.print( ","); 
  Serial.print( b );
  Serial.print( ",");
  Serial.print( c );

  Serial.print( "\n" );   // Finish with a newline
  //Serial.println( "" ); // either of these two work.

  delay(50);

}
