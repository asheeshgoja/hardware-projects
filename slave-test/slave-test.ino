#include <String.h>
//#include <ctype.h>
#include <GPRS_Shield_Arduino.h>
//#include "Vcc.h"

//#include <avr/sleep.h>
//#include <avr/power.h>


const int RED_LED = 9;
const int GREEN_LED = 9;
const int BLUE_LED = 9;
const int WAKE_UP_INT_PIN = 2;

#define PIN_TX    5
#define PIN_RX    6
#define BAUDRATE  9600



const float VccMin   = 0.0;           // Minimum expected Vcc level, in Volts.
const float VccMax   = 5.0;           // Maximum expected Vcc level, in Volts.
const float VccCorrection = 1.0 / 1.0; // Measured Vcc by multimeter divided by reported Vcc
//Vcc vcc(VccCorrection);



char end_c[2];
String responseStringFromA6Command = "";



//char http_cmd[] = "id=50620,ts=04/01/01 00:24:14+00,cnt=1";
char buffer[512];
GPRS gprs(PIN_TX, PIN_RX, BAUDRATE);

int startMillis = 0;
String pkgCnt = "NA";
String dropBoxOpenCnt = "NA";

const int SIM_MOSFET_PIN = 10;



void setup() {

  pinMode (13, OUTPUT);
  pinMode(SIM_MOSFET_PIN, OUTPUT);
  digitalWrite(SIM_MOSFET_PIN, LOW);

  Serial.begin(BAUDRATE);
  serialDebug("slave setup init");

  //  delay(3000);

  //  startMillis = millis();



  serialDebug("starting slave mcu");
  //return;

  //gprs.powerReset(SIM_800_RST_PIN);

  //setup_sim_800_goja();


  //  sendInitCompleteSignalToMaster();
  serialDebug("slave mcu setup complete, waiting for send signal from master mcu...");

  // Serial.println("Time elapsed in init = " + String(millis() - startMillis));


}


int retryCnt = 0;
void loop()
{

  retryCnt = 0;
  delay(1000);

  String data = "";

  while (Serial.available() > 0) {
    char inByte = Serial.read();
    data += String(inByte);
  }

  Serial.println("powering on SIM module" );
  int mills_S = millis();
  digitalWrite(SIM_MOSFET_PIN, HIGH);

  delay(10000);

  startMillis = millis();


  data += "";
  Serial.println("data = " + data );
  String counters = splitStringAndGet(splitStringAndGet(data, ':', 1), '=', 1);
  pkgCnt = splitStringAndGet(counters, ',', 0);
  dropBoxOpenCnt = splitStringAndGet(counters, ',', 1);
  //delay(1000);
  Serial.println("Received counters from master, pkg cnt = " + pkgCnt + ", dropbox open cnt = " + dropBoxOpenCnt ) ;
  //delay(1000);
  while ( !setupSimAndSendMessage() && retryCnt++ < 3 )
  {
    Serial.println("message tranimission failed, retrying .., retry cnt = " + String(retryCnt));
    delay(1000);
  }


  if ( retryCnt < 4 )
  {
    Serial.println("Transmission succesful, Total Time elapsed = " + String(millis() - startMillis));
    sendCompeletionSignalToMasterMcu();

  }
  else
  {
    Serial.println("Transmission failed, Total Time elapsed = " + String(millis() - startMillis));
    sendFaliureSignalToMasterMcu();
  }
  Serial.println("powering off  SIM module" );
  //delay(1000);
  digitalWrite(SIM_MOSFET_PIN, LOW);

  int mills_E = millis();
  Serial.println("Total time spent in SIM = " + String( mills_E - mills_S) );
  //delay(1000);
  //powerDownMcu();
  //delay(100);

  digitalWrite( SDA, LOW);
  digitalWrite( SCL, LOW);


  //}

  if (data.length() > 0)
    serialDebug(data);

  //serialDebug("waiting for cmd from master mcu..");
  //delay(2000);
}

String splitStringAndGet(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {
    0, -1
  };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


String print_sim_response()
{

  char reply_buf[255];
  //String resp = "";
  sim900_clean_buffer(reply_buf, 255);
  sim900_read_buffer(reply_buf, 255);
  //resp= String(reply_buf);
  serialDebug(reply_buf);

  return String(reply_buf);
}

bool setupSimAndSendMessage()
{
  // int batt_level = getBatteryLevel();

  //CSQ , first parameter id the singnal strength , sencond is the BBit error rate
  //see http://m2msupport.net/m2msupport/signal-quality/
  //high first param is good , low or zero second is good ( range 0 - 7)


  //sim900_send_cmd("AT + CIPMUX = 0\r\n");
  //print_sim_response();

  sim900_send_cmd("AT+CSTT=\"apn.konekt.io\"\r\n");
  print_sim_response();
//  delay(1000);


  sim900_check_with_cmd("AT+CIICR\r\n", "OK\r\n", CMD);
  print_sim_response();
 // delay(1000);


  //Get local IP address
  char ipAddr[32];
  sim900_send_cmd("AT+CIFSR\r\n");
  sim900_clean_buffer(ipAddr, 32);
  sim900_read_buffer(ipAddr, 32);
  serialDebug(ipAddr);



  byte i;
  char *p, *s;
  char ip_string[16];

  if (NULL != strstr(ipAddr, "ERROR") ) {
    serialDebug("Sim 800 failed to acquired ipaddress, retrying..");


    sim900_send_cmd("AT+CIPSHUT\r\n");
    print_sim_response();
    delay(3000);
    return false;
  }
  else
  {
    s = ipAddr + 11;
    p = strstr((char *)(s), "\r\n"); //p is last character \r\n
    if (NULL != s) {
      i = 0;
      while (s < p) {
        ip_string[i++] = *(s++);
      }
      ip_string[i] = '\0';
    }

    char c = String(ip_string).charAt(1) ;
    serialDebug( String("char at 2 is " ) + String(c));

    if (isDigit(c))
    {
      serialDebug("Sim 800 init successully acquired this ipaddress : " + String(ip_string)) ;

    }
    else
    {
      serialDebug( String("first octet of acquired ipaddress is invalid")  );
      sim900_send_cmd("AT+CIPSHUT\r\n");
      print_sim_response();
      delay(3000);

      return false;

    }
  }


 // delay(2000);

  sim900_send_cmd("AT + CIPSTART = \"TCP\",\"ec2-54-227-228-246.compute-1.amazonaws.com\",9000\r\n");

  print_sim_response();

  sim900_send_cmd("AT + CSQ\r\n");
  print_sim_response();
  String resp;

  int cipSendRetryCnt = 0;

  //delay(1000);


  char http_cmd[50] = "";
  sprintf(http_cmd, "%d,%s,%s", 12345, pkgCnt.c_str(), dropBoxOpenCnt.c_str());
  serialDebug("Sending this message to the node server : " + String(http_cmd) );

  // String http_cmd =
  //delay(1000);
  //setp complete

  gprs.send(http_cmd, strlen(http_cmd));
  //delay(2000);
  serialDebug(String("send response - " ) + print_sim_response()  );

  
  Serial.println(" message sent");
  delay(1000);
  gprs.close();
  gprs.disconnect();

  return true;

}


void sendCompeletionSignalToMasterMcu()
{
  String debugMsg = "SLAVE-SIGNAL:PROCESSING-COMPLETE";
  serialDebug(debugMsg.c_str());
}

void sendFaliureSignalToMasterMcu()
{
  String debugMsg = "SLAVE-SIGNAL:PROCESSING-FAILED";
  serialDebug(debugMsg.c_str());
}

void sendInitCompleteSignalToMaster()
{
  serialDebug("SLAVE-SIGNAL:SETUP-COMPLETE");
}

void serialDebug(String str)
{
  Serial.println(str);
}

void serialDebug(int i)
{
  Serial.println(i);
}

void powerDownMcu()
{

  digitalWrite(SIM_MOSFET_PIN, LOW);
  digitalWrite( 13, LOW);
  serialDebug("powering down slave mcu.");
  delay(1000);

  attachInterrupt(0, pin2Interrupt, LOW);
  //  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  //  sleep_enable();
  // sleep_mode();
}

void pin2Interrupt()
{

}

