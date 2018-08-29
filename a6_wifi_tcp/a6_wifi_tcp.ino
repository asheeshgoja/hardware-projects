#include <String.h>
#include <Esp.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "PubSubClient.h"


SoftwareSerial A6board (14, 12); //y , g
#define A6baud 9600
char end_c[2];
#define OK 1
#define NOTOK 2
#define TIMEOUT 3
#define RST 2


const char* ssid     = "ssid";//clear1
const char* password = "password";
const char* mqtt_server = "ec2-54-84-99-110.compute-1.amazonaws.com";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:
  A6board.begin(A6baud);   // the GPRS baud rate
  Serial.begin(115200);

  end_c[0] = 0x1a;
  end_c[1] = '\0';
  Serial.println("");
  Serial.println("Start");



  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println("ssid");

  WiFi.begin("ssid", "password");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  A6board.println("AT+CREG?");
  delay(1000);
  Serial.println( A6read() );
  delay(3000);
  A6board.println("AT+CREG?");
  delay(1000);
  Serial.println( A6read() );
  delay(3000);
  A6board.println("AT+CREG?");
  delay(1000);
  Serial.println( A6read() );
  delay(3000);
}

void loop()
{
  char str[512];
  sprintf(str, "{\"a6_trackingNo\":\"1Z232232AASSD22\"}", 1);
  Serial.println(str);
  postToServer(str);
  delay(1000);
}

void postToServer(char* data) {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  Serial.println(data);
  client.publish("outTopic", data);
}

void call(String number)
{
  A6board.println("AT+CREG?");
  delay(1000);
  Serial.println( A6read() );
  delay(2000);

  A6board.println("AT+SNFS=0");//
  delay(1000);
  Serial.println( A6read() );
  A6board.println(String("ATD" + number));//
  delay(3000);
  Serial.println( A6read() );

  Serial.println("hello");
  delay(10000);
}

String A6read() {
  String reply = "";
  if (A6board.available())  {
    reply = A6board.readString();
  }
  //  Serial.print("Reply: ");
  //  Serial.println(reply);
  return reply;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char buf[255] = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    buf[i] = (char)payload[i];
  }
  Serial.println("");
  String topic_s(topic);
  Serial.println(String(buf));

  if (topic_s == "call")
    call(String(buf));

  if (topic_s == "text")
    SendTextMessage(String(buf));

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("call");
      client.subscribe("text");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void SendTextMessage(String msg)
{
  A6board.println("AT+CMGF=1\r");//, "OK", "yy", 2000, 2); //Because we want to send the SMS in text mode
  delay(1000);
  Serial.println( A6read() );
  A6board.println("AT+CMGS = \"7326904664\"");// ">", "yy", 20000, 2); //send sms message, be careful need to add a country code before the cellphone number
  delay(100);
  A6board.println(msg);//the content of the message
  A6board.println(end_c);//the ASCII code of the ctrl+z is 26
  delay(1000);
  Serial.println( A6read() );
  A6board.println();
  delay(10000);
}


