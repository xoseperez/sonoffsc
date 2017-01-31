/*

COMMUNICATIONS MODULE

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include "SerialLink.h"
SerialLink link(Serial, false);

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
const PROGMEM char at_r[] = "AT+R";
const PROGMEM char at_g[] = "AT+G";
const PROGMEM char at_b[] = "AT+B";
const PROGMEM char at_effect[] = "AT+EFFECT";
const PROGMEM char at_rgb_exec[] = "AT+RGBEXEC";

// -----------------------------------------------------------------------------
// VALUES
// -----------------------------------------------------------------------------

float temperature;
int humidity;
int light;
float dust;
int noise;
uint32 rgb;

float getTemperature() { return temperature; }
float getHumidity() { return humidity; }
float getLight() { return light; }
float getDust() { return dust; }
float getNoise() { return noise; }
uint32 getRGB() { return rgb; }

// -----------------------------------------------------------------------------
// COMMUNICATIONS
// -----------------------------------------------------------------------------

bool commsGet(char * key) {
    return false;
}

bool commsSet(char * key, int value) {

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

    return false;

}

void commConfigure() {
    link.send_P(at_every, getSetting("sensorEvery", SENSOR_EVERY).toInt());
    link.send_P(at_clap, getSetting("clapEnabled", SENSOR_CLAP_ENABLED).toInt() == 1 ? 1 : 0);
    link.send_P(at_push, 1);
}

void commsSetup() {

    link.onGet(commsGet);
    link.onSet(commsSet);

    static WiFiEventHandler e3 = WiFi.onStationModeConnected([](const WiFiEventStationModeConnected& event) {
        commConfigure();
    });

    static WiFiEventHandler e4 = WiFi.onSoftAPModeStationConnected([](const WiFiEventSoftAPModeStationConnected& event) {
        commConfigure();
    });

}

void commsLoop() {
    link.handle();
}

void commSendRGB(int r, int g, int b) {

	link.send_P(at_r, r);
	link.send_P(at_g, g);
	link.send_P(at_b, b);
	link.send_P(at_rgb_exec, 1);
}