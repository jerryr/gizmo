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
#define S_SSID F("ssid")
#define S_PASSWD F("passwd")
#define S_UPDATE_URL F("updateUrl")
#define S_MQTT_USER F("mqttUser")
#define S_MQTT_PASSWD F("mqttPasswd")
#define S_MQTT_SERVER F("mqttServer")
#define S_MQTT_PORT F("mqttPort")
#define S_MY_NAME F("myName")
#endif
