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
  String control_topic;
  byte state;
  byte type;
};
enum connection_types {
  MOTION, SWITCH, SERIAL_LIGHTS
};
#endif
