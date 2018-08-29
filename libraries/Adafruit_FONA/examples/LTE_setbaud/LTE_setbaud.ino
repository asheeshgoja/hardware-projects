/* This code sets the baud rate of the LTE shield to a slower baud rate
 *  than the default 115200 baud rate. You only need to run this code
 *  once for it to take effect! (At least for SIM7000E versions)
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

#include "Adafruit_FONA.h"

// Default
//#define FONA_RX 2
//#define FONA_TX 3
//#define FONA_RST 4

// For LTE shield
#define FONA_PWRKEY 7
#define FONA_RST 8
//#define FONA_DTR 8 // Connect with solder jumper
//#define FONA_RI 9 // Need to enable via AT commands
#define FONA_TX 5 // Microcontroller RX
#define FONA_RX 6 // Microcontroller TX
//#define T_ALERT 12 // Connect with solder jumper

// this is a large buffer for replies
char replybuffer[255];

// We default to using software serial. If you want to use hardware serial
// (because softserial isnt supported) comment out the following three lines
// and uncomment the HardwareSerial line
#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

// Hardware serial is also possible!
//  HardwareSerial *fonaSerial = &Serial1;

// Use this for FONA 2G or 3G
//Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

// Use this one for FONA LTE
// Notice how we don't include the reset pin because it's reserved for emergencies on the LTE module!
Adafruit_FONA_LTE fona = Adafruit_FONA_LTE();

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

void setup() {



 //FET
  pinMode(10, OUTPUT);
  digitalWrite(10,LOW);
  delay(1000);
  digitalWrite(10, HIGH);
  delay(1000);


  
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
  fonaSerial->begin(115200);
  fona.begin(*fonaSerial);

  // send the command to reset the baud rate to 4800
  fona.setBaudrate(4800);

  // restart with 4800 baud
  fonaSerial->begin(4800);
  Serial.println(F("Initializing @ 4800 baud..."));

  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  Serial.println(F("FONA is OK"));

  // Print module IMEI number.
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("Module IMEI: "); Serial.println(imei);
  }

}

void loop() {
}
