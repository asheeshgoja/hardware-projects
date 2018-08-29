#define prog_char           char PROGMEM

#define prog_char_strcmp(a, b)          strcmp_P((a), (b))
// define prog_char_strncmp(a, b, c)				strncmp_P((a), (b), (c))
#define prog_char_strstr(a, b)          strstr_P((a), (b))
#define prog_char_strlen(a)           strlen_P((a))
#define prog_char_strcpy(to, fromprogmem)   strcpy_P((to), (fromprogmem))

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
                        //DEBUG_PRINTLN(("SPACE"));
                        break;
                }

                while (mySerial->available()) {
                        char c =  mySerial->read();
                        if (c == '\r') continue;
                        if (c == 0xA) {
                                if (replyidx == 0) // the first 0x0A is ignored
                                        continue;

                                if (!multiline) {
                                        timeout = 0; // the second 0x0A is the end of the line
                                        break;
                                }
                        }
                        replybuffer[replyidx] = c;
                        //DEBUG_PRINT(c, HEX); DEBUG_PRINT("#"); DEBUG_PRINTLN(c);
                        replyidx++;
                }

                if (timeout == 0) {
                        //DEBUG_PRINTLN(("TIMEOUT"));
                        break;
                }
                delay(1);
        }
        replybuffer[replyidx] = 0; // null term
        return replyidx;
}

void flushInput() {
        // Read all available serial input to flush pending data.
        uint16_t timeoutloop = 0;
        while (timeoutloop++ < 40) {
                while (mySerial->available()) {
                        mySerial->read();
                        timeoutloop = 0; // If char was received reset the timer
                }
                delay(1);
        }
}


uint8_t getReply(char *send) {

        flushInput();
        Serial.print(("\t---> ")); Serial.println(send);
        mySerial->println(send);
        uint8_t l = readline();
        Serial.print (("\t<--- ")); Serial.println(replybuffer);
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
        Serial.println(("Initializing @ 4800 baud..."));

}



public:

void sendTestTcpMessage()
{
        //get iccid
        sendATCmd("AT+CCID");
        Serial.print("Module CCID: "); Serial.println(replybuffer);

        sendATCmd("AT+CEREG?");
        Serial.print("CEREG: "); Serial.println(replybuffer);
          delay(3000);

        //CSTT
        sendATCmd("AT+CSTT=\"VZWINTERNET\"\r\n");
        Serial.print("CSTT response: "); Serial.println(replybuffer);
        delay(5000);

        //CIICR
        sendATCmd("AT+CIICR");
        Serial.print("CIICR response: "); Serial.println(replybuffer);
        delay(3000);


        //CIFSR
        char ipAddress[20] = "";
        sendATCmdWithReply("AT+CIFSR", ipAddress);
        Serial.print("IPAddress : "); Serial.println(ipAddress);
        delay(1000);

        return;

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



uint8_t getGPS(uint8_t arg, char *buffer, uint8_t maxbuff) {
        int32_t x = arg;


        getReply(("AT+CGNSINF"));

        char *p = prog_char_strstr(replybuffer, (prog_char*)("SINF"));
        if (p == 0) {
                buffer[0] = 0;
                return 0;
        }

        p+=6;

        uint8_t len = max(maxbuff-1, strlen(p));
        strncpy(buffer, p, len);
        buffer[len] = 0;

        readline(); // eat 'OK'
        return len;
}

int8_t GPSstatus(void) {

        // 808 V2 uses GNS commands and doesn't have an explicit 2D/3D fix status.
        // Instead just look for a fix and if found assume it's a 3D fix.
        getReply(("AT+CGNSINF"));
        char *p = prog_char_strstr(replybuffer, (prog_char*)("+CGNSINF: "));
        if (p == 0) return -1;
        p+=10;
        readline();         // eat 'OK'
        if (p[0] == '0')
        {
          Serial.println("GPS is not even on!");
          return 0;         // GPS is not even on!
        }

        p+=2;         // Skip to second value, fix status.
        //DEBUG_PRINTLN(p);
        // Assume if the fix status is '1' then we have a 3D fix, otherwise no fix.
        if (p[0] == '1') return 3;
        else return 1;


        // else
        return 0;
}


boolean getGPS(float *lat, float *lon, float *speed_kph, float *heading, float *altitude) {


        char gpsbuffer[120];

        // we need at least a 2D fix
        if (GPSstatus() < 2)
        {
              Serial.println("No gps signal");
                return false;
        }
        // grab the mode 2^5 gps csv from the sim808
        uint8_t res_len = getGPS(32, gpsbuffer, 120);

        // make sure we have a response
        if (res_len == 0)
        {

                return false;
        }


        // Parse 808 V2 response.  See table 2-3 from here for format:
        // http://www.adafruit.com/datasheets/SIM800%20Series_GNSS_Application%20Note%20V1.00.pdf

        // skip GPS run status
        char *tok = strtok(gpsbuffer, ",");
        if (!tok) return false;

        // skip fix status
        tok = strtok(NULL, ",");
        if (!tok) return false;

        // skip date
        tok = strtok(NULL, ",");
        if (!tok) return false;

        // grab the latitude
        char *latp = strtok(NULL, ",");
        if (!latp) return false;

        // grab longitude
        char *longp = strtok(NULL, ",");
        if (!longp) return false;

        *lat = atol(latp);
        *lon = atol(longp);

        // only grab altitude if needed
        if (altitude != NULL) {
                // grab altitude
                char *altp = strtok(NULL, ",");
                if (!altp) return false;

                *altitude = atol(altp);
        }

        // only grab speed if needed
        if (speed_kph != NULL) {
                // grab the speed in km/h
                char *speedp = strtok(NULL, ",");
                if (!speedp) return false;

                *speed_kph = atol(speedp);
        }

        // only grab heading if needed
        if (heading != NULL) {

                // grab the speed in knots
                char *coursep = strtok(NULL, ",");
                if (!coursep) return false;

                *heading = atol(coursep);
        }


}

void enableGPS() {
        char resp[40] = "";
        sendATCmdWithReply("AT+CGPSPWR=1", resp);
        Serial.println(resp);
}

};
