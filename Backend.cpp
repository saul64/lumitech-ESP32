#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "Backend.h"

extern String target_ssid, target_password;

void sendRosettaIpToBackend() {
  const String apiUrl = "http://192.168.0.25:3000/roseta/received-ip";
  bool success = false;

  while (!success) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi no conectado, esperando...");
      delay(1000);
      continue;
    }

    HTTPClient http;
    StaticJsonDocument<300> jsonDoc;
    jsonDoc["rossette_ip"] = WiFi.localIP().toString();

    String jsonStr;
    serializeJson(jsonDoc, jsonStr);

    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");

    Serial.println("Enviando IP al backend...");
    int httpResponseCode = http.POST(jsonStr);

    if (httpResponseCode > 0) {
      Serial.println("Respuesta del servidor: " + http.getString());
      success = true;  
    } else {
      Serial.println("Error al enviar IP. Código: " + String(httpResponseCode));
      Serial.println("Reintentando en 3 segundos...");
      delay(3000);  // wait before retrying
    }

    http.end();
  }
}

