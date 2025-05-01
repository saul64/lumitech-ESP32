#include <WiFi.h>
#include "WiFiFunctions.h"
#include "Backend.h"
#include "AccessPoint.h"

extern String target_ssid, target_password;
extern bool uuidReceived;
extern const char* ap_ssid;

void connectToWiFi() {
  Serial.println("Desconectando AP...");
  WiFi.softAPdisconnect(true);
  delay(1000);

  Serial.println("Conectando a WiFi:");
  WiFi.mode(WIFI_STA);
  WiFi.begin(target_ssid.c_str(), target_password.c_str());

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado a WiFi exitosamente.");
    delay(5000);

    sendRosettaIpToBackend();
    sendCredentialsToBackend();
  } else {
    Serial.println("\nError al conectar a WiFi.");
    setupAccessPoint(ap_ssid);
  }
}
