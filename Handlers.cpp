#include <ArduinoJson.h>
#include <WebServer.h>
#include "Handlers.h"
#include "WiFiFunctions.h"
#include "Backend.h"

extern WebServer server;
extern String target_ssid, target_password, received_uuid;
extern bool uuidReceived;

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

void handleSetUUID() {
  if (server.method() != HTTP_POST) {
    server.send(405, "Method Not Allowed", "Only POST allowed");
    return;
  }

  if (!server.hasArg("plain")) {
    server.send(400, "Bad Request", "No body");
    return;
  }

  String body = server.arg("plain");

  StaticJsonDocument<200> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, body);

  if (error) {
    server.send(400, "Bad Request", "Invalid JSON");
    return;
  }

  received_uuid = jsonDoc["uuid"].as<String>();
  uuidReceived = true;

  Serial.println("UUID recibido:");
  Serial.println(received_uuid);

  server.send(200, "application/json", "{\"message\": \"UUID recibido\"}");

  if (uuidReceived && target_ssid.length() > 0 && target_password.length() > 0) {
    sendCredentialsToBackend();
  }
}
