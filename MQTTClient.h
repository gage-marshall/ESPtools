#ifndef ESPTOOLS_MQTTCLIENT_H
#define ESPTOOLS_MQTTCLIENT_H

#include <Arduino.h>
#include <PubSubClient.h>

namespace ESPtools {
namespace MQTT {

// Initialize MQTT client with network client, broker address, port
void begin(PubSubClient& client, const char* broker, uint16_t port = 1883);

// Connect to broker with optional credentials
bool connect(const char* clientId, const char* user = nullptr, const char* pass = nullptr);

// Subscribe to topic
bool subscribe(const char* topic);

// Publish message
bool publish(const char* topic, const char* payload);

// Call in main loop to process MQTT and dispatch incoming messages
void loop();

}
}

#endif
