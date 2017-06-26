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

    if (uint8_t event = _button.loop()) {

		if (event == EVENT_RELEASED) {

            DEBUG_MSG("[BUTTON] Button pressed. Event: %d Length:%d\n", _button.getEventCount(), _button.getEventLength());

            if (_button.getEventCount() == 1) {

                if(_button.getEventLength() >= 3000) {
                    ESP.reset();
                }

            }

            if (_button.getEventCount() >= 2) {
                createAP();
            }
            
        }

    }

}
