//#include "Adafruit_FONA.h"


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



class Ups_Sim7000A //: public Stream
{

    SoftwareSerial* mySerial;
    FONAFlashStringPtr ok_reply;
    uint8_t _type;
    int8_t _rstpin;

  public:

    Ups_Sim7000A()
    {
      mySerial = 0;
      ok_reply = F("OK");
      _rstpin = 99;
      _type = FONA_LTE_A;
    }




    //uint8_t readline(uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS, boolean multiline = false);

    uint8_t readline(uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS , boolean multiline = false ) {
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


    uint8_t getReply(char *send, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS) {
      flushInput();


      DEBUG_PRINT(F("\t---> ")); DEBUG_PRINTLN(send);


      mySerial->println(send);

      uint8_t l = readline(timeout);

      DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);

      return l;
    }

    uint8_t getReply(FONAFlashStringPtr send, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS) {
      flushInput();


      DEBUG_PRINT(F("\t---> ")); DEBUG_PRINTLN(send);


      mySerial->println(send);

      uint8_t l = readline(timeout);

      DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);

      return l;
    }

    // Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
    uint8_t getReply(FONAFlashStringPtr prefix, char *suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS) {
      flushInput();


      DEBUG_PRINT(F("\t---> ")); DEBUG_PRINT(prefix); DEBUG_PRINTLN(suffix);


      mySerial->print(prefix);
      mySerial->println(suffix);

      uint8_t l = readline(timeout);

      DEBUG_PRINT (F("\t<--- ")); DEBUG_PRINTLN(replybuffer);

      return l;
    }

    // Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
    uint8_t getReply(FONAFlashStringPtr prefix, int32_t suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS) {
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

    uint8_t sendATCmdWithReply(char* cmd, char* resp) {

      memset(replybuffer, '\0', 255 );

      getReply(cmd);

      int len = strlen(replybuffer);

      
      strncpy(resp, replybuffer, len);
      resp[len] = 0;

      readline(); // eat 'OK'

      return strlen(replybuffer);
    }


    boolean sendCheckReply(char *send, char *reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS) {
      if (! getReply(send, timeout) )
        return false;
      return (strcmp(replybuffer, reply) == 0);
    }

    boolean sendCheckReply(FONAFlashStringPtr send, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS) {
      if (! getReply(send, timeout) )
        return false;

      return (prog_char_strcmp(replybuffer, (prog_char*)reply) == 0);
    }

    boolean sendCheckReply(char* send, FONAFlashStringPtr reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS) {
      if (! getReply(send, timeout) )
        return false;
      return (prog_char_strcmp(replybuffer, (prog_char*)reply) == 0);
    }





    boolean begin(Stream &port) {
      mySerial = &port;

      if (_rstpin != 99) { // Pulse the reset pin only if it's not an LTE module
        DEBUG_PRINTLN(F("Resetting the module..."));
        pinMode(_rstpin, OUTPUT);
        digitalWrite(_rstpin, HIGH);
        delay(10);
        digitalWrite(_rstpin, LOW);
        delay(100);
        digitalWrite(_rstpin, HIGH);
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
      if (_rstpin != 99) sendCheckReply(F("AT+CVHU=0"), ok_reply);

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



  public:

    void sendTestTcpMessage()
    {
      //get iccid
      sendATCmd("AT+CCID");
      Serial.print("Module CCID: "); Serial.println(replybuffer);


      //CSTT
      sendATCmd("AT+CSTT=\"VZWINTERNET\"\r\n");
      Serial.print("CSTT response: "); Serial.println(replybuffer);
      delay(1000);

      //CIICR
      sendATCmd("AT+CIICR");
      Serial.print("CIICR response: "); Serial.println(replybuffer);
      delay(1000);


      //CIFSR
      char ipAddress[20] = "";
      sendATCmdWithReply("AT+CIFSR", ipAddress);
      Serial.print("IPAddress : "); Serial.println(ipAddress);
      delay(1000);


      //CIPSTART
      sendATCmd("AT+CIPSTART=\"TCP\",\"smartbox2.eastus2.cloudapp.azure.com\",\"9000\"");
      Serial.print("CIPSTART response: "); Serial.println(replybuffer);
      delay(1000);


      char msg[20] = "hello smartbox";
      char num[4];
      itoa(strlen(msg), num, 10);

      fonaSerial->write("AT+CIPSEND=");
      fonaSerial->write(num);
      fonaSerial->write("\r\n");
      delay(500);
      fonaSerial->write(msg);
      delay(500);
      fonaSerial->write((char)26);
      delay(1000);
      Serial.print("CIPSEND response: "); Serial.println(replybuffer);


      //CIPCLOSE
      sendATCmd("AT+CIPCLOSE");
      Serial.print("CIPCLOSE response: "); Serial.println(replybuffer);
      delay(1000);


      //CIPSHUT
      sendATCmd("AT+CIPSHUT");
      Serial.print("CIPSEND CIPSHUT: "); Serial.println(replybuffer);
      delay(1000);
    }

    boolean setBaudrate(uint16_t baud) {
      return sendCheckReply(F("AT+IPR="), baud, ok_reply);
    }

};



