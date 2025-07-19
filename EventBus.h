#ifndef ESPTOOLS_EVENTBUS_H
#define ESPTOOLS_EVENTBUS_H

#include <Arduino.h>
#include <functional>

namespace ESPtools {
namespace EventBus {

using Handler = std::function<void(const String& payload)>;

// Subscribe to a topic with a callback
void subscribe(const String& topic, Handler cb);

// Publish a message to a topic
void publish(const String& topic, const String& payload);

// Initialize EventBus
void begin();

}
}

#endif
