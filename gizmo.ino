extern "C" {
  #include "user_interface.h"
}
#include "gizmo.h"
uint8 mode;
#define STA 1
#define AP 2
void setup() {
  struct station_config staconf;
  if(!wifi_station_get_config_default(&staconf)) {
    // No saved wifi, we need to start in AP mode
    // And serve up a portal to configure the STA mode
    start_portal();
  }
  else {
    // WiFi is configured, see if it connects
    uint8 count = 0;
    do {
      delay(100);
      count++;
    }
    while(wifi_station_get_connect_status() == STATION_CONNECTING && count > 10);
  }

}

void loop() {
  if(mode == AP) {
    handle_ap_clients();
  }
}
