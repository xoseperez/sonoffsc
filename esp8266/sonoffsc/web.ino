/*

WEBSERVER MODULE

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <Hash.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <Ticker.h>

AsyncWebServer * _server;
AsyncWebSocket ws("/ws");
Ticker deferred;

#include "static/index.html.gz.h"
char _last_modified[50];

typedef struct {
    IPAddress ip;
    unsigned long timestamp = 0;
} ws_ticket_t;
ws_ticket_t _ticket[WS_BUFFER_SIZE];

// -----------------------------------------------------------------------------
// WEBSOCKETS
// -----------------------------------------------------------------------------

bool wsSend(const char * payload) {
    if (ws.count() > 0) {
        DEBUG_MSG_P(PSTR("[WEBSOCKET] Broadcasting '%s'\n"), payload);
        ws.textAll(payload);
    }
}

bool wsSend(uint32_t client_id, const char * payload) {
    DEBUG_MSG_P(PSTR("[WEBSOCKET] Sending '%s' to #%ld\n"), payload, client_id);
    ws.text(client_id, payload);
}

void wsMQTTCallback(unsigned int type, const char * topic, const char * payload) {

    if (type == MQTT_CONNECT_EVENT) {
        wsSend("{\"mqttStatus\": true}");
    }

    if (type == MQTT_DISCONNECT_EVENT) {
        wsSend("{\"mqttStatus\": false}");
    }

}

void _wsParse(uint32_t client_id, uint8_t * payload, size_t length) {

    // Parse JSON input
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject((char *) payload);
    if (!root.success()) {
        DEBUG_MSG_P(PSTR("[WEBSOCKET] Error parsing data\n"));
        ws.text(client_id, "{\"message\": \"Error parsing data!\"}");
        return;
    }

    // Check actions
    if (root.containsKey("action")) {

        String action = root["action"];

        DEBUG_MSG_P(PSTR("[WEBSOCKET] Requested action: %s\n"), action.c_str());

        if (action.equals("reset")) {
            ESP.restart();
        }

        if (action.equals("reconnect")) {

            // Let the HTTP request return and disconnect after 100ms
            deferred.once_ms(100, wifiDisconnect);

        }

    };

    // Check config
    if (root.containsKey("config") && root["config"].is<JsonArray&>()) {

        JsonArray& config = root["config"];
        DEBUG_MSG_P(PSTR("[WEBSOCKET] Parsing configuration data\n"));

        unsigned char webMode = WEB_MODE_NORMAL;

        bool save = false;
        bool changed = false;
        bool changedMQTT = false;
        bool apiEnabled = false;
        bool clapEnabled = false;
        unsigned int network = 0;
        String adminPass;

        for (unsigned int i=0; i<config.size(); i++) {

            String key = config[i]["name"];
            String value = config[i]["value"];

            if (key == "webMode") {
                webMode = value.toInt();
                continue;
            }

            // Check password
            if (key == "adminPass1") {
                adminPass = value;
                continue;
            }
            if (key == "adminPass2") {
                if (!value.equals(adminPass)) {
                    ws.text(client_id, "{\"message\": \"Passwords do not match!\"}");
                    return;
                }
                if (value.length() == 0) continue;
                ws.text(client_id, "{\"action\": \"reload\"}");
                key = String("adminPass");
            }

            // Checkboxes
            if (key == "apiEnabled") {
                apiEnabled = true;
                continue;
            }
            if (key == "clapEnabled") {
                clapEnabled = true;
                continue;
            }

            if (key == "ssid") {
                key = key + String(network);
            }
            if (key == "pass") {
                key = key + String(network);
            }
            if (key == "ip") {
                key = key + String(network);
            }
            if (key == "gw") {
                key = key + String(network);
            }
            if (key == "mask") {
                key = key + String(network);
            }
            if (key == "dns") {
                key = key + String(network);
                ++network;
            }

            if (value != getSetting(key)) {
                //DEBUG_MSG_P(PSTR("[WEBSOCKET] Storing %s = %s\n", key.c_str(), value.c_str()));
                setSetting(key, value);
                save = changed = true;
                if (key.startsWith("mqtt")) changedMQTT = true;
            }

        }

        if (webMode == WEB_MODE_NORMAL) {

            // Checkboxes
            if (apiEnabled != (getSetting("apiEnabled").toInt() == 1)) {
                setSetting("apiEnabled", apiEnabled);
                save = changed = true;
            }

            if (clapEnabled != (getSetting("clapEnabled", SENSOR_CLAP_ENABLED).toInt() == 1)) {
                setSetting("clapEnabled", clapEnabled);
                save = changed = true;
            }

            // Clean wifi networks
            int i = 0;
            while (i < network) {
                if (getSetting("ssid" + String(i)).length() == 0) {
                    delSetting("ssid" + String(i));
                    break;
                }
                if (getSetting("pass" + String(i)).length() == 0) delSetting("pass" + String(i));
                if (getSetting("ip" + String(i)).length() == 0) delSetting("ip" + String(i));
                if (getSetting("gw" + String(i)).length() == 0) delSetting("gw" + String(i));
                if (getSetting("mask" + String(i)).length() == 0) delSetting("mask" + String(i));
                if (getSetting("dns" + String(i)).length() == 0) delSetting("dns" + String(i));
                ++i;
            }
            while (i < WIFI_MAX_NETWORKS) {
                if (getSetting("ssid" + String(i)).length() > 0) {
                    save = changed = true;
                }
                delSetting("ssid" + String(i));
                delSetting("pass" + String(i));
                delSetting("ip" + String(i));
                delSetting("gw" + String(i));
                delSetting("mask" + String(i));
                delSetting("dns" + String(i));
                ++i;
            }

        }

        // Save settings
        if (save) {

            saveSettings();
            wifiConfigure();
            otaConfigure();
            buildTopics();
            commsConfigure();

            // Check if we should reconfigure MQTT connection
            if (changedMQTT) {
                mqttDisconnect();
            }

        }

        if (changed) {
            ws.text(client_id, "{\"message\": \"Changes saved\"}");
        } else {
            ws.text(client_id, "{\"message\": \"No changes detected\"}");
        }

    }

}

void _wsStart(uint32_t client_id) {

    char chipid[6];
    sprintf(chipid, "%06X", ESP.getChipId());

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    bool changePassword = false;
    #if FORCE_CHANGE_PASS == 1
        String adminPass = getSetting("adminPass", ADMIN_PASS);
        if (adminPass.equals(ADMIN_PASS)) changePassword = true;
    #endif

    if (changePassword) {

        root["webMode"] = WEB_MODE_PASSWORD;

    } else {

        root["webMode"] = WEB_MODE_NORMAL;

        root["app"] = APP_NAME;
        root["version"] = APP_VERSION;
        root["buildDate"] = __DATE__;
        root["buildTime"] = __TIME__;

        root["manufacturer"] = String(MANUFACTURER);
        root["chipid"] = chipid;
        root["mac"] = WiFi.macAddress();
        root["device"] = String(DEVICE);
        root["hostname"] = getSetting("hostname", HOSTNAME);
        root["network"] = getNetwork();
        root["deviceip"] = getIP();

        root["mqttStatus"] = mqttConnected();
        root["mqttServer"] = getSetting("mqttServer", MQTT_SERVER);
        root["mqttPort"] = getSetting("mqttPort", MQTT_PORT);
        root["mqttUser"] = getSetting("mqttUser");
        root["mqttPassword"] = getSetting("mqttPassword");

        root["mqttTopic"] = getSetting("mqttTopic", MQTT_TOPIC);
        root["mqttTopicLight"] = getSetting("mqttTopicLight", MQTT_TOPIC_LIGHT);
        root["mqttTopicTemp"] = getSetting("mqttTopicTemp", MQTT_TOPIC_TEMPERATURE);
        root["mqttTopicHum"] = getSetting("mqttTopicHum", MQTT_TOPIC_HUMIDITY);
        root["mqttTopicNoise"] = getSetting("mqttTopicNoise", MQTT_TOPIC_NOISE);
        root["mqttTopicDust"] = getSetting("mqttTopicDust", MQTT_TOPIC_DUST);
        root["mqttTopicMovement"] = getSetting("mqttTopicMovement", MQTT_TOPIC_MOVE);
        root["mqttTopicClap"] = getSetting("mqttTopicClap", MQTT_TOPIC_CLAP);

        root["apiVisible"] = 0;
        root["apiEnabled"] = getSetting("apiEnabled").toInt() == 1;
        root["apiKey"] = getSetting("apiKey");

        #if ENABLE_DOMOTICZ

            root["dczVisible"] = 1;
            root["dczTopicIn"] = getSetting("dczTopicIn", DOMOTICZ_IN_TOPIC);
            root["dczTopicOut"] = getSetting("dczTopicOut", DOMOTICZ_OUT_TOPIC);

            root["dczIdxTemp"] = getSetting("dczIdxTemp").toInt();
            root["dczIdxHum"] = getSetting("dczIdxHum").toInt();
            root["dczIdxNoise"] = getSetting("dczIdxNoise").toInt();
            root["dczIdxMovement"] = getSetting("dczIdxMovement").toInt();
            root["dczIdxLight"] = getSetting("dczIdxLight").toInt();
            root["dczIdxDust"] = getSetting("dczIdxDust").toInt();

        #endif

        root["sensorTemp"] = getTemperature();
        root["sensorHum"] = getHumidity();
        root["sensorLight"] = getLight();
        root["sensorNoise"] = getNoise();
        root["sensorDust"] = getDust();
        root["sensorMove"] = getMovement();
        root["sensorEvery"] = getSetting("sensorEvery", SENSOR_EVERY).toInt();
        root["clapEnabled"] = getSetting("clapEnabled", SENSOR_CLAP_ENABLED).toInt() == 1;

        root["maxNetworks"] = WIFI_MAX_NETWORKS;
        JsonArray& wifi = root.createNestedArray("wifi");
        for (byte i=0; i<WIFI_MAX_NETWORKS; i++) {
            if (getSetting("ssid" + String(i)).length() == 0) break;
            JsonObject& network = wifi.createNestedObject();
            network["ssid"] = getSetting("ssid" + String(i));
            network["pass"] = getSetting("pass" + String(i));
            network["ip"] = getSetting("ip" + String(i));
            network["gw"] = getSetting("gw" + String(i));
            network["mask"] = getSetting("mask" + String(i));
            network["dns"] = getSetting("dns" + String(i));
        }

    }

    String output;
    root.printTo(output);
    ws.text(client_id, (char *) output.c_str());

}

bool _wsAuth(AsyncWebSocketClient * client) {

    IPAddress ip = client->remoteIP();
    unsigned long now = millis();
    unsigned short index = 0;

    for (index = 0; index < WS_BUFFER_SIZE; index++) {
        if ((_ticket[index].ip == ip) && (now - _ticket[index].timestamp < WS_TIMEOUT)) break;
    }

    if (index == WS_BUFFER_SIZE) {
        DEBUG_MSG_P(PSTR("[WEBSOCKET] Validation check failed\n"));
        ws.text(client->id(), "{\"message\": \"Session expired, please reload page...\"}");
        return false;
    }

    return true;

}

void _wsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){

    static uint8_t * message;

    // Authorize
    #ifndef NOWSAUTH
        if (!_wsAuth(client)) return;
    #endif

    if (type == WS_EVT_CONNECT) {
        IPAddress ip = client->remoteIP();
        DEBUG_MSG_P(PSTR("[WEBSOCKET] #%u connected, ip: %d.%d.%d.%d, url: %s\n"), client->id(), ip[0], ip[1], ip[2], ip[3], server->url());
        _wsStart(client->id());
    } else if(type == WS_EVT_DISCONNECT) {
        DEBUG_MSG_P(PSTR("[WEBSOCKET] #%u disconnected\n"), client->id());
    } else if(type == WS_EVT_ERROR) {
        DEBUG_MSG_P(PSTR("[WEBSOCKET] #%u error(%u): %s\n"), client->id(), *((uint16_t*)arg), (char*)data);
    } else if(type == WS_EVT_PONG) {
        DEBUG_MSG_P(PSTR("[WEBSOCKET] #%u pong(%u): %s\n"), client->id(), len, len ? (char*) data : "");
    } else if(type == WS_EVT_DATA) {

        AwsFrameInfo * info = (AwsFrameInfo*)arg;

        // First packet
        if (info->index == 0) {
            message = (uint8_t*) malloc(info->len);
        }

        // Store data
        memcpy(message + info->index, data, len);

        // Last packet
        if (info->index + len == info->len) {
            _wsParse(client->id(), message, info->len);
            free(message);
        }

    }

}

// -----------------------------------------------------------------------------
// WEBSERVER
// -----------------------------------------------------------------------------

void _logRequest(AsyncWebServerRequest *request) {
    DEBUG_MSG("[WEBSERVER] Request: %s %s\n", request->methodToString(), request->url().c_str());
}

bool _authenticate(AsyncWebServerRequest *request) {
    String password = getSetting("adminPass", ADMIN_PASS);
    char httpPassword[password.length() + 1];
    password.toCharArray(httpPassword, password.length() + 1);
    return request->authenticate(HTTP_USERNAME, httpPassword);
}

void _onAuth(AsyncWebServerRequest *request) {

    _logRequest(request);

    if (!_authenticate(request)) return request->requestAuthentication();

    IPAddress ip = request->client()->remoteIP();
    unsigned long now = millis();
    unsigned short index;
    for (index = 0; index < WS_BUFFER_SIZE; index++) {
        if (_ticket[index].ip == ip) break;
        if (_ticket[index].timestamp == 0) break;
        if (now - _ticket[index].timestamp > WS_TIMEOUT) break;
    }
    if (index == WS_BUFFER_SIZE) {
        request->send(429);
    } else {
        _ticket[index].ip = ip;
        _ticket[index].timestamp = now;
        request->send(204);
    }

}

void _onHome(AsyncWebServerRequest *request) {

    _logRequest(request);

    if (request->header("If-Modified-Since").equals(_last_modified)) {

        request->send(304);

    } else {

        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html_gz, index_html_gz_len);
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("Last-Modified", _last_modified);
        request->send(response);

    }

}

bool _apiAuth(AsyncWebServerRequest *request) {

    if (getSetting("apiEnabled").toInt() == 0) {
        DEBUG_MSG("[WEBSERVER] HTTP API is not enabled\n");
        request->send(403);
        return false;
    }

    if (!request->hasParam("apikey", (request->method() == HTTP_PUT))) {
        DEBUG_MSG("[WEBSERVER] Missing apikey parameter\n");
        request->send(403);
        return false;
    }

    AsyncWebParameter* p = request->getParam("apikey", (request->method() == HTTP_PUT));
    if (!p->value().equals(getSetting("apiKey"))) {
        DEBUG_MSG("[WEBSERVER] Wrong apikey parameter\n");
        request->send(403);
        return false;
    }

    return true;

}

void _onSensors(AsyncWebServerRequest *request) {

    _logRequest(request);

    if (!_apiAuth(request)) return;

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    root["temperature"] = getTemperature();
    root["humidity"] = getHumidity();
    root["dust"] = getDust();
    root["light"] = getLight();
    root["noise"] = getNoise();
    root["movement"] = getMovement();

    String output;
    root.printTo(output);
    request->send(200, "application/json", output);

}

void webSetup() {

    // Create server
    _server = new AsyncWebServer(getSetting("webPort", WEBSERVER_PORT).toInt());

    // Setup websocket
    ws.onEvent(_wsEvent);
    mqttRegister(wsMQTTCallback);

    // Cache the Last-Modifier header value
    sprintf(_last_modified, "%s %s GMT", __DATE__, __TIME__);

    // Setup webserver
    _server->addHandler(&ws);

    // Rewrites
    _server->rewrite("/", "/index.html");

    // Serve home (basic authentication protection)
    _server->on("/index.html", HTTP_GET, _onHome);
    _server->on("/auth", HTTP_GET, _onAuth);

    // TODO: API entry points
    _server->on("/api/sensors", HTTP_GET, _onSensors);

    // 404
    _server->onNotFound([](AsyncWebServerRequest *request){
        request->send(404);
    });

    // Run server
    _server->begin();
    DEBUG_MSG_P(PSTR("[WEBSERVER] Webserver running on port %d\n"), getSetting("webPort", WEBSERVER_PORT).toInt());

}
