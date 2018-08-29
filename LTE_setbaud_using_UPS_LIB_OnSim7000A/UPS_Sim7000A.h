#include "Adafruit_FONA.h"

// For LTE shield
#define FONA_PWRKEY 7
#define FONA_RST 8


typedef const __FlashStringHelper *  FONAFlashStringPtr;
#define prog_char            char PROGMEM
#define prog_char_strcmp(a, b)          strcmp_P((a), (b))
#define prog_char_strstr(a, b)          strstr_P((a), (b))
#define prog_char_strlen(a)           strlen_P((a))
#define prog_char_strcpy(to, fromprogmem)   strcpy_P((to), (fromprogmem))



#define FONA800L 1
#define FONA800H 6

#define FONA808_V1 2
#define FONA808_V2 3

#define FONA3G_A 4
#define FONA3G_E 5

#define FONA_LTE_A 7
#define FONA_LTE_C 8
#define FONA_LTE_E 9

#define FONA_DEFAULT_TIMEOUT_MS 500

#define DEBUG_PRINT(...)    Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...)    Serial.println(__VA_ARGS__)

FONAFlashStringPtr ok_reply;


SoftwareSerial* mySerial;

// this is a large buffer for replies
char replybuffer[255];

// We default to using software serial. If you want to use hardware serial
// (because softserial isnt supported) comment out the following three lines
// and uncomment the HardwareSerial line

//SoftwareSerial *mySerial = &fonaSS;

// Hardware serial is also possible!
//  HardwareSerial *mySerial = &Serial1;

// Use this for FONA 2G or 3G
//Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

// Use this one for FONA LTE
// Notice how we don't include the reset pin because it's reserved for emergencies on the LTE module!
//Adafruit_FONA_LTE fona = Adafruit_FONA_LTE();

boolean sendCheckReply(char *send, char *reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
boolean sendCheckReply(FONAFlashStringPtr send, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
boolean sendCheckReply(char* send, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);



uint8_t readline(uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS, boolean multiline = false);

uint8_t readline(uint16_t timeout, boolean multiline ) {
  uint16_t replyidx = 0;

  while (timeout--) {
    if (replyidx >= 254) {
      //DEBUG_PRINTLN(F("SPACE"));
      break;
    }

    while (mySerial->available()) {
      char c =  mySerial->read();
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
    while (mySerial->available()) {
      mySerial->read();
      timeoutloop = 0;  // If char was received reset the timer
    }
    delay(1);
  }
}

uint8_t getReply(char *send, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS) ;

uint8_t getReply(char *send, uint16_t timeout) {
  flushInput();


  DEBUG_PRINT(F("\t---> ")); DEBUG_PRINTLN(send);


  mySerial->println(send);

  uint8_t l = readline(timeout);

  DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);

  return l;
}

uint8_t getReply(FONAFlashStringPtr send, uint16_t timeout) {
  flushInput();


  DEBUG_PRINT(F("\t---> ")); DEBUG_PRINTLN(send);


  mySerial->println(send);

  uint8_t l = readline(timeout);

  DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);

  return l;
}

// Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
uint8_t getReply(FONAFlashStringPtr prefix, char *suffix, uint16_t timeout) {
  flushInput();


  DEBUG_PRINT(F("\t---> ")); DEBUG_PRINT(prefix); DEBUG_PRINTLN(suffix);


  mySerial->print(prefix);
  mySerial->println(suffix);

  uint8_t l = readline(timeout);

  DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);

  return l;
}

// Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
uint8_t getReply(FONAFlashStringPtr prefix, int32_t suffix, uint16_t timeout) {
  flushInput();


  DEBUG_PRINT(F("\t---> ")); DEBUG_PRINT(prefix); DEBUG_PRINTLN(suffix, DEC);


  mySerial->print(prefix);
  mySerial->println(suffix, DEC);

  uint8_t l = readline(timeout);

  DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);

  return l;
}

// Send prefix, suffix, suffix2, and newline. Return response (and also set replybuffer with response).
uint8_t getReply(FONAFlashStringPtr prefix, int32_t suffix1, int32_t suffix2, uint16_t timeout) {
  flushInput();


  DEBUG_PRINT(F("\t---> ")); DEBUG_PRINT(prefix);
  DEBUG_PRINT(suffix1, DEC); DEBUG_PRINT(','); DEBUG_PRINTLN(suffix2, DEC);


  mySerial->print(prefix);
  mySerial->print(suffix1);
  mySerial->print(',');
  mySerial->println(suffix2, DEC);

  uint8_t l = readline(timeout);

  DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);

  return l;
}






uint8_t sendATCmd(char* cmd) {

  memset(replybuffer, '\0', 255 );

  getReply(cmd);

  // up to 15 chars
  //strncpy(retval, replybuffer, 20);
  //retval[20] = 0;

  readline(); // eat 'OK'

  return strlen(replybuffer);
}



boolean TCPsend(char *packet, uint8_t len) {

  DEBUG_PRINT(F("AT+CIPSEND="));
  DEBUG_PRINTLN(len);
  DEBUG_PRINTLN();


  mySerial->print(F("AT+CIPSEND="));
  mySerial->println(len);
  readline();

  DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);

  if (replybuffer[0] != '>') return false;

  mySerial->write(packet, len);
  readline(3000); // wait up to 3 seconds to send the data

  DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);


  return (strcmp(replybuffer, "SEND OK") == 0);
}




boolean sendCheckReply(char *send, char *reply, uint16_t timeout) {
  if (! getReply(send, timeout) )
    return false;
  /*
    for (uint8_t i=0; i<strlen(replybuffer); i++) {
    DEBUG_PRINT(replybuffer[i], HEX); DEBUG_PRINT(" ");
    }
    DEBUG_PRINTLN();
    for (uint8_t i=0; i<strlen(reply); i++) {
      DEBUG_PRINT(reply[i], HEX); DEBUG_PRINT(" ");
    }
    DEBUG_PRINTLN();
  */
  return (strcmp(replybuffer, reply) == 0);
}

boolean sendCheckReply(FONAFlashStringPtr send, FONAFlashStringPtr reply, uint16_t timeout) {
  if (! getReply(send, timeout) )
    return false;

  return (prog_char_strcmp(replybuffer, (prog_char*)reply) == 0);
}

boolean sendCheckReply(char* send, FONAFlashStringPtr reply, uint16_t timeout) {
  if (! getReply(send, timeout) )
    return false;
  return (prog_char_strcmp(replybuffer, (prog_char*)reply) == 0);
}


// Send prefix, suffix, and newline.  Verify FONA response matches reply parameter.
boolean sendCheckReply(FONAFlashStringPtr prefix, char *suffix, FONAFlashStringPtr reply, uint16_t timeout) {
  getReply(prefix, suffix, timeout);
  return (prog_char_strcmp(replybuffer, (prog_char*)reply) == 0);
}

// Send prefix, suffix, and newline.  Verify FONA response matches reply parameter.
boolean sendCheckReply(FONAFlashStringPtr prefix, int32_t suffix, FONAFlashStringPtr reply, uint16_t timeout) {
  getReply(prefix, suffix, timeout);
  return (prog_char_strcmp(replybuffer, (prog_char*)reply) == 0);
}

// Send prefix, suffix, suffix2, and newline.  Verify FONA response matches reply parameter.
boolean sendCheckReply(FONAFlashStringPtr prefix, int32_t suffix1, int32_t suffix2, FONAFlashStringPtr reply, uint16_t timeout) {
  getReply(prefix, suffix1, suffix2, timeout);
  return (prog_char_strcmp(replybuffer, (prog_char*)reply) == 0);
}



uint8_t _type;
boolean ups_begin(SoftwareSerial* port) {
 

  ok_reply = F("OK");


  mySerial = port;

  if (FONA_RST != 99) { // Pulse the reset pin only if it's not an LTE module
    DEBUG_PRINTLN(F("Resetting the module..."));
    pinMode(FONA_RST, OUTPUT);
    digitalWrite(FONA_RST, HIGH);
    delay(10);
    digitalWrite(FONA_RST, LOW);
    delay(100);
    digitalWrite(FONA_RST, HIGH);
  }

  DEBUG_PRINTLN(F("Attempting to open comm with ATs"));
  // give 7 seconds to reboot
  int16_t timeout = 7000;

  while (timeout > 0) {
    while (mySerial->available()) mySerial->read();
    if (sendCheckReply(F("AT"), ok_reply))
      break;
    while (mySerial->available()) mySerial->read();
    if (sendCheckReply(F("AT"), F("AT")))
      break;
    delay(500);
    timeout -= 500;
  }

  if (timeout <= 0) {
#ifdef ADAFRUIT_FONA_DEBUG
    DEBUG_PRINTLN(F("Timeout: No response to AT... last ditch attempt."));
#endif
    sendCheckReply(F("AT"), ok_reply);
    delay(100);
    sendCheckReply(F("AT"), ok_reply);
    delay(100);
    sendCheckReply(F("AT"), ok_reply);
    delay(100);
  }

  // turn off Echo!
  sendCheckReply(F("ATE0"), ok_reply);
  delay(100);

  if (! sendCheckReply(F("ATE0"), ok_reply)) {
    return false;
  }

  // turn on hangupitude
  if (FONA_RST != 99) sendCheckReply(F("AT+CVHU=0"), ok_reply);

  delay(100);
  flushInput();


  DEBUG_PRINT(F("\t---> ")); DEBUG_PRINTLN("ATI");

  mySerial->println("ATI");
  readline(500, true);

  DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);



  if (prog_char_strstr(replybuffer, (prog_char *)F("SIM808 R14")) != 0) {
    _type = FONA808_V2;
  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIM808 R13")) != 0) {
    _type = FONA808_V1;
  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIM800 R13")) != 0) {
    _type = FONA800L;
  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIMCOM_SIM5320A")) != 0) {
    _type = FONA3G_A;
  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIMCOM_SIM5320E")) != 0) {
    _type = FONA3G_E;
  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIM7000A R13")) != 0) {
    _type = FONA_LTE_A;
  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIM7000C R13")) != 0) {
    _type = FONA_LTE_C;
  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIM7000E R13")) != 0) {
    _type = FONA_LTE_E;
  }


  if (_type == FONA800L) {
    // determine if L or H

    DEBUG_PRINT(F("\t---> ")); DEBUG_PRINTLN("AT+GMM");

    mySerial->println("AT+GMM");
    readline(500, true);

    DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);


    if (prog_char_strstr(replybuffer, (prog_char *)F("SIM800H")) != 0) {
      _type = FONA800H;
    }
  }

#if defined(FONA_PREF_SMS_STORAGE)
  sendCheckReply(F("AT+CPMS=" FONA_PREF_SMS_STORAGE "," FONA_PREF_SMS_STORAGE "," FONA_PREF_SMS_STORAGE), ok_reply);
#endif

  return true;
}


boolean setBaudrate(uint16_t baud) {
  return sendCheckReply(F("AT+IPREX="), baud, ok_reply);
}

