#ifndef _LINESENSOR_H
#define _LINESENSOR_H

#define LINE_LEFT_PIN A2
#define LINE_CENTRE_PIN A3
#define LINE_RIGHT_PIN  A4

// int val(), float norm(), float lim(), 
// boolean onLine(), boolean notOnLine(), cal()
class lineSensor_c {

  private:
    int pin;
    int minVal = 1024.0;
    int maxVal = 0.0;
    // default value if not calibrated
    float limit = 512.0; //more is dark, less is light
  
  public:
    // Constructor, accepts a pin number as
    // argument and sets this as input.
    lineSensor_c( int which_pin ) {
       // Record which pin we used.
       pin = which_pin;
       // Set this pin to input.
       pinMode( pin, INPUT );
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

#endif
