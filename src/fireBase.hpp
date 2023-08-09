//
// Created by User on 7/12/2023.
//

#ifndef BAUNAVI_MAIN_FIREBASE_HPP
#define BAUNAVI_MAIN_FIREBASE_HPP
#include<Firebase_ESP_Client.h>
#include<Firebase.h>
#include "structures.h"

#define API_KEY "AIzaSyCZVDiwImQyq3B9-xpylEe_3ye8jpOjUwI"
#define DATABASE_URL "https://bau-navi-default-rtdb.europe-west1.firebasedatabase.app/"

class fireBase {
private:
    fireBase();

public:
    static fireBase *getInstance();

    static fireBase *fireBase_;
    FirebaseData fbdo;

    FirebaseAuth auth;
    FirebaseConfig config;
    void sendUpdate(fireBaseData& data);
    FirebaseJson *prepareData(fireBaseData &data, int index);

    void getUpdate();
};


#endif //BAUNAVI_MAIN_FIREBASE_HPP
