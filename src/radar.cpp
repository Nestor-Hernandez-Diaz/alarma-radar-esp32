#include "radar.h"
#include "config.h"
#include "web_server.h"
#include <ESP32Servo.h>
#include <NewPing.h>

Servo servoRadarPan;
Servo servoRadarTilt;
ESP32PWM buzzerPWM;
NewPing sonar(PIN_HCSR04_TRIG, PIN_HCSR04_ECHO, MAX_RADAR_DIST_CM);

int angle = 0;
int angleStep = RADAR_ANGLE_STEP; 
unsigned long lastRadarMove = 0;
const int radarMoveInterval = RADAR_MOVE_INTERVAL; 

bool alarmMuted = false;
unsigned long lastAlarmToggle = 0;
bool alarmState = false;

volatile bool alarmActive = false;
volatile int currentAngle = 0;
volatile int currentTiltAngle = 90;
volatile int currentDistance = 0;

// Timers for non-blocking alerts
unsigned long visitorDetectedTime = 0;
unsigned long lastVisitorInZone = 0;

void radarInit() {
    servoRadarPan.setPeriodHertz(50);
    servoRadarPan.attach(PIN_SERVO_RADAR_PAN, SERVO_PAN_MIN_US, SERVO_PAN_MAX_US); 

    servoRadarTilt.setPeriodHertz(50);
    servoRadarTilt.attach(PIN_SERVO_RADAR_TILT, SERVO_PAN_MIN_US, SERVO_PAN_MAX_US); // Límites calibrados seguros para SG90

    // Inicializar buzzer usando la clase ESP32PWM
    buzzerPWM.attachPin(PIN_BUZZER, 1000, 10);
    buzzerPWM.write(0); // Silencio inicial

    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);
    
    servoRadarPan.write(0);
    servoRadarTilt.write(currentTiltAngle);
    Serial.println("Radar: Inicializado.");
}

void setRadarTilt(int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    currentTiltAngle = angle;
    servoRadarTilt.write(angle);
}

void muteAlarm() {
    alarmMuted = true;
    if (buzzerPWM.read() != 0) {
        buzzerPWM.write(0);
    }
}

// Handles buzzer and LED rhythms based on the current state (non-blocking)
void updateBuzzerLED() {
    static SystemState prevBuzzerState = STATE_SECURE;
    static unsigned long buzzerStateStartTime = 0;
    
    if (currentState != prevBuzzerState) {
        buzzerStateStartTime = millis();
        prevBuzzerState = currentState;
        alarmMuted = false; // Reset mute when transitioning states
        if (buzzerPWM.read() != 0) {
            buzzerPWM.write(0);
        }
        digitalWrite(PIN_LED, LOW);
    }
    
    unsigned long elapsed = millis() - buzzerStateStartTime;
    
    switch (currentState) {
        case STATE_SECURE:
            alarmActive = false;
            if (buzzerPWM.read() != 0) {
                buzzerPWM.write(0);
            }
            digitalWrite(PIN_LED, LOW);
            break;
            
        case STATE_ALERT:
            alarmActive = true;
            if (alarmMuted) {
                if (buzzerPWM.read() != 0) {
                    buzzerPWM.write(0);
                }
                digitalWrite(PIN_LED, HIGH); // LED stays on
            } else {
                // Siren sweep: 600Hz to 1200Hz back and forth in a 1-second cycle
                int cycleTime = elapsed % 1000;
                int freq;
                if (cycleTime < 500) {
                    freq = 600 + (cycleTime * 1.2); // Subida
                } else {
                    freq = 1200 - ((cycleTime - 500) * 1.2); // Bajada
                }
                buzzerPWM.writeTone(freq);
                
                // Parpadeo de LED a 4Hz (cada 250ms)
                digitalWrite(PIN_LED, (elapsed / 250) % 2 ? HIGH : LOW);
            }
            break;
            
        case STATE_WAITING:
            alarmActive = false;
            // Play Ding-Dong sequence once using buzzerPWM.writeTone()
            if (elapsed < 200) { // "Ding"
                buzzerPWM.writeTone(880); // 880 Hz
                digitalWrite(PIN_LED, HIGH);
            } else if (elapsed >= 200 && elapsed < 350) { // Gap
                if (buzzerPWM.read() != 0) {
                    buzzerPWM.write(0);
                }
                digitalWrite(PIN_LED, LOW);
            } else if (elapsed >= 350 && elapsed < 750) { // "Dong"
                buzzerPWM.writeTone(659); // 659 Hz
                digitalWrite(PIN_LED, HIGH);
            } else { // End of chime
                if (buzzerPWM.read() != 0) {
                    buzzerPWM.write(0);
                }
                digitalWrite(PIN_LED, LOW);
            }
            break;
            
        case STATE_GRANTED:
            alarmActive = false;
            // A happy longer beep (1000Hz for 400ms)
            if (elapsed < 400) {
                buzzerPWM.writeTone(1000);
                digitalWrite(PIN_LED, HIGH);
            } else {
                if (buzzerPWM.read() != 0) {
                    buzzerPWM.write(0);
                }
                digitalWrite(PIN_LED, HIGH); // LED stays on for opening
            }
            break;
            
        case STATE_DENIED:
            alarmActive = false;
            // A low warnings tone or long beep (300Hz for 800ms)
            if (elapsed < 800) {
                buzzerPWM.writeTone(300);
                digitalWrite(PIN_LED, HIGH);
            } else {
                if (buzzerPWM.read() != 0) {
                    buzzerPWM.write(0);
                }
                digitalWrite(PIN_LED, LOW);
            }
            break;
    }
}

void radarLoop() {
    // Run non-blocking alert rhythms
    updateBuzzerLED();

    if (millis() - lastRadarMove >= radarMoveInterval) {
        lastRadarMove = millis();
        
        servoRadarPan.write(angle);
        currentAngle = angle;
        
        // Measure distance
        unsigned int uS = sonar.ping();
        currentDistance = uS / US_ROUNDTRIP_CM;
        if (currentDistance == 0) currentDistance = MAX_RADAR_DIST_CM;
        
        // Impresión de depuración en monitor serie cada 500ms
        static unsigned long lastDebugPrint = 0;
        if (millis() - lastDebugPrint > 500) {
            Serial.printf("Radar Debug -> Angulo: %d | Distancia: %d cm | Estado: %d\n", currentAngle, currentDistance, currentState);
            lastDebugPrint = millis();
        }
        
        // Only classify and alert if we aren't in a transient authorized state (GRANTED/DENIED)
        if (currentState != STATE_GRANTED && currentState != STATE_DENIED) {
            
            if (currentDistance >= ALARM_MIN_DIST_CM && currentDistance <= VISITOR_MAX_DIST_CM) {
                // Zone 1: Visitor at the door
                currentState = STATE_WAITING;
                lastVisitorInZone = millis();
            } 
            else if (currentDistance > VISITOR_MAX_DIST_CM && currentDistance <= ALARM_MAX_DIST_CM) {
                // Zone 2: Intruder in the garden
                // Only alert if we were secure or alert (don't override waiting state unless they leave)
                if (currentState != STATE_WAITING) {
                    currentState = STATE_ALERT;
                } else {
                    // If we were waiting, check if they retreated back to the garden
                    if (millis() - lastVisitorInZone > 5000) {
                        currentState = STATE_ALERT;
                    }
                }
            } 
            else {
                // Zone 3: Clear
                if (currentState == STATE_ALERT) {
                    currentState = STATE_SECURE;
                } 
                else if (currentState == STATE_WAITING) {
                    // Auto reset waiting state if visitor left the door for more than 5 seconds
                    if (millis() - lastVisitorInZone > 5000) {
                        currentState = STATE_SECURE;
                    }
                }
            }
        }
        
        notifyClients(); // Notificar a los clientes web de la nueva posición y distancia

        angle += angleStep;
        if (angle >= 180 || angle <= 0) {
            angleStep = -angleStep; // Reverse direction
        }
    }
}