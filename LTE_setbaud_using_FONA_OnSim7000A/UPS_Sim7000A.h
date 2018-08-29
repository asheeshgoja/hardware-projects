#include "Adafruit_FONA.h"

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

uint8_t readline(uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS, boolean multiline = false);

uint8_t readline(uint16_t timeout, boolean multiline ) {
  uint16_t replyidx = 0;

  while (timeout--) {
    if (replyidx >= 254) {
      //DEBUG_PRINTLN(F("SPACE"));
      break;
    }

    while (fonaSerial->available()) {
      char c =  fonaSerial->read();
      if (c == '\r') continue;
      if (c == 0xA) {
        if (replyidx == 0)   // the first 0x0A is ignored
          continue;

        if (!multiline) {
          timeout = 0;         // the second 0x0A is the end of the line
          break;
        }
      }
      replybuffer[replyidx] = c;
      //DEBUG_PRINT(c, HEX); DEBUG_PRINT("#"); DEBUG_PRINTLN(c);
      replyidx++;
    }

    if (timeout == 0) {
      //DEBUG_PRINTLN(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  replybuffer[replyidx] = 0;  // null term
  return replyidx;
}


void flushInput() {
  // Read all available serial input to flush pending data.
  uint16_t timeoutloop = 0;
  while (timeoutloop++ < 40) {
    while (fonaSerial->available()) {
      fonaSerial->read();
      timeoutloop = 0;  // If char was received reset the timer
    }
    delay(1);
  }
}

uint8_t getReply(char *send, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS) ;

uint8_t getReply(char *send, uint16_t timeout) {
  flushInput();


  DEBUG_PRINT(F("\t---)> ")); DEBUG_PRINTLN(send);


  fonaSerial->println(send);

  uint8_t l = readline(timeout);

  DEBUG_PRINT (F("\t<(--- ")); DEBUG_PRINTLN(replybuffer);

  return l;
}



uint8_t sendATCmd(char* cmd) {

  memset(replybuffer, '\0', 255 );

  getReply(cmd);

  // up to 15 chars
  //strncpy(retval, replybuffer, 20);
  //retval[20] = 0;

  fona.readline(); // eat 'OK'

  return strlen(replybuffer);
}



boolean TCPsend(char *packet, uint8_t len) {

  DEBUG_PRINT(F("AT+CIPSEND="));
  DEBUG_PRINTLN(len);
  DEBUG_PRINTLN();


  fonaSerial->print(F("AT+CIPSEND="));
  fonaSerial->println(len);
  readline();

  DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);

  if (replybuffer[0] != '>') return false;

  fonaSerial->write(packet, len);
  readline(3000); // wait up to 3 seconds to send the data

  DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);


  return (strcmp(replybuffer, "SEND OK") == 0);
}




