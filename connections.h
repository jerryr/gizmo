#ifndef GIZMO_CONNECTIONS_H
#define GIZMO_CONNECTIONS_H
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
void setup_connection(JsonObject& connection);
void update_connections(String &);
void process_connections();
void read_connections(const String &str);
struct connection {
  String status_topic;
  byte state;
};
#endif
