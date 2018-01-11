//------------------------------------------------------------------------------
// GENERAL
//------------------------------------------------------------------------------

#define DEVICE_NAME             APP_NAME
#define MANUFACTURER            "ITEAD STUDIO"
#define ADMIN_PASS              "fibonacci" // Default password (WEB, OTA, WIFI)
#define USE_PASSWORD            1           // Insecurity caution! Disabling this will disable password querying completely.
#define LOOP_DELAY_TIME         10          // Delay for this millis in the main loop [0-250]

#define ARRAYINIT(type, name, ...) \
    type name[] = {__VA_ARGS__};

//------------------------------------------------------------------------------
// TELNET
//------------------------------------------------------------------------------

#ifndef TELNET_SUPPORT
#define TELNET_SUPPORT          1               // Enable telnet support by default
#endif

#ifndef TELNET_STA
#define TELNET_STA              0               // By default, disallow connections via STA interface
#endif

#define TELNET_PORT             23              // Port to listen to telnet clients
#define TELNET_MAX_CLIENTS      1               // Max number of concurrent telnet clients

//------------------------------------------------------------------------------
// DEBUG
//------------------------------------------------------------------------------

// Serial debug log

#ifndef DEBUG_SERIAL_SUPPORT
#define DEBUG_SERIAL_SUPPORT    1               // Do not enable serial debug log
#endif

#ifndef DEBUG_PORT
#define DEBUG_PORT              Serial          // Default debugging port
#endif

#ifndef SERIAL_BAUDRATE
#define SERIAL_BAUDRATE         9600            // Default baudrate
#endif


//------------------------------------------------------------------------------

// UDP debug log
// To receive the message son the destination computer use nc:
// nc -ul 8113

#ifndef DEBUG_UDP_SUPPORT
#define DEBUG_UDP_SUPPORT       0               // Enable UDP debug log
#endif
#define DEBUG_UDP_IP            IPAddress(192, 168, 1, 100)
#define DEBUG_UDP_PORT          8113

//------------------------------------------------------------------------------

#ifndef DEBUG_TELNET_SUPPORT
#define DEBUG_TELNET_SUPPORT    TELNET_SUPPORT  // Enable telnet debug log if telnet is enabled too
#endif

#if DEBUG_TELNET_SUPPORT
#undef TELNET_SUPPORT
#define TELNET_SUPPORT          1
#endif

//------------------------------------------------------------------------------

// General debug options and macros
#define DEBUG_SUPPORT           DEBUG_SERIAL_SUPPORT || DEBUG_UDP_SUPPORT || DEBUG_TELNET_SUPPORT

#if DEBUG_SUPPORT
    #define DEBUG_MSG(...) debugSend(__VA_ARGS__)
    #define DEBUG_MSG_P(...) debugSend_P(__VA_ARGS__)
#endif

#ifndef DEBUG_MSG
    #define DEBUG_MSG(...)
    #define DEBUG_MSG_P(...)
#endif

//------------------------------------------------------------------------------
// TERMINAL
//------------------------------------------------------------------------------

#ifndef TERMINAL_SUPPORT
#define TERMINAL_SUPPORT         0               // Terminal support conflicts comms
#endif

//------------------------------------------------------------------------------
// SYSTEM CHECK
//------------------------------------------------------------------------------

#ifndef SYSTEM_CHECK_ENABLED
#define SYSTEM_CHECK_ENABLED    1               // Enable crash check by default
#endif

#define SYSTEM_CHECK_TIME       60000           // The system is considered stable after these many millis
#define SYSTEM_CHECK_MAX        5               // After this many crashes on boot
                                                // the system is flagged as unstable

//------------------------------------------------------------------------------
// EEPROM
//------------------------------------------------------------------------------

#define EEPROM_SIZE             4096            // EEPROM size in bytes
#define EEPROM_CUSTOM_RESET     0               // Address for the reset reason (1 byte)
#define EEPROM_CRASH_COUNTER    1               // Address for the crash counter (1 byte)
#define EEPROM_DATA_END         2               // End of custom EEPROM data block

//------------------------------------------------------------------------------
// HEARTBEAT
//------------------------------------------------------------------------------

#define HEARTBEAT_INTERVAL          300000      // Interval between heartbeat messages (in ms)
#define UPTIME_OVERFLOW             4294967295  // Uptime overflow value

//------------------------------------------------------------------------------
// RESET
//------------------------------------------------------------------------------

#define CUSTOM_RESET_HARDWARE   1               // Reset from hardware button
#define CUSTOM_RESET_WEB        2               // Reset from web interface
#define CUSTOM_RESET_TERMINAL   3               // Reset from terminal
#define CUSTOM_RESET_MQTT       4               // Reset via MQTT
#define CUSTOM_RESET_RPC        5               // Reset via RPC (HTTP)
#define CUSTOM_RESET_OTA        6               // Reset after successful OTA update
#define CUSTOM_RESET_HTTP       7               // Reset via HTTP GET
#define CUSTOM_RESET_NOFUSS     8               // Reset after successful NOFUSS update
#define CUSTOM_RESET_UPGRADE    9               // Reset after update from web interface
#define CUSTOM_RESET_FACTORY    10              // Factory reset from terminal

#define CUSTOM_RESET_MAX        10

PROGMEM const char custom_reset_hardware[] = "Hardware button";
PROGMEM const char custom_reset_web[] = "Reboot from web interface";
PROGMEM const char custom_reset_terminal[] = "Reboot from terminal";
PROGMEM const char custom_reset_mqtt[] = "Reboot from MQTT";
PROGMEM const char custom_reset_rpc[] = "Reboot from RPC";
PROGMEM const char custom_reset_ota[] = "Reboot after successful OTA update";
PROGMEM const char custom_reset_http[] = "Reboot from HTTP";
PROGMEM const char custom_reset_nofuss[] = "Reboot after successful NoFUSS update";
PROGMEM const char custom_reset_upgrade[] = "Reboot after successful web update";
PROGMEM const char custom_reset_factory[] = "Factory reset";
PROGMEM const char* const custom_reset_string[] = {
    custom_reset_hardware, custom_reset_web, custom_reset_terminal,
    custom_reset_mqtt, custom_reset_rpc, custom_reset_ota,
    custom_reset_http, custom_reset_nofuss, custom_reset_upgrade,
    custom_reset_factory
};

//------------------------------------------------------------------------------
// BUTTON
//------------------------------------------------------------------------------

#ifndef BUTTON_DEBOUNCE_DELAY
#define BUTTON_DEBOUNCE_DELAY       50          // Debounce delay (ms)
#endif

#ifndef BUTTON_DBLCLICK_DELAY
#define BUTTON_DBLCLICK_DELAY       500         // Time in ms to wait for a second (or third...) click
#endif

#ifndef BUTTON_LNGCLICK_DELAY
#define BUTTON_LNGCLICK_DELAY       1000        // Time in ms holding the button down to get a long click
#endif

#ifndef BUTTON_LNGLNGCLICK_DELAY
#define BUTTON_LNGLNGCLICK_DELAY    10000       // Time in ms holding the button down to get a long-long click
#endif

#define BUTTON_PIN                  0

// -----------------------------------------------------------------------------
// WIFI
// -----------------------------------------------------------------------------

#define WIFI_CONNECT_TIMEOUT    60000       // Connecting timeout for WIFI in ms
#define WIFI_RECONNECT_INTERVAL 180000      // If could not connect to WIFI, retry after this time in ms
#define WIFI_MAX_NETWORKS       5           // Max number of WIFI connection configurations
#define WIFI_AP_MODE            AP_MODE_ALONE
#define WIFI_SLEEP_ENABLED      1           // Enable WiFi light sleep

// Optional hardcoded configuration (up to 2 different networks)
//#define WIFI1_SSID              "..."
//#define WIFI1_PASS              "..."
//#define WIFI1_IP                "192.168.1.201"
//#define WIFI1_GW                "192.168.1.1"
//#define WIFI1_MASK              "255.255.255.0"
//#define WIFI1_DNS               "8.8.8.8"
//#define WIFI2_SSID              "..."
//#define WIFI2_PASS              "..."

#define WIFI_RSSI_1M            -30         // Calibrate it with your router reading the RSSI at 1m
#define WIFI_PROPAGATION_CONST  4           // This is typically something between 2.7 to 4.3 (free space is 2)

// -----------------------------------------------------------------------------
// WEB
// -----------------------------------------------------------------------------

#ifndef WEB_SUPPORT
#define WEB_SUPPORT             1           // Enable web support (http, api)
#endif

#ifndef WEB_EMBEDDED
#define WEB_EMBEDDED            1           // Build the firmware with the web interface embedded in
#endif

// This is not working at the moment!!
// Requires ASYNC_TCP_SSL_ENABLED to 1 and ESP8266 Arduino Core 2.4.0
#define WEB_SSL_ENABLED         0           // Use HTTPS web interface

#define WEB_MODE_NORMAL         0
#define WEB_MODE_PASSWORD       1

#define WEB_USERNAME            "admin"     // HTTP username
#define WEB_FORCE_PASS_CHANGE   1           // Force the user to change the password if default one
#define WEB_PORT                80          // HTTP port

// -----------------------------------------------------------------------------
// WEBSOCKETS
// -----------------------------------------------------------------------------

// This will only be enabled if WEB_SUPPORT is 1 (this is the default value)

#define WS_BUFFER_SIZE          5           // Max number of secured websocket connections
#define WS_TIMEOUT              1800000     // Timeout for secured websocket

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

// This will only be enabled if WEB_SUPPORT is 1 (this is the default value)

#define API_ENABLED             0           // Do not enable API by default
#define API_BUFFER_SIZE         15          // Size of the buffer for HTTP GET API responses
#define API_REAL_TIME_VALUES    0           // Show filtered/median values by default (0 => median, 1 => real time)

// -----------------------------------------------------------------------------
// MDNS
// -----------------------------------------------------------------------------

#ifndef MDNS_SUPPORT
#define MDNS_SUPPORT            1           // Publish services using mDNS by default
#endif

// -----------------------------------------------------------------------------
// SPIFFS
// -----------------------------------------------------------------------------

#ifndef SPIFFS_SUPPORT
#define SPIFFS_SUPPORT          0           // Do not add support for SPIFFS by default
#endif

// -----------------------------------------------------------------------------
// OTA
// -----------------------------------------------------------------------------

#define OTA_PORT                8266        // OTA port

// -----------------------------------------------------------------------------
// NOFUSS
// -----------------------------------------------------------------------------

#ifndef NOFUSS_SUPPORT
#define NOFUSS_SUPPORT          0          // Do not enable support for NoFuss by default
#endif

#define NOFUSS_ENABLED          0           // Do not perform NoFUSS updates by default
#define NOFUSS_SERVER           ""          // Default NoFuss Server
#define NOFUSS_INTERVAL         3600000     // Check for updates every hour

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------

#ifndef MQTT_SUPPORT
#define MQTT_SUPPORT            1           // MQTT support
#endif


#ifndef MQTT_USE_ASYNC
#define MQTT_USE_ASYNC          1           // Use AysncMQTTClient (1) or PubSubClient (0)
#endif

// MQTT OVER SSL
// Using MQTT over SSL works pretty well but generates problems with the web interface.
// It could be a good idea to use it in conjuntion with WEB_SUPPORT=0.
// Requires ASYNC_TCP_SSL_ENABLED to 1 and ESP8266 Arduino Core 2.4.0.
//
// You can use it with MQTT_USE_ASYNC=1 (AsyncMqttClient library)
// but you might experience hiccups on the web interface, so my recommendation is:
// WEB_SUPPORT=0
//
// If you use it with MQTT_USE_ASYNC=0 (PubSubClient library)
// you will have to disable all the modules that use ESPAsyncTCP, that is:
// ALEXA_SUPPORT=0, INFLUXDB_SUPPORT=0, TELNET_SUPPORT=0 and WEB_SUPPORT=0
//
// You will need the fingerprint for your MQTT server, example for CloudMQTT:
// $ echo -n | openssl s_client -connect m11.cloudmqtt.com:24055 > cloudmqtt.pem
// $ openssl x509 -noout -in cloudmqtt.pem -fingerprint -sha1

#define MQTT_SSL_ENABLED        0           // By default MQTT over SSL will not be enabled
#define MQTT_SSL_FINGERPRINT    ""          // SSL fingerprint of the server

#define MQTT_ENABLED            0           // Do not enable MQTT connection by default
#define MQTT_AUTOCONNECT        1           // If enabled and MDNS_SUPPORT=1 will perform an autodiscover and
                                            // autoconnect to the first MQTT broker found if none defined
#define MQTT_SERVER             ""          // Default MQTT broker address
#define MQTT_USER               ""          // Default MQTT broker usename
#define MQTT_PASS               ""          // Default MQTT broker password
#define MQTT_PORT               1883        // MQTT broker port
#define MQTT_TOPIC              "/test/sonoffsc"    // Default MQTT base topic
#define MQTT_RETAIN             true        // MQTT retain flag
#define MQTT_QOS                0           // MQTT QoS value for all messages
#define MQTT_KEEPALIVE          30          // MQTT keepalive value

#define MQTT_RECONNECT_DELAY_MIN    5000    // Try to reconnect in 5 seconds upon disconnection
#define MQTT_RECONNECT_DELAY_STEP   5000    // Increase the reconnect delay in 5 seconds after each failed attempt
#define MQTT_RECONNECT_DELAY_MAX    120000  // Set reconnect time to 2 minutes at most

#define MQTT_SKIP_RETAINED      1           // Skip retained messages on connection
#define MQTT_SKIP_TIME          1000        // Skip messages for 1 second anter connection

#define MQTT_USE_JSON           0           // Group messages in a JSON body
#define MQTT_USE_JSON_DELAY     100         // Wait this many ms before grouping messages
#define MQTT_QUEUE_MAX_SIZE     10          // Size of the MQTT queue when MQTT_USE_JSON is enabled

// Topics
#define MQTT_TOPIC_STATUS       "status"
#define MQTT_TOPIC_IP           "ip"
#define MQTT_TOPIC_VERSION      "version"
#define MQTT_TOPIC_HEARTBEAT    "heartbeat"
#define MQTT_TOPIC_MODE         "mode"
#define MQTT_TOPIC_INTERVAL     "interval"
#define MQTT_TOPIC_TEMPERATURE  "temperature"
#define MQTT_TOPIC_HUMIDITY     "humidity"
#define MQTT_TOPIC_NOISE        "noise"
#define MQTT_TOPIC_CLAP         "clap"
#define MQTT_TOPIC_DUST         "dust"
#define MQTT_TOPIC_LIGHT        "light"
#define MQTT_TOPIC_RGB			"color"
#define MQTT_TOPIC_BRIGHTNESS   "brightness"
#define MQTT_TOPIC_SPEED        "speed"
#define MQTT_TOPIC_EFFECT		"effect"
#define MQTT_TOPIC_MOVE			"movement"

#define MQTT_TOPIC_TIME         "time"
#define MQTT_TOPIC_HOSTNAME     "host"

#define MQTT_TOPIC_JSON         "data"
#define MQTT_TOPIC_ACTION       "action"
#define MQTT_ACTION_RESET       "reboot"

// Internal MQTT events (do not change)
#define MQTT_CONNECT_EVENT      0
#define MQTT_DISCONNECT_EVENT   1
#define MQTT_MESSAGE_EVENT      2

// Custom get and set postfixes
// Use something like "/status" or "/set", with leading slash
// Since 1.9.0 the default value is "" for getter and "/set" for setter
#define MQTT_USE_GETTER         ""
#define MQTT_USE_SETTER         "/set"

// -----------------------------------------------------------------------------
// SETTINGS
// -----------------------------------------------------------------------------

#ifndef SETTINGS_AUTOSAVE
#define SETTINGS_AUTOSAVE       1           // Autosave settings o force manual commit
#endif

#define SETTINGS_MAX_LIST_COUNT 10          // Maximum index for settings lists

// -----------------------------------------------------------------------------
// DOMOTICZ
// -----------------------------------------------------------------------------

#ifndef DOMOTICZ_SUPPORT
#define DOMOTICZ_SUPPORT        MQTT_SUPPORT    // Build with domoticz (if MQTT)
#endif

#if DOMOTICZ_SUPPORT
#undef MQTT_SUPPORT
#define MQTT_SUPPORT            1               // If Domoticz enabled enable MQTT
#endif

#define DOMOTICZ_ENABLED        0               // Disable domoticz by default
#define DOMOTICZ_IN_TOPIC       "domoticz/in"   // Default subscription topic
#define DOMOTICZ_OUT_TOPIC      "domoticz/out"  // Default publication topic

// -----------------------------------------------------------------------------
// NTP
// -----------------------------------------------------------------------------

#ifndef NTP_SUPPORT
#define NTP_SUPPORT             1               // Build with NTP support by default
#endif

#define NTP_SERVER              "pool.ntp.org"  // Default NTP server
#define NTP_TIME_OFFSET         1               // Default timezone offset (GMT+1)
#define NTP_DAY_LIGHT           true            // Enable daylight time saving by default
#define NTP_UPDATE_INTERVAL     1800            // NTP check every 30 minutes

// -----------------------------------------------------------------------------
// ALEXA
// -----------------------------------------------------------------------------

// This setting defines whether Alexa support should be built into the firmware
#ifndef ALEXA_SUPPORT
#define ALEXA_SUPPORT           1               // Enable Alexa support by default (9.5Kb)
#endif

// This is default value for the alexaEnabled setting that defines whether
// this device should be discoberable and respond to Alexa commands.
// Both ALEXA_SUPPORT and alexaEnabled should be 1 for Alexa support to work.
#define ALEXA_ENABLED           1

// -----------------------------------------------------------------------------
// COMMS
// -----------------------------------------------------------------------------

#define COMMS_DEFAULT_TRIES     1

// -----------------------------------------------------------------------------
// NOTIFICATIONS
// -----------------------------------------------------------------------------

#define LOCAL_NOTIFICATION      0
#define NOTIFICATION_EFFECT     12
#define NOTIFICATION_SPEED      128
#define NOTIFICATION_COLOR      255UL           // Blue
#define NOTIFICATION_BRIGHTNESS 255
#define NOTIFICATION_TIME       30              // Seconds

// -----------------------------------------------------------------------------
// SENSORS
// -----------------------------------------------------------------------------

#define SENSOR_EVERY            60
#define SENSOR_CLAP_ENABLED     1

#define SENSOR_TEMPERATURE_MIN  -10
#define SENSOR_TEMPERATURE_MAX  50
#define SENSOR_PRESSURE_MIN     870
#define SENSOR_PRESSURE_MAX     1100
#define SENSOR_DUST_MIN         0
#define SENSOR_DUST_MAX         1
#define SENSOR_HUMIDITY_MIN     10
#define SENSOR_HUMIDITY_MAX     100

#define FAN_DELAY               0

// -----------------------------------------------------------------------------
// LED
// -----------------------------------------------------------------------------

#define LED_PIN                 13
