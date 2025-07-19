#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>

// Easy button handling for ESP32, with various press status states

namespace ESPtools {
namespace Input {

#define MAX_BUTTONS 4

enum ButtonState {
  BUTTON_RELEASED,
  BUTTON_PRESSED
};

void configureButton(uint8_t index, uint8_t gpioPin);

void pollButtons();

bool isPressed(uint8_t index);

bool wasPressed(uint8_t index);

bool wasReleased(uint8_t index);

}
}
#endif
