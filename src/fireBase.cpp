//
// Created by User on 7/12/2023.
//

#include "fireBase.hpp"
#include "addons/TokenHelper.h"
#include "structures.h"

fireBase* fireBase::fireBase_ = nullptr;
fireBase::fireBase() {
    //FIREBASE PART
    config.api_key = API_KEY;

    /* Assign the RTDB URL (required) */
    config.database_url = DATABASE_URL;
    // authentication through email
    //auth.user.email = USER_EMAIL;
    //auth.user.password = USER_PASSWORD;
    /* Sign up */
    if (Firebase.signUp(&config, &auth, "", "")){
        Serial.println("ok");
    }
    else{
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

fireBase *fireBase::getInstance() {
    if(fireBase_==nullptr){
        fireBase_ = new fireBase();
    }
    return fireBase_;
}

void fireBase::sendUpdate(fireBaseData& data) {
    if (!Firebase.ready()){
        Serial.println("Firebase not init!");
        return;
    }

    Serial_Printf("Setting array...");
    auto dataArray = prepareData(data);
    if (dataArray != nullptr) {
        auto success = Firebase.RTDB.setArray(&fbdo, std::to_string(ESP.getChipId())+"/floors/modules/1", std::move(dataArray));
        Serial_Printf("%s\n", success ? "OK" : fbdo.errorReason().c_str());
    } else {
        Serial_Printf("Failed to prepare data array.\n");
    }
    delete dataArray;
}

FirebaseJsonArray *fireBase::prepareMac(uint8_t *macAddr) {
    FirebaseJsonArray* array = new FirebaseJsonArray;
    for (int i = 0; i < 6; i++) {
        array->add(macAddr[i]);
    }

    return array;
}

FirebaseJsonArray *fireBase::prepareData(fireBaseData& data) {
    FirebaseJsonArray* array = new FirebaseJsonArray;
    FirebaseJson json;
    for (auto& x : data.espData) {

        FirebaseJson json;

        json.add("serialId", x.serialId);
        json.add("Role", (int)x.role);
        json.add("Status", "WORKING");
        json.add("Charge", x.charge);

        array->add(json);

    }
    String str;
    array->toString(str, true /* prettify option */);
    Serial.println("\n---------");
    Serial.println(str);
    return array;
}


char *fireBase::getWiFi() {
    return nullptr;
}