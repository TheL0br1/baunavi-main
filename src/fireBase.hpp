//
// Created by User on 7/12/2023.
//

#ifndef BAUNAVI_MAIN_FIREBASE_HPP
#define BAUNAVI_MAIN_FIREBASE_HPP
#include<Firebase_ESP_Client.h>
#include<Firebase.h>

#define API_KEY "AIzaSyCZVDiwImQyq3B9-xpylEe_3ye8jpOjUwI"
#define DATABASE_URL "https://bau-navi-default-rtdb.europe-west1.firebasedatabase.app/"
#define USER_EMAIL "l0br1artem@gmail.com"
#define USER_PASSWORD "artem0077702005"
class fireBase {
private:
    fireBase();

public:
    static fireBase *getInstance();

    static fireBase *fireBase_;
    FirebaseData fbdo;

    FirebaseAuth auth;
    FirebaseConfig config;
};


#endif //BAUNAVI_MAIN_FIREBASE_HPP
