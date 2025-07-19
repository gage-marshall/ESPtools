#ifndef WIFI_ENTERPRISE_H
#define WIFI_ENTERPRISE_H

#include <Arduino.h>

// Easy creation of WPA2 interface inside ESPtools namespace

namespace ESPtools {
namespace WiFi {

bool connect(const char* ssid, const char* identity, const char* password);

bool isConnected();

}
}

#endif
