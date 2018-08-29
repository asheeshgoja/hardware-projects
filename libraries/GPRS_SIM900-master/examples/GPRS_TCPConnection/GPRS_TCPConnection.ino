
#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define PIN_TX    5
#define PIN_RX    6
//make sure that the baud rate of SIM900 is 9600!
//you can use the AT Command(AT+IPR=9600) to set it through SerialDebug
#define BAUDRATE  9600

char http_cmd[] = "id=50620,ts=04/01/01 00:24:14+00,cnt=1";
char buffer[512];
GPRS gprs(PIN_TX, PIN_RX, BAUDRATE);
void setup(){
  Serial.begin(9600);
  // use DHCP
  while(!gprs.init()) {
      delay(1000);
      Serial.print("init error\r\n");
  }
  delay(3000);    
  // attempt DHCP
  while(!gprs.join(F("apn.konekt.io"))) {
      Serial.println("gprs join network error");
      delay(2000);
  }

  // successful DHCP
  Serial.print("IP Address is ");
  Serial.println(gprs.getIPAddress());

  if(!gprs.connect(TCP,"ec2-54-227-228-246.compute-1.amazonaws.com", 9000)) {
      Serial.println("connect error");
  }else{
      Serial.println("connect mbed.org success");
  }

  Serial.println("waiting to fetch...");
  gprs.send(http_cmd, sizeof(http_cmd)-1);
  while (true) {
      int ret = gprs.recv(buffer, sizeof(buffer)-1);
      if (ret <= 0){
          Serial.println("fetch over...");
          break; 
      }
      buffer[ret] = '\0';
      Serial.print("Recv: ");
      Serial.print(ret);
      Serial.print(" bytes: ");
      Serial.println(buffer);
  }
  gprs.close();
  gprs.disconnect();
}

void loop(){

}
