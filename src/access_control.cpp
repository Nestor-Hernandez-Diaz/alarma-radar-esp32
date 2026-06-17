#include "access_control.h"
#include "config.h"
#include <ESP32Servo.h>

Servo servoDoor;

#define DOOR_CLOSED_ANGLE 0
#define DOOR_OPEN_ANGLE 90

volatile bool doorOpen = false;
volatile SystemState currentState = STATE_SECURE;

unsigned long stateTimerStart = 0;
bool doorStateApplied = false;

void accessControlInit() {
    servoDoor.setPeriodHertz(50);
    servoDoor.attach(PIN_SERVO_PUERTA, 800, 2200);
    
    servoDoor.write(DOOR_CLOSED_ANGLE);
    doorOpen = false;
    currentState = STATE_SECURE;
    Serial.println("Access Control: Inicializado.");
}

void setDoorState(bool openDoor) {
    Serial.printf("Access Control: Cambiando estado de puerta -> %s\n", openDoor ? "ABIERTA" : "CERRADA");
    if (openDoor) {
        servoDoor.write(DOOR_OPEN_ANGLE);
        doorOpen = true;
    } else {
        servoDoor.write(DOOR_CLOSED_ANGLE);
        doorOpen = false;
    }
}

void accessControlLoop() {
    static SystemState lastState = STATE_SECURE;
    
    if (currentState != lastState) {
        stateTimerStart = millis();
        lastState = currentState;
        doorStateApplied = false;
    }
    
    if (currentState == STATE_GRANTED) {
        if (!doorStateApplied) {
            setDoorState(true);
            doorStateApplied = true;
        }
        if (millis() - stateTimerStart >= 5000) { // Auto close after 5 seconds
            setDoorState(false);
            currentState = STATE_SECURE;
        }
    } else if (currentState == STATE_DENIED) {
        if (!doorStateApplied) {
            setDoorState(false);
            doorStateApplied = true;
        }
        if (millis() - stateTimerStart >= 3000) { // Reset state after 3 seconds
            currentState = STATE_SECURE;
        }
    } else {
        // For secure, alert, waiting: door remains closed unless forced.
        if (currentState == STATE_SECURE || currentState == STATE_ALERT || currentState == STATE_WAITING) {
            if (doorOpen) {
                setDoorState(false);
            }
        }
    }
}