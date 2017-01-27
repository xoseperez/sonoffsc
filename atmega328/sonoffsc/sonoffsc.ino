/*

Sonoff SC
Copyright (C) 2017 by Xose Pérez <xose dot perez at gmail dot com>

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


#include <Adafruit_NeoPixel.h>
#include <DHT.h>
#include <SerialLink.h>

#include <Adafruit_NeoPixel.h>




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE         115200

#define LED_PIN                 7

#define LDR_PIN                 A3

#define SHARP_LED_PIN           9
#define SHARP_READ_PIN          A1
#define SHARP_SAMPLING_TIME	    280
#define SHARP_DELTA_TIME		40
#define SHARP_SLEEP_TIME		9680

#define DHT_PIN                 6
#ifndef DHT_TYPE
#define DHT_TYPE                DHT11
#endif

#define RGB_PIN					11

#define ADC_COUNTS              1024
#define MICROPHONE_PIN          A2

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

#define DEFAULT_EVERY           0
#define DEFAULT_PUSH            0
#define DEFAULT_CLAP            0
#define DEFAULT_THRESHOLD       0

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
const PROGMEM char at_led[] = "AT+LED";
const PROGMEM char at_rgb[] = "AT+RGB";

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
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, RGB_PIN, NEO_GRB + NEO_KHZ800);

SerialLink link(Serial);
DHT dht(DHT_PIN, DHT_TYPE);
int clapTimings[CLAP_BUFFER_SIZE];
byte clapPointer = 0;

// If push == false the slave waits for the master to request for values
// If push == true the slaves sends messages
//
// If every == 0 values are retrieved upon request
// If every > 0 values are retrieved every <every> seconds and cached/sent
//
// If push == true and every == 0 messages are never sent.

bool push = DEFAULT_PUSH;
bool clap = DEFAULT_CLAP;
unsigned int every = DEFAULT_EVERY;
unsigned int threshold = DEFAULT_THRESHOLD;

float temperature;
int humidity;
float dust;
int light;
int noise;

bool rgbExec = false;

//unsigned int noise_count = 0;
//unsigned long noise_sum = 0;
//unsigned int noise_peak = 0;
//unsigned int noise_min = 1024;
//unsigned int noise_max = 0;

unsigned int noise_buffer[NOISE_BUFFER_SIZE] = {0};
unsigned int noise_buffer_pointer = 0;
unsigned int noise_buffer_sum = 0;

// -----------------------------------------------------------------------------
// ACTUATORS
// -----------------------------------------------------------------------------

void ledStatus(bool status) {
    digitalWrite(LED_PIN, status ? HIGH : LOW);
}

void rgbStatus(uint32_t color){
	colorWipe(color, 50);
}

bool ledStatus() {
    return digitalRead(LED_PIN) == HIGH;
}

// -----------------------------------------------------------------------------
// SENSORS
// -----------------------------------------------------------------------------

int getLight() {
    return map(analogRead(LDR_PIN), 0, ADC_COUNTS, 100, 0);
}

// 0.5V/0.1mg/m3
float getDust() {

    digitalWrite(SHARP_LED_PIN, LOW);
	delayMicroseconds(SHARP_SAMPLING_TIME);

	float reading = analogRead(SHARP_READ_PIN);

	delayMicroseconds(SHARP_DELTA_TIME);
	digitalWrite(SHARP_LED_PIN, HIGH);
	//delayMicroseconds(SHARP_SLEEP_TIME);

    // mg/m3
	float dust = 0.17 * reading * (5.0 / 1024.0) - 0.1;
    if (dust < 0) dust = 0;
    return dust;

}

float getTemperature() {
    return dht.readTemperature();
}

int getHumidity() {
    return dht.readHumidity();
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

void setRGB(int red = 0, int green = 0, int blue = 0) {

	// set rgb mix here....
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
    //if (millis() - last_reading < NOISE_READING_DELAY) return;
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

bool linkGet(char * key) {  // 

    if (strcmp_P(key, at_push) == 0) {
        link.send(key, push ? 1 : 0, false);
        return true;
    }

    if (strcmp_P(key, at_led) == 0) {
        link.send(key, ledStatus() ? 1 : 0, false);
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
        link.send(key, 10 * temperature, false);
        return true;
    }

    if (strcmp_P(key, at_hum) == 0) {
        if (every == 0) humidity = getHumidity();
        link.send(key, humidity, false);
        return true;
    }

    if (strcmp_P(key, at_noise) == 0) {
        if (every == 0) noise = getNoise();
        link.send(key, noise, false);
        return true;
    }

    if (strcmp_P(key, at_dust) == 0) {
        if (every == 0) dust = getDust();
        link.send(key, 100 * dust, false);
        return true;
    }

    if (strcmp_P(key, at_light) == 0) {
        if (every == 0) light = getLight();
        link.send(key, light, false);
        return true;
    }

    return false;

}

bool linkSet(char * key, int value) {

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
            every = 1000 * value;
            return true;
        }
    }

    if (strcmp_P(key, at_thld) == 0) {
        if (0 <= value && value <= 100) {
            threshold = value;
            return true;
        }
    }

    if (strcmp_P(key, at_led) == 0) {
        if (0 <= value && value <= 1) {
            ledStatus(value == 1);
            return true;
        }
    }
	if (strcmp_P(key, at_rgb) == 0) {  // rgb value sent
		// wont check the value here... Just now
			rgbStatus(value);
			return true;
	}
    return false;

}

void linkSetup() {
    link.onGet(linkGet);
    link.onSet(linkSet);
}

void linkLoop() {
    link.handle();
}

// -----------------------------------------------------------------------------
// MAIN
// -----------------------------------------------------------------------------

void setup() {

    Serial.begin(SERIAL_BAUDRATE);
    link.send_P(at_hello, 1);

	// Neopixel setup
	strip.begin();
	strip.setBrightness(30); //adjust brightness here
	strip.show(); // Initialize all pixels to 'off'

    linkSetup();

    pinMode(LED_PIN, OUTPUT);
    ledStatus(false);
    pinMode(LDR_PIN, INPUT);
    pinMode(DHT_PIN, INPUT);
    pinMode(SHARP_LED_PIN, OUTPUT);
    pinMode(SHARP_READ_PIN, INPUT);
    pinMode(MICROPHONE_PIN, INPUT_PULLUP);
	pinMode(RGB_PIN, OUTPUT);

    dht.begin();

}

void loop() {

    static unsigned long last = 0;

    linkLoop();

	// If AT+EVERY>0 then we are sending a signal every so many seconds
    if ((every > 0) && ((millis() - last > every) || (last == 0))) {

        last = millis();

        temperature = getTemperature();
        if (push) link.send_P(at_temp, 10 * temperature, false);

        noiseLoop();

        humidity = getHumidity();
        if (push) link.send_P(at_hum, humidity, false);

        noiseLoop();

        dust = getDust();
        if (push) link.send_P(at_dust, 100 * dust, false);

        noiseLoop();

        light = getLight();
        if (push) link.send_P(at_light, light, false);

        noiseLoop();

        noise = getNoise();
        if (push) link.send_P(at_noise, noise, false);

    }


    noiseLoop();

}
