//http://192.168.4.1/config.html

#include <String.h>
#include <Esp.h>
#include <ESP8266WiFi.h>
//#include <ESP8266HTTPClient.h>
#include "PubSubClient.h"
#include <EEPROM.h>



const int LED_PIN = 12; //2
const int BUTTON_PIN = 16; //0


#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

#include "helpers.h"
#include "global.h"

#include "Page_Root.h"
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPsettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"
#include "example.h"

#define ACCESS_POINT_NAME  "goja-ups-iot-ap"
#define ACCESS_POINT_PASSWORD  "11111111"

const char* ssid     = "ssid";//clear1
const char* password = "password";
String mqtt_server = "smartbox3.eastus2.cloudapp.azure.com";

WiFiClient espClient;
PubSubClient client(espClient);
bool stop = false;

bool button_pressed = false;


boolean AdminEnabled = true;


void setup() {
  // put your setup code here, to run once:
  EEPROM.begin(1024);
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN_16);




  if (digitalRead(BUTTON_PIN) == HIGH)
  {
    AdminEnabled = true;
    Serial.println("Admin Enabled");
  }
  else
  {
    AdminEnabled = false;
    Serial.println("Admin disabled");
  }

  Serial.println("Starting ES8266");
  if (!ReadConfig())
  {
    // DEFAULT CONFIG
    config.ssid = "ssid";
    config.password = "password";
    config.dhcp = true;
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 100;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 1;
    config.ntpServerName = "0.de.pool.ntp.org";
    config.Update_Time_Via_NTP_Every =  0;
    config.timezone = -10;
    config.daylight = true;
    config.DeviceName = "Not Named";
    config.AutoTurnOff = false;
    config.AutoTurnOn = false;
    config.TurnOffHour = 0;
    config.TurnOffMinute = 0;
    config.TurnOnHour = 0;
    config.TurnOnMinute = 0;
    WriteConfig();
    Serial.println("General config applied");
  }

  //config.ssid = "ssid";
  //config.password = "password";
  //config.mqttServer = "ec2-54-227-228-246.compute-1.amazonaws.com";

  if (AdminEnabled)
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
    Serial.println("");
    Serial.print("Hotspot on, ");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    server.on ( "/", processExample  );
    server.on ( "/admin/filldynamicdata", filldynamicdata );

    server.on ( "/favicon.ico",   []() {
      Serial.println("favicon.ico");
      server.send ( 200, "text/html", "" );
    }  );


    server.on ( "/admin.html", []() {
      Serial.println("admin.html");
      server.send ( 200, "text/html", PAGE_AdminMainPage );
    }  );
    server.on ( "/config.html", send_network_configuration_html );
    server.on ( "/info.html", []() {
      Serial.println("info.html");
      server.send ( 200, "text/html", PAGE_Information );
    }  );
    server.on ( "/ntp.html", send_NTP_configuration_html  );
    server.on ( "/general.html", send_general_html  );
    //  server.on ( "/example.html", []() { server.send ( 200, "text/html", PAGE_EXAMPLE );  } );
    server.on ( "/style.css", []() {
      Serial.println("style.css");
      server.send ( 200, "text/plain", PAGE_Style_css );
    } );
    server.on ( "/microajax.js", []() {
      Serial.println("microajax.js");
      server.send ( 200, "text/plain", PAGE_microajax_js );
    } );
    server.on ( "/admin/values", send_network_configuration_values_html );
    server.on ( "/admin/connectionstate", send_connection_state_values_html );
    server.on ( "/admin/infovalues", send_information_values_html );
    server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
    server.on ( "/admin/generalvalues", send_general_configuration_values_html);
    server.on ( "/admin/devicename",     send_devicename_value_html);

    server.onNotFound ( []() {
      Serial.println("Page Not Found");
      server.send ( 400, "text/html", "Page not Found" );
    }  );
    server.begin();
    Serial.println( "HTTP server started" );
    tkSecond.attach(1, Second_Tick);
    UDPNTPClient.begin(2390);  // Port for NTP receive
  }
  else
  {
    WiFi.mode(WIFI_STA);
    ConfigureWifi();

    mqtt_server = config.mqttServer;

    client.setServer(mqtt_server.c_str(), 1883);
    client.setCallback(callback);

    delay(1000);

    Serial.println(String("posting hello to mqtt @ ") + mqtt_server );
    postToServer("hello a6");


  }



  //  Serial.println();
  //  Serial.println();
  //  Serial.print("Connecting to ");
  //  Serial.println(ssid);
  //
  //  WiFi.begin(ssid, password);
  //  while (WiFi.status() != WL_CONNECTED) {
  //    Serial.print(".");
  //    digitalWrite(LED_PIN, HIGH);
  //    delay(500);
  //    digitalWrite(LED_PIN, LOW);
  //    delay(500);
  //  }
  //
  //  Serial.println("");
  //  Serial.println("WiFi connected");
  //  Serial.println("IP address: ");
  //  Serial.println(WiFi.localIP());
  //





}

void loop() {



  if (config.Update_Time_Via_NTP_Every  > 0 )
  {
    if (cNTP_Update > 5 && firstStart)
    {
      NTPRefresh();
      cNTP_Update = 0;
      firstStart = false;
    }
    else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) )
    {

      NTPRefresh();
      cNTP_Update = 0;
    }
  }

  if (DateTime.minute != Minute_Old)
  {
    Minute_Old = DateTime.minute;
    if (config.AutoTurnOn)
    {
      if (DateTime.hour == config.TurnOnHour && DateTime.minute == config.TurnOnMinute)
      {
        Serial.println("SwitchON");
      }
    }


    Minute_Old = DateTime.minute;
    if (config.AutoTurnOff)
    {
      if (DateTime.hour == config.TurnOffHour && DateTime.minute == config.TurnOffMinute)
      {
        Serial.println("SwitchOff");
      }
    }
  }
  server.handleClient();




  if (AdminEnabled)
  {
    yield();
    analogWrite(LED_PIN, 300);
    delay(150);
    yield();
    analogWrite(LED_PIN, 0);
    delay(200);
    yield();
  }
  else
  {

    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Connecting to ");
      Serial.print(ssid);
      Serial.println("...");
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
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


    if ((digitalRead(BUTTON_PIN) == HIGH ))
    {
      postToServer("button_pressed");
      //digitalWrite(BUTTON_PIN, HIGH);
      // button_pressed = true;

      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      //delay(3000);
    }
  }
}

void postToServer(char* data) {


  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Serial.println(data);
  client.publish("outTopic", data);


  ///if (!client.connected()) {
  //reconnect();
  //}
  //client.loop();
  //Serial.println(data);
  client.publish("outTopic", data);
}

void callback(char* topic, byte* payload, unsigned int length) {
  char buf[255] = "";
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    buf[i] = (char)payload[i];
  }

  String topic_s(topic);
  Serial.println(topic_s);

  if (String(buf) == "button_pressed_preocessed")
  {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);

    delay(1000);

    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);

    delay(1000);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);


    Serial.println(String(buf));

  }


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
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}




