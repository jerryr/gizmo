#include "gizmo.h"
#include "connections.h"
#include "mqtt.h"
#include "serial_lights.h"

struct connection *connections[20];
extern unsigned long start_time;
struct mqtt_callback *serial = NULL;
void control_switch(String message, void *cbarg);

void update_connections(String& url) {
  HTTPClient hclient;
  hclient.begin(url);
  if(!hclient.GET()) {
    Serial.println("Error when issueing GET");
    return;
  }
  String ret = hclient.getString();
  read_connections(ret);
  File f = SPIFFS.open(CONNECTIONS_FILE_NAME, "w");
  f.write((byte *)ret.c_str(), ret.length());
  f.close();
}

void read_connections(const String &str) {  
  StaticJsonBuffer<400> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(str);
  JsonArray& c  = root["connections"];
  int i;
  for(i=0; i<c.size(); i++) {
    JsonObject& item = c[i];
    setup_connection(item);
  }
}

void setup_connection(JsonObject& conn) {
  String type = conn["type"];
  if(type == "motion") {
    uint8_t gpio = conn["gpio"];
    pinMode(gpio, INPUT);
    String topic = conn["status_topic"];
    if(connections[gpio]) {
      if(connections[gpio]->cb) {
        delete connections[gpio]->cb;    
      }
      delete connections[gpio];
    }
    connections[gpio] = new connection();
    connections[gpio]->status_topic = topic;
    connections[gpio]->type = MOTION;
    connections[gpio]->cb = NULL; // No topics to subscribe for motion detectors
  }
  else if(type == "serial") {
    // No GPIO for serial lights, since they always connect to RX
    if(serial){
      mqtt_unset_message_callback(serial);
      delete serial;
    }
    String control_topic = conn["control_topic"];
    struct mqtt_callback *cb = new struct mqtt_callback;
    cb->topic = control_topic;
    cb->cb = control_serial_lights;
    cb->cbarg = NULL;
    serial = cb;
    mqtt_set_message_callback(cb);
  }
  else if(type == "switch") {
    String control_topic = conn["control_topic"];
    String status_topic = conn["status_topic"];
    uint8_t gpio = conn["gpio"];
    if(connections[gpio]) {
      if(connections[gpio]->cb) {
        delete connections[gpio]->cb;    
      }
      delete connections[gpio];      
    }
    connections[gpio] = new connection();
    connections[gpio]->status_topic = status_topic;
    connections[gpio]->gpio = gpio;
    pinMode(gpio, OUTPUT);
    struct mqtt_callback *cb = new mqtt_callback();
    cb->topic = control_topic;
    cb->cb = control_switch;    
    cb->cbarg = connections[gpio];    
    connections[gpio]->cb = cb;
    mqtt_set_message_callback(cb);
  }
  else {
    Serial.println("Unknown type of connection");
  } 
}

void process_connections() {
  int i;
  for(i=0; i<19; i++){
    if(connections[i]) {      
      struct connection *c = connections[i];
      switch(c->type) {
        case MOTION:
            byte curstate = digitalRead(i);
            if(curstate != c->state) {
              mqtt_publish(c->status_topic, curstate? "OPEN": "CLOSED");
              c->state = curstate;
            break;
        }
      }
    }
  }
  if(serial) {
    animate_serial_lights();
  }
}

void control_switch(String message, void *cbarg) {
  struct connection *conn = (struct connection *) cbarg;
  if(message == "ON") {
    digitalWrite(conn->gpio, HIGH);
  }
  else if (message == "OFF"){
    digitalWrite(conn->gpio, LOW);
  }
  else {
    Serial.printf("Unknown switch control message: %s\n", message.c_str());
  }
}


