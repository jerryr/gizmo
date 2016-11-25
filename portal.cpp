#include <ESP8266WebServer.h>
#include <ESP8266WiFiAP.h>
#include <ArduinoJson.h>
#include "portal.h"
#include "gizmo.h"

ESP8266WebServer webserver;
void start_portal() {
  Serial.println(F("Setting up webserver"));
  // Serve portal
  webserver.on("/", serve_root);
  webserver.on("/api/bootstrap", HTTP_POST, save_bootstrap);
  webserver.on("/api/bootstrap", HTTP_GET, get_bootstrap);
  webserver.serveStatic("/js", SPIFFS, "/web/js");
  webserver.serveStatic("/html", SPIFFS, "/web/html");
  webserver.begin();
  // Get selection and save the STA config
}

void serve_root() {
  webserver.sendHeader("Location", "/html/configure.html");
  webserver.send(302, "text/plain", "Redirected");
}
void save_bootstrap() {
  // Save bootstrap
  // TODO: sanitize and validate input
  String ssid = webserver.arg("ssid");
  String pwd = webserver.arg("password");
  String updateUrl = webserver.arg("update-url");
  webserver.send(200, "text/plain", "Bootstrap complete!");
  WiFi.disconnect();
  WiFi.persistent(true);
  WiFi.enableSTA(true);
  WiFi.enableAP(false);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid.c_str(), pwd.c_str());
  WiFi.waitForConnectResult();
  // TODO: validate that the connect succeeded before writing to bootstrap file
  StaticJsonBuffer <200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["ssid"] = ssid;
  root["password"] = pwd;
  root["updateUrl"] = updateUrl;
  File f = SPIFFS.open(BOOTSTRAP_FILE_NAME, "w");
  root.printTo(f);
  f.close();
  Serial.println(F("Saved bootstrap, rebooting..."));
  ESP.restart();
}
void handle_ap_clients() {
  //Serial.println("handleClients");
  webserver.handleClient();
}
void get_bootstrap() {
  if(SPIFFS.exists(BOOTSTRAP_FILE_NAME)) {
    File f = SPIFFS.open(BOOTSTRAP_FILE_NAME, "r");
    webserver.streamFile(f, "application/json");
    f.close();
  }
  else {
    webserver.send(404, "text/plain", "Bootstrap file not found");
  }
}
