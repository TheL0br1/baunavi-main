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
    WiFi.forceSleepBegin();
    WiFi.softAP(pEspWrapper->wifiName.c_str());

    delay(10000);
    WiFi.forceSleepWake();
    delay(1);
    WiFi.softAP(pEspWrapper->wifiName.c_str());
    delay(10);
    //Set values to send


    // Send message via ESP-NOW to all peers
    myData dataToSend = pEspWrapper->prepareDataToSend();
    int res = esp_now_send(pEspWrapper->server.macAddr, reinterpret_cast<u8 *>(&dataToSend), sizeof(dataToSend));
    espWrapper::printMAC(reinterpret_cast<const int *>(pEspWrapper->server.macAddr));
    Serial.print(res);

}