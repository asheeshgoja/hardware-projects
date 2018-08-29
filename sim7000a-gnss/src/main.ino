
// For LTE shield
#define SIM7000A_PWRKEY 7
#define SIM7000A_RST 8
#define SIM7000A_TX 5 // Microcontroller RX
#define SIM7000A_RX 6 // Microcontroller TX

#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(SIM7000A_TX, SIM7000A_RX);
SoftwareSerial *fonaSerial = &fonaSS;

#include "UPS_Sim7000A.h"


Ups_Sim7000A fona = Ups_Sim7000A();


uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

void setup() {



  //FET
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
  delay(1000);
  digitalWrite(10, HIGH);
  delay(1000);



  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);


  while (!Serial); // Wait for serial

  pinMode(SIM7000A_RST, OUTPUT);
  digitalWrite(SIM7000A_RST, HIGH); // Default state

  pinMode(SIM7000A_PWRKEY, OUTPUT);
  // Turn on the SIM7000 by pulsing PWRKEY low for at least 72ms
  digitalWrite(SIM7000A_PWRKEY, LOW);
  delay(100);
  digitalWrite(SIM7000A_PWRKEY, HIGH);

  delay(4000);

  Serial.begin(115200);
  Serial.println(F("FONA set baudrate"));

  Serial.println(F("First trying 115200 baud"));
  // start at 115200 baud
  fonaSerial->begin(115200);
  fona.begin(*fonaSerial);



  fona.sendTestTcpMessage();

  fona.enableGPS();

}

float latitude, longitude, speed_kph, heading, altitude;

void loop() {

  while (!fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude)) {
  Serial.println(F("Failed to get GPS location, retrying..."));
  delay(2000); // Retry every 2s
}

}
