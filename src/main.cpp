#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "radar.h"
#include "access_control.h"
#include "web_server.h"
#include <ESP32Servo.h>

// Define uninitialized extern variables from config.h
const char* WIFI_SSID = "Karen";
const char* WIFI_PASS = "coco1234";

unsigned long lastDisplayUpdate = 0;
const int displayUpdateInterval = 1000;

void setup() {
    Serial.begin(115200);
    
    // Centralizar la asignación de timers de hardware para servos y buzzer
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    
    // Initialize components
    displayInit();
    displayShowStatus("Conectando WiFi...");
    
    accessControlInit();
    radarInit();
    webServerInit();
    
    displayShowStatus(getIPAddress().c_str());
}

void loop() {
    // Non-blocking loops
    radarLoop();
    accessControlLoop();
    webServerLoop();
    
    // Lectura de comandos manuales desde el Monitor Serial
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.equalsIgnoreCase("OPEN")) {
            currentState = STATE_GRANTED;
        } else if (input.equalsIgnoreCase("CLOSE")) {
            currentState = STATE_SECURE;
        }
    }
    
    // Update display occasionally to not block I2C bus too much
    if (millis() - lastDisplayUpdate > displayUpdateInterval) {
        lastDisplayUpdate = millis();
        displayShowStatus(getIPAddress().c_str());
    }
}
