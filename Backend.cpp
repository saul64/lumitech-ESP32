#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "Backend.h"

extern String target_ssid, target_password, received_uuid;
extern bool uuidReceived;

void sendCredentialsToBackend() {
  if (!uuidReceived) {
    Serial.println("UUID aún no recibido.");
    return;
  }

  HTTPClient http;
  String apiUrl = "http://192.168.0.25:3000/roseta/register";

  StaticJsonDocument<400> jsonDoc;
  jsonDoc["wifi_ssid"] = target_ssid;
  jsonDoc["wifi_password"] = target_password;
  jsonDoc["rosette_mac"] = WiFi.macAddress();
  jsonDoc["uuid_owner"] = received_uuid;

  String jsonStr;
  serializeJson(jsonDoc, jsonStr);

  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(jsonStr);

  if (httpResponseCode > 0) {
    Serial.println("Respuesta del servidor: " + http.getString());
  } else {
    Serial.println("Error al enviar. Código: " + String(httpResponseCode));
    Serial.println("Mensaje de error: " + http.errorToString(httpResponseCode));
    Serial.println("Detalles: " + http.getString());
  }

  http.end();
}

void sendRosettaIpToBackend() {
  HTTPClient http;
  String apiUrl = "http://192.168.0.25:3000/roseta/received-ip";

  StaticJsonDocument<300> jsonDoc;
  jsonDoc["rossette_ip"] = WiFi.localIP().toString();

  String jsonStr;
  serializeJson(jsonDoc, jsonStr);

  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(jsonStr);

  if (httpResponseCode > 0) {
    Serial.println("Respuesta del servidor: " + http.getString());
  } else {
    Serial.println("Error al enviar IP. Código: " + String(httpResponseCode));
    Serial.println("Mensaje: " + http.errorToString(httpResponseCode));
    Serial.println("Detalles: " + http.getString());
  }

  http.end();
}
