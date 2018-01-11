/*

LED MODULE

Copyright (C) 2018 by Xose Pérez <xose dot perez at gmail dot com>

*/

// -----------------------------------------------------------------------------
// LED
// -----------------------------------------------------------------------------

bool _ledStatus(bool status) {
    digitalWrite(LED_PIN, status ? LOW : HIGH);
    return status;
}

bool _ledStatus() {
    return (digitalRead(LED_PIN) == LOW);
}

bool _ledToggle() {
    return _ledStatus(!_ledStatus());
}

void _ledBlink(unsigned long delayOff, unsigned long delayOn) {
    static unsigned long next = millis();
    if (next < millis()) {
        next += (_ledToggle() ? delayOn : delayOff);
    }
}

// -----------------------------------------------------------------------------

void ledSetup() {
    pinMode(LED_PIN, OUTPUT);
}

void ledLoop() {
    if (wifiConnected()) {
        if (WiFi.getMode() == WIFI_AP) {
            _ledBlink(2000, 2000);
        } else {
            _ledBlink(5000, 500);
        }
    } else {
        _ledBlink(500, 500);
    }
}
