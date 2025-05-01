#include <ArduinoJson.h>
#include <WebServer.h>
#include "Handlers.h"
#include "WiFiFunctions.h"
#include "Backend.h"
#include <WiFi.h>
#include "DHT.h"
#include <HTTPClient.h>

#define DHTPIN 4      
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

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

void sendSensorData() {
  static unsigned long lastSendTime = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastSendTime >= 5000) { 
    lastSendTime = currentMillis;


    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Verifica si los datos leídos son válidos
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Error al leer el sensor DHT11");
      return;
    }


    StaticJsonDocument<200> jsonDoc;
    jsonDoc["temperature"] = temperature;
    jsonDoc["humidity"] = humidity;
    jsonDoc["rosette_mac"] = WiFi.macAddress();

    String response;
    serializeJson(jsonDoc, response);

    HTTPClient http;
    http.begin("http://192.168.0.25/roseta/sensor-data");  
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(response);

    if (httpResponseCode > 0) {
      String responseBody = http.getString();
      Serial.println("Datos enviados a NestJS correctamente");
      Serial.println(responseBody);
    } else {
      Serial.println("Error al enviar los datos");
    }
  }
}

