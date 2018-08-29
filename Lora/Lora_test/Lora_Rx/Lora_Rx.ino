// Arduino9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Arduino9x_TX

#include <SPI.h>
#include <RH_RF95.h>
#include <dht11.h>


#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 13

#include <dht11.h>
dht11 DHT11;
#define DHT11PIN 6

const int WHITE_LED = 8;

void setup()
{
  pinMode(A0, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);

  digitalWrite(RFM95_RST, HIGH);

  //while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Arduino LoRa RX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}
//Celsius to Fahrenheit conversion
double Fahrenheit(double celsius)
{
  return 1.8 * celsius + 32;
}

// fast integer version with rounding
//int Celcius2Fahrenheit(int celcius)
//{
//  return (celsius * 18 + 5)/10 + 32;
//}


//Celsius to Kelvin conversion
double Kelvin(double celsius)
{
  return celsius + 273.15;
}

// dewPoint function NOAA
// reference (1) : http://wahiduddin.net/calc/density_algorithms.htm
// reference (2) : http://www.colorado.edu/geography/weather_station/Geog_site/about.htm
//
double dewPoint(double celsius, double humidity)
{
  // (1) Saturation Vapor Pressure = ESGG(T)
  double RATIO = 373.15 / (273.15 + celsius);
  double RHS = -7.90298 * (RATIO - 1);
  RHS += 5.02808 * log10(RATIO);
  RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / RATIO ))) - 1) ;
  RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
  RHS += log10(1013.246);

  // factor -3 is to adjust units - Vapor Pressure SVP * humidity
  double VP = pow(10, RHS - 3) * humidity;

  // (2) DEWPOINT = F(Vapor Pressure)
  double T = log(VP / 0.61078); // temp var
  return (241.88 * T) / (17.558 - T);
}

// delta max = 0.6544 wrt dewPoint()
// 6.9 x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity * 0.01);
  double Td = (b * temp) / (a - temp);
  return Td;
}


void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len))
    {
      int chk = DHT11.read(DHT11PIN);

      Serial.print("Read sensor: ");
      switch (chk)
      {
        case DHTLIB_OK:
          Serial.println("OK");
          break;
        case DHTLIB_ERROR_CHECKSUM:
          Serial.println("Checksum error");
          break;
        case DHTLIB_ERROR_TIMEOUT:
          Serial.println("Time out error");
          break;
        default:
          Serial.println("Unknown error");
          break;
      }

      Serial.print("Humidity (%): ");
      Serial.println((float)DHT11.humidity, 2);

      Serial.print("Temperature (°C): ");
      Serial.println((float)DHT11.temperature, 2);

      Serial.print("Temperature (°F): ");
      Serial.println(Fahrenheit(DHT11.temperature), 2);

      Serial.print("Temperature (°K): ");
      Serial.println(Kelvin(DHT11.temperature), 2);

      Serial.print("Dew Point (°C): ");
      Serial.println(dewPoint(DHT11.temperature, DHT11.humidity));

      Serial.print("Dew PointFast (°C): ");
      Serial.println(dewPointFast(DHT11.temperature, DHT11.humidity));

      // Send a reply
      uint8_t data[30] = "";
      memset(data, 0, 30);
      char h[10] , t[10] , dp[10] , fdp[10] ;
      dtostrf(DHT11.humidity, 4, 1, h);
      dtostrf(Fahrenheit(DHT11.temperature), 4, 1, t);
      dtostrf(dewPoint(DHT11.temperature, DHT11.humidity), 4, 1, dp);
      dtostrf(dewPointFast(DHT11.temperature, DHT11.humidity), 4, 1, fdp);

      sprintf(data, "%s,%s,%s,%s", h, t , dp , fdp );
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      digitalWrite(LED, LOW);

      analogWrite(WHITE_LED, 300);
      delay(100);
      analogWrite(WHITE_LED, 0);

    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}
