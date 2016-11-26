#include "gizmo.h"
#include "portal.h"
#include "mqtt.h"
#include "connections.h"
uint8 mode;
#define STA 1
#define AP 2
volatile bool update_requested = false;
volatile bool reset_requested = false;
char updateUrl[100];
void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  //SPIFFS.remove(BOOTSTRAP_FILE_NAME);
  if(SPIFFS.exists(BOOTSTRAP_FILE_NAME)) {
    // Read file and extract config
    File f = SPIFFS.open(BOOTSTRAP_FILE_NAME, "r");
    if(!f) {
      Serial.println(F("Open of config file failed. Falling back to unconfigured mode"));
      mode = AP;
      start_ap();
      start_portal();
    }
    else {
      int size = f.size();
      char *config = (char *)malloc(size);
      f.readBytes(config, size);
      StaticJsonBuffer<400> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(config);
      const char *updateChar = root[S_UPDATE_URL];
      strncpy(updateUrl, updateChar, 100);
      String mqttuser = root[S_MQTT_USER];
      String mqttpasswd = root[S_MQTT_PASSWD];
      String mqttserver = root[S_MQTT_SERVER];
      uint16_t mqttport = root[S_MQTT_PORT];
      String myname = root[S_MY_NAME];
      String ssid = root[S_SSID];
      String password = root[S_PASSWD];
      Serial.println(F("\Connecting to wifi"));
      if(!WiFi.waitForConnectResult() == WL_CONNECTED) {
        Serial.println("Could not connect to WiFi.. rebooting");
        SPIFFS.remove(BOOTSTRAP_FILE_NAME);
        SPIFFS.end();
        ESP.restart();
      }
      Serial.println("Local IP is:" + WiFi.localIP());
      mqtt_connect(mqttuser, mqttpasswd, mqttserver, mqttport, myname);
      free(config);
      mode = STA;
    }
    f.close();
    // Load the connections
    if(SPIFFS.exists(CONNECTIONS_FILE_NAME)) {
      f = SPIFFS.open(CONNECTIONS_FILE_NAME, "r");
      int size = f.size();
      char *config = (char *) malloc(size);
      f.readBytes(config, size);
      f.close();
      const String &contents = String(config);
      read_connections(contents);
      free(config);
    }  
  }
  else {
    Serial.println(F("No bootstrap information"));
    mode = AP;
    start_ap();
    start_portal();
  }


}

void loop() {
  if(mode == AP) {
    handle_ap_clients();
  }
  else {
    mqtt_loop();
    if(update_requested) {
      Serial.println("Checking for OTA update...");
      check_for_ota_update(updateUrl);
      update_requested = false;
      return;
    }
    if(reset_requested) {
      reset_config();
      reset_requested = false;
      return;
    }
    process_connections();
  }
  delay(50);
}

void start_ap() {
  WiFi.disconnect();
  WiFi.persistent(false);
  Serial.println("Starting AP");
  // Use gizmo-<chipid> as the AP name
  char ap_name[32];
  snprintf(ap_name, 32, "gizmo-%08X", ESP.getFlashChipId());
  // Start AP
  WiFi.softAP(ap_name);

  Serial.printf("Access point name: %s\n", ap_name);

}

void check_for_ota_update(const char *update_url) {
      // Try for an OTA update
    Serial.println("Checking for updates...\n");
    ESPhttpUpdate.rebootOnUpdate(true);
    t_httpUpdate_return ret = ESPhttpUpdate.update(update_url, GIZMO_VERSION);
    switch(ret) {
       case HTTP_UPDATE_FAILED:
        Serial.println("[update] Update failed.");
        break;
       case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update] Update no Update.");
        break;
      default:
        Serial.printf("[update] unknown return code: %d\n", ret);
        break;
    }
}

void reset_config() {
  SPIFFS.remove(BOOTSTRAP_FILE_NAME);
  Serial.println("Removed bootstrap information");
  ESP.restart();
}

