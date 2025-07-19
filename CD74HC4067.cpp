#include "CD74HC4067.h"

namespace ESPtools {
namespace Mux {

CD74HC4067::CD74HC4067(uint8_t s0Pin,
                       uint8_t s1Pin,
                       uint8_t s2Pin,
                       uint8_t s3Pin,
                       int8_t enPin)
  : _s0(s0Pin), _s1(s1Pin), _s2(s2Pin), _s3(s3Pin), _en(enPin) {}

void CD74HC4067::begin() {
  pinMode(_s0, OUTPUT);
  pinMode(_s1, OUTPUT);
  pinMode(_s2, OUTPUT);
  pinMode(_s3, OUTPUT);
  if (_en >= 0) {
    pinMode(_en, OUTPUT);
    digitalWrite(_en, LOW);
  }
}

void CD74HC4067::selectChannel(uint8_t channel) {
  channel &= 0x0F;
  digitalWrite(_s0, channel & 0x01);
  digitalWrite(_s1, (channel >> 1) & 0x01);
  digitalWrite(_s2, (channel >> 2) & 0x01);
  digitalWrite(_s3, (channel >> 3) & 0x01);
}

int CD74HC4067::readAnalog(uint8_t analogPin) {

  return analogRead(analogPin);
}

void CD74HC4067::disable() {
  if (_en >= 0) {
    digitalWrite(_en, HIGH);
  }
}

}
}