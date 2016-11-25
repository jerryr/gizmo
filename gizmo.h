#ifndef GIZMO_H
#define GIZMO_H
#include "FS.h"
#include "ESP8266WiFi.h"
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>
#define BOOTSTRAP_FILE_NAME "/config/gizmo-config.json"
// Increment the version for updates
#define GIZMO_VERSION "0.0.3"
void start_ap();
#endif
