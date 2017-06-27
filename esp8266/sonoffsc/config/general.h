//------------------------------------------------------------------------------
// GENERAL
//------------------------------------------------------------------------------

#define SERIAL_BAUDRATE         57600
#define HOSTNAME                APP_NAME
#define DEVICE                  APP_NAME
#define MANUFACTURER            "ITEAD STUDIO"
#define HEARTBEAT_INTERVAL      300000

// -----------------------------------------------------------------------------
// DEBUG
// -----------------------------------------------------------------------------

// Only for debugging, won't work while both microcontrollers working together
//#define DEBUG_PORT                  Serial

// Uncomment and configure these lines to enable remote debug via udpDebug
// To receive the message son the destination computer use nc (nc -ul 8115)
//#define DEBUG_UDP_IP            IPAddress(192, 168, 1, 100)
//#define DEBUG_UDP_PORT          8115

#define DEBUG_MESSAGE_MAX_LENGTH    80

#if defined(DEBUG_PORT) | defined(DEBUG_UDP_IP)
    #define DEBUG_MSG(...) debugSend(__VA_ARGS__)
    #define DEBUG_MSG_P(...) debugSend_P(__VA_ARGS__)
#endif

#ifndef DEBUG_MSG
    #define DEBUG_MSG(...)
    #define DEBUG_MSG_P(...)
#endif

// -----------------------------------------------------------------------------
// WIFI & WEB
// -----------------------------------------------------------------------------

#define WIFI_RECONNECT_INTERVAL 300000
#define WIFI_MAX_NETWORKS       5
#define ADMIN_PASS              "fibonacci"
#define HTTP_USERNAME           "admin"
#define WS_BUFFER_SIZE          5
#define WS_TIMEOUT              1800000
#define WEBSERVER_PORT          80
#define DNS_PORT                53
#define FORCE_CHANGE_PASS       1

#define WEB_MODE_NORMAL         0
#define WEB_MODE_PASSWORD       1

// -----------------------------------------------------------------------------
// OTA
// -----------------------------------------------------------------------------

#define OTA_PORT                8266

// -----------------------------------------------------------------------------
// HARDWARE
// -----------------------------------------------------------------------------

#define BUTTON_PIN              0
#define LED_PIN                 13

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------

#define MQTT_USE_ASYNC          1

#define MQTT_SERVER             "192.168.1.100"
#define MQTT_PORT               1883
#define MQTT_RETAIN             true
#define MQTT_QOS                0
#define MQTT_KEEPALIVE          30
#define MQTT_RECONNECT_DELAY    10000
#define MQTT_SKIP_RETAINED      1
#define MQTT_SKIP_TIME          1000
#define MQTT_CONNECT_EVENT      0
#define MQTT_DISCONNECT_EVENT   1
#define MQTT_MESSAGE_EVENT      2

#define MQTT_TOPIC              "/test/sonoffsc"

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
#define MQTT_TOPIC_RGB			"rgb"
#define MQTT_TOPIC_EFFECT		"effect"
#define MQTT_TOPIC_MOVE			"movement"

#define MQTT_TOPIC_ACTION       "action"
#define MQTT_ACTION_RESET       "reset"

// -----------------------------------------------------------------------------
// BEHAVIOUR
// -----------------------------------------------------------------------------

#define LOCAL_NOTIFICATION      1
#define NOTIFICATION_ON_EFFECT  47
#define NOTIFICATION_OFF_EFFECT 48

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

#define ENABLE_REST_API         1

// -----------------------------------------------------------------------------
// DOMOTICZ
// -----------------------------------------------------------------------------

#define ENABLE_DOMOTICZ         1
#define DOMOTICZ_IN_TOPIC       "domoticz/in"
#define DOMOTICZ_OUT_TOPIC      "domoticz/out"

// -----------------------------------------------------------------------------
// SENSORS
// -----------------------------------------------------------------------------

#define SENSOR_EVERY            60
#define SENSOR_CLAP_ENABLED     1

// -----------------------------------------------------------------------------
// NTP
// -----------------------------------------------------------------------------

#define NTP_SERVER              "pool.ntp.org"
#define NTP_TIME_OFFSET         1
#define NTP_DAY_LIGHT           true
#define NTP_UPDATE_INTERVAL     1800
