// Created by User on 6/30/2023.
//

#include <espnow.h>
#include <HardwareSerial.h>
#include <pins_arduino.h>
#include <core_esp8266_waveform.h>
#include <EEPROM.h>
#include <user_interface.h>
#include <ESP8266WiFi.h>
#include "espWrapper.hpp"
#include "structures.h"
#include "callBacks.hpp"
#include "constants.h"
espWrapper* espWrapper::espWrapper_;

espWrapper::espWrapper(){
    Serial.println("start constructor2");
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("none");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        Serial.println(esp_now_init());
        return;
    }
    Serial.println("ESP-NOW initializated succes");
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    this->initEEPromData();
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(wifiName.c_str());
    Serial.println(WiFi.macAddress());
    if (server.macAddr[0] != 0xFF && server.macAddr[0] != 0) {
        Serial.println("PAIRED TO:");
        printMAC(reinterpret_cast<const int *>(server.macAddr));
        if(addPear())
        {
            esp_now_send((server.macAddr),
                         reinterpret_cast<u8 *>(&pairingData), sizeof(pairingData));
            pairingStatus = PAIR_PAIRED;
            delay(1000);
        }  // add the peerInfo to the peer list
    }
    else{
        pairingData = structMessagePairing(initWifi, serialId);
    }
    esp_now_register_recv_cb(reinterpret_cast<esp_now_recv_cb_t>(OnDataRecv));
    esp_now_register_send_cb(reinterpret_cast<esp_now_send_cb_t>(OnDataSent));
    while (pairingStatus != PAIR_PAIRED) {
        start = millis();
        autoPairing();
        delay(100);
    }
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("notInit");
    WiFi.setSleepMode(WIFI_LIGHT_SLEEP);

}

espWrapper* espWrapper::getInstance() {
    Serial.println("start constructor");
    if(espWrapper_== nullptr){
        Serial.println("start constructor2");
        Serial.print("size of class: ");
        Serial.println(sizeof(espWrapper));
        espWrapper_ = new espWrapper();

        Serial.println("end constructor2");

    }
    Serial.println("end constructor");
    return espWrapper_;


}


bool espWrapper::addPear(uint8_t *macAddr, uint8_t channel) {
    memset(&peerInfo, 0, sizeof(peerInfo));
    esp_now_peer_info_t *peer = &peerInfo;
    memcpy(peerInfo.peer_addr, server.macAddr, sizeof(peerInfo.peer_addr));

    peerInfo.channel = server.channel;  // pick a channel
    peerInfo.encrypt = 0;     // no encryption
    // check if the peer exists
    bool exists = esp_now_is_peer_exist(peerInfo.peer_addr);
    if (exists) {
        // Slave already paired.
        Serial.println("Already Paired");
        return true;
    } else {
        auto addStatus = esp_err_t(esp_now_add_peer(peer->peer_addr, ESP_NOW_ROLE_COMBO, peer->channel, nullptr, 0));  // add the peerInfo to the peer list
        Serial.print("Pairing to ");
        printMAC(reinterpret_cast<const int *>(peer->peer_addr));
        Serial.println();

        //Serial.println(peer->channel);
        if (addStatus == ESP_OK) {
            // Pair success
            Serial.println("Pair success");
            return true;
        } else {
            Serial.println("Pair failed");
            return false;
        }
    }
}
void espWrapper::printMAC(const int* mac_addr) {
    for (int i = 0; i < 6; ++i) {
        // Use the 'printf' function to format and print each byte
        printf("%02X", mac_addr[i]);

        // Print a colon (':') separator between each byte, except for the last byte
        if (i < 5) {
            Serial.print(":");
        }
    }

    Serial.println();
}

void espWrapper::printMAC() {
    Serial.println(WiFi.macAddress());
}

void espWrapper::initEEPromData() {
    Serial.begin(115200);
    EEPROM.begin(EEPROM_ALLOC);
    Serial.print("EEPROM INIT: ");
    Serial.print(EEPROM_ALLOC);
    Serial.println(" bytes"); // Initialize EEPROM with 512 bytes of space
    if (EEPROM.read(0) != 0xFF) {
        for (int i = 0; i < EEPROM.length(); i++) {
            EEPROM.write(i, 0xFF);
        }
    }

    EEPROM.get(eepromIterator, initWifi);
    eepromIterator++;
    if(initWifi){
        EEPROM.get(eepromIterator, wifiName);
        Serial.print("Wifi name:");
        Serial.println(wifiName.c_str() );
    }
}

myData espWrapper::prepareDataToSend() {
    return {serialId, getCharge()};
}

double espWrapper::getCharge(){
    return (analogRead(A0) / 1023.0) * VREF * ((DIV_R1 + DIV_R2) / DIV_R2);
}

bool espWrapper::setWifi(char *WifiName) {
    this->wifiName = WifiName;
    WiFi.softAP(this->wifiName.c_str());
    initWifi = true;
    EEPROM.put(eepromIterator, initWifi);
    eepromIterator+=sizeof(initWifi);
    EEPROM.put(eepromIterator, wifiName);
    return true;
}



