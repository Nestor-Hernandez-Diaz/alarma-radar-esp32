#ifndef WEB_SERVER_MODULE_H
#define WEB_SERVER_MODULE_H

#include <Arduino.h>

void webServerInit();
void webServerLoop();
void notifyClients();
String getIPAddress();

#endif // WEB_SERVER_MODULE_H