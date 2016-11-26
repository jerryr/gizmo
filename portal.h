#ifndef GIZMO_PORTAL_H
#define GIZMO_PORTAL_H
#include <FS.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiAP.h>
#include <ArduinoJson.h>
void start_portal();
void serve_root();
void save_bootstrap();
void handle_ap_clients();
void get_bootstrap();
#endif
