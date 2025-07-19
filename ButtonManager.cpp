#include "ButtonManager.h"

namespace ESPtools {
namespace Input {

struct Button {
  uint8_t pin = 255;
  ButtonState current = BUTTON_RELEASED;
  ButtonState last = BUTTON_RELEASED;
  unsigned long lastDebounceTime = 0;
};

Button buttons[MAX_BUTTONS];
const unsigned long debounceDelay = 50;

void configureButton(uint8_t index, uint8_t gpioPin) {
  if (index >= MAX_BUTTONS) return;
  buttons[index].pin = gpioPin;
  pinMode(gpioPin, INPUT_PULLUP);
}

void pollButtons() {
  for (int i = 0; i < MAX_BUTTONS; i++) {
    if (buttons[i].pin == 255) continue;

    int reading = digitalRead(buttons[i].pin);
    ButtonState state = (reading == LOW) ? BUTTON_PRESSED : BUTTON_RELEASED;


    if (state != buttons[i].current && (millis() - buttons[i].lastDebounceTime) > debounceDelay) {
      buttons[i].lastDebounceTime = millis();
      buttons[i].last = buttons[i].current;
      buttons[i].current = state;
    }
  }
}

bool isPressed(uint8_t index) {
  if (index >= MAX_BUTTONS) return false;
  return buttons[index].current == BUTTON_PRESSED;
}

bool wasPressed(uint8_t index) {
  if (index >= MAX_BUTTONS) return false;
  if (buttons[index].last == BUTTON_RELEASED && buttons[index].current == BUTTON_PRESSED) {
    buttons[index].last = buttons[index].current;
    return true;
  }
  return false;
}

bool wasReleased(uint8_t index) {
  if (index >= MAX_BUTTONS) return false;
  if (buttons[index].last == BUTTON_PRESSED && buttons[index].current == BUTTON_RELEASED) {
    buttons[index].last = buttons[index].current;
    return true;
  }
  return false;
}

}
}