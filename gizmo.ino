#include "gizmo.h"
#include "portal.h"
uint8 mode;
#define STA 1
#define AP 2
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
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(config);
      const char *updateUrl = root["updateUrl"];
      free(config);
      mode = STA;
      check_for_ota_update(updateUrl);
    }
    f.close();
    
    Serial.println(F("\n\nConnecting to wifi\n"));
    if(!WiFi.waitForConnectResult() == WL_CONNECTED) {
      Serial.println("Could not connect to WiFi.. rebooting");
      ESP.restart();
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

