#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "AccessPoint.h"
#include "Handlers.h"
#include "WiFiFunctions.h"
#include "Backend.h"

// Variables globales
const char* ap_ssid = "Roseta_AP";
WebServer server(80);

String target_ssid;
String target_password;
String received_uuid;
bool uuidReceived = false;

void setup() {
  Serial.begin(115200);
  setupAccessPoint(ap_ssid);

  server.on("/set-wifi-credentials", HTTP_POST, handleSetWifiCredentials);
  server.on("/send-data", HTTP_GET, handleSendData);

  server.begin();

  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  server.handleClient();
  sendSensorData();
}
