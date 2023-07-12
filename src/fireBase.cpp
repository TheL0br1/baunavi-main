//
// Created by User on 7/12/2023.
//

#include "fireBase.hpp"
#include "addons/TokenHelper.h"
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
