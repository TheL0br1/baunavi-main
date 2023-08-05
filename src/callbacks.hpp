//
// Created by User on 7/11/2023.
//
// callback functions for esp-now

#ifndef BAUNAVI_MAIN_CALLBACKS_HPP
#define BAUNAVI_MAIN_CALLBACKS_HPP

#include <HardwareSerial.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "espWrapper.hpp"

// Callback when data is sent
void OnDataRecv(uint8_t *mac_addr, uint8_t *incomingData, int len) {
    espWrapper *pEspWrapper = espWrapper::getInstance();
    Serial.print(len);
    Serial.print(" bytes of data received from : ");
    pEspWrapper->printMAC(mac_addr);
    myData data = myData(ESP.getChipId(), 0.1, );
    messagePairing pairing = messagePairing(ESP.getChipId(), MAIN);
    auto recievedPairing = messagePairing(0, MAIN);
    Serial.println();
    uint8_t type = incomingData[0];  // first message byte is the type of message
    switch (type) {
        case DATA:  // the message is data type
            memcpy_P(&data, incomingData, sizeof(myData));
            data.print();
            pEspWrapper->updateData(data);
            break;


        case PAIRING:  // the message is a pairing request
            memcpy(&recievedPairing, incomingData, sizeof(messagePairing));
            Serial.println(pairing.msgType);
            Serial.print("Pairing request from: ");
            pEspWrapper->printMAC(mac_addr);
            Serial.println();
            Serial.println(pairing.channel);
            Serial.println("pairingMessage data");
            recievedPairing.print();
            if (true) {  // do not replay to server itself
                // Server is in AP_STA mode: peers need to send data to server soft AP MAC address

                WiFi.macAddress(pairing.macAddr);
                pairing.channel = WiFi.channel();
                pairing.msgType = PAIRING;
                pairing.serialId = ESP.getChipId();
                pairing.role = MAIN;
                Serial.println("send response");

                if(pEspWrapper->addPear(mac_addr, recievedPairing.channel)){
                }
                WiFi.softAPmacAddress(pairing.macAddr);
                if (esp_now_is_peer_exist((mac_addr))) {
                    esp_err_t result = esp_err_t(esp_now_send(mac_addr, (uint8_t *) &pairing, sizeof(messagePairing)));
                    Serial.println(result);
                    if(result == ESP_OK){
                        Serial.println("PEER CONNECTED");
                        pEspWrapper->addClient(recievedPairing);
                    } else {
                        Serial.println("PEER NOT CONNECTED");
                    }
                } else {
                    Serial.println("PEER NOT CONNECTED");
                }
            }
    }
}

void OnDataSent(uint8_t *mac_addr, int sendStatus) {
    espWrapper *pEspWrapper = espWrapper::getInstance();
    Serial.print("Last Packet Send Status: ");
    if (sendStatus == ESP_OK) {
        Serial.println("Delivery Success to ");
        pEspWrapper->printMAC(mac_addr);

    } else {
        Serial.println("Delivery Failed to ");
        pEspWrapper->printMAC(mac_addr);
        Serial.print("Status:");
        Serial.println(sendStatus);
    }
    Serial.println();
}

#endif //BAUNAVI_MAIN_CALLBACKS_HPP
