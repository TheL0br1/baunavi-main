#include<SD.h>
#include <Firebase_ESP_Client.h>
#include<Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <EEPROM.h>
#include "structures.h"
#include "espWrapper.hpp"

espWrapper *pEspWrapper;
void setup() {
    Serial.begin(115200);
    Serial.println("esp started");
    pEspWrapper = espWrapper::getInstance();


}



void loop() {




}