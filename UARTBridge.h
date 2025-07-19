#ifndef ESPTOOLS_UARTBRIDGE_H
#define ESPTOOLS_UARTBRIDGE_H

#include <Arduino.h>

// Helper bridge, mainly intended to feed UART input to EventBus

namespace ESPtools {
namespace UART {

// Initialize UART bridge on given port and baud rate
void begin(HardwareSerial& port, uint32_t baud);

// Call in main loop to process incoming lines
void loop();

}
}

#endif
