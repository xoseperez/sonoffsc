/*

Sonoff SC
Copyright (C) 2017-2018 by Xose Pérez <xose dot perez at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include <WS2812FX.h>
#include <DHT.h>
#include <SerialLink.h>
#include <Ticker.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE         9600

#define FAN_PIN                 7
#define FAN_OFF_DELAY           0

#define LDR_PIN                 A3

#define SHARP_LED_PIN           9
#define SHARP_READ_PIN          A1
#define SHARP_SAMPLING_TIME	    280
#define SHARP_DELTA_TIME		40
#define SHARP_SLEEP_TIME		9680

#define DHT_PIN                 6
#ifndef DHT_TYPE
// Uncomment the sensor type that you have (comment the other if applicable)
//#define DHT_TYPE                DHT11
#define DHT_TYPE                DHT22
#endif
#define DHT_EXPIRES             60000

#define RGB_PIN			        12
#define RGB_COUNT               24
#define RGB_TIMEOUT             0
#define RGB_SPEED               255
#define RGB_BRIGHTNESS          255
#define RGB_COLOR               0x000000
#define RGB_EFFECT              FX_MODE_STATIC

#define ADC_COUNTS              1024
#define MICROPHONE_PIN          A2

#define MW_PIN                  13

//#define NOISE_READING_DELAY     100
#define NOISE_READING_WINDOW    20
#define NOISE_BUFFER_SIZE       20

#define CLAP_DEBOUNCE_DELAY     150
#define CLAP_TIMEOUT_DELAY      1000
#define CLAP_SENSIBILITY        80
#define CLAP_COUNT_TRIGGER      4
#define CLAP_BUFFER_SIZE        7
#define CLAP_TOLERANCE          1.50

#define MAX_SERIAL_BUFFER       20

#define DEFAULT_EVERY           60
#define DEFAULT_PUSH            0
#define DEFAULT_CLAP            0
#define DEFAULT_THRESHOLD       0

#define NULL_VALUE              -999

// -----------------------------------------------------------------------------
// Keywords
// -----------------------------------------------------------------------------

const PROGMEM char at_hello[] = "AT+HELLO";
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
const PROGMEM char at_fan[] = "AT+FAN";
const PROGMEM char at_fanoff[] = "AT+FANOFF";
const PROGMEM char at_timeout[] = "AT+TIMEOUT";
const PROGMEM char at_effect[] = "AT+EFFECT";
const PROGMEM char at_color[] = "AT+COLOR";
const PROGMEM char at_bright[] = "AT+BRIGHT";
const PROGMEM char at_speed[] = "AT+SPEED";
const PROGMEM char at_move[] = "AT+MOVE";

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
WS2812FX ws2812fx = WS2812FX(RGB_COUNT, RGB_PIN, NEO_GRB + NEO_KHZ800);
bool rgbRunning = false;
unsigned long rgbTimeout = RGB_TIMEOUT;
unsigned long rgbStart = 0;

SerialLink link(Serial);
DHT dht(DHT_PIN, DHT_TYPE);
int clapTimings[CLAP_BUFFER_SIZE];
byte clapPointer = 0;

Ticker fanTicker;
bool dustPush = false;

// If push == false the slave waits for the master to request for values
// If push == true the slaves sends messages
//
// If every == 0 values are retrieved upon request
// If every > 0 values are retrieved every <every> seconds and cached/sent
//
// If push == true and every == 0 messages are never sent.

bool push = DEFAULT_PUSH;
bool clap = DEFAULT_CLAP;
unsigned long every = 1000L * DEFAULT_EVERY;
unsigned int threshold = DEFAULT_THRESHOLD;
unsigned long fanoff = FAN_OFF_DELAY;

float temperature = NULL_VALUE;
int humidity = NULL_VALUE;
float dust = NULL_VALUE;
int light = NULL_VALUE;
int noise = NULL_VALUE;
bool movement;

//unsigned int noise_count = 0;
//unsigned long noise_sum = 0;
//unsigned int noise_peak = 0;
//unsigned int noise_min = 1024;
//unsigned int noise_max = 0;

unsigned int noise_buffer[NOISE_BUFFER_SIZE] = {0};
unsigned int noise_buffer_pointer = 0;
unsigned int noise_buffer_sum = 0;

// -----------------------------------------------------------------------------
// FAN
// -----------------------------------------------------------------------------

void fanStatus(bool status) {
    digitalWrite(FAN_PIN, status ? HIGH : LOW);
}

bool fanStatus() {
    return digitalRead(FAN_PIN) == HIGH;
}

// -----------------------------------------------------------------------------
// RGB LED
// -----------------------------------------------------------------------------

void rgbLoop() {

    if (rgbRunning && (rgbTimeout > 0)) {
        if (millis() - rgbStart > rgbTimeout) {
            ws2812fx.setMode(FX_MODE_FADE);
            rgbRunning = false;
        }
    }

    ws2812fx.service();

}

void rgbOff() {
    ws2812fx.setColor(0);
    ws2812fx.setMode(FX_MODE_STATIC);
}

void rgbEffect(unsigned int effect) {
    ws2812fx.setMode(effect);
    rgbStart = millis();
    rgbRunning = true;
}

void rgbColor(unsigned long color) {
    ws2812fx.setColor(color);
    rgbStart = millis();
    rgbRunning = true;
}

// -----------------------------------------------------------------------------
// SENSORS
// -----------------------------------------------------------------------------

int getLight() {
    return map(analogRead(LDR_PIN), 0, ADC_COUNTS, 100, 0);
}

// 0.5V ==> 100ug/m3
float getDust() {

    digitalWrite(SHARP_LED_PIN, LOW);
	delayMicroseconds(SHARP_SAMPLING_TIME);

	float reading = analogRead(SHARP_READ_PIN);

	delayMicroseconds(SHARP_DELTA_TIME);
	digitalWrite(SHARP_LED_PIN, HIGH);

    // mg/m3
	float dust = 170.0 * reading * (5.0 / 1024.0) - 100.0;
    if (dust < 0) dust = 0;
    return dust;

}

void getDustDefer(bool push = false) {
    if (fanoff > 0) {
        fanStatus(true);
        dustPush = push;
        fanTicker.setInterval(fanoff);
        fanTicker.setCallback([](){
            fanTicker.stop();
            dust = getDust();
            fanStatus(false);
            if (dustPush) link.send_P(at_dust, dust, false);
        });
        fanTicker.start();
    } else {
        dust = getDust();
        if (push) link.send_P(at_dust, dust, false);
    }
}

void loadTempAndHum() {

    // Check at most once every minute
    static unsigned long last = 0;
    if (millis() - last < DHT_EXPIRES) return;

    // Retrieve data
    double h = dht.readHumidity();
    double t = dht.readTemperature();

    // Check values
    if (isnan(h) || isnan(t)) return;
    temperature = t;
    humidity = h;

    // Only set new expiration time if good reading
    last = millis();

}

float getTemperature() {
    loadTempAndHum();
    return temperature;
}

int getHumidity() {
    loadTempAndHum();
    return humidity;
}

int getNoise() {

    int value = 0;

    //if (noise_count > 0) {

        value = noise_buffer_sum / NOISE_BUFFER_SIZE;

        //Serial.print("CNT : "); Serial.println(noise_count);
        //Serial.print("SUM : "); Serial.println(noise_sum / noise_count);
        //Serial.print("PEAK: "); Serial.println(noise_peak / noise_count);
        //Serial.print("MAX : "); Serial.println(noise_max);
        //Serial.print("MIN : "); Serial.println(noise_min);
        //Serial.print("VAL : "); Serial.println(value);

        //noise_count = 0;
        //noise_sum = 0;
        //noise_peak = 0;
        //noise_min = ADC_COUNTS;
        //noise_max = 0;

    //}

    return value;

}

// -----------------------------------------------------------------------------
// MICROWAVE
// -----------------------------------------------------------------------------

bool getMovement() {
    return digitalRead(MW_PIN) == HIGH;
}

void moveLoop(bool force = false) {
    bool value = getMovement();
    if (force || (movement != value)) {
        link.send_P(at_move, value ? 1 : 0, false);
    }
    movement = value;
}

// -----------------------------------------------------------------------------
// MIC
// -----------------------------------------------------------------------------

void clapDecode() {

    // at least 2 claps
    if (clapPointer > 0) {

        byte code = 2;
        if (clapPointer > 1) {
            int length = clapTimings[0] * CLAP_TOLERANCE;
            for(byte i=1; i<clapPointer; i++) {
                code <<= 1;
                if (clapTimings[i] > length) code += 1;
            }
        }

        link.send_P(at_code, code);

    }

    // reset
    clapPointer = 0;

}

void clapRecord(int value) {

    static bool reading = false;
    static unsigned long last_clap;
    static int counts = 0;
    unsigned long current = millis();
    unsigned long span = current - last_clap;

    if (value > CLAP_SENSIBILITY) {
        ++counts;
    } else {
        counts = 0;
    }

    if (counts == CLAP_COUNT_TRIGGER) {

        //Serial.print("Value: "); Serial.println(value);

        // Is it the first clap?
        if (!reading) {

            last_clap = current;
            reading = true;

        // or not
        } else {

            //Serial.print("Span : "); Serial.println(span);

            // timed out
            if (span > CLAP_TIMEOUT_DELAY) {

                clapDecode();

                // reset
                reading = false;

            } else if (span < CLAP_DEBOUNCE_DELAY) {

                // do nothing

            // new clap!
            } else if (clapPointer < CLAP_BUFFER_SIZE) {
                clapTimings[clapPointer] = span;
                last_clap = current;
                clapPointer++;

            // buffer overrun
            } else {
                clapPointer = 0;
                reading = false;
            }

        }

    // check if we have to process it
    } else if (reading) {

        if (span > CLAP_TIMEOUT_DELAY) {

            clapDecode();

            // back to idle
            reading = false;

        }

    }

}

void noiseLoop() {

    static unsigned long last_reading = 0;
    static unsigned int triggered = 0;

    unsigned int sample;
    //unsigned int count = 0;
    //unsigned long sum;
    unsigned int min = ADC_COUNTS;
    unsigned int max = 0;

    // Check MIC every NOISE_READING_DELAY
    // if (millis() - last_reading < NOISE_READING_DELAY) return;
    last_reading = millis();

    while (millis() - last_reading < NOISE_READING_WINDOW) {
        sample = analogRead(MICROPHONE_PIN);
        //++count;
        //sum += sample;
        if (sample < min) min = sample;
        if (sample > max) max = sample;
    }

    //++noise_count;
    //unsigned int average = 100 * (sum / count) / ADC_COUNTS;
    //noise_sum += average;

    unsigned int peak = map(max - min, 0, ADC_COUNTS, 0, 100);
    //Serial.println(peak);
    if (clap) clapRecord(peak);

    noise_buffer_sum = noise_buffer_sum + peak - noise_buffer[noise_buffer_pointer];
    noise_buffer[noise_buffer_pointer] = peak;
    noise_buffer_pointer = (noise_buffer_pointer + 1) % NOISE_BUFFER_SIZE;

    //noise_peak += peak;
    //if (max > noise_max) noise_max = max;
    //if (min < noise_min) noise_min = min;

    if (threshold > 0) {
        unsigned int value = noise_buffer_sum / NOISE_BUFFER_SIZE;
        if (value > threshold) {
            if (value > triggered) {
                link.send_P(at_noise, value);
                triggered = value;
            }
        } else if (triggered > 0) {
            link.send_P(at_noise, value);
            triggered = 0;
        }
    }

}

// -----------------------------------------------------------------------------
// COMMUNICATION
// -----------------------------------------------------------------------------

// How to respond to AT+...=? requests
bool linkGet(char * key) {

    if (strcmp_P(key, at_push) == 0) {
        link.send(key, push ? 1 : 0, false);
        return true;
    }

    if (strcmp_P(key, at_fan) == 0) {
        link.send(key, fanStatus() ? 1 : 0, false);
        return true;
    }

    if (strcmp_P(key, at_fanoff) == 0) {
        link.send(key, fanoff, false);
        return true;
    }

    if (strcmp_P(key, at_clap) == 0) {
        link.send(key, clap ? 1 : 0, false);
        return true;
    }

    if (strcmp_P(key, at_thld) == 0) {
        link.send(key, threshold, false);
        return true;
    }

    if (strcmp_P(key, at_every) == 0) {
        link.send(key, every / 1000, false);
        return true;
    }

    if (strcmp_P(key, at_temp) == 0) {
        if (every == 0) temperature = getTemperature();
        if (temperature == NULL_VALUE) return false;
        link.send(key, 10 * temperature, false);
        return true;
    }

    if (strcmp_P(key, at_hum) == 0) {
        if (every == 0) humidity = getHumidity();
        if (humidity == NULL_VALUE) return false;
        link.send(key, humidity, false);
        return true;
    }

    if (strcmp_P(key, at_noise) == 0) {
        if (every == 0) noise = getNoise();
        if (noise == NULL_VALUE) return false;
        link.send(key, noise, false);
        return true;
    }

    if (strcmp_P(key, at_dust) == 0) {
        if (every == 0) {
            getDustDefer(true);
        } else {
            if (dust == NULL_VALUE) return false;
            link.send(key, dust, false);
        }
        return true;
    }

    if (strcmp_P(key, at_light) == 0) {
        if (every == 0) light = getLight();
        if (light == NULL_VALUE) return false;
        link.send(key, light, false);
        return true;
    }

    if (strcmp_P(key, at_move) == 0) {
        link.send(key, getMovement() ? 1 : 0, false);
        return true;
    }

    if (strcmp_P(key, at_timeout) == 0) {
        link.send(key, rgbTimeout, false);
        return true;
    }

    if (strcmp_P(key, at_effect) == 0) {
        link.send(key, ws2812fx.getMode(), false);
        return true;
    }

    if (strcmp_P(key, at_color) == 0) {
        link.send(key, ws2812fx.getColor(), false);
        return true;
    }

    if (strcmp_P(key, at_speed) == 0) {
        link.send(key, ws2812fx.getSpeed(), false);
        return true;
    }

    if (strcmp_P(key, at_bright) == 0) {
        link.send(key, ws2812fx.getBrightness(), false);
        return true;
    }

    return false;

}

// Functions for responding to AT+...=<long> commands that set values and functions
bool linkSet(char * key, long value) {

    if (strcmp_P(key, at_push) == 0) {
        if (0 <= value && value <= 1) {
            push = value == 1;
            return true;
        }
    }

    if (strcmp_P(key, at_clap) == 0) {
        if (0 <= value && value <= 1) {
            clap = value == 1;
            return true;
        }
    }

    if (strcmp_P(key, at_every) == 0) {
        if (5 <= value && value <= 300) {
            every = 1000L * value;
            return true;
        }
    }

    if (strcmp_P(key, at_thld) == 0) {
        if (0 <= value && value <= 100) {
            threshold = value;
            return true;
        }
    }

    if (strcmp_P(key, at_fan) == 0) {
        if (0 <= value && value <= 1) {
            fanStatus(value == 1);
            return true;
        }
    }

    if (strcmp_P(key, at_fanoff) == 0) {
        fanoff = value;
        return true;
    }

    if (strcmp_P(key, at_timeout) == 0) {
        if (0 <= value) {
            rgbTimeout = value;
            return true;
        }
	}

	if (strcmp_P(key, at_color) == 0) {
        if (0 <= value && value <= 0xFFFFFF) {
            rgbColor(value);
            return true;
        }
	}

    if (strcmp_P(key, at_effect) == 0) {
        if (0 <= value && value < MODE_COUNT) {
            rgbEffect(value);
            return true;
        }
	}

    if (strcmp_P(key, at_speed) == 0) {
        if (SPEED_MIN <= value && value <= SPEED_MAX) {
            ws2812fx.setSpeed(value);
            return true;
        }
	}

    if (strcmp_P(key, at_bright) == 0) {
        if (BRIGHTNESS_MIN <= value && value <= BRIGHTNESS_MAX) {
            ws2812fx.setBrightness(value);
            return true;
        }
	}

    return false;

}

//Setup callbacks when AT commands are recieved
void linkSetup() {
    link.onGet(linkGet);
    link.onSet(linkSet);
}

// Check for incoming AT+ commands
void linkLoop() {
    link.handle();
}

// -----------------------------------------------------------------------------
// MAIN
// -----------------------------------------------------------------------------

void setup() {

	// Setup Serial port
    Serial.begin(SERIAL_BAUDRATE);
    link.send_P(at_hello, 1);

    linkSetup();

	// Setup physical pins on the ATMega328
    pinMode(FAN_PIN, OUTPUT);
	pinMode(LDR_PIN, INPUT);
    pinMode(DHT_PIN, INPUT);
    pinMode(SHARP_LED_PIN, OUTPUT);
    pinMode(SHARP_READ_PIN, INPUT);
    pinMode(MICROPHONE_PIN, INPUT_PULLUP);
    pinMode(MW_PIN, INPUT);

	// Switch FAN off
    fanStatus(false);

	// Setup the DHT Thermometer/Humidity Sensor
    dht.begin();

	// Neopixel setup and start animation
    ws2812fx.init();
    ws2812fx.setBrightness(RGB_BRIGHTNESS);
    ws2812fx.setSpeed(RGB_SPEED);
    ws2812fx.setColor(RGB_COLOR);
    ws2812fx.setMode(RGB_EFFECT);
    ws2812fx.start();

    rgbStart = millis();
    rgbRunning = true;

}

void loop() {

    static unsigned long last = 0;

    linkLoop();

	// If AT+EVERY>0 then we are sending a signal every so many seconds
    if ((every > 0) && ((millis() - last > every) || (last == 0))) {

        last = millis();

        getDustDefer(push);

        temperature = getTemperature();
        if (push) link.send_P(at_temp, 10 * temperature, false);

        noiseLoop();

        humidity = getHumidity();
        if (push) link.send_P(at_hum, humidity, false);

        noiseLoop();

        light = getLight();
        if (push) link.send_P(at_light, light, false);

        noiseLoop();

        noise = getNoise();
        if (push) link.send_P(at_noise, noise, false);

        moveLoop(true);

    }

    fanTicker.update();
    noiseLoop();
    moveLoop();
    rgbLoop();

}
