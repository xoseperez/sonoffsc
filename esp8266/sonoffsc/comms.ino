/*

COMMUNICATIONS MODULE

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include "SerialLink.h"

SerialLink link(Serial, false);

const PROGMEM char at_hello[] = "AT+HELLO";
const PROGMEM char at_push[] = "AT+PUSH";
const PROGMEM char at_every[] = "AT+EVERY";
const PROGMEM char at_temp[] = "AT+TEMP";
const PROGMEM char at_hum[] = "AT+HUM";
const PROGMEM char at_dust[] = "AT+DUST";
const PROGMEM char at_noise[] = "AT+NOISE";
const PROGMEM char at_light[] = "AT+LIGHT";
const PROGMEM char at_clap[] = "AT+CLAP";
const PROGMEM char at_code[] = "AT+CODE";
const PROGMEM char at_thld[] = "AT+THLD";
const PROGMEM char at_fan[] = "AT+FAN";
const PROGMEM char at_fanoff[] = "AT+FANOFF";
const PROGMEM char at_timeout[] = "AT+TIMEOUT";
const PROGMEM char at_effect[] = "AT+EFFECT";
const PROGMEM char at_color[] = "AT+COLOR";
const PROGMEM char at_bright[] = "AT+BRIGHT";
const PROGMEM char at_speed[] = "AT+SPEED";
const PROGMEM char at_move[] = "AT+MOVE";

// -----------------------------------------------------------------------------
// VALUES
// -----------------------------------------------------------------------------

float temperature;
int humidity;
int light;
float dust;
int noise;
bool movement;

bool gotResponse = false;
long response;

float getTemperature() { return temperature; }
float getHumidity() { return humidity; }
float getLight() { return light; }
float getDust() { return dust; }
float getNoise() { return noise; }
float getMovement() { return movement; }

// -----------------------------------------------------------------------------
// COMMUNICATIONS
// -----------------------------------------------------------------------------

bool commsGet(char * key) {
    return false;
}

bool commsSet(char * key, long value) {

    char buffer[50];

    if (strcmp_P(key, at_code) == 0) {
        mqttSend(getSetting("mqttTopicClap", MQTT_TOPIC_CLAP).c_str(), String(value).c_str());
        return true;
    }

    if (strcmp_P(key, at_temp) == 0) {
        temperature = (float) value / 10;
        if (temperature < SENSOR_TEMPERATURE_MIN || SENSOR_TEMPERATURE_MAX < temperature) return false;
        mqttSend(getSetting("mqttTopicTemp", MQTT_TOPIC_TEMPERATURE).c_str(), String(temperature).c_str());
        domoticzSend("dczIdxTemp", temperature);
        sprintf(buffer, "{\"sensorTemp\": %s}", String(temperature).c_str());
        wsSend(buffer);
        return true;
    }

    if (strcmp_P(key, at_hum) == 0) {
        humidity = value;
        if (humidity < SENSOR_HUMIDITY_MIN || SENSOR_HUMIDITY_MAX < humidity) return false;
        mqttSend(getSetting("mqttTopicHum", MQTT_TOPIC_HUMIDITY).c_str(), String(humidity).c_str());
        domoticzSend("dczIdxHum", humidity);
        sprintf(buffer, "{\"sensorHum\": %d}", humidity);
        wsSend(buffer);
        return true;
    }

    if (strcmp_P(key, at_light) == 0) {
        light = value;
        if (light < 0 || 100 < light) return false;
        mqttSend(getSetting("mqttTopicLight", MQTT_TOPIC_LIGHT).c_str(), String(light).c_str());
        domoticzSend("dczIdxLight", light);
        sprintf(buffer, "{\"sensorLight\": %d}", light);
        wsSend(buffer);
        return true;
    }

    if (strcmp_P(key, at_dust) == 0) {
        dust = (float) value / 100;
        if (dust < SENSOR_DUST_MIN || SENSOR_DUST_MAX < dust) return false;
        mqttSend(getSetting("mqttTopicDust", MQTT_TOPIC_DUST).c_str(), String(dust).c_str());
        domoticzSend("dczIdxDust", dust);
        sprintf(buffer, "{\"sensorDust\": %s}", String(dust).c_str());
        wsSend(buffer);
        return true;
    }

    if (strcmp_P(key, at_noise) == 0) {
        noise = value;
        if (noise < 0 || 100 < noise) return false;
        mqttSend(getSetting("mqttTopicNoise", MQTT_TOPIC_NOISE).c_str(), String(noise).c_str());
        domoticzSend("dczIdxNoise", noise);
        sprintf(buffer, "{\"sensorNoise\": %d}", noise);
        wsSend(buffer);
        return true;
    }

    if (strcmp_P(key, at_move) == 0) {
        movement = value;
        mqttSend(getSetting("mqttTopicMovement", MQTT_TOPIC_MOVE).c_str(), movement ? "1" : "0");
        domoticzSend("dczIdxMovement", movement);
        sprintf(buffer, "{\"sensorMove\": %d}", movement ? 1 : 0);
        wsSend(buffer);
        #if LOCAL_NOTIFICATION
            sendNotification(movement);
        #endif
        return true;
    }

    gotResponse = true;
    response = value;

    return true;

}

bool send_P_repeat(const char * command, long payload, unsigned char tries = COMMS_DEFAULT_TRIES) {
    link.clear();
    while (tries--) {
        delay(50);
        link.send_P(command, payload);
    }
}

void commsConfigure() {
    link.clear();
    delay(200);
    send_P_repeat(at_every, getSetting("sensorEvery", SENSOR_EVERY).toInt());
    send_P_repeat(at_clap, getSetting("clapEnabled", SENSOR_CLAP_ENABLED).toInt() == 1 ? 1 : 0);
    send_P_repeat(at_push,1);
}

void commsSetup() {

    link.onGet(commsGet);
    link.onSet(commsSet);
    link.clear();
    delay(200);

    // Set FAN mode depending on delay
    send_P_repeat(at_fanoff, FAN_DELAY);
    send_P_repeat(at_fan, FAN_DELAY == 0);

}

void commsLoop() {
    link.handle();
}
