#include <SoftwareSerial.h>

SoftwareSerial slaveMcuSerial(11, 12);


char str[4];

void setup() {


  pinMode(11, INPUT);
  pinMode(12, OUTPUT );

  digitalWrite(11, LOW);
  digitalWrite(12, LOW);

//  digitalWrite(10, LOW);


  Serial.begin(9600);
  slaveMcuSerial.begin(9600);
}

void loop() {
  
  int value = millis(); //this would be much more exciting if it was a sensor value
  slaveMcuSerial.println("master transmission : " + String(value) );

  delay(1000);
//
//  String data;
//
//  while (slaveMcuSerial.available() > 0) {
//    char inByte = slaveMcuSerial.read();
//    //Serial.write(inByte);
//    data += String(inByte);
//  }
//
//  if (data.length() > 0)
//    Serial.println(data);

    //delay(100);

    Serial.println(slaveMcuSerial.readString());
}
