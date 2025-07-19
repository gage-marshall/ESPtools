#include "WiFiEnterprise.h"
#include <WiFi.h>
#include <esp_wpa2.h>

namespace ESPtools {
namespace WiFi {

bool connect(const char* ssid, const char* identity, const char* password) {
  ::WiFi.disconnect(true);
  delay(100);

  ::WiFi.mode(WIFI_STA);

  esp_wifi_sta_wpa2_ent_set_identity((uint8_t*)identity, strlen(identity));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t*)identity, strlen(identity));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t*)password, strlen(password));
  esp_wifi_sta_wpa2_ent_enable();

  ::WiFi.begin(ssid);

  unsigned long start = millis();
  while (::WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }

  return (::WiFi.status() == WL_CONNECTED);
}

bool isConnected() {
  return (::WiFi.status() == WL_CONNECTED);
}

}
}