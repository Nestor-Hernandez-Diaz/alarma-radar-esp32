#include "display.h"
#include "config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void displayInit() {
    Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        return;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Sistema Iniciando...");
    display.display();
}

void displayShowStatus(const char* ipStr) {
    static SystemState lastDispState = (SystemState)-1;
    static String lastDispIp = "";
    
    if (currentState == lastDispState && String(ipStr) == lastDispIp) {
        return; // Evita redibujar innecesariamente reduciendo el parpadeo
    }
    lastDispState = currentState;
    lastDispIp = String(ipStr);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    // Header
    display.setCursor(0, 0);
    display.println(F("--- MONITOREO HOGAR ---"));
    
    // IP Address at the bottom
    display.setCursor(0, 56);
    display.print(F("IP: "));
    display.print(ipStr);
    
    // Draw status screens based on currentState
    switch(currentState) {
        case STATE_SECURE:
            display.setTextSize(1);
            display.setCursor(5, 18);
            display.println(F("SISTEMA: SEGURO"));
            display.setCursor(5, 30);
            display.println(F("Buscando intrusos"));
            display.setCursor(5, 42);
            display.println(F("en el perimetro..."));
            
            // Draw a closed lock icon
            display.drawRect(95, 24, 16, 12, SSD1306_WHITE);
            display.drawCircle(103, 24, 6, SSD1306_WHITE);
            break;
            
        case STATE_ALERT:
            // Alarm warning
            display.setTextSize(2);
            display.setCursor(10, 18);
            display.println(F("!ALERTA!"));
            display.setTextSize(1);
            display.setCursor(10, 38);
            display.println(F("INTRUSO DETECTADO"));
            
            // Draw flashing warning border
            {
                static bool flash = false;
                flash = !flash;
                if(flash) {
                    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
                    display.drawRect(2, 2, SCREEN_WIDTH-4, SCREEN_HEIGHT-4, SSD1306_WHITE);
                }
            }
            break;
            
        case STATE_WAITING:
            display.setTextSize(2);
            // Centrado vertical y horizontal para texto grande
            display.setCursor(16, 16);
            display.print(F("¿QUIEN"));
            display.setCursor(16, 36);
            display.print(F("ESTA AHI?"));
            break;
            
        case STATE_GRANTED:
            display.setTextSize(2);
            display.setCursor(5, 18);
            display.println(F("ADELANTE"));
            display.setTextSize(1);
            display.setCursor(5, 38);
            display.println(F("Acceso Concedido"));
            
            // Draw open lock
            display.drawRect(95, 28, 16, 12, SSD1306_WHITE);
            display.drawLine(95, 28, 95, 22, SSD1306_WHITE);
            display.drawCircle(103, 22, 6, SSD1306_WHITE);
            display.fillRect(99, 23, 9, 6, SSD1306_BLACK);
            break;
            
        case STATE_DENIED:
            display.setTextSize(2);
            display.setCursor(5, 18);
            display.println(F("DENEGADO"));
            display.setTextSize(1);
            display.setCursor(5, 38);
            display.println(F("Acceso Rechazado"));
            
            // Draw an X
            display.drawLine(95, 22, 115, 42, SSD1306_WHITE);
            display.drawLine(115, 22, 95, 42, SSD1306_WHITE);
            break;
    }
    
    display.display();
}