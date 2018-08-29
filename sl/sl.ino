char str[4];

void setup() {
  Serial.begin(9600);
}

void loop() {
  int value = millis(); //this would be much more exciting if it was a sensor value
  Serial.println("Slave transmission : " + String(value) );
//
  delay(1000);
//  
  String data;

//  while (Serial.available() > 0) {
//    char inByte = Serial.read();
//    //Serial.write(inByte);
//    data += String(inByte);
//  }
    data = Serial.readString();
    
  if (data.length() > 0)
    Serial.println(data);


}

