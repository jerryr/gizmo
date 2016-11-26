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
  String myname = webserver.arg(S_MY_NAME);
  Serial.println("myname = " + myname);
  String updateUrl = webserver.arg(S_UPDATE_URL);
  String ssid = webserver.arg(S_SSID);
  String pwd = webserver.arg(S_PASSWD);
  String mqttserver = webserver.arg(S_MQTT_SERVER);
  String p = webserver.arg(S_MQTT_PORT);
  uint16_t mqttport = atoi(p.c_str());
  Serial.printf("p = %s, port = %d\n", p.c_str(), mqttport);
  String mqttuser = webserver.arg(S_MQTT_USER);
  String mqttpasswd = webserver.arg(S_MQTT_PASSWD);
  webserver.send(200, "text/plain", "Bootstrap complete!");
  webserver.stop();
  WiFi.disconnect();
  WiFi.softAPdisconnect();
  WiFi.persistent(true);
  WiFi.enableSTA(true);
  WiFi.enableAP(false);
  WiFi.softAPdisconnect();
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
  WiFi.hostname(myname);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid.c_str(), pwd.c_str());
  if(WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Failed to connect to wifi; try again");
    // TODO: serve an error page on http
    return;
  }
  Serial.println(WiFi.localIP());
  StaticJsonBuffer <400> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root[S_SSID] = ssid;
  root[S_PASSWD] = pwd;
  root[S_UPDATE_URL] = updateUrl;
  root[S_MQTT_USER] = mqttuser;
  root[S_MQTT_PASSWD] = mqttpasswd;
  root[S_MQTT_SERVER] = mqttserver;
  root[S_MQTT_PORT] = mqttport;
  root[S_MY_NAME] = myname;
  root["end"] = "end";
  File f = SPIFFS.open(BOOTSTRAP_FILE_NAME, "w");
  root.printTo(f);
  delay(100);
  f.close();
  delay(100);
  SPIFFS.end();
  Serial.println(F("Saved bootstrap, rebooting..."));
  delay(100);
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
