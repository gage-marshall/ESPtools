#include "MQTTClient.h"
#include "EventBus.h"
#include <vector>

namespace ESPtools {
namespace MQTT {

static PubSubClient* mqttClient = nullptr;

static const char*      _clientIdG       = nullptr;
static const char*      _userG           = nullptr;
static const char*      _passG           = nullptr;
static std::vector<String> _subscribedTopics;

static void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String t(topic);
  String p;
  for (unsigned int i = 0; i < length; ++i) p += (char)payload[i];
  EventBus::publish(t, p);
}

void begin(PubSubClient& client, const char* broker, uint16_t port) {
  mqttClient = &client;
  mqttClient->setServer(broker, port);
  mqttClient->setCallback(mqttCallback);
}

bool connect(const char* clientId, const char* user, const char* pass) {
  if (!mqttClient) return false;

  _clientIdG = clientId;
  _userG     = user;
  _passG     = pass;

  bool ok = false;
  if (user && pass) ok = mqttClient->connect(clientId, user, pass);
  else               ok = mqttClient->connect(clientId);

  if (ok) {
    for (auto &t : _subscribedTopics) {
      mqttClient->subscribe(t.c_str());
    }
  }
  return ok;
}

bool subscribe(const char* topic) {
  if (!mqttClient) return false;
  bool ok = mqttClient->subscribe(topic);
  if (ok) {
    String t(topic);

    bool found = false;
    for (auto &e : _subscribedTopics) {
      if (e == t) { found = true; break; }
    }
    if (!found) _subscribedTopics.push_back(t);
  }
  return ok;
}

bool publish(const char* topic, const char* payload) {
  return mqttClient ? mqttClient->publish(topic, payload) : false;
}

void loop() {
  if (!mqttClient) return;

  if (!mqttClient->connected() && _clientIdG) {
    if (_userG && _passG) {
      mqttClient->connect(_clientIdG, _userG, _passG);
    } else {
      mqttClient->connect(_clientIdG);
    }

    for (auto &t : _subscribedTopics) {
      mqttClient->subscribe(t.c_str());
    }
  }
  mqttClient->loop();
}

}
}