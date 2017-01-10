/*

DOMOTICZ MODULE

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

template<typename T> void domoticzSend(unsigned int idx, T value) {
    #if ENABLE_DOMOTICZ
        char payload[50];
        sprintf(payload, "{\"idx\": %d, \"nvalue\": %s, \"svalue\": \"\"}", idx, String(value).c_str());
        mqttSendRaw(getSetting("dczTopicIn", DOMOTICZ_IN_TOPIC).c_str(), payload);
    #endif
}

template<typename T> void domoticzSend(const char * setting, T value) {
    #if ENABLE_DOMOTICZ
        unsigned int idx = getSetting(setting).toInt();
        if (idx > 0) domoticzSend(idx, value);
    #endif
}
