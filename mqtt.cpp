#include "gizmo.h"
#include "mqtt.h"

WiFiClient wifi;
PubSubClient mqttClient(wifi);
String control_topic;
String heartbeat_topic;
extern bool update_requested;
extern bool reset_requested;
long last_heartbeat = 0;
String myname;
void mqtt_callback(char *topic, byte *message, unsigned int length) {
  if(control_topic == topic) {
    if(!strncmp((const char *)message, "update", length)) {
      update_requested = true;
    }
    else if(!strncmp((const char *)message, "reset", length)) {
      reset_requested = true;
    }
  }
}

void mqtt_connect(String user, String password, String server, uint16_t port, String clientid) {
  Serial.println(F("Connecting to MQTT server"));
  mqttClient.setServer(server.c_str(), port);
  mqttClient.setCallback(mqtt_callback);
  if(mqttClient.connect(clientid.c_str(), user.c_str(), password.c_str())) {
    Serial.println(F("Connected to mqtt"));
  }
  else {
    Serial.println(F("Failed to connect to mqtt broker"));
  }
  control_topic = "/" + clientid + "/update";
  Serial.printf("Subscribing to update topic %s \n", control_topic.c_str());
  mqttClient.subscribe(control_topic.c_str());

  heartbeat_topic = "/heartbeat/" + clientid;
  last_heartbeat = millis();
  myname = clientid;
}

void mqtt_publish(String topic, String payload) {
  mqttClient.publish(topic.c_str(), payload.c_str());
}

void mqtt_loop() {
  // TODO: handle reconnects
  mqttClient.loop();
  long cur = millis();
  if((cur - last_heartbeat) > 10000) {
    last_heartbeat = cur;
    mqttClient.publish(heartbeat_topic.c_str(), myname.c_str());
  }
}

