/***************************************************
  This is a library for our Adafruit FONA Cellular Module

  Designed specifically to work with the Adafruit FONA
  ----> http://www.adafruit.com/products/1946
  ----> http://www.adafruit.com/products/1963

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/
#ifndef ADAFRUIT_FONA_H
#define ADAFRUIT_FONA_H

#include "FONAConfig.h"
#include "FONAExtIncludes.h"
#include "FONAPlatform.h"



#define FONA800L 1
#define FONA800H 6

#define FONA808_V1 2
#define FONA808_V2 3

#define FONA3G_A 4
#define FONA3G_E 5

#define FONA_LTE_A 7
#define FONA_LTE_C 8
#define FONA_LTE_E 9

// Set the preferred SMS storage.
//   Use "SM" for storage on the SIM.
//   Use "ME" for internal storage on the FONA chip
#define FONA_PREF_SMS_STORAGE "\"SM\""
//#define FONA_PREF_SMS_STORAGE "\"ME\""

#define FONA_HEADSETAUDIO 0
#define FONA_EXTAUDIO 1

#define FONA_STTONE_DIALTONE 1
#define FONA_STTONE_BUSY 2
#define FONA_STTONE_CONGESTION 3
#define FONA_STTONE_PATHACK 4
#define FONA_STTONE_DROPPED 5
#define FONA_STTONE_ERROR 6
#define FONA_STTONE_CALLWAIT 7
#define FONA_STTONE_RINGING 8
#define FONA_STTONE_BEEP 16
#define FONA_STTONE_POSTONE 17
#define FONA_STTONE_ERRTONE 18
#define FONA_STTONE_INDIANDIALTONE 19
#define FONA_STTONE_USADIALTONE 20

#define FONA_DEFAULT_TIMEOUT_MS 500
#define FONA_NO_RST_PIN 99

#define FONA_HTTP_GET   0
#define FONA_HTTP_POST  1
#define FONA_HTTP_HEAD  2

#define FONA_CALL_READY 0
#define FONA_CALL_FAILED 1
#define FONA_CALL_UNKNOWN 2
#define FONA_CALL_RINGING 3
#define FONA_CALL_INPROGRESS 4

class Adafruit_FONA : public FONAStreamType {
  public:
    Adafruit_FONA(int8_t);
    boolean begin(FONAStreamType &port);
    uint8_t type();

    // Stream
    int available(void);
    size_t write(uint8_t x);
    int read(void);
    int peek(void);
    void flush();

    // FONA 3G requirements
    boolean setBaudrate(uint16_t baud);

    // IMEI
    uint8_t getIMEI(char *imei);


    // Helper functions to verify responses.
    boolean expectReply(FONAFlashStringPtr reply, uint16_t timeout = 10000);
    boolean sendCheckReply(char *send, char *reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
    boolean sendCheckReply(FONAFlashStringPtr send, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
    boolean sendCheckReply(char* send, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);


  public:
    int8_t _rstpin;
    uint8_t _type;

    char replybuffer[255];
    FONAFlashStringPtr apn;
    FONAFlashStringPtr apnusername;
    FONAFlashStringPtr apnpassword;
    boolean httpsredirect;
    FONAFlashStringPtr useragent;
    FONAFlashStringPtr ok_reply;




    void flushInput();
    uint16_t readRaw(uint16_t b);
    uint8_t readline(uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS, boolean multiline = false);
    uint8_t getReply(char *send, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
    uint8_t getReply(FONAFlashStringPtr send, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
    uint8_t getReply(FONAFlashStringPtr prefix, char *suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
    uint8_t getReply(FONAFlashStringPtr prefix, int32_t suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
    uint8_t getReply(FONAFlashStringPtr prefix, int32_t suffix1, int32_t suffix2, uint16_t timeout); // Don't set default value or else function call is ambiguous.
    uint8_t getReplyQuoted(FONAFlashStringPtr prefix, FONAFlashStringPtr suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);

    boolean sendCheckReply(FONAFlashStringPtr prefix, char *suffix, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
    boolean sendCheckReply(FONAFlashStringPtr prefix, int32_t suffix, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
    boolean sendCheckReply(FONAFlashStringPtr prefix, int32_t suffix, int32_t suffix2, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
    boolean sendCheckReplyQuoted(FONAFlashStringPtr prefix, FONAFlashStringPtr suffix, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);

    boolean parseReply(FONAFlashStringPtr toreply,
                       uint16_t *v, char divider  = ',', uint8_t index = 0);
    boolean parseReply(FONAFlashStringPtr toreply,
                       char *v, char divider  = ',', uint8_t index = 0);
    boolean parseReplyQuoted(FONAFlashStringPtr toreply,
                             char *v, int maxlen, char divider, uint8_t index);

    boolean sendParseReply(FONAFlashStringPtr tosend,
                           FONAFlashStringPtr toreply,
                           uint16_t *v, char divider = ',', uint8_t index = 0);




    FONAStreamType *mySerial;
};

class Adafruit_FONA_LTE : public Adafruit_FONA {

  public:
    Adafruit_FONA_LTE () : Adafruit_FONA(FONA_NO_RST_PIN) {
      _type = FONA_LTE_A;
    }

    boolean setBaudrate(uint16_t baud);
    // boolean enableGPRS(boolean onoff);
    //    boolean playToolkitTone(uint8_t t, uint16_t len);
    boolean hangUp(void);
    //    boolean pickUp(void);
};

#endif
