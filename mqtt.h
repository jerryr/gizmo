#ifndef GIZMO_MQTT_H
#define GIZMO_MQTT_H
#include <PubSubClient.h>
void mqtt_connect(String user, String password, String mqttserver, uint16_t mqttport, String clientid);
void mqtt_loop();
void mqtt_publish(String topic, String message);
#endif
