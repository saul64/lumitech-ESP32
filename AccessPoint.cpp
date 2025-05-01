#include <WiFi.h>
#include "AccessPoint.h"

void setupAccessPoint(const char* ssid) {
  WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  WiFi.softAP(ssid);
  Serial.println("Access Point iniciado");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
}
