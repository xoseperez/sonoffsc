/*

LIGHTS MODULE

Copyright (C) 2016-2018 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <Ticker.h>

Ticker _lights_defer;
unsigned char _channels[3] = {0};

// -----------------------------------------------------------------------------
// LIGHTS
// -----------------------------------------------------------------------------

void _fromRGB(const char * rgb) {

    char * p = (char *) rgb;
    if (strlen(p) == 0) return;

    // if color begins with a # then assume HEX RGB
    if (p[0] == '#') {

        ++p;
        unsigned long value = strtoul(p, NULL, 16);
        _channels[0] = (value >> 16) & 0xFF;
        _channels[1] = (value >> 8) & 0xFF;
        _channels[2] = (value) & 0xFF;

    // it's a temperature in mireds
    } else if (p[0] == 'M') {

        unsigned long mireds = atol(p + 1);
        _fromMireds(mireds);

    // it's a temperature in kelvin
    } else if (p[0] == 'K') {

        unsigned long kelvin = atol(p + 1);
        _fromKelvin(kelvin);

    // otherwise assume decimal values separated by commas
    } else {

        char * tok;
        unsigned char count = 0;

        tok = strtok(p, ",");
        while (tok != NULL) {
            _channels[count] = atoi(tok);
            if (++count == 3) break;
            tok = strtok(NULL, ",");
        }

        // RGB but less than 3 values received
        if (count < 3) {
            _channels[1] = _channels[0];
            _channels[2] = _channels[0];
        }

    }

}

// Thanks to Sacha Telgenhof for sharing this code in his AiLight library
// https://github.com/stelgenhof/AiLight
void _fromKelvin(unsigned long kelvin) {

    // Calculate colors
    unsigned int red = (kelvin <= 66)
        ? 255
        : 329.698727446 * pow((kelvin - 60), -0.1332047592);
    unsigned int green = (kelvin <= 66)
        ? 99.4708025861 * log(kelvin) - 161.1195681661
        : 288.1221695283 * pow(kelvin, -0.0755148492);
    unsigned int blue = (kelvin >= 66)
        ? 255
        : ((kelvin <= 19)
            ? 0
            : 138.5177312231 * log(kelvin - 10) - 305.0447927307);

    // Save values
    _channels[0] = constrain(red, 0, 255);
    _channels[1] = constrain(green, 0, 255);
    _channels[2] = constrain(blue, 0, 255);

}

// Color temperature is measured in mireds (kelvin = 1e6/mired)
void _fromMireds(unsigned long mireds) {
    if (mireds == 0) mireds = 1;
    unsigned long kelvin = constrain(1000000UL / mireds, 1000, 40000) / 100;
    _fromKelvin(kelvin);
}

void _lightsMqttCallback(unsigned int type, const char * topic, const char * payload) {

	// When connected, subscribe to the topic
	if (type == MQTT_CONNECT_EVENT) {
		mqttSubscribe(MQTT_TOPIC_RGB);
        mqttSubscribe(MQTT_TOPIC_EFFECT);
        mqttSubscribe(MQTT_TOPIC_BRIGHTNESS);
        mqttSubscribe(MQTT_TOPIC_SPEED);
	}

	// Messages
	if (type == MQTT_MESSAGE_EVENT) {

        String t = mqttSubtopic((char*) topic);

        if (t.equals(MQTT_TOPIC_RGB)) {
			sendColor((char*) payload);
		}

        if (t.equals(MQTT_TOPIC_BRIGHTNESS)) {
            unsigned char brightness = constrain(atoi(payload), 0, 255);
			sendBrightness(brightness);
		}

        if (t.equals(MQTT_TOPIC_SPEED)) {
            unsigned char speed = constrain(atoi(payload), 0, 255);
			sendSpeed(speed);
		}

        if (t.equals(MQTT_TOPIC_EFFECT)) {
            unsigned char effect = constrain(atoi(payload), 0, 53);
			sendEffect(effect);
		}

	}

}

//------------------------------------------------------------------------------

void sendEffect(long effect) {
    DEBUG_MSG_P(PSTR("[LIGHTS] Effect to %d\n"), effect);
    commsSend(at_effect, effect);
}

void sendColor(unsigned long color) {
    DEBUG_MSG_P(PSTR("[LIGHTS] Color to %lu\n"), color);
    commsSend(at_color, color);
}

void sendBrightness(unsigned char brightness) {
    DEBUG_MSG_P(PSTR("[LIGHTS] Brightness to %d\n"), brightness);
    commsSend(at_bright, brightness);
}

void sendColor(const char * rgb) {
    _fromRGB(rgb);
    unsigned long color = _channels[0] << 16 | _channels[1] << 8 | _channels[2];
    sendColor(color);
}

void sendSpeed(unsigned char speed) {
    DEBUG_MSG_P(PSTR("[LIGHTS] Speed to %d\n"), speed);
    commsSend(at_speed, speed);
}

void sendNotification(bool state, unsigned long time) {

    if (state) {
        sendBrightness(NOTIFICATION_BRIGHTNESS);
        sendEffect(NOTIFICATION_EFFECT);
        sendSpeed(NOTIFICATION_SPEED);
        sendColor(NOTIFICATION_COLOR);
    } else {
        sendColor(0UL);
    }

    if (time > 0) _lights_defer.once(time, sendNotification, !state);

}

void sendNotification(bool state) {
    sendNotification(state, 0);
}

void lightsSetup() {
    mqttRegister(_lightsMqttCallback);
    commsSend(at_timeout, 0);
}

void lightsLoop() {
}
