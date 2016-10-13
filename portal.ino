#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WebServer.h>

extern "C" {
  #include "user_interface.h"
}

ESP8266WebServer webserver;

void start_portal() {
  // Use gizmo-<chipid> as the AP name
  String ap_name = "gizmo-" + ESP.getFlashChipId();
  // Start AP
  WiFi.softAP(ap_name.c_str());
  // Serve portal
  webserver.on("/", serve_root);
  webserver.on("/configure", HTTP_POST, save_config);
  // Get selection and save the STA config
}

void serve_root() {
  // Show list of access points through scan
  // And allow user to select
  String html=F("<html><body>"
                "<form method=\"post\" action=\"/configure\">"
                "<input type=text name=ssid/><br>"
                "<input type=text name=password/><br>"
                "<input type=submit>"
                "</form>"
                "</body></html>");
  webserver.sendContent(html);
}
void save_config() {
  String ssid = webserver.arg("ssid");
  String pwd = webserver.arg("password");
  struct station_config staconfig;
  os_memcpy(staconfig.ssid, ssid.c_str(), strlen(ssid.c_str()) + 1);
  os_memcpy(staconfig.password, pwd.c_str(), strlen(pwd.c_str()+ 1));
  wifi_set_opmode(0x01); // Set the default mode to STA
  wifi_station_set_config(&staconfig); // Store the STA configuration
  ESP.reset();
  
}
void handle_ap_clients() {
  webserver.handleClient();
}

