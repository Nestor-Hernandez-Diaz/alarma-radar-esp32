#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Pines recomendados
#define PIN_HCSR04_TRIG  5
#define PIN_HCSR04_ECHO  18
#define PIN_SERVO_RADAR_PAN  4
#define PIN_SERVO_RADAR_TILT 27
#define PIN_SERVO_PUERTA 13
#define PIN_OLED_SDA     21
#define PIN_OLED_SCL     22
#define PIN_BUZZER       25
#define PIN_LED          26

// ==========================================
// 🛠️ PERFILES DE CONFIGURACIÓN DEL SERVO RADAR
// Descomenta SOLO UNO de los perfiles para probar
// ==========================================

// Perfil 1: SEGURIDAD MÁXIMA (Ideal para servos con batería baja o cables tensos. No se traba, giro ~120°)
#define SERVO_PAN_MIN_US      1000
#define SERVO_PAN_MAX_US      2000
#define RADAR_ANGLE_STEP      3
#define RADAR_MOVE_INTERVAL   120

// Perfil 2: EQUILIBRADO Y SUAVE (Giro de ~160°, silencioso y con buen torque. Recomendado)
//#define SERVO_PAN_MIN_US      800
//#define SERVO_PAN_MAX_US      2200
//#define RADAR_ANGLE_STEP      2
//#define RADAR_MOVE_INTERVAL   80

// Perfil 3: RECORRIDO COMPLETO (Giro de ~180° completo, pasos firmes e industriales)
// #define SERVO_PAN_MIN_US      600
// #define SERVO_PAN_MAX_US      2300
// #define RADAR_ANGLE_STEP      5
// #define RADAR_MOVE_INTERVAL   200

// Constantes de radar y alarma
#define ALARM_MIN_DIST_CM 3
#define VISITOR_MAX_DIST_CM 15
#define ALARM_MAX_DIST_CM 50
#define MAX_RADAR_DIST_CM 60

// Credenciales WiFi (ajustar en código o aquí para pruebas)
extern const char* WIFI_SSID;
extern const char* WIFI_PASS;

// Estados del sistema
enum SystemState {
    STATE_SECURE,
    STATE_ALERT,
    STATE_WAITING,
    STATE_GRANTED,
    STATE_DENIED
};

// Variables Globales de Estado compartidas
extern volatile SystemState currentState;
extern volatile bool alarmActive;
extern volatile bool doorOpen;
extern volatile int currentAngle;
extern volatile int currentTiltAngle;
extern volatile int currentDistance;

#endif // CONFIG_H