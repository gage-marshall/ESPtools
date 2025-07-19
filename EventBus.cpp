#include "EventBus.h"

namespace ESPtools {
namespace EventBus {


static const uint8_t MAX_SUBSCRIPTIONS = 16;
struct Subscription { String topic; Handler cb; };
static Subscription subscriptions[MAX_SUBSCRIPTIONS];
static uint8_t subCount = 0;

void begin() {
  subCount = 0;
}

void subscribe(const String& topic, Handler cb) {
  if (subCount < MAX_SUBSCRIPTIONS) {
    subscriptions[subCount++] = { topic, cb };
  }
}

void publish(const String& topic, const String& payload) {
  for (uint8_t i = 0; i < subCount; ++i) {
    if (subscriptions[i].topic == topic) {
      subscriptions[i].cb(payload);
    }
  }
}

}
}