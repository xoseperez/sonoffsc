/*

ALEXA MODULE

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if ALEXA_SUPPORT

#include <fauxmoESP.h>

fauxmoESP alexa;

// -----------------------------------------------------------------------------
// ALEXA
// -----------------------------------------------------------------------------

void alexaConfigure() {
    alexa.enable(getSetting("alexaEnabled", ALEXA_ENABLED).toInt() == 1);
}

void alexaSetup() {

    alexa.addDevice("clap");

    alexa.onMessage([](unsigned char device_id, const char * name, bool state) {

        DEBUG_MSG("[FAUXMO] %s state: %s\n", name, state ? "ON" : "OFF");

        if (strcmp(name, "clap") == 0) {
            setSetting("clapEnabled", state);
            if (state) {
                wsSend((char *) "{\"clapEnabled\": true}");
            } else {
                wsSend((char *) "{\"clapEnabled\": false}");
            }
            commsConfigure();
        }

    });

}

void alexaLoop() {
    alexa.handle();
}

#endif
