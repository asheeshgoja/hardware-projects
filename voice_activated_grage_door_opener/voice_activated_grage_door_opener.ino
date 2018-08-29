#include "PubSubClient.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const int TRANSISTOR_BASE_PIN = 12;
const int RED_LED_PIN = 14;

const char* ssid     = "ssid";//clear1
const char* password = "password";
const char* mqtt_server = "ec2-54-227-228-246.compute-1.amazonaws.com";

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(TRANSISTOR_BASE_PIN, OUTPUT);
  digitalWrite(TRANSISTOR_BASE_PIN, LOW);
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, HIGH);


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
  postToServer("hello garage door opener");

  flashLed();
  flashLed();
}

void flashLed()
{
  digitalWrite(RED_LED_PIN, HIGH);
  delay(500);
  digitalWrite(RED_LED_PIN, LOW);
  delay(500);
}
void loop() {
  // put your main code here, to run repeatedly:

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    delay(100);
    WiFi.begin(ssid, password);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect("garage_door_opener_client")) {
        client.setCallback(callback);
        client.subscribe("garageDoorInTopic");
      }
    }

    if (client.connected())
      client.loop();
  }

}

void postToServer(char* data) {
  client.loop();
  client.publish("garageDoorOutTopic", data);
}

void callback(char* topic, byte* payload, unsigned int length) {
  char buf[255] = "";
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    buf[i] = (char)payload[i];
  }

  String topic_s(topic);
  Serial.println(topic_s);
  Serial.println(String(buf));

  if (String(buf) == "open_garage_door")
  {
    flashLed();

    Serial.println("Setting transistor base to high.");
    digitalWrite(TRANSISTOR_BASE_PIN, HIGH);
    delay(1000);
    digitalWrite(TRANSISTOR_BASE_PIN, LOW);
    Serial.println("Setting transistor base to back to low");
    Serial.println(String(buf));

    flashLed();
  }


}
