
#include "gizmo.h"
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
      // TODO: start AP etc
    }
    else {
      int size = f.size();
      char *config = (char *)malloc(size);
      f.readBytes(config, size);
      // TODO: parse config file etc.
      free(config);
      mode = STA;
    }
    f.close();
    
    Serial.println(F("Connecting to wifi"));
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
  //String ap_name = "gizmo-" + ESP.getFlashChipId();
  char ap_name[32];
  snprintf(ap_name, 32, "gizmo-%08X", ESP.getFlashChipId());
  // Start AP
  WiFi.softAP(ap_name);  
  
  Serial.printf("Access point name: %s\n", ap_name);

}

