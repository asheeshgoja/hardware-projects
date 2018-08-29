// **** INCLUDES *****
//#define ARDUINO
#include "LowPower.h"
//#include <ADXL345.h>
//ADXL345 adxl;
// Use pin 2 as wake up pin
const int wakeUpPin = 2;
//const int RED_LED = 10;
const int GREEN_LED = 11;
//const int BLUE_LED = 12;


void wakeUp()
{
  // Just a handler for the pin interrupt.

}

void setup()
{

  Serial.begin(9600);

  // Configure wake up pin as input.
  // This will consumes few uA of current.
  pinMode(GREEN_LED, OUTPUT);
  //  pinMode(BLUE_LED, OUTPUT);
  // pinMode(RED_LED, OUTPUT);

  pinMode(wakeUpPin, INPUT);
  digitalWrite(wakeUpPin, HIGH);

  digitalWrite(GREEN_LED, HIGH);
  // digitalWrite(RED_LED, HIGH);
  // digitalWrite(BLUE_LED, HIGH);

  //setupAccelerometer();
  Serial.println("start");

  delay(1000);

}

void loop()
{

  // Allow wake up pin to trigger interrupt on low.
  attachInterrupt(0, wakeUp, LOW);

  // Enter power down state with ADC and BOD module disabled.
  // Wake up when wake up pin is low.
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  // Disable external pin interrupt on wake up pin.
  detachInterrupt(0);

  // Do something here
  // Example: Read sensor, data logging, data transmission.




  digitalWrite(GREEN_LED, LOW);
  delay(5000);
  digitalWrite(GREEN_LED, HIGH);
  Serial.println("awake");
  //displayOrientation();

}

//void setupAccelerometer()
//{
//  adxl.powerOn();
//
//  //set activity/ inactivity thresholds (0-255)
//  adxl.setActivityThreshold(75); //62.5mg per increment
//  adxl.setInactivityThreshold(75); //62.5mg per increment
//  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
//
//  //look of activity movement on this axes - 1 == on; 0 == off
//  adxl.setActivityX(1);
//  adxl.setActivityY(1);
//  adxl.setActivityZ(1);
//
//  //look of inactivity movement on this axes - 1 == on; 0 == off
//  adxl.setInactivityX(1);
//  adxl.setInactivityY(1);
//  adxl.setInactivityZ(1);
//
//  //look of tap movement on this axes - 1 == on; 0 == off
//  adxl.setTapDetectionOnX(1);
//  adxl.setTapDetectionOnY(1);
//  adxl.setTapDetectionOnZ(1);
//
//  //set values for what is a tap, and what is a double tap (0-255)
//  adxl.setTapThreshold(5000); //62.5mg per increment
//  adxl.setTapDuration(15); //625μs per increment
//  adxl.setDoubleTapLatency(80); //1.25ms per increment
//  adxl.setDoubleTapWindow(200); //1.25ms per increment
//
//  //set values for what is considered freefall (0-255)
//  adxl.setFreeFallThreshold(0x05); //(5 - 9) recommended - 62.5mg per increment
//  adxl.setFreeFallDuration(0x5); //(20 - 70) recommended - 5ms per increment
//
//  //setting all interupts to take place on int pin 1
//  //I had issues with int pin 2, was unable to reset it
//  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
//  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
//  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
//  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
//  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );
//
//  //register interupt actions - 1 == on; 0 == off
//  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
//  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
//  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
//  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
//  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);
//
//}


void displayOrientation()
{
  int x, y, z;
//  adxl.readAccel(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

  char str[512];
  sprintf(str, "{\"coordinates\":\"%d,%d,%d\"}", x, y, z);
  Serial.println(str);
}

