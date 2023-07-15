//
// Created by User on 7/11/2023.
//

#ifndef BAUNAVI_MAIN_ESPWRAPPER_HPP
#define BAUNAVI_MAIN_ESPWRAPPER_HPP



#include <cstdint>
#include <string>
#include "structures.h"

class espWrapper {
public:
    uint8_t macAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t channel=1;
    long long start;
    connectionData clients[5]= {connectionData()};
    uint32_t serialId = 0;
    messagePairing pairingData = messagePairing(true, 0);
    esp_now_peer_info_t peerInfo{};
    std::string wifiName="null";
    bool initWifi = false;
    unsigned long previousMillis{};
    double charge=-1;
    int messageSended=0;
    int conCount = 0;
    PairingStatus pairingStatus = PAIR_REQUEST;
    uint16_t eepromIterator = 1;
    static espWrapper *espWrapper_;
    static void printMAC();
    double getCharge();
    bool addPear(uint8_t *macAddr, uint8_t channel, connectionData conData);
    static void printMAC(const uint8_t* mac_addr);
    static espWrapper* getInstance();
    void initEEPromData();
    unsigned long currentMillis{};
    bool setWifi(char* WifiName);
    myData prepareDataToSend();

private:
    espWrapper();



};



#endif //BAUNAVI_MAIN_ESPWRAPPER_HPP
