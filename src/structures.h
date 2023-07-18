//for esp8266-32 compatibility
#include <cstdint>
#include<list>
typedef u8 uint8_t;
#ifndef ESP32
typedef enum {
    ESP_OK = 0,                 /*!< No error */
    ESP_ERR_NO_MEM,             /*!< Out of memory */
    ESP_ERR_INVALID_ARG,        /*!< Invalid argument */
    ESP_ERR_INVALID_STATE,      /*!< Invalid state */
    ESP_ERR_INVALID_SIZE,       /*!< Invalid size */
    ESP_ERR_NOT_FOUND,          /*!< Not found */
    ESP_ERR_NOT_SUPPORTED,      /*!< Not supported */
    ESP_ERR_TIMEOUT,            /*!< Timeout */
    ESP_ERR_INVALID_RESPONSE,   /*!< Invalid response */
    ESP_ERR_INVALID_CRC,        /*!< Invalid CRC */
    ESP_ERR_INVALID_VERSION,    /*!< Invalid version */
    ESP_ERR_INVALID_MAC,        /*!< Invalid MAC address */
    ESP_ERR_WIFI_BASE = 0x3000, /*!< Starting number of WiFi error codes */
    ESP_ERR_MESH_BASE = 0x4000, /*!< Starting number of Mesh error codes */
} esp_err_t;

typedef struct {
    uint8_t peer_addr[6]; /**< MAC address of the peer */
    uint8_t channel;      /**< Communication channel */
    uint8_t ifidx;        /**< Interface index */
    uint8_t encrypt;      /**< Peer encryption flag */
} esp_now_peer_info_t;
#endif

//structures for messaging
enum MessageType { PAIRING, DATA,
    SET_INIT,
};
enum PairingStatus { PAIR_REQUEST,
    PAIR_REQUESTED,
    PAIR_PAIRED,
};
enum EspRole { MAIN,
    SWITCH,
    BASE,
};
struct structMessage {
    MessageType msgType;
    char WiFiName[99];


};

struct messagePairing {
    MessageType msgType;
    uint8_t macAddr[6];
    uint8_t channel;
    EspRole role;
    uint32_t serialId;
    bool initWifi;
    messagePairing(bool initWifi, uint32_t serialId, EspRole role) :
    msgType(PAIRING),initWifi(initWifi), serialId(serialId), role(role){
        WiFi.macAddress(macAddr);
        channel = WiFi.channel();
    }
};
struct connectionData{
    MessageType msgType;
    EspRole role;
    uint8_t *macAddr;
    uint8_t channel;
    connectionData() :channel(1), role(MAIN), msgType(DATA) {
        macAddr = new uint8_t[6]; // Выделение памяти для массива macAddr
        for (int i = 0; i < 6; ++i) {
            macAddr[i] = 0xFF;
        }
    }
    connectionData(uint8_t channel, uint8_t* macAddr, EspRole role):
    channel(channel), role(role), msgType(DATA){
        this->macAddr = new uint8_t[6]; // Выделение памяти для массива macAddr
        memcpy_P(this->macAddr, macAddr, sizeof(uint8_t)*6);

    }

};

struct myData{
    MessageType type;
    EspRole role;
    uint32_t serialId;
    double charge;
    myData(uint32_t serialId, double charge): type(DATA), serialId(serialId), role(BASE), charge(charge){

    }
    void print(){
        Serial.print("MessageType: ");
        Serial.println(this->type);
        Serial.print("Role: ");
        Serial.println(this->role);
        Serial.print("Serial id ");
        Serial.println(this->serialId);
        Serial.print("Charge: ");
        Serial.println(this->charge);
    }
};
struct EspData{
    EspRole role;
    uint32_t serialId;
    double charge;
    char WifiName[99];
    EspData(connectionData data){
        this->role = data.role;
        this->serialId = data.serialId;
        this->charge = data.charge;
        memcpy(this->WifiName, data.WiFiName, sizeof(char)*99);
    }

};
struct fireBaseData{
    std::list<EspData> espData;
    int floor;
    fireBaseData(int floor, std::list<EspData> espData): floor(floor), espData(espData){

    }
};