#include "gizmo.h"
#include "mqtt.h"
#include "connections.h"

WiFiClient wifi;
PubSubClient mqttClient(wifi);
String control_topic;
extern bool update_requested;
extern bool reset_requested;
long last_heartbeat = 0;
String myname;
void mqtt_callback(char *topic, byte *message, unsigned int length) {
  //Serial.printf("Got a message: %s\n", message);
  if(control_topic == topic) {
      StaticJsonBuffer<400> jsonBuffer;
      char *buf = (char *)malloc((length + 1) * sizeof(char));
      os_memcpy(buf, message, length);
      buf[length] = '\0';
      JsonObject& root = jsonBuffer.parseObject(buf);
      String cmd = root["command"];
      Serial.println("cmd = " + cmd);
      if(cmd == "update") {
        update_requested = true;
      }
      else if(cmd == "reset") {
        reset_requested = true;
      }
      else if(cmd == "configure") {
        String url = root["url"];
        update_connections(url);
      }
      else {
        Serial.println("Unknown command received");
      }
      free(buf);
  } else {
    Serial.printf("Received message on unknown topic %s\n", topic);
  }
}

String _user;
String _password;

bool _connect() {
  return mqttClient.connect(myname.c_str(), _user.c_str(), _password.c_str());
}
void mqtt_connect(String user, String password, String server, uint16_t port, String clientid) {
  Serial.println(F("Connecting to MQTT server"));
  mqttClient.setServer(server.c_str(), port);
  mqttClient.setCallback(mqtt_callback);
  _user = user;
  _password = password;
  if(_connect()) {
    Serial.println(F("Connected to mqtt"));
  }
  else {
    Serial.println(F("Failed to connect to mqtt broker"));
  }
  control_topic = "/" + clientid + "/update";
  Serial.printf("Subscribing to update topic %s \n", control_topic.c_str());
  mqttClient.subscribe(control_topic.c_str());

  last_heartbeat = millis();
  myname = clientid;
}

void mqtt_publish(String topic, String payload) {
  mqttClient.publish(topic.c_str(), payload.c_str());
}

void mqtt_loop() {
  if(!mqttClient.connected()) {
     _connect();
  }
  mqttClient.loop();
  long cur = millis();
  if((cur - last_heartbeat) > 10000) {
    last_heartbeat = cur;
    String heartbeat_topic = "/heartbeat/" + myname;
    mqttClient.publish(heartbeat_topic.c_str(), myname.c_str());
  }
}

