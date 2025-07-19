#include "UARTBridge.h"
#include "EventBus.h"

namespace ESPtools {
namespace UART {

static HardwareSerial* uartPort = nullptr;
static String inputBuffer;

void begin(HardwareSerial& port, uint32_t baud) {
  uartPort = &port;
  uartPort->begin(baud);
}

void loop() {
  if (!uartPort) return;
  while (uartPort->available()) {
    char c = uartPort->read();
    if (c == '\n') {
      if (inputBuffer.endsWith("\r")) {
        inputBuffer.remove(inputBuffer.length() - 1);
      }
      int sep = inputBuffer.indexOf(':');
      if (sep > 0) {
        String topic = inputBuffer.substring(0, sep);
        String payload = inputBuffer.substring(sep + 1);
        EventBus::publish(topic, payload);
      }
      inputBuffer = "";
    } else {
      inputBuffer += c;
    }
  }
}


}
}