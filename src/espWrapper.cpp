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
    Serial.println(WiFi.macAddress());
    this->initWiFi();
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        Serial.println(esp_now_init());
        return;
    }
    Serial.println("ESP-NOW initializated succes");
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  //  this->initEEPromData();
    Serial.println(WiFi.macAddress());
    esp_now_register_recv_cb(reinterpret_cast<esp_now_recv_cb_t>(OnDataRecv));
    esp_now_register_send_cb(reinterpret_cast<esp_now_send_cb_t>(OnDataSent));
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(std::to_string(ESP.getChipId()).c_str());
    Serial.println(WiFi.macAddress());
    WiFi.macAddress(macAddr);
    updateData(myData{serialId, getCharge(), MAIN});


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
    memcpy(peerInfo.peer_addr, macAddr, sizeof(peerInfo.peer_addr));

    peerInfo.channel = channel;  // pick a channel
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
        printMAC(peer->peer_addr);
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
void espWrapper::printMAC(const uint8_t* mac_addr){
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print(macStr);
    Serial.println();
}

void espWrapper::printMAC() {
    Serial.println(WiFi.macAddress());
}



double espWrapper::getCharge(){
    return (analogRead(A0) / 1023.0) * VREF * ((DIV_R1 + DIV_R2) / DIV_R2);
}


fireBaseData espWrapper::prepareDataToFireBase() {
    return fireBaseData_;
}

void espWrapper::initWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(300);
    }
    Serial.println("Access Point Created");
}

void espWrapper::updateData(myData data) {
    bool flag = false;
    for(auto &x : fireBaseData_.espData){
        if(x.serialId == data.serialId){
            x = EspData(data);
            flag = true;
            return;
        }
    }
    if(!flag){
        fireBaseData_.espData.push_back(EspData(data));
    }

}

void espWrapper::addClient(messagePairing data) {
    clients[conCount] = connectionData(data);
    conCount++;
}



