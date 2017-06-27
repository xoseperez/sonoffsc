/*

LIGHTS MODULE

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

// -----------------------------------------------------------------------------
// LIGHTS
// -----------------------------------------------------------------------------

// Thanks to Sacha Telgenhof for sharing this code in his AiLight library
// https://github.com/stelgenhof/AiLight
void color_temperature2array(unsigned int temperature, unsigned int * array) {

    // Force boundaries and conversion
    temperature = constrain(temperature, 1000, 40000) / 100;

    // Calculate colors
    unsigned int red = (temperature <= 66)
        ? 255
        : 329.698727446 * pow((temperature - 60), -0.1332047592);
    unsigned int green = (temperature <= 66)
        ? 99.4708025861 * log(temperature) - 161.1195681661
        : 288.1221695283 * pow(temperature, -0.0755148492);
    unsigned int blue = (temperature >= 66)
        ? 255
        : ((temperature <= 19)
            ? 0
            : 138.5177312231 * log(temperature - 10) - 305.0447927307);

    // Save values
    array[0] = constrain(red, 0, 255);
    array[1] = constrain(green, 0, 255);
    array[2] = constrain(blue, 0, 255);
}

void color_string2array(const char * rgb, unsigned int * array) {

    char * p = (char *) rgb;
    if (strlen(p) == 0) return;

    // if color begins with a # then assume HEX RGB
    if (p[0] == '#') {

        ++p;
        unsigned long value = strtol(p, NULL, 16);
        array[0] = (value >> 16) & 0xFF;
        array[1] = (value >> 8) & 0xFF;
        array[2] = (value) & 0xFF;

    // it's a temperature
    } else if (p[strlen(p)-1] == 'K') {

        p[strlen(p)-1] = 0;
        unsigned int temperature = atoi(p);
        color_temperature2array(temperature, array);

    // otherwise assume decimal values separated by commas
    } else {

        char * tok;
        tok = strtok(p, ",");
        array[0] = atoi(tok);
        tok = strtok(NULL, ",");

        // if there are more than one value assume R,G,B
        if (tok != NULL) {
            array[1] = atoi(tok);
            tok = strtok(NULL, ",");
            if (tok != NULL) {
                array[2] = atoi(tok);
            } else {
                array[2] = 0;
            }

        // only one value set red, green and blue to the same value
        } else {
            array[2] = array[1] = array[0];
        }

    }

}

void sendEffect(long effect) {
    delay(200);
    send_P_repeat(at_effect, effect);
}

void sendColor(const char * rgb) {

    unsigned int components[3];
    color_string2array(rgb, components);
    unsigned long color = components[0] + components[1] * 256 + components[2] * 256 * 256;

    delay(200);
    send_P_repeat(at_timeout, 0);
    send_P_repeat(at_speed, 100);
    send_P_repeat(at_bright, 255);
    send_P_repeat(at_color, color);

}

void sendNotification(bool state) {
    sendEffect(state ? NOTIFICATION_ON_EFFECT : NOTIFICATION_OFF_EFFECT);
}

void lightsMqttCallback(unsigned int type, const char * topic, const char * payload) {

	// When connected, subscribe to the topic
	if (type == MQTT_CONNECT_EVENT) {
		mqttSubscribe(MQTT_TOPIC_RGB);
        mqttSubscribe(MQTT_TOPIC_EFFECT);
	}

	// Messages
	if (type == MQTT_MESSAGE_EVENT) {

        String t = mqttSubtopic((char*) topic);

        if (t.equals(MQTT_TOPIC_RGB)) {
			sendColor((char*) payload);
		}

        if (t.equals(MQTT_TOPIC_EFFECT)) {
			sendEffect(atoi((char *) payload));
		}

	}

}

void lightsSetup() {
    mqttRegister(lightsMqttCallback);
}

void lightsLoop() {
}
