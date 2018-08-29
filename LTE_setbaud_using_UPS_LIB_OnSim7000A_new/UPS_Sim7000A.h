char replybuffer[255];
class Ups_Sim7000A //: public Stream
{
  
    SoftwareSerial* mySerial;

  public:

    Ups_Sim7000A()
    {
      mySerial = 0;
    }

    uint8_t readline() {

      bool multiline = false;
      int timeout = 500;

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


    uint8_t getReply(char *send) {

      flushInput();
      Serial.print(F("\t---> ")); Serial.println(send);
      mySerial->println(send);
      uint8_t l = readline();
      Serial.print (F("\t<--- ")); Serial.println(replybuffer);
      return l;
    }


    uint8_t sendATCmd(char* cmd) {

      memset(replybuffer, '\0', 255 );
      getReply(cmd);
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




    boolean begin(Stream &port) {
      mySerial = &port;

      sendATCmd("AT");
      Serial.print("AT response: "); Serial.println(replybuffer);
      delay(1000);

      sendATCmd("ATE0");
      Serial.print("ATE0 response: "); Serial.println(replybuffer);
      delay(1000);

      // send the command to reset the baud rate to 4800
      //setBaudrate(4800);
      sendATCmd("AT+IPR=4800");
      Serial.print("IPR response: "); Serial.println(replybuffer);
      delay(1000);


      // restart with 4800 baud
      mySerial->begin(4800);
      Serial.println(F("Initializing @ 4800 baud..."));

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

      mySerial->write("AT+CIPSEND=");
      mySerial->write(num);
      mySerial->write("\r\n");
      delay(500);
      mySerial->write(msg);
      delay(500);
      mySerial->write((char)26);
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

};




