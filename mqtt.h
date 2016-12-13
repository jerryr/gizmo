#ifndef GIZMO_MQTT_H
#define GIZMO_MQTT_H
#include <PubSubClient.h>
void mqtt_connect(String user, String password, String mqttserver, uint16_t mqttport, String clientid);
void mqtt_loop();
void mqtt_publish(String topic, String message);
struct mqtt_callback {
  String topic;
  void (*cb)(String, void *);
  void *cbarg;
  uint8_t pos;
};
#define MAX_CALLBACKS 50
void mqtt_set_message_callback(struct mqtt_callback *cb);
void mqtt_unset_message_callback(struct mqtt_callback *cb);
#endif
