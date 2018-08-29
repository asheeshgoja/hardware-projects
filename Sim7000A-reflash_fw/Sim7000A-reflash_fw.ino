

/* This code allows you to freely test AT commands by entering them into
    the serial monitor! You can type in "ON" to turn on the
    SIM7000 and "OFF" to turn it off, as well as "RESET" and "BAUD<value>"
    to set a new baud rate! Please note that you should set the serial
    monitor to "No line ending"

    Asterisks (***) indicate debug text and arrows (-->) indicate sending
    an AT command. Everything else is a response from the module!

    Another way to experiment with AT commands is to use the
    "AT Command Library" here: https://github.com/botletics/AT-Command-Library

    Author: Timothy Woo (www.botletics.com)
    Github: https://github.com/botletics/NB-IoT-Shield
    Last Updated: 1/30/2018
    License: GNU GPL v3.0
*/

#include <SoftwareSerial.h> // This is for communicating with the SIM7000 module

// For LTE shield
#define FONA_PWRKEY 7
#define FONA_RST 8
#define FONA_TX 5 // Microcontroller RX
#define FONA_RX 6 // Microcontroller TX

SoftwareSerial fona = SoftwareSerial(FONA_TX, FONA_RX);

uint32_t baudRate;

void setup() {
  Serial.begin(115200);
  Serial.println("*****AT Command Test*****");


  pinMode(10, OUTPUT);

  digitalWrite(10, LOW);
  delay(1000);

  digitalWrite(10, HIGH);

  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);


  pinMode(FONA_PWRKEY, OUTPUT);
  digitalWrite(FONA_PWRKEY, HIGH);

  pinMode(FONA_RST, OUTPUT);
  digitalWrite(FONA_RST, HIGH);

  FONApower(true); // Turn on the shield
  
  
  delay(4500); // Let the shield power up


  return;

  
  
  // Hard-code baud rate
  fona.begin(115200); // Default baud rate
  //  fona.begin(4800);

  Serial.println("Turning off echo!");
  //delay(1000);
  //fona.println("AAAAAAAAAAAAT");
  fona.println(" --> ATE0"); // Turn off echo
}

void loop() {


  if (Serial.available()) {
    String userCmd = Serial.readString();

    if (userCmd == "ON") FONApower(true);
    else if (userCmd == "OFF") FONApower(false);
    else if (userCmd == "RESET") FONAreset();
    else if (userCmd == "FACTORY") facReset();
    else if (userCmd.indexOf("BAUD") != -1) {
      baudRate = userCmd.substring(4).toInt();
      Serial.print("*** Switching to "); Serial.print(baudRate); Serial.println(" baud");
      Serial.print(" --> ");
      fona.println("AT+IPR=" + String(baudRate));
      delay(100); // Give it some time to chill
      fona.begin(baudRate);
    }
    else {
      Serial.print(" --> ");
      Serial.print(userCmd);
      fona.println(userCmd);
    }
  }

  if (fona.available()) {
    Serial.write(fona.read());
  }
}

void flushInput() {
  while (Serial.available()) {
    char c = Serial.read(); // Flush pending data
  }
}

// This function turns the SIM7000 on or off.
// Enter "ON" or "OFF" in the serial monitor
void FONApower(bool option) {
  if (option) {
    Serial.println("*** Turning ON (takes about 4.2s)");
    digitalWrite(FONA_PWRKEY, LOW);
    delay(100); // At least 72ms

    pinMode(FONA_PWRKEY, INPUT);
    //digitalWrite(FONA_PWRKEY, HIGH);
  }
  else {
    Serial.println("*** Turning OFF (takes about 1.3s)");
    fona.println("AT+CPOWD=1");

    // Alternatively, pulse PWRKEY:
    //    digitalWrite(FONA_PWRKEY, LOW);
    //    delay(1400); // At least 1.2s
    //    digitalWrite(FONA_PWRKEY, HIGH);
  }
}

// This function resets the FONA, used mainly in the case of an emergency!
void FONAreset() {
  Serial.println("*** Resetting module...");
  digitalWrite(FONA_RST, LOW);
  delay(100); // Between 50-500ms
  digitalWrite(FONA_RST, HIGH);
}

// This function factory resets the SIM7000
void facReset() {
  Serial.println("*** Factory resetting...");
  Serial.print(" --> AT&F0");
  fona.println("AT&F0");
}
