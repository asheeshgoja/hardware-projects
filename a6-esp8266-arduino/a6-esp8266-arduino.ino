#include <String.h>
#include <SoftwareSerial.h>

//SoftwareSerial ESPserial(2, 3); // RX | TX

SoftwareSerial A6board  (5, 6); //green 5, yellor 6
#define A6baud 9600
#define SERIALTIMEOUT 3000
char end_c[2];
#define OK 1
#define NOTOK 2
#define TIMEOUT 3
#define RST 2

const int CALL_PIN = 10;
const int TEXT_PIN = 11;

void setup()
{
  Serial.begin(9600);
  setup_a6();
  setup_esp8266();
  pinMode(TEXT_PIN, INPUT);
  pinMode(CALL_PIN, INPUT);
}

void setup_a6() {

  //wdt_disable();
  A6board.begin(A6baud);   // the GPRS baud rate
  // the GPRS baud rate
  // ctrlZ String definition
  end_c[0] = 0x1a;
  end_c[1] = '\0';
  Serial.println("Start");

  //pinMode(RST, OUTPUT);
  //digitalWrite(RST, HIGH);
  //delay(8000);
  //digitalWrite(RST, LOW);
  //delay(500);
  if (A6begin() != OK) {
    Serial.println("Error");
    while (1 == 1);
  }
}


void setup_esp8266()
{
  //Serial.begin(115200);     // communication with the host computer
  //while (!Serial)   { ; }

  // Start the software serial for communication with the ESP8266
  //ESPserial.begin(9600);

  Serial.println("");
  Serial.println("Remember to to set Both NL & CR in the serial monitor.");
  Serial.println("Ready");
  Serial.println("");
}
void loop()
{
  // listen for communication from the ESP8266 and then write it to the serial monitor

  //


  //if ( ESPserial.available() )
  //{
  //    String c =  ESPserial.readStringUntil('\n');
  //    c.trim();
  //    String cmd = c + "";
  //    Serial.println(cmd );
  //    if ( cmd == "call" )
  //    {
  //      Serial.println("calling.....");
  //      DialVoiceCall();
  //      Serial.println("call complete !!");
  //    }
  //}


  //A6command("AT+CREG?", "OK", "yy", 1000, 2);
  //Serial.println("Waiting for command");

  // listen for user input and send it to the ESP8266
  //if ( Serial.available() )       {
  //  ESPserial.write( Serial.read() );
  // }

  delay(100);
  if (digitalRead(CALL_PIN) == HIGH)
  {
    Serial.println("calling.....");
    DialVoiceCall();
    Serial.println("call complete !!");

  }
  
  delay(100);
  if (digitalRead(TEXT_PIN) == HIGH)
  {
    Serial.println("texting.....");
    SendTextMessage();
    Serial.println("texting complete !!");

  }

  
}

//a6
void SendTextMessage()
{
  A6command("AT+CMGF=1\r", "OK", "yy", 2000, 2); //Because we want to send the SMS in text mode
  delay(100);
  A6command("AT+CMGS = \"2012580293\"", ">", "yy", 20000, 2); //send sms message, be careful need to add a country code before the cellphone number
  delay(100);
  A6board.println("hello ups iot demo. your package was hit by an extreme vertical force!");//the content of the message
  A6board.println(end_c);//the ASCII code of the ctrl+z is 26
  A6board.println();
}

///DialVoiceCall
///this function is to dial a voice call
void DialVoiceCall()
{
  //  A6board.println("AT+SNFS=0");
  //  A6board.println("ATD7326904664;");
  //  return;

  A6command("AT+SNFS=0", "OK", "yy", 20000, 2);
  A6command("ATD2012580293;", "OK", "yy", 20000, 2); //dial the number
  delay(10000);
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



