//
// Created by User on 7/11/2023.
//

#ifndef BAUNAVI_MAIN_CALLBACKS_HPP
#define BAUNAVI_MAIN_CALLBACKS_HPP

#include <HardwareSerial.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "espWrapper.hpp"

// Callback when data is sent
void OnDataRecv(int *mac_addr, int*incomingData, int len) {
    espWrapper *pEspWrapper = espWrapper::getInstance();
    Serial.print(len);
    Serial.print(" bytes of data received from : ");
    pEspWrapper->printMAC(mac_addr);

    Serial.println();
    uint8_t type = incomingData[0];  // first message byte is the type of message
    switch (type) {
        case DATA:  // the message is data type
            myData data = myData(0, 0);
            memcpy(&data, incomingData, sizeof(myData));
            Serial.print("Charge_v: ");
            Serial.println(data.charge);
            Serial.println();
            break;


        case PAIRING:  // the message is a pairing request
            memcpy(&pairingData, incomingData, sizeof(pairingData));
            Serial.println(pairingData.msgType);
            Serial.println(pairingData.id);
            Serial.print("Pairing request from: ");
            printMAC(mac_addr);
            Serial.println();
            Serial.println(pairingData.channel);
            if (pairingData.id > 0) {  // do not replay to server itself
                if (pairingData.msgType == PAIRING) {
                    pairingData.id = 0;  // 0 is server
                    // Server is in AP_STA mode: peers need to send data to server soft AP MAC address
                    memcpy(&pairingData.macAddr, mac_addr, sizeof(pairingData.macAddr));
                    pairingData.channel = chan;
                    Serial.println("send response");
                    memcpy(&clients[conCount].macAddr, mac_addr, sizeof(clients[conCount].macAddr));
                    clients[conCount].channel = chan;
                    EEPROM.put(sizeof(connection_data) * conCount + 1, clients[conCount]);
                    EEPROM.commit();
                    if(addPeer(mac_addr)){
                        conCount++;
                    }
                    WiFi.softAPmacAddress(pairingData.macAddr);
                    if (esp_now_is_peer_exist(mac_addr)) {
                        esp_err_t result = esp_err_t(esp_now_send(mac_addr, (uint8_t *)&pairingData, sizeof(pairingData)));
                        Serial.println(result);
                    } else {
                        Serial.println("PEER NOT CONNECTED");
                    }

                }
            }
            break;
    }
}
void OnDataSent(int *mac_addr, int sendStatus) {
    Serial.print("Last Packet Send Status: ");
    if (sendStatus == ESP_OK) {
        Serial.println("Delivery Success to ");
        printMAC(mac_addr);

    } else {
        Serial.println("Delivery Failed to ");
        printMAC(mac_addr);
        Serial.print("Status:");
        Serial.println(sendStatus);
    }
    Serial.println();
}
#endif //BAUNAVI_MAIN_CALLBACKS_HPP
