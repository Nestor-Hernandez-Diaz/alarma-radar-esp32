#include "web_server.h"
#include "config.h"
#include "web_content.h"
#include "access_control.h"
#include "radar.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

unsigned long lastWsUpdate = 0;
const int wsUpdateInterval = 80; // Optimizado de 50ms a 80ms para reducir congestión

String getIPAddress() {
    if(WiFi.status() == WL_CONNECTED) {
        return WiFi.localIP().toString();
    }
    return "No Conectado";
}

void notifyClients() {
    if (ws.count() == 0) return; // Guard para evitar procesar si no hay clientes
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "{\"type\":\"radar\",\"angle\":%d,\"tilt\":%d,\"dist\":%d,\"alarm\":%s,\"doorOpen\":%s,\"state\":%d}", 
        currentAngle, currentTiltAngle, currentDistance, alarmActive ? "true":"false", doorOpen ? "true":"false", (int)currentState);
    ws.textAll(buffer);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        String message = "";
        for (size_t i = 0; i < len; i++) {
            message += (char)data[i];
        }
        
        if (message == "OPEN_DOOR") {
            currentState = STATE_GRANTED;
        } else if (message == "CLOSE_DOOR") {
            currentState = STATE_SECURE;
        } else if (message == "MUTE_ALARM") {
            muteAlarm();
        } else if (message == "GRANT_ACCESS") {
            currentState = STATE_GRANTED;
        } else if (message == "DENY_ACCESS") {
            currentState = STATE_DENIED;
        } else if (message.startsWith("TILT:")) {
            int tiltVal = message.substring(5).toInt();
            setRadarTilt(tiltVal);
        }
        
        notifyClients();
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf("WS Client %u connected\n", client->id());
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("WS Client %u disconnected\n", client->id());
    } else if (type == WS_EVT_DATA) {
        handleWebSocketMessage(arg, data, len);
    }
}

void webServerInit() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    Serial.print("Connecting to WiFi");
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20) {
        delay(500);
        Serial.print(".");
        retries++;
    }
    Serial.println();
    
    if(WiFi.status() == WL_CONNECTED) {
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("WiFi connection failed.");
    }
    
    ws.onEvent(onEvent);
    server.addHandler(&ws);
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });

    server.begin();
}

void webServerLoop() {
    ws.cleanupClients();
}