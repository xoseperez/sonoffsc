//------------------------------------------------------------------------------
// GENERAL
//------------------------------------------------------------------------------

#define SERIAL_BAUDRATE         115200
#define HOSTNAME                APP_NAME
#define DEVICE                  APP_NAME
#define MANUFACTURER            "ITEAD STUDIO"
#define HEARTBEAT_INTERVAL      300000

// -----------------------------------------------------------------------------
// WIFI & WEB
// -----------------------------------------------------------------------------

#define WIFI_RECONNECT_INTERVAL 300000
#define WIFI_MAX_NETWORKS       5
#define ADMIN_PASS              "fibonacci"
#define HTTP_USERNAME           "admin"
#define WS_BUFFER_SIZE          5
#define WS_TIMEOUT              1800000
#define DNS_PORT                53

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

#define MQTT_IP_TOPIC           "/ip"
#define MQTT_VERSION_TOPIC      "/version"
#define MQTT_HEARTBEAT_TOPIC    "/heartbeat"

#define MQTT_MODE_TOPIC         "/mode"
#define MQTT_INTERVAL_TOPIC     "/interval"


#define MQTT_TEMPERATURE_TOPIC  "/temperature"
#define MQTT_HUMIDITY_TOPIC     "/humidity"
#define MQTT_NOISE_TOPIC        "/noise"
#define MQTT_CLAP_TOPIC         "/clap"
#define MQTT_DUST_TOPIC         "/dust"
#define MQTT_LIGHT_TOPIC        "/light"
#define MQTT_RGB_TOPIC			"/rgb"

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
