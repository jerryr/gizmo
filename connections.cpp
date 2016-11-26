#include "gizmo.h"
#include "connections.h"
#include "mqtt.h"

struct connection *connections[20];

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
    Serial.println("Found a motion detector!!!");
    uint8_t gpio = conn["gpio"];
    pinMode(gpio, INPUT);
    String topic = conn["status_topic"];
    if(connections[gpio]) {      
      free(connections[gpio]);
    }
    connections[gpio] = new connection();
    connections[gpio]->status_topic = topic;
  }
  else {
    Serial.println("Unknown type of connection");
  } 
}

void process_connections() {
  int i;
  for(i=0; i<20; i++){
    if(connections[i]) {
      struct connection *c = connections[i];
      byte curstate = digitalRead(i);
      if(curstate != c->state) {
        mqtt_publish(c->status_topic, curstate? "ON": "OFF");
        c->state = curstate;
      }
    }
  }
}

