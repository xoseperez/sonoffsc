/*

FAUXMO MODULE

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <fauxmoESP.h>

fauxmoESP fauxmo;

// -----------------------------------------------------------------------------
// FAUXMO
// -----------------------------------------------------------------------------

void fauxmoSetup() {

    fauxmo.addDevice("clap");

    fauxmo.onMessage([](unsigned char device_id, const char * name, bool state) {

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

void fauxmoLoop() {
    fauxmo.handle();
}
