#include <ArduinoJson.h>
#include <WebServer.h>
#include "Handlers.h"
#include "WiFiFunctions.h"
#include "Backend.h"
#include <WiFi.h>

extern WebServer server;
extern String target_ssid, target_password, received_uuid;

void handleSetWifiCredentials() {
  if (server.method() != HTTP_POST) {
    server.send(405, "Method Not Allowed", "Only POST allowed");
    return;
  }

  if (!server.hasArg("plain")) {
    server.send(400, "Bad Request", "No body");
    return;
  }

  String body = server.arg("plain");
  Serial.println("Cuerpo recibido:");
  Serial.println(body);

  StaticJsonDocument<200> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, body);

  if (error) {
    server.send(400, "Bad Request", "Invalid JSON");
    return;
  }

  target_ssid = jsonDoc["wifi_ssid"].as<String>();
  target_password = jsonDoc["wifi_password"].as<String>();

  Serial.println("Credenciales recibidas:");
  Serial.println("SSID: " + target_ssid);
  Serial.println("Password: " + target_password);

  server.send(200, "application/json", "{\"message\": \"Credenciales recibidas\"}");

  connectToWiFi();
}

void handleSendData() {
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["wifi_ssid"] = target_ssid;
  jsonDoc["wifi_password"] = target_password;
  jsonDoc["rosette_mac"] = WiFi.macAddress();
  jsonDoc["rosette_ip"] =  WiFi.localIP();

  String jsonStr;
  serializeJson(jsonDoc, jsonStr);

  server.send(200, "application/json", jsonStr);
}