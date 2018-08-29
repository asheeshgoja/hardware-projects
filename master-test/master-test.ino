#define ARDUINO

#include <String.h>
#include <Wire.h>

#include "HCSR04.h"
#include "Vcc.h"
#include <SoftwareSerial.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>


#include <TimerOne.h>
#include <EEPROM.h>



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
const int VOLTAGE_BOOSTER_MOSFET_PIN = 5;



HCSR04 UltrasonicSensor(TRIGGGER_PIN, ECHO_PIN);

SoftwareSerial slaveMcuSerial(11, 12);



struct DropCounterStruct {
  int packageDetectedCount = 0;
  int dropBropOpenCount = 0;
};

DropCounterStruct dropBoxCounters;


enum EnumSlaveMcuState {
  OFF,
  ON,
  REQUIRES_ACTIVATION,
};

volatile EnumSlaveMcuState SlaveMcuState = OFF;
int slaveMcuTimerDuration = 10; // seconds

int slaveTimeStartMillis = 0;
volatile int timerCounter = 0;
volatile int wdt_counter = 0;
volatile bool disable_wdt = false;


volatile int slaveMcuActivationCounter = 0;
int millisAtLastDoorClose = 0;
bool doorClosed = false;
bool displayCoordinateAndIrVals = false ;
bool doorClosedForMoreThatExpiryTime = true;
int millisAtLastDetectionOfPackageDrop = 0;



void setup() {

  pinMode(13, OUTPUT);
  pinMode(11, INPUT);
  pinMode(12, OUTPUT );
  pinMode(VOLTAGE_BOOSTER_MOSFET_PIN, OUTPUT );
  pinMode(SLAVE_MCU_P_CHANNEL_MOSFET_PIN, OUTPUT );

  
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(VOLTAGE_BOOSTER_MOSFET_PIN, HIGH);
// digitalWrite(SLAVE_MCU_P_CHANNEL_MOSFET_PIN, LOW );

 
//return;

  pinMode(TRIGGGER_PIN, OUTPUT);

  pinMode(ECHO_PIN, INPUT);
  pinMode(INFRARED_PIN, INPUT );

  pinMode(WAKE_UP_INT_PIN, INPUT);



  pinMode(INFRARED_MOSFET_PIN, OUTPUT);

  digitalWrite(WAKE_UP_INT_PIN, HIGH);


  digitalWrite(INFRARED_MOSFET_PIN, HIGH);

  digitalWrite(SLAVE_MCU_P_CHANNEL_MOSFET_PIN, LOW );

  Serial.begin(9600);
  slaveMcuSerial.begin(9600);

  Serial.println("Start master mcu setup");
  setupAccelerometer();
  //  Timer1.initialize( (slaveMcuTimerDuration / 2) * 1000000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  //  Timer1.attachInterrupt( activateSlaveMcu ); // attach the service routine here

  setupWTD();
  //wdt_counter = 1000;
  ////wdt_enable(WDTO_8S);
  millisAtLastDetectionOfPackageDrop = millis();

  Serial.println("master mcu setup complete");
}



void incrementDropBoxOpenedCount()
{
  dropBoxCounters.dropBropOpenCount++;
  EEPROM.put(0, dropBoxCounters);
}

void incrementPackageCount()
{
  dropBoxCounters.packageDetectedCount++;
  EEPROM.put(0, dropBoxCounters);
}

void resetCounters()
{
  dropBoxCounters.dropBropOpenCount = 0;
  dropBoxCounters.packageDetectedCount = 0;
  EEPROM.put(0, dropBoxCounters);
}

void getCountersFromEEPROM()
{
  EEPROM.get(0, dropBoxCounters);
}






void loop()
{

  //char cntrDmp[100];
  //sprintf(cntrDmp, "wc = %d , sac = %d , dis_wdt = %d , SlMcuSt = %d",wdt_counter, slaveMcuActivationCounter , disable_wdt , SlaveMcuState);

  //Serial.println("looop");



  if ( wdt_counter <  3  )
  {
    //Serial.println("WDT wake interrupt");
    if (disable_wdt == false)
    {
      Serial.println("WDT sleep....");
      enterSleep();
      return;
    }
  }
  else
  {
    wdt_counter = 0;


    if ( slaveMcuActivationCounter > 0 ) // 8 * 4 secs period , wtd max prescale is 8 secs
    {
      slaveMcuActivationCounter = 0;
      if (  dropBoxCounters.packageDetectedCount > 0 && (SlaveMcuState == OFF )  )
      {
        Serial.println("Non zero package count detected, activating slave mcu"  );
        triggerSlaveMcu();
      }
    }

    if (digitalRead(WAKE_UP_INT_PIN) == HIGH && (SlaveMcuState == OFF ) ) // door is still closed
    {
      Serial.println("WDT wake up , door is closed , so sleep again....");
      enterSleep();
      return;
    }

  }


  doorClosedForMoreThatExpiryTime = false;

  if ( (digitalRead(WAKE_UP_INT_PIN) == HIGH) && (SlaveMcuState == OFF ) )
  {


    if (doorClosed == false)
    {
      millisAtLastDoorClose = millis();
      doorClosed = true;
    }

    int millisSinceLastDoorClose = millis() -  millisAtLastDoorClose ;
    //Serial.println("millis since last door close = " + String(millisSinceLastDoorClose) );
    if ( millisSinceLastDoorClose > 3000 )
    {

      doorClosedForMoreThatExpiryTime = true ;
      Serial.println("Wakup pin low timer expired, entering sleep....");
      disable_wdt = false;
      enterSleep();
    }

  }
  else
  {
    doorClosed = false;

  }

  int ir_val = analogRead(INFRARED_PIN);

  if (displayCoordinateAndIrVals)
    Serial.println("ir val = " + String(ir_val) );

  int x = displayOrientationAndGetZaxis();
//&& ( x < -60 ) 
  if ( ir_val > 320  )
  {
    if (doorClosedForMoreThatExpiryTime && SlaveMcuState == ON)
    {
      Serial.println("False detection, skip..");
    }
    else
    {
      int millsSinceLastDetection = millis() - millisAtLastDetectionOfPackageDrop;
      if ( millsSinceLastDetection > 1000)
      {
        incrementPackageCount();
        Serial.println("Infared sensor detected package drop, package count = " + String(dropBoxCounters.packageDetectedCount) );
        millisAtLastDetectionOfPackageDrop = millis();
      }
    }
  }

  if (SlaveMcuState == ON)
  {
    checkSlaveStatus();
  }


}


int packageCountSentToSlaveForUpload = 0;
void checkSlaveStatus()
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
    packageCountSentToSlaveForUpload = dropBoxCounters.packageDetectedCount;
    slaveMcuSerial.println("MASTER-CMD:DBC=" + String(packageCountSentToSlaveForUpload) + String(",") +  String(dropBoxCounters.dropBropOpenCount) + String(",") );
    delay(100);
    slaveMcuSerial.println("MASTER-CMD:DBC=" + String(packageCountSentToSlaveForUpload) + String(",") +  String(dropBoxCounters.dropBropOpenCount) + String(",") );
  }

  if ( data.indexOf("SLAVE-SIGNAL:PROCESSING-FAILED") != -1 )
  {
    Serial.println("Received faliure signal from slave mcu, resetting slave now..");
    turnOffSlaveMcuCircuit();

    if (digitalRead(WAKE_UP_INT_PIN) == HIGH)
    {
      enterSleep();
    }
  }

  if ( data.indexOf("SLAVE-SIGNAL:PROCESSING-COMPLETE") != -1 )
  {
    dropBoxCounters.packageDetectedCount -= packageCountSentToSlaveForUpload;
    //resetCounters();
    Serial.println("Received successful message transmission signal from slave mcu, resetting slave now..");
    turnOffSlaveMcuCircuit();

    if (digitalRead(WAKE_UP_INT_PIN) == HIGH)
    {
      enterSleep();
    }
  }

}

void turnOffSlaveMcuCircuit()
{

  disable_wdt = false;

  Serial.println("Shutting down slave");
  digitalWrite(SLAVE_MCU_P_CHANNEL_MOSFET_PIN, LOW);
  digitalWrite(VOLTAGE_BOOSTER_MOSFET_PIN, LOW);



  int slaveTimeEndMillis = millis();

  Serial.println("TIme elpased in slave mcu = " + String(slaveTimeEndMillis - slaveTimeStartMillis));
  // slaveMcuActive = false;
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
  delay(10);

  SlaveMcuState = OFF;

  // Timer1.initialize((slaveMcuTimerDuration / 2) * 1000000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  //  Timer1.attachInterrupt( activateSlaveMcu ); // attach the service routine here

}

int send_count = 0;
void triggerSlaveMcu()
{
  disable_wdt = true;

  Serial.println("slave mcu powewred on.., send count = " + String(send_count++) );

  digitalWrite(SLAVE_MCU_P_CHANNEL_MOSFET_PIN, HIGH);
  digitalWrite(VOLTAGE_BOOSTER_MOSFET_PIN, HIGH);
  SlaveMcuState = ON;

  slaveTimeStartMillis = millis();


  // Timer1.detachInterrupt();
  // Timer1.stop();

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
  if (displayCoordinateAndIrVals)
    Serial.println(str);
  return x;
}



void boxOpenReedSwitchInterrupt(void)
{
  disable_wdt = true;

}

void enterSleep(void)
{
  //return;

  Serial.println("sleeping...");
  Serial.flush();

  while (!(UCSR0A & (1 << UDRE0)))  // Wait for empty transmit buffer
    UCSR0A |= 1 << TXC0;  // mark transmission not complete
  while (!(UCSR0A & (1 << TXC0)));   // Wait for the transmission to complete


  if (SlaveMcuState != OFF)
    return;

  //Serial.println("master mcu entering sleep mode");
  digitalWrite(INFRARED_MOSFET_PIN, LOW);
  //delay(100);

  /* Setup pin2 as an interrupt and attach handler. */
  attachInterrupt(0, boxOpenReedSwitchInterrupt, LOW);
  //delay(100);

  //wdt_enable(WDTO_8S);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  sleep_enable();

  sleep_mode();

  sleep_disable();

  //wdt_disable();

  detachInterrupt(0);

  //digitalWrite(13, HIGH);
  digitalWrite(INFRARED_MOSFET_PIN, HIGH);
  //delay(100);
  setupAccelerometer();
  //Serial.begin(9600);
  //delay(100);
  //digitalWrite(13, LOW);

  //incrementDropBoxOpenedCount();
}

void setupWTD()
{
  /*** Setup the WDT ***/

  /* Clear the reset flag. */
  MCUSR &= ~(1 << WDRF);

  /* In order to change WDE or the prescaler, we need to
     set WDCE (This will allow updates for 4 clock cycles).
  */
  WDTCSR |= (1 << WDCE) | (1 << WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1 << WDP0 | 1 << WDP3; /* 8.0 seconds */

  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);

  Serial.println("WTD timer setup for a period of 8 secs");
}




ISR(WDT_vect)
{
  //wdt_interrupt = true;
  wdt_counter++;
  slaveMcuActivationCounter++;
  //  digitalWrite(13, HIGH);
  //  delay(50);
  //  digitalWrite(13, LOW);
  //  delay(50);
  //  digitalWrite(13, HIGH);
  //  delay(50);
  //  digitalWrite(13, LOW);
  //  delay(50);
  //  digitalWrite(13, HIGH);
  //  delay(50);
  //  digitalWrite(13, LOW);
}


