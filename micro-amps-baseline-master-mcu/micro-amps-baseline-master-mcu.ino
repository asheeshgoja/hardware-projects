#define ARDUINO

#include <String.h>
#include <Wire.h>

#include "HCSR04.h"
#include "Vcc.h"
#include <SoftwareSerial.h>

#include <avr/sleep.h>
#include <avr/power.h>

#include <TimerOne.h>


#include <ADXL345.h>
ADXL345 adxl;
#define ADXL345_DEVICE (0x53)    // ADXL345 device address
#define ADXL345_TO_READ (6)

const float VccMin   = 0.0;           // Minimum expected Vcc level, in Volts.
const float VccMax   = 5.0;           // Maximum expected Vcc level, in Volts.
const float VccCorrection = 1.0 / 1.0; // Measured Vcc by multimeter divided by reported Vcc
Vcc vcc(VccCorrection);

int maximumRange = 25 ;// cm; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance;

//const int RED_LED = 10;
//const int GREEN_LED = 11;
//const int BLUE_LED = 12;
const int INFRARED_PIN = 3;
const int SIM_800_RST_PIN = 9;
const int ECHO_PIN = 14;// Echo Pin
const int TRIGGGER_PIN = 15; // Trigger Pin
const int WAKE_UP_INT_PIN = 2;//sleep mode wakeup interrrupt

const int INFRARED_MOSFET_PIN = 4;
//const int INFRARED_MOSFET_PIN = 5;
//const int ACCELO_MOSFET_PIN = 6;
const int SLAVE_MCU_P_CHANNEL_MOSFET_PIN = 10;


volatile bool pollSlaveMcuForStatus = false;

HCSR04 UltrasonicSensor(TRIGGGER_PIN, ECHO_PIN);

SoftwareSerial slaveMcuSerial(11, 12);

int slaveTimeStartMillis = 0;

void setup() {

  pinMode(13, OUTPUT);
  pinMode(11, INPUT);
  pinMode(12, OUTPUT );
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);


  //pinMode(SLAVE_MCU_MOSFET_PIN, INPUT);
  pinMode(SLAVE_MCU_P_CHANNEL_MOSFET_PIN, OUTPUT );


  pinMode(TRIGGGER_PIN, OUTPUT);
  //  pinMode(INFRARED_POWER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(INFRARED_PIN, INPUT );
  //pinMode(GREEN_LED, OUTPUT);
  // pinMode(BLUE_LED, OUTPUT);
  //pinMode(RED_LED, OUTPUT);
  pinMode(WAKE_UP_INT_PIN, INPUT);



  pinMode(INFRARED_MOSFET_PIN, OUTPUT);
  //pinMode(ACCELO_MOSFET_PIN, OUTPUT);


  //  digitalWrite(GREEN_LED, HIGH);
  // digitalWrite(RED_LED, HIGH);
  //digitalWrite(BLUE_LED, HIGH);
  //  digitalWrite(RED_LED, LOW);
  digitalWrite(WAKE_UP_INT_PIN, HIGH);


  digitalWrite(INFRARED_MOSFET_PIN, HIGH);
  //digitalWrite(ACCELO_MOSFET_PIN, LOW);


  digitalWrite(SLAVE_MCU_P_CHANNEL_MOSFET_PIN, LOW );
  //digitalWrite(SLAVE_MCU_MOSFET_PIN, LOW);



  Serial.begin(9600);
  slaveMcuSerial.begin(9600);



  Serial.println("Start");
  //return;
  //Wire.begin();

  //setupSlaveMcuI2CBus();

  setupAccelerometer();

  Serial.println("setup complete");
}

int cnt = 0;
void loop()
{

  delay(3000);

  digitalWrite(13, HIGH);
  delay(2000);
  digitalWrite(13, LOW);


  enterSleep();

  return;


  //Serial.println("loop begin");
  bool pkg_drop_event = false;

  int ir_val = analogRead(INFRARED_PIN);
  Serial.println("ir val = " + String(ir_val) );

  int x = displayOrientationAndGetZaxis();

  if ( ir_val > 600 )
  {
    Serial.println("Infared sensor detected package drop");
    //delay(3000);
    pkg_drop_event = true;
  }

  //    if ( isThereAPackageInFrontOfMe() )
  //    {
  //      //Serial.println("Ultrasonic sensor detected package drop");
  //      pkg_drop_event = true;
  //    }

  if ( cnt == 500 )
  {
    cnt = 0;
    if (getBatteryLevel() > 80)
    {
      //      flashAllLeds();
      Serial.println("Health check ok");
    }

    if (getBatteryLevel() < 80 )
    {
      // flashRedLed();
    }
  }



  if ( pkg_drop_event )
  {

    digitalWrite(SLAVE_MCU_P_CHANNEL_MOSFET_PIN, LOW);
    pollSlaveMcuForStatus = false;
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    delay(2000);

    enterSleep();
  }

  // noInterrupts();
  //if ( pollSlaveMcuForStatus )
  //{
  //requestDataFromSlaveMcu();
  // pollSlaveMcuForStatus = false;
  //}
  //  interrupts();

  // delay(10);

  // requestDataFromSlaveMcuOverSerial();

  checkSlaveStatus();


  //  if (digitalRead(WAKE_UP_INT_PIN) == HIGH)
  //  {
  //    delay(2000);
  //    Serial.println("Wakup pin low, entering sleep....");
  //    enterSleep();
  //  }

}


void checkSlaveStatus()
{

  if (pollSlaveMcuForStatus)
  {
    String data;

    while (slaveMcuSerial.available() > 0) {
      char inByte = slaveMcuSerial.read();
      //Serial.write(inByte);
      data += String(inByte);
    }

    if (data.length() > 0)
      Serial.println(data);


    if ( data.indexOf("SLAVE-SIGNAL:SETUP-COMPLETE") != -1 )
    {
      Serial.println("Slave setup cmd recieved, initiating messsage uplink..");

      delay(100);
      slaveMcuSerial.println("MASTER-CMD:" + String("5,5.5") );
      delay(100);
      slaveMcuSerial.println("MASTER-CMD:" + String("5,5.5") );


    }

    if ( data.indexOf("SLAVE-SIGNAL:PROCESSIG-COMPLETE") != -1 )
    {
      Serial.println("Shutting down slave");
      digitalWrite(SLAVE_MCU_P_CHANNEL_MOSFET_PIN, LOW);

      int slaveTimeEndMillis = millis();

      Serial.println("TIme elpased in slave mcu = " + String(slaveTimeEndMillis - slaveTimeStartMillis));
      pollSlaveMcuForStatus = false;
      digitalWrite(11, LOW);
      digitalWrite(12, LOW);
      delay(2000);

      enterSleep();
    }
  }

}

void onSlavePollTimerEvent()
{
  //return;
  pollSlaveMcuForStatus = true;

}


void triggerSlaveMcu()
{
  //  Wire.beginTransmission(8); // transmit to slave mcu on address 8
  // Wire.write(10);              // sends x
  //  Wire.endTransmission();    // stop transmitting


  Serial.println("slave mcu powewred on.." );

  digitalWrite(SLAVE_MCU_P_CHANNEL_MOSFET_PIN, HIGH);

  slaveTimeStartMillis = millis();
  //  digitalWrite(SLAVE_MCU_MOSFET_PIN, HIGH);LOW


  //, digitalRead( SLAVE_MCU_MOSFET_PIN ) ^ 1 );

  //delay(100);
  pollSlaveMcuForStatus = true;

  // Timer1.initialize(500000); // 8 mhz is twice this time
  // Timer1.attachInterrupt(onSlavePollTimerEvent); // requestDataFromSlaveMcu to run every 1 seconds
}

bool isThereAPackageInFrontOfMe()
{
  int distance = senseDistance();

  Serial.println("diatance = " + String(distance) );


  if (distance >= maximumRange || distance <= minimumRange)
  {
    return false;
  }
  else
  {
    //Serial.println("Package dected in a UPS drop box !");
    return true;
  }

}

int senseDistance()
{
  float distance = UltrasonicSensor.ping(58, 615); // Formula: uS / 58 = centimeters ; 500 = offset HC-SR0; 615 = offset US-015
  return distance;
}


int getBatteryLevel()
{
  float v = vcc.Read_Volts();
  Serial.print("VCC = ");
  Serial.print(v);
  Serial.println(" Volts");

  float p = vcc.Read_Perc(VccMin, VccMax);
  Serial.print("VCC = ");
  Serial.print(p);
  Serial.println(" %");

  return p;
}

void setupAccelerometer()
{
  adxl.powerOn();
}


int displayOrientationAndGetZaxis()
{
  int x, y, z;
  adxl.readAccel(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

  char str[512];
  sprintf(str, "{\"coordinates\":\"%d,%d,%d\"}", x, y, z);
  Serial.println(str);
  return x;
}



void pin2Interrupt(void)
{
}

void enterSleep(void)
{


  Serial.println("master mcu entering sleep mode");
  digitalWrite(INFRARED_MOSFET_PIN, LOW);
  delay(1000);

  /* Setup pin2 as an interrupt and attach handler. */
  attachInterrupt(0, pin2Interrupt, FALLING);
  delay(100);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  sleep_enable();

  sleep_mode();

  /* The program will continue from here. */

  /* First thing to do is disable sleep. */
  sleep_disable();
  detachInterrupt(0);

  digitalWrite(13, HIGH);
  digitalWrite(INFRARED_MOSFET_PIN, HIGH);
  delay(100);
  setupAccelerometer();
  digitalWrite(13, LOW);


}


