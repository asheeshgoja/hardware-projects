#include <String.h>
#include <SoftwareSerial.h>

#define OK 1
#define NOTOK 2
#define TIMEOUT 3
#define RST 2

//#define A6board Serial1

#ifndef A6board
SoftwareSerial A6board (5, 6);
#define A6baud 9600
#else
#define A6baud 115200
#endif

#define SERIALTIMEOUT 3000


#define echoPin 14 // Echo Pin
#define trigPin 15 // Trigger Pin

int maximumRange = 8 ;// cm; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance;


char end_c[2];

void setup() {

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  A6board.begin(A6baud);   // the GPRS baud rate
  Serial.begin(115200);    // the GPRS baud rate
  // ctrlZ String definition
  end_c[0] = 0x1a;
  end_c[1] = '\0';
  Serial.println("Start");

  pinMode(RST, OUTPUT);
  digitalWrite(RST, HIGH);
  delay(5000);
  digitalWrite(RST, LOW);
  delay(500);
  if (A6begin() != OK) {
    Serial.println("Error");
    while (1 == 1);
  }
}

void loop()
{
  //after start up the program, you can using terminal to connect the serial of gprs shield,
  //if you input 't' in the terminal, the program will execute SendTextMessage(), it will show how to send a sms message,
  //if input 'd' in the terminal, it will execute DialVoiceCall(), etc.
  // GetSignalQuality();
  //A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  //Serial.println("Waiting for command");

  if ( isThereAPackageInFrontOfMe() )
  {
    //SendTextMessage();
    sendGojaFunGSM(1, 1);
  }
  //  if (Serial.available())
  //    switch (Serial.read())
  //    {
  //      case 't':
  //        Serial.println("Send SMS");
  //        SendTextMessage();
  //        break;
  //      case 'v':
  //        Serial.println("Make voice call");
  //        DialVoiceCall();
  //        break;
  //      case 'h':
  //        Serial.println("not implemented");
  //        SubmitHttpRequest();
  //        break;
  //      case 's':
  //        Serial.println("----------------------Post value to Sparkfun-------------------------------");
  //        float batt = (float)analogRead(A0) * 5.165 / 594.0; // conversion factor measured with multimeter
  //        sendGojaFunGSM(1, batt);
  //        break;
  //    }
  //  if ( A6board.available())
  //    Serial.write( A6board.read());

  delay(10);
}


///SendTextMessage()
///this function is to send a sms message
void SendTextMessage()
{
  A6command("AT+CMGF=1\r", "OK", "yy", 2000, 2); //Because we want to send the SMS in text mode
  delay(100);
  A6command("AT+CMGS = \"7326904664\"", ">", "yy", 20000, 2); //send sms message, be careful need to add a country code before the cellphone number
  delay(100);
  A6board.println("Package was just dropped in a drop box!");//the content of the message
  A6board.println(end_c);//the ASCII code of the ctrl+z is 26
  A6board.println();
}

///DialVoiceCall
///this function is to dial a voice call
void DialVoiceCall()
{
  A6command("AT+SNFS=0", "OK", "yy", 20000, 2);
  A6command("ATD7326904664", "OK", "yy", 20000, 2); //dial the number
}

///SubmitHttpRequest()
///this function is submit a http request
///attention:the time of delay is very important, it must be set enough
void SubmitHttpRequest()
{
  A6command("AT+CSQ", "OK", "yy", 20000, 2);
  A6command("AT+CGATT?", "OK", "yy", 20000, 2);
  A6command("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", "OK", "yy", 20000, 2); //setting the SAPBR, the connection type is using gprs
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+SAPBR=3,1,\"gprs.swisscom.ch\",\"\"", "OK", "yy", 20000, 2); //setting the APN, the second need you fill in your local apn server
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+SAPBR=1,1", "OK", "yy", 20000, 2); //setting the SAPBR, for detail you can refer to the AT command mamual
  A6board.println("AT+HTTPINIT"); //init the HTTP request

  delay(2000);
  ShowSerialData();

  A6board.println("AT+HTTPPARA=\"URL\",\"www.google.com.hk\"");// setting the httppara, the second parameter is the website you want to access
  delay(1000);

  ShowSerialData();

  A6board.println("AT+HTTPACTION=0");//submit the request
  delay(10000);//the delay is very important, the delay time is base on the return from the website, if the return datas are very large, the time required longer.
  //while(!mySerial.available());

  ShowSerialData();

  A6board.println("AT+HTTPREAD");// read the data from the website you access
  delay(300);

  ShowSerialData();

  A6board.println("");
  delay(100);
}


bool sendGojaFunGSM(byte sparkfunType, float value1) {
  String host = "ec2-54-227-228-246.compute-1.amazonaws.com";
  //  String publicKey   = "QGyWvOR04Mcqw7oZ2Qrp";
  //  String privateKey = "Jqym6Ma281SynPV6qer5";
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CGATT?", "OK", "yy", 20000, 2);
  A6command("AT+CGATT=1", "OK", "yy", 20000, 2);
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CGDCONT=1,\"IP\",\"ccdata\"", "OK", "yy", 20000, 2); //bring up wireless connection
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CGACT=1,1", "OK", "yy", 10000, 2);
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  // A6command("AT+CIICR", "OK", "yy", 20000, 2); //bring up wireless connection
  // A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CIFSR", "OK", "yy", 20000, 2); //get local IP adress
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CIPSTART=\"TCP\",\"" + host + "\",80", "CONNECT OK", "yy", 25000, 2); //start up the connection
  // A6input();
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CIPSEND", ">", "yy", 10000, 1); //begin send data to remote server
  delay(500);
  A6board.print("GET /IoT/ID/");
  A6board.print(String(millis()));
  //  A6board.print("?private_key=");
  // A6board.print(privateKey);
  A6board.print("?pak_drp=1");
  //A6board.print(value1, 2);
  A6board.print(" HTTP/1.1");
  A6board.print("\r\n");
  A6board.print("HOST: ");
  A6board.print(host);
  A6board.print("\r\n");
  A6board.print("\r\n");

  Serial.print("GET /IoT/ID/1212121?/");
  //Serial.print(publicKey);
  //Serial.print("?private_key=");
  //Serial.print(privateKey);
  Serial.print("&b_l=");
  Serial.print(value1, 2);
  Serial.print(" HTTP/1.1");
  Serial.print("\r\n");
  Serial.print("HOST: ");
  Serial.print(host);
  Serial.print("\r\n");
  Serial.print("\r\n");

  A6command(end_c, "HTTP/1.1", "yy", 30000, 1); //begin send data to remote server
  //A6board.println(end_c); //sending ctrlZ
  unsigned long   entry = millis();
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CIPCLOSE", "OK", "yy", 15000, 1); //sending
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  delay(100);
  Serial.println("-------------------------End------------------------------");
}


///sparkfun()///
bool sendSparkfunGSM(byte sparkfunType, float value1) {
  String host = "data.sparkfun.com";
  String publicKey   = "QGyWvOR04Mcqw7oZ2Qrp";
  String privateKey = "Jqym6Ma281SynPV6qer5";
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CGATT?", "OK", "yy", 20000, 2);
  A6command("AT+CGATT=1", "OK", "yy", 20000, 2);
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CGDCONT=1,\"IP\",\"ccdata\"", "OK", "yy", 20000, 2); //bring up wireless connection
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CGACT=1,1", "OK", "yy", 10000, 2);
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  // A6command("AT+CIICR", "OK", "yy", 20000, 2); //bring up wireless connection
  // A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CIFSR", "OK", "yy", 20000, 2); //get local IP adress
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CIPSTART=\"TCP\",\"" + host + "\",80", "CONNECT OK", "yy", 25000, 2); //start up the connection
  // A6input();
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CIPSEND", ">", "yy", 10000, 1); //begin send data to remote server
  delay(500);
  A6board.print("GET /input/");
  A6board.print(publicKey);
  A6board.print("?private_key=");
  A6board.print(privateKey);
  A6board.print("&battery=");
  A6board.print(value1, 2);
  A6board.print(" HTTP/1.1");
  A6board.print("\r\n");
  A6board.print("HOST: ");
  A6board.print(host);
  A6board.print("\r\n");
  A6board.print("\r\n");

  Serial.print("GET /input/");
  Serial.print(publicKey);
  Serial.print("?private_key=");
  Serial.print(privateKey);
  Serial.print("&battery=");
  Serial.print(value1, 2);
  Serial.print(" HTTP/1.1");
  Serial.print("\r\n");
  Serial.print("HOST: ");
  Serial.print(host);
  Serial.print("\r\n");
  Serial.print("\r\n");

  A6command(end_c, "HTTP/1.1", "yy", 30000, 1); //begin send data to remote server
  //A6board.println(end_c); //sending ctrlZ
  unsigned long   entry = millis();
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CIPCLOSE", "OK", "yy", 15000, 1); //sending
  A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  delay(100);
  Serial.println("-------------------------End------------------------------");
}


byte A6waitFor(String response1, String response2, int timeOut) {
  unsigned long entry = millis();
  int count = 0;
  String reply = A6read();
  byte retVal = 99;
  do {
    reply = A6read();
    if (reply != "") {
      Serial.print((millis() - entry));
      Serial.print(" ms ");
      Serial.println(reply);
    }
  } while ((reply.indexOf(response1) + reply.indexOf(response2) == -2) && millis() - entry < timeOut );
  if ((millis() - entry) >= timeOut) {
    retVal = TIMEOUT;
  } else {
    if (reply.indexOf(response1) + reply.indexOf(response2) > -2) retVal = OK;
    else retVal = NOTOK;
  }
  //  Serial.print("retVal = ");
  //  Serial.println(retVal);
  return retVal;
}

String A6waitForAndGetResponse(String response1, String response2, int timeOut) {
  unsigned long entry = millis();
  int count = 0;
  String reply = A6read();
  byte retVal = 99;
  do {
    reply = A6read();
    if (reply != "") {
      Serial.print((millis() - entry));
      Serial.print(" ms ");
      Serial.println(reply);
    }
  } while ((reply.indexOf(response1) + reply.indexOf(response2) == -2) && millis() - entry < timeOut );
  if ((millis() - entry) >= timeOut) {
    retVal = TIMEOUT;
  } else {
    if (reply.indexOf(response1) + reply.indexOf(response2) > -2) retVal = OK;
    else retVal = NOTOK;
  }
  //  Serial.print("retVal = ");
  //  Serial.println(retVal);
  return reply;
}

byte A6command(String command, String response1, String response2, int timeOut, int repetitions) {
  byte returnValue = NOTOK;
  byte count = 0;
  while (count < repetitions && returnValue != OK) {
    A6board.println(command);
    Serial.print("Command: ");
    Serial.println(command);
    if (A6waitFor(response1, response2, timeOut) == OK) {
      //     Serial.println("OK");
      returnValue = OK;
    } else returnValue = NOTOK;
    count++;
  }
  return returnValue;
}




void A6input() {
  String hh;
  char buffer[100];
  while (1 == 1) {
    if (Serial.available()) {
      hh = Serial.readStringUntil('\n');
      hh.toCharArray(buffer, hh.length() + 1);
      if (hh.indexOf("ende") == 0) {
        A6board.write(end_c);
        Serial.println("ende");
      } else {
        A6board.write(buffer);
        A6board.write('\n');
      }
    }
    if (A6board.available()) {
      Serial.write(A6board.read());
    }
  }
}


bool A6begin() {
  A6board.println("AT+CREG?");
  byte hi = A6waitFor("1,", "5,", 1500);  // 1: registered, home network ; 5: registered, roaming
  while ( hi != OK) {
    A6board.println("AT+CREG?");
    hi = A6waitFor("1,", "5,", 1500);
  }



  A6board.println("AT+CSQ");
  A6waitFor("1,", "5,", 1500);

  A6board.println("AT+CCLK?");
  Serial.println(A6waitForAndGetResponse("1,", "5,", 1500));



  if (A6command("AT&F0", "OK", "yy", 5000, 2) == OK) {   // Reset to factory settings
    if (A6command("ATE0", "OK", "yy", 5000, 2) == OK) {  // disable Echo
      if (A6command("AT+CMEE=2", "OK", "yy", 5000, 2) == OK) return OK;  // enable better error messages
      else return NOTOK;
    }
  }
}

void ShowSerialData()
{
  unsigned long entry = millis();
  while ( A6board.available() != 0 && millis() - entry < SERIALTIMEOUT)
    Serial.println(A6board.readStringUntil('\n'));
}

String A6read() {
  String reply = "";
  if (A6board.available())  {
    reply = A6board.readString();
  }
  //  Serial.print("Reply: ");
  //  Serial.println(reply);
  return reply;
}


bool isThereAPackageInFrontOfMe()
{
  int distance = senseDistance();


  if (distance >= maximumRange || distance <= minimumRange)
  {
    return false;
  }
  else
  {
    Serial.println("Package dected in a UPS drop box !");
    return true;
  }

}

int senseDistance()
{

  int distance = -1;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  //Calculate the distance (in cm) based on the speed of sound.
  distance = duration / 58.2;
  return distance;

}
