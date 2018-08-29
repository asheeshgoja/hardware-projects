#include <String.h>
#include <Esp.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "PubSubClient.h"

const char* ssid     = "ssid";//clear1
const char* password = "password";
const char* mqtt_server = "ec2-54-84-99-110.compute-1.amazonaws.com";

WiFiClient espClient;
PubSubClient client(espClient);
bool stop = false;
const int CALL_PIN = 12; //2
const int TEXT_PIN = 14; //0

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
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

  postToServer("hello a6");

  pinMode(TEXT_PIN, OUTPUT);
  pinMode(CALL_PIN, OUTPUT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, password);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect("arduinoClient")) {
        client.setCallback(callback);
        client.subscribe("inTopic");
      }
    }

    if (client.connected())
      client.loop();
  }
}

void postToServer(char* data) {
  ///if (!client.connected()) {
  //reconnect();
  //}
  client.loop();
  //Serial.println(data);
  client.publish("outTopic", data);
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  char buf[255] = "";
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    buf[i] = (char)payload[i];
  }
  //Serial.println("");
  String topic_s(topic);
  Serial.println(topic_s);

  if (String(buf) == "call")
  {
    digitalWrite(CALL_PIN, HIGH);
    delay(100);
    digitalWrite(CALL_PIN, LOW);  
    Serial.println(String(buf));

  }

  if (String(buf) == "text")
  {
    digitalWrite(TEXT_PIN, HIGH);
    delay(100);
    digitalWrite(TEXT_PIN, LOW);
    Serial.println(String(buf));
  }

}




