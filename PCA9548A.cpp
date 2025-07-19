#include "PCA9548A.h"

namespace ESPtools {
namespace Mux {

PCA9548A::PCA9548A(uint8_t address, TwoWire &wire)
  : _wire(wire), _address(address), _mask(0x00) {}

bool PCA9548A::begin() {
  _wire.begin();
  _wire.beginTransmission(_address);
  if (_wire.endTransmission() == 0) {
    _mask = 0x00;
    return true;
  }
  return false;
}

void PCA9548A::selectBus(uint8_t bus) {
  bus &= 0x07;
  _mask = (1 << bus);
  _wire.beginTransmission(_address);
  _wire.write(_mask);
  _wire.endTransmission();
}

void PCA9548A::disableAll() {
  _mask = 0x00;
  _wire.beginTransmission(_address);
  _wire.write(_mask);
  _wire.endTransmission();
}

uint8_t PCA9548A::enabledMask() const {
  return _mask;
}

}
}