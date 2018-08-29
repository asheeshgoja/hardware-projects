FONAFlashStringPtr ok_reply;


boolean ups_fona_begin() {

  if (FONA_RST != 99) { // Pulse the reset pin only if it's not an LTE module
    Serial.println(F("Resetting the module..."));
    pinMode(FONA_RST, OUTPUT);
    digitalWrite(FONA_RST, HIGH);
    delay(10);
    digitalWrite(FONA_RST, LOW);
    delay(100);
    digitalWrite(FONA_RST, HIGH);
  }

  Serial.println(F("Attempting to open comm with ATs"));
  // give 7 seconds to reboot
  int16_t timeout = 7000;

  while (timeout > 0) {
    while (fonaSerial->available()) fonaSerial->read();
    if (sendCheckReply(F("AT"), ok_reply, FONA_DEFAULT_TIMEOUT_MS ) )
      break;
    while (fonaSerial->available()) fonaSerial->read();
    if (sendCheckReply(F("AT"), F("AT"),FONA_DEFAULT_TIMEOUT_MS )) 
      break;
    delay(500);
    timeout-=500;
  }

  if (timeout <= 0) {
#ifdef ADAFRUIT_FONA_DEBUG
    Serial.println(F("Timeout: No response to AT... last ditch attempt."));
#endif
    sendCheckReply(F("AT"), ok_reply, FONA_DEFAULT_TIMEOUT_MS);
    delay(100);
    sendCheckReply(F("AT"), ok_reply, FONA_DEFAULT_TIMEOUT_MS);
    delay(100);
    sendCheckReply(F("AT"), ok_reply, FONA_DEFAULT_TIMEOUT_MS);
    delay(100);
  }

  // turn off Echo!
  sendCheckReply(F("ATE0"), ok_reply, FONA_DEFAULT_TIMEOUT_MS);
  delay(100);

  if (! sendCheckReply(F("ATE0"), ok_reply, FONA_DEFAULT_TIMEOUT_MS )) {
    return false;
  }

  // turn on hangupitude
  if (FONA_RST != 99) sendCheckReply(F("AT+CVHU=0"), ok_reply, FONA_DEFAULT_TIMEOUT_MS);

  delay(100);
  flushInput();


  Serial.print(F("\t---> ")); Serial.println("ATI");

  fonaSerial->println("ATI");
  readline(500, true);

  Serial.print (F("\t<--- ")); Serial.println(replybuffer);


//
//  if (prog_char_strstr(replybuffer, (prog_char *)F("SIM808 R14")) != 0) {
//    _type = FONA808_V2;
//  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIM808 R13")) != 0) {
//    _type = FONA808_V1;
//  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIM800 R13")) != 0) {
//    _type = FONA800L;
//  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIMCOM_SIM5320A")) != 0) {
//    _type = FONA3G_A;
//  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIMCOM_SIM5320E")) != 0) {
//    _type = FONA3G_E;
//  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIM7000A R13")) != 0) {
//    _type = FONA_LTE_A;
//  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIM7000C R13")) != 0) {
//    _type = FONA_LTE_C;
//  } else if (prog_char_strstr(replybuffer, (prog_char *)F("SIM7000E R13")) != 0) {
//    _type = FONA_LTE_E;
//  } 


//  if (_type == FONA800L) {
//    // determine if L or H
//
//  Serial.print(F("\t---> ")); Serial.println("AT+GMM");
//
//    fonaSerial->println("AT+GMM");
//    readline(500, true);
//
//  Serial.print (F("\t<--- ")); Serial.println(replybuffer);
//
//
////    if (prog_char_strstr(replybuffer, (prog_char *)F("SIM800H")) != 0) {
////      //_type = FONA800H;
////    }
//  }

#if defined(FONA_PREF_SMS_STORAGE)
    sendCheckReply(F("AT+CPMS=" FONA_PREF_SMS_STORAGE "," FONA_PREF_SMS_STORAGE "," FONA_PREF_SMS_STORAGE), ok_reply);
#endif

  return true;

}
