/*

BUTTON MODULE

Copyright (C) 2016-2018 by Xose Pérez <xose dot perez at gmail dot com>

*/

// -----------------------------------------------------------------------------
// BUTTON
// -----------------------------------------------------------------------------

#include <DebounceEvent.h>

DebounceEvent _button = DebounceEvent(BUTTON_PIN, BUTTON_PUSHBUTTON, BUTTON_DEBOUNCE_DELAY, BUTTON_DBLCLICK_DELAY);

void buttonSetup() {
}

void buttonLoop() {

    if (uint8_t event = _button.loop()) {

		if (event == EVENT_RELEASED) {

            DEBUG_MSG("[BUTTON] Button pressed. Event: %d Length:%d\n", _button.getEventCount(), _button.getEventLength());

            if (_button.getEventCount() == 1) {

                if(_button.getEventLength() >= BUTTON_LNGLNGCLICK_DELAY) {
                    deferredReset(100, CUSTOM_RESET_HARDWARE);
                    ESP.restart();
                }

                if(_button.getEventLength() >= BUTTON_LNGCLICK_DELAY) {
                    DEBUG_MSG_P(PSTR("\n\nFACTORY RESET\n\n"));
                    settingsFactoryReset();
                    deferredReset(100, CUSTOM_RESET_FACTORY);
                    ESP.restart();
                }

            }

            if (_button.getEventCount() >= 2) {
                createAP();
            }

        }

    }

}
