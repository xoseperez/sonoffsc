/*

ALEXA MODULE

Copyright (C) 2016-2018 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if ALEXA_SUPPORT

#include <fauxmoESP.h>

fauxmoESP alexa;

// -----------------------------------------------------------------------------
// ALEXA
// -----------------------------------------------------------------------------

void _alexaWebSocketOnSend(JsonObject& root) {
    root["alexaVisible"] = 1;
    root["alexaEnabled"] = getSetting("alexaEnabled", ALEXA_ENABLED).toInt() == 1;
}

void _alexaConfigure() {
    alexa.enable(getSetting("alexaEnabled", ALEXA_ENABLED).toInt() == 1);
}

// -----------------------------------------------------------------------------

void alexaSetup() {

    // Load & cache settings
    _alexaConfigure();

    #if WEB_SUPPORT

        // Websockets
        wsOnSendRegister(_alexaWebSocketOnSend);
        wsOnAfterParseRegister(_alexaConfigure);

    #endif

    alexa.addDevice("clap");

    alexa.onSetState([](unsigned char device_id, const char * name, bool state) {

        DEBUG_MSG("[ALEXA] %s state: %s\n", name, state ? "ON" : "OFF");

        if (strcmp(name, "clap") == 0) {
            setSetting("clapEnabled", state);
            commsConfigure();
            #if WEB_SUPPORT
                if (state) {
                    wsSend((char *) "{\"clapEnabled\": true}");
                } else {
                    wsSend((char *) "{\"clapEnabled\": false}");
                }
            #endif
        }

    alexa.onGetState([](unsigned char device_id, const char * name) {
		if (strcmp(name, "clap") == 0) {
			return getSetting("clapEnabled", 0).toInt();
		}
    });

    });

}

void alexaLoop() {
    alexa.handle();
}

#endif
