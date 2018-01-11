/*

DOMOTICZ MODULE

Copyright (C) 2016-2018 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if DOMOTICZ_SUPPORT

#include <ArduinoJson.h>

bool _dcz_enabled = false;

//------------------------------------------------------------------------------
// Private methods
//------------------------------------------------------------------------------

#if MQTT_SUPPORT

void _domoticzMqttSubscribe(bool value) {

    String dczTopicOut = getSetting("dczTopicOut", DOMOTICZ_OUT_TOPIC);
    if (value) {
        mqttSubscribeRaw(dczTopicOut.c_str());
    } else {
        mqttUnsubscribeRaw(dczTopicOut.c_str());
    }

}

#endif // MQTT_SUPPORT

#if WEB_SUPPORT

void _domoticzWebSocketOnSend(JsonObject& root) {

    root["dczVisible"] = 1;
    root["dczEnabled"] = getSetting("dczEnabled", DOMOTICZ_ENABLED).toInt() == 1;
    root["dczTopicIn"] = getSetting("dczTopicIn", DOMOTICZ_IN_TOPIC);
    root["dczTopicOut"] = getSetting("dczTopicOut", DOMOTICZ_OUT_TOPIC);

    root["dczIdxTemp"] = getSetting("dczIdxTemp").toInt();
    root["dczIdxHum"] = getSetting("dczIdxHum").toInt();
    root["dczIdxNoise"] = getSetting("dczIdxNoise").toInt();
    root["dczIdxMovement"] = getSetting("dczIdxMovement").toInt();
    root["dczIdxLight"] = getSetting("dczIdxLight").toInt();
    root["dczIdxDust"] = getSetting("dczIdxDust").toInt();

}

#endif // WEB_SUPPORT

void _domoticzConfigure() {
    _dcz_enabled = getSetting("dczEnabled", DOMOTICZ_ENABLED).toInt() == 1;
	#if MQTT_SUPPORT
	    _domoticzMqttSubscribe(_dcz_enabled);
	#endif
}

//------------------------------------------------------------------------------
// Public API
//------------------------------------------------------------------------------

template<typename T> void domoticzSend(const char * key, T nvalue, const char * svalue) {
    if (!_dcz_enabled) return;
    unsigned int idx = getSetting(key).toInt();
    if (idx > 0) {
        char payload[128];
        snprintf(payload, sizeof(payload), "{\"idx\": %d, \"nvalue\": %s, \"svalue\": \"%s\"}", idx, String(nvalue).c_str(), svalue);
        mqttSendRaw(getSetting("dczTopicIn", DOMOTICZ_IN_TOPIC).c_str(), payload);
    }
}

template<typename T> void domoticzSend(const char * key, T nvalue) {
    domoticzSend(key, nvalue, "");
}

void domoticzSetup() {
    _domoticzConfigure();
    #if WEB_SUPPORT
        wsOnSendRegister(_domoticzWebSocketOnSend);
        wsOnAfterParseRegister(_domoticzConfigure);
    #endif
}

bool domoticzEnabled() {
    return _dcz_enabled;
}

#endif
