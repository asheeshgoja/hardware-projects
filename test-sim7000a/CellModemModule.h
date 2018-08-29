#include "UPS_sim900.h"

struct DropCounterStruct {
  int packageDetectedCount = 0;
  int dropBropOpenCount = 0;
  bool pickupButtonPressed = false;
  bool hbTriggeredReboot = false;
};

DropCounterStruct dropBoxCounters;


const int DEVICE_ID = 9920 ;

const int INFRARED_PIN = 3;
const int INFRARED_MOSFET_PIN = 4;

const float VccMin   = 0.0;           // Minimum expected Vcc level, in Volts.
const float VccMax   = 4.6;           // Maximum expected Vcc level, in Volts.
const float VccCorrection = 1 / 1; // Measured Vcc by multimeter divided by reported Vcc

volatile long timerCounter = 0;
volatile long wdt_counter = 0;
volatile bool disable_wdt = true;

volatile long slaveMcuActivationCounter = 0;
volatile long heartBeatCounter = 0;

volatile int sleep_entered = false;
volatile bool driverPickupButtonPressed = false;
volatile bool uploadInProgress = false;


#define BAUDRATE  115200


char end_c[2];
String responseStringFromA6Command = "";

const int Sim_On_Delay_Time = 3000;

const int  PIN_TX = 5 ;
const int  PIN_RX = 6 ;
const int WAKE_UP_INT_PIN = 2;

const int DRIVER_PICKUP_BUTTON_PIN = 3;
const int SIM_MOSFET_PIN = 10;
const int SIM_PWR_DWN_PIN = 7;
const int LOGIG_SHIFTER_EN = 9;


SoftwareSerial* gprsSerial = new SoftwareSerial(PIN_TX, PIN_RX);


long millisAtLastDetectionOfPackageDrop = 0;
bool sendSuccessful = false;
volatile bool abortSimUplink = false;


bool setupSimAndSendMessage(char* message);
void syncTimeWithSim800();
int  getBatteryLevel();
void sendMsg(char* message);

char msg[20] = "";
char tcp_cmd[30] = "";
char sim_reply_buf[101];
void print_sim_response();


void setupCellModemModule()
{
  Serial.begin(BAUDRATE);


  pinMode(8, INPUT);
  //digitalWrite(FONA_RST, HIGH);

  sim900_init(gprsSerial, BAUDRATE);

  pinMode(SIM_MOSFET_PIN, OUTPUT);
  digitalWrite(SIM_MOSFET_PIN, LOW);

  pinMode(SIM_PWR_DWN_PIN, OUTPUT);
  digitalWrite(SIM_MOSFET_PIN, HIGH);
  Serial.println("SIM FET pin on");


  pinMode(LOGIG_SHIFTER_EN, OUTPUT);
  digitalWrite(LOGIG_SHIFTER_EN, LOW);
  Serial.println("level shifter en pin low");
}


int getBatteryLevel()
{
  return 100;

}
bool isDoorClosed()
{

  return digitalRead(WAKE_UP_INT_PIN) == LOW ? true : false;

}


//#define DOOR_OPEN_RETURN
//
//#define spinDelay(t){delay(t);\
//  }


#define DOOR_OPEN_RETURN

//#define DOOR_OPEN_RETURN if( ! isDoorClosed() ) \
//  { digitalWrite(SIM_MOSFET_PIN, LOW); \
//    digitalWrite(INFRARED_MOSFET_PIN, HIGH);\
//    abortSimUplink = true;\
//    return false; }

#define spinDelay(t){\
    delay(t);\
  }


void sendMessgeUsingSim800(char* message)
{
  uploadInProgress = true;
  //attachInterrupt(digitalPinToInterrupt(2), boxOpenReedSwitchInterrupt, HIGH);

  sendMsg(message);

  //detachInterrupt(digitalPinToInterrupt(2));
  uploadInProgress = false;
}

void sendMsg(char* message)
{
  int simPowerRecycleCnt = 0;
  sendSuccessful = false;
  abortSimUplink = false;

  const int maxSimRecycleCnt = 2;
  const int maxSendMsgRetryCnt = 3;

  while (simPowerRecycleCnt++ < maxSimRecycleCnt )
  {
    long mills_S = millis();

    Serial.println(F("SIM fet turned on"));
    digitalWrite(SIM_MOSFET_PIN, HIGH);

    delay(1000);

    Serial.println("SIM Power on pin pulsed for 200ms");
    digitalWrite(SIM_PWR_DWN_PIN, LOW);
    delay(100);
    digitalWrite(SIM_PWR_DWN_PIN, HIGH);

    delay(4000);
    Serial.println("SIM7000 A ready for AT cmds");


    Serial.println("changing baud to 9600");
    sim900_send_cmd(" --> ATE0");
    print_sim_response();
    delay(100);
    sim900_send_cmd("AT+IPR=9600");
    delay(100);
    print_sim_response();
    delay(1000);
    gprsSerial->begin(9600);
    delay(1000);
    sim900_send_cmd("AT\r\n");
    print_sim_response();


    int retryCnt = 0;
    while ( !setupSimAndSendMessage(message) && retryCnt++ < maxSendMsgRetryCnt )
    {
      Serial.print(F("UL fl, c= "));  Serial.println(retryCnt);
      spinDelay(1000);
      if (retryCnt > maxSendMsgRetryCnt)
        break;
    }

    digitalWrite(SIM_MOSFET_PIN, LOW);

    long mills_E = millis();
    Serial.print(F("TM SIM = ")) ; Serial.println( mills_E - mills_S );

    //reset
    millisAtLastDetectionOfPackageDrop = millis();


    if ( retryCnt < 4 )
    {
      sendSuccessful = true;
      Serial.println(F("UL S"));
      break;
    }
    else
    {
      sendSuccessful = false;
      Serial.print(F("RC fl c=")); Serial.println(retryCnt);
      spinDelay(3000);
    }
  }

  if ( simPowerRecycleCnt > 2 )
  {
    Serial.println(F("UL F!"));
  }

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


//String sim_response_str;

void print_sim_response()
{
  //char reply_buf[100];
  memset(sim_reply_buf, '\0', 100 );
  //String resp = "";
  sim900_clean_buffer(sim_reply_buf, 100);
  sim900_read_buffer(sim_reply_buf, 100);
  sim_reply_buf[100] = '\0';
  Serial.print("SIM7000 response : ");
  Serial.println(sim_reply_buf);
  //sim_response = String(reply_buf);
}





int send(const char * str, int len)
{
  //char cmd[32];
  char num[4];
  if (len > 0) {
    //snprintf(cmd,sizeof(cmd),"AT+CIPSEND=%d\r\n",len);
    //sprintf(cmd,"AT+CIPSEND=%d\r\n",len);
    sim900_send_cmd("AT+CIPSEND=");
    itoa(len, num, 10);
    sim900_send_cmd(num);
    if (!sim900_check_with_cmd("\r\n", ">", CMD)) {
      //if(!sim900_check_with_cmd(cmd,">",CMD)) {
      return 0;
    }
    /*if(0 != sim900_check_with_cmd(str,"SEND OK\r\n", DEFAULT_TIMEOUT * 10 ,DATA)) {
        return 0;
      }*/
    delay(500);
    sim900_send_cmd(str);
    delay(500);
    sim900_send_End_Mark();
    if (!sim900_wait_for_resp("SEND OK\r\n", DATA, DEFAULT_TIMEOUT * 10, DEFAULT_INTERCHAR_TIMEOUT * 10)) {
      return 0;
    }
  }
  return len;
}

void close()
{
  sim900_send_cmd("AT+CIPCLOSE\r\n");
}

void disconnect()
{
  sim900_send_cmd("AT+CIPSHUT\r\n");
}

void getSignalStrength(int *buffer)
{
  //AT+CSQ            --> 6 + CR = 10
  //+CSQ: <rssi>,<ber>      --> CRLF + 5 + CRLF = 9
  //OK              --> CRLF + 2 + CRLF =  6

  byte i = 0;
  char gprsBuffer[26];
  char *p, *s;
  char buffers[4];
  sim900_flush_serial();
  sim900_send_cmd("AT+CSQ\r");
  sim900_clean_buffer(gprsBuffer, 26);
  sim900_read_buffer(gprsBuffer, 26, DEFAULT_TIMEOUT);
  if (NULL != (s = strstr(gprsBuffer, "+CSQ:"))) {
    s = strstr((char *)(s), " ");
    s = s + 1;  //We are in the first phone number character
    p = strstr((char *)(s), ","); //p is last character """
    if (NULL != s) {
      i = 0;
      while (s < p) {
        buffers[i++] = *(s++);
      }
      buffers[i] = '\0';
    }
    *buffer = atoi(buffers);
    return true;
  }
  return false;
}


void sendPickupCompleteNotificationMessage()
{

  int avg_ir_val = 0;


  digitalWrite(INFRARED_MOSFET_PIN, HIGH);
  delay(100);

  int ir_val_1 = analogRead(INFRARED_PIN);
  delay(100);
  int ir_val_2 = analogRead(INFRARED_PIN);

  avg_ir_val = (ir_val_1 + ir_val_2) / 2;

  digitalWrite(INFRARED_MOSFET_PIN, LOW);


  int batt_level = getBatteryLevel();

  //char msg[20] = "";
  memset(msg, 0, 20);
  sprintf(msg, "PK:%d,%d", DEVICE_ID,  batt_level);
  //debugPrint("HB msg = " + String(msg) );
  sendMessgeUsingSim800(msg);

  if (sendSuccessful)
  {
    dropBoxCounters.packageDetectedCount = 0;
    dropBoxCounters.dropBropOpenCount = 0 ;
    dropBoxCounters.pickupButtonPressed = false;
    //saveCountersToEeprom();
  }
}


void sendHeartBeatMessage()
{

  int avg_ir_val = 0;


  digitalWrite(INFRARED_MOSFET_PIN, HIGH);
  delay(100);

  int ir_val_1 = analogRead(INFRARED_PIN);
  delay(100);
  int ir_val_2 = analogRead(INFRARED_PIN);

  avg_ir_val = (ir_val_1 + ir_val_2) / 2;

  digitalWrite(INFRARED_MOSFET_PIN, LOW);


  int batt_level = getBatteryLevel();

  //char msg[20] = "";
  memset(msg, 0, 20);
  sprintf(msg, "HB:%d,%d,%d", DEVICE_ID, avg_ir_val , batt_level);
  //debugPrint("HB msg = " + String(msg) );
  sendMessgeUsingSim800(msg);
}


void sendPackageCountMessage()
{
  int startCnt = dropBoxCounters.packageDetectedCount;
  int dropBoxOpenCnt = dropBoxCounters.dropBropOpenCount;
  int batt_level = getBatteryLevel();


  memset(msg, 0, 20);
  sprintf(msg, "DT:%d,%d,%d,%d", DEVICE_ID, startCnt, dropBoxOpenCnt , batt_level);
  sendMessgeUsingSim800(msg);

  if (sendSuccessful)
  {
    dropBoxCounters.packageDetectedCount = 0;
    dropBoxCounters.dropBropOpenCount = 0 ;

    //saveCountersToEeprom();
  }

}


bool setupSimAndSendMessage4(char* message)
{

  for (;;)
  {
    Serial.println("AT") ;
    print_sim_response();
    delay(1000);
    sim900_send_cmd("AT+CSTT=\"VZWINTERNET\"\r\n");
    print_sim_response();

    delay(1000);
  }

}



bool setupSimAndSendMessage(char* message)
{
  DOOR_OPEN_RETURN
  //CSQ , first parameter id the singnal strength , sencond is the BBit error rate
  //see http://m2msupport.net/m2msupport/signal-quality/
  //high first param is good , low or zero second is good ( range 0 - 7)

  sim900_send_cmd("AT\r\n");
  print_sim_response();
  DOOR_OPEN_RETURN


  //sim900_send_cmd("AT + CSQ\r\n");
  //print_sim_response();
  //  //debugPrint(sim_reply_buf);
  //  //debugPrint(splitStringAndGet(sim_reply_buf, ':', 1  ));
  //  String signalStrength = splitStringAndGet(splitStringAndGet(sim_reply_buf, ':', 1  ), '\r', 0 );
  //  //debugPrint(signalStrength);

  //debugPrint("Signal S = " + String(ss[0]));
  DOOR_OPEN_RETURN

  sim900_send_cmd("AT+CSTT=\"VZWINTERNET\"\r\n");
  print_sim_response();
  DOOR_OPEN_RETURN

  sim900_check_with_cmd("AT+CIICR\r\n", "OK\r\n", CMD);
  print_sim_response();
  DOOR_OPEN_RETURN

  //Get local IP address
  char ipAddr[32];
  sim900_send_cmd("AT+CIFSR\r\n");
  sim900_clean_buffer(ipAddr, 32);
  sim900_read_buffer(ipAddr, 32);
  //debugPrint(ipAddr);
  DOOR_OPEN_RETURN


  byte i;
  char *p, *s;
  char ip_string[16];

  if (NULL != strstr(ipAddr, "ERROR") ) {
    Serial.println( F("f ip"));
    sim900_send_cmd("AT+CIPSHUT\r\n");
    print_sim_response();
    spinDelay(3000);
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
    //debugPrint( String("char at 2 is " ) + String(c));

    if (isDigit(c))
    {
      Serial.println(ip_string) ;
    }
    else
    {
      Serial.println(F("f ip"));
      sim900_send_cmd("AT+CIPSHUT\r\n");
      print_sim_response();
      spinDelay(3000);

      return false;

    }
  }

  spinDelay(1000);

  //sim900_send_cmd("AT + CIPSTART = \"TCP\",\"ec2-107-22-60-122.compute-1.amazonaws.com\",9000\r\n");
  //sim900_send_cmd("AT + CIPSTART = \"TCP\",\"ec2-54-227-228-246.compute-1.amazonaws.com\",9000\r\n");
  //sim900_send_cmd("AT + CIPSTART = \"TCP\",\"ec2-54-71-155-20.us-west-2.compute.amazonaws.com\",32528\r\n");
  //sim900_send_cmd("AT + CIPSTART = \"TCP\",\"ec2-54-91-40-14.compute-1.amazonaws.com\",9000\r\n");

  //azure svrs
  //sim900_send_cmd("AT + CIPSTART = \"TCP\",\"smartbox1.eastus2.cloudapp.azure.com\",9000\r\n"); //azure iot svr 1
  //sim900_send_cmd("AT + CIPSTART = \"TCP\",\"smartbox2.eastus2.cloudapp.azure.com\",9000\r\n"); //azure iot svr 1
  sim900_send_cmd("AT + CIPSTART = \"TCP\",\"smartbox3.eastus2.cloudapp.azure.com\",9000\r\n"); //azure iot svr 1


  //sim900_send_cmd("AT + CIPSTART = \"TCP\",\"ocp.nhale.org\",30016\r\n"); //azure iot svr 1

  print_sim_response();
  DOOR_OPEN_RETURN

  spinDelay(1000);
  String resp;


  int ss[2];
  getSignalStrength(ss);
  DOOR_OPEN_RETURN

  memset(tcp_cmd, 0, 30);
  sprintf(tcp_cmd, "%s,%d,%d", message, ss[0], ss[1] );
  // //debugPrint("Sending this message to the node server : " + String(http_cmd) );

  send(tcp_cmd, strlen(tcp_cmd));
  print_sim_response();
  DOOR_OPEN_RETURN
  delay(2000);

  ////debugPrint(String("send response - " ) + sim_response  );

  //if (sim_response.indexOf("1973") == -1)
  if ( strcmp(sim_reply_buf, "1973") != 0)
  {
    //debugPrint( "MT failed retry"  );
    return false;
  }

  //Serial.print(DEVICE_ID);
  Serial.println(F(" : snd S :)"));
  delay(1000);

  syncTimeWithSim800();
  DOOR_OPEN_RETURN


  close();
  disconnect();

  return true;

}



//time
void digitalClockDisplay() {

  //  char buf[25];
  //  memset(buf, 0, 25);
  //  sprintf(buf, "%d/%d/%d %d:%d:%d  " , month(), day(), year(), hour(), minute(), second() );
  //  Serial.println(buf);
}


void setMcuDateTimeFromSim800()
{

  return;


}

void syncTimeWithSim800()
{

  return;


}



