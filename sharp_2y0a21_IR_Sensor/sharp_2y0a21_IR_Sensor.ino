int sensorpin = 0;                 // analog pin used to connect the sharp sensor
int val = 0;                 // variable to store the values from sensor(initially zero)
int red_led = 4;

void setup()
{
  Serial.begin(9600);               // starts the serial monitor
  pinMode(sensorpin, INPUT);
  pinMode(red_led, OUTPUT);
 // digitalWrite(red_led, HIGH);
}
 
void loop()
{
  val = analogRead(sensorpin);       // reads the value of the sharp sensor
  Serial.println(val);            // prints the value of the sensor to the serial monitor
  if(val > 500)
  {
    Serial.println("dectedted package!!"); 
    digitalWrite(red_led, HIGH);
    delay(1000);
    digitalWrite(red_led, LOW);  
    
  }
  delay(10);                    // wait for this much time before printing next value
}
