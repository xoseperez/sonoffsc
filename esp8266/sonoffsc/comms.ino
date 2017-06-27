/*

COMMUNICATIONS MODULE

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include "SerialLink.h"
#include <Ticker.h>

SerialLink link(Serial, false);
Ticker defer;

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
        mqttSend(getSetting("mqttTopicClap", MQTT_CLAP_TOPIC).c_str(), value);
        return true;
    }

    if (strcmp_P(key, at_temp) == 0) {
        temperature = (float) value / 10;
        mqttSend(getSetting("mqttTopicTemp", MQTT_TEMPERATURE_TOPIC).c_str(), temperature);
        domoticzSend("dczIdxTemp", temperature);
        sprintf(buffer, "{\"sensorTemp\": %s}", String(temperature).c_str());
        wsSend(buffer);
        return true;
    }

    if (strcmp_P(key, at_hum) == 0) {
        humidity = value;
        mqttSend(getSetting("mqttTopicHum", MQTT_HUMIDITY_TOPIC).c_str(), humidity);
        domoticzSend("dczIdxHum", humidity);
        sprintf(buffer, "{\"sensorHum\": %d}", humidity);
        wsSend(buffer);
        return true;
    }

    if (strcmp_P(key, at_light) == 0) {
        light = value;
        mqttSend(getSetting("mqttTopicLight", MQTT_LIGHT_TOPIC).c_str(), light);
        domoticzSend("dczIdxLight", light);
        sprintf(buffer, "{\"sensorLight\": %d}", light);
        wsSend(buffer);
        return true;
    }

    if (strcmp_P(key, at_dust) == 0) {
        dust = (float) value / 100;
        mqttSend(getSetting("mqttTopicDust", MQTT_DUST_TOPIC).c_str(), dust);
        domoticzSend("dczIdxDust", dust);
        sprintf(buffer, "{\"sensorDust\": %s}", String(dust).c_str());
        wsSend(buffer);
        return true;
    }

    if (strcmp_P(key, at_noise) == 0) {
        noise = value;
        mqttSend(getSetting("mqttTopicNoise", MQTT_NOISE_TOPIC).c_str(), noise);
        domoticzSend("dczIdxNoise", noise);
        sprintf(buffer, "{\"sensorNoise\": %d}", noise);
        wsSend(buffer);
        return true;
    }

    if (strcmp_P(key, at_move) == 0) {
        movement = value;
        mqttSend(getSetting("mqttTopicMovement", MQTT_MOVE_TOPIC).c_str(), movement ? "1" : "0");
        domoticzSend("dczIdxMovement", movement);
        sprintf(buffer, "{\"sensorMove\": %d}", movement ? 1 : 0);
        wsSend(buffer);
        movement ? commsLightOn() : commsLightOff();
        return true;
    }

    gotResponse = true;
    response = value;

    return true;

}

bool send_P_repeat(const char * command, long payload, unsigned char tries = 3) {
    while (tries--) {
        link.send_P(command, payload);
        delay(10);
    }
}

void commsLightOn() {
    send_P_repeat(at_effect,47);
}

void commsLightOff() {
    send_P_repeat(at_effect,48);
}

void commsConfigure() {
    link.clear();
    delay(200);
    send_P_repeat(at_every, getSetting("sensorEvery", SENSOR_EVERY).toInt());
    send_P_repeat(at_clap, getSetting("clapEnabled", SENSOR_CLAP_ENABLED).toInt() == 1 ? 1 : 0);
    send_P_repeat(at_push,1);
    //defer.once(5, commsLightOff);
}

void commsSetup() {
    link.onGet(commsGet);
    link.onSet(commsSet);
    link.clear();
    delay(200);
    send_P_repeat(at_timeout,0);
    send_P_repeat(at_speed,100);
    send_P_repeat(at_bright,0);
    send_P_repeat(at_color,0x0000FF);
    commsLightOn();
}

void commsLoop() {
    link.handle();
}
