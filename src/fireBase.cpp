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
        auto success = Firebase.RTDB.setArray(&fbdo, std::to_string(ESP.getChipId())+"/floors/modules/1", dataArray);
        Serial_Printf("%s\n", success ? "OK" : fbdo.errorReason().c_str());
    } else {
        Serial_Printf("Failed to prepare data array.\n");
    }
    delete dataArray;
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

void fireBase::getUpdate() {
    if (!Firebase.ready()){
        Serial.println("Firebase not init!");
        return;
    }
    bool update = false;
    Firebase.RTDB.getBool(&fbdo, std::to_string(ESP.getChipId())+"/update", &update);

    if(!update){
        Serial_Printf("No update.\n");
        return;
    }
    Serial_Printf("Getting array...");
    FirebaseJsonArray dataArray;
    Firebase.RTDB.getArray(&fbdo, std::to_string(ESP.getChipId())+"/floors/modules/1", &dataArray);

    if ( Firebase.RTDB.getArray(&fbdo,
                                std::to_string(ESP.getChipId())+"/floors/modules",
                                &dataArray)) {
        String dataArrayStr;
        dataArray.toString(dataArrayStr);
        Serial_Printf("OK\n");
        Serial_Printf("Array size: %d\n", dataArray.size());
        for (size_t i = 0; i < dataArray.size(); i++) {
            FirebaseJsonData json;
            dataArray.get(json, (std::to_string(i)+"/serialId"));
            Serial.println((json.to<int>()));
            dataArray.get(json, (std::to_string(i)+"/Role"));
            Serial.println((json.to<int>()));
            //EspRole role = (EspRole)json.to<int>();
            dataArray.get(json, (std::to_string(i)+"/Status"));
            Serial.println((json.to<String>()));
            dataArray.get(json, (std::to_string(i)+"/Charge"));
            Serial.println((json.to<int>()));
            Serial.println(json.to<const char*>());
            Serial_Printf("---------\n");
        }
    } else {
        Serial_Printf("Failed to get data array.\n");
    }
}
