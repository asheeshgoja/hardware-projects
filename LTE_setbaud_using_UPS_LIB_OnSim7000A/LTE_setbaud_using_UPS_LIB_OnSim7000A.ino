/* This code sets the baud rate of the LTE shield to a slower baud rate
    than the default 115200 baud rate. You only need to run this code
    once for it to take effect! (At least for SIM7000E versions)
*/

/***************************************************
  This is an example for our Adafruit FONA Cellular Module
  since the FONA 3G does not do auto-baud very well, this demo
  fixes the baud rate to 4800 from the default 115200

  Designed specifically to work with the Adafruit FONA 3G
  ----> http://www.adafruit.com/products/2691
  ----> http://www.adafruit.com/products/2687

  These cellular modules use TTL Serial to communicate, 2 pins are
  required to interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include <SoftwareSerial.h>
#include "UPS_Sim7000A.h"


#define FONA_TX 5 // Microcontroller RX
#define FONA_RX 6 // Microcontroller TX

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);

void setup() {


  //FET
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
  delay(1000);
  digitalWrite(10, HIGH);
  delay(1000);

  //LEVEL SHIFTER
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);


  while (!Serial); // Wait for serial

  pinMode(FONA_RST, OUTPUT);
  digitalWrite(FONA_RST, HIGH); // Default state

  pinMode(FONA_PWRKEY, OUTPUT);
  // Turn on the SIM7000 by pulsing PWRKEY low for at least 72ms
  pinMode(FONA_PWRKEY, LOW);
  delay(100);
  pinMode(FONA_PWRKEY, HIGH);

  delay(4000);

  Serial.begin(115200);
  Serial.println(F("FONA set baudrate"));

  Serial.println(F("First trying 115200 baud"));
  // start at 115200 baud
  fonaSS.begin(115200);
  ups_begin(&fonaSS);

  // send the command to reset the baud rate to 4800
  setBaudrate(9600);

  // restart with 4800 baud
  fonaSS.begin(9600);
  Serial.println(F("Initializing @ 9600 baud..."));

  delay(1000);

  if (! ups_begin(&fonaSS)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  Serial.println(F("FONA is OK"));
//
//  // Print module IMEI number.
//  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
//  uint8_t imeiLen = fona.getIMEI(imei);
//  if (imeiLen > 0) {
//    Serial.print("Module IMEI: "); Serial.println(imei);
//  }
//
//  //get iccid
//  sendATCmd("AT+CCID");
//  Serial.print("Module CCID: "); Serial.println(replybuffer);
//
//
//  //CSTT
//  sendATCmd("AT+CSTT=\"VZWINTERNET\"\r\n");
//  Serial.print("CSTT response: "); Serial.println(replybuffer);
//  delay(1000);
//
//  //CIICR
//  sendATCmd("AT+CIICR");
//  Serial.print("CIICR response: "); Serial.println(replybuffer);
//  delay(1000);
//
//
//  //CIFSR
//  sendATCmd("AT+CIFSR");
//  Serial.print("CIFSR response: "); Serial.println(replybuffer);
//  delay(3000);
//
//
//  //CIPSTART
//  sendATCmd("AT+CIPSTART=\"TCP\",\"smartbox2.eastus2.cloudapp.azure.com\",\"9000\"");
//  Serial.print("CIPSTART response: "); Serial.println(replybuffer);
//  delay(2000);
//
//  
//  char msg[20] = "hello smartbox";
//  char num[4];
//  itoa(strlen(msg), num, 10);  
//  
//  fonaSerial->write("AT+CIPSEND=");
//  fonaSerial->write(num);
//  fonaSerial->write("\r\n");
//  delay(500);
//  fonaSerial->write(msg);
//  delay(500);
//  fonaSerial->write((char)26);
//  delay(1000);
//  Serial.print("CIPSEND response: "); Serial.println(replybuffer);
//
//
//  //CIPCLOSE
//  sendATCmd("AT+CIPCLOSE");
//  Serial.print("CIPCLOSE response: "); Serial.println(replybuffer);
//  delay(1000);
//
//
//  //CIPSHUT
//  sendATCmd("AT+CIPSHUT");
//  Serial.print("CIPSEND CIPSHUT: "); Serial.println(replybuffer);
//  delay(1000);



}

void loop() {
}
