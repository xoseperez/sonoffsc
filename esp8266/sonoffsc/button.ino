/*

BUTTON MODULE

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

// -----------------------------------------------------------------------------
// BUTTON
// -----------------------------------------------------------------------------

#include <DebounceEvent.h>

DebounceEvent _button = DebounceEvent(BUTTON_PIN);

void buttonSetup() {
}

void buttonLoop() {

    if (_button.loop()) {
        uint8_t event = _button.getEvent();
        DEBUG_MSG("[BUTTON] Button pressed. Event: %d\n", event);
        if (event == EVENT_DOUBLE_CLICK) createAP();
        if (event == EVENT_LONG_CLICK) ESP.reset();
        //if (event == EVENT_SINGLE_CLICK) relayToggle(i);
    }

}
