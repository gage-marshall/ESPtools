#include "PCF8575.h"

namespace ESPtools {
namespace IOExpander {
namespace PCF8575 {

PCF8575::PCF8575(uint8_t address, TwoWire &wire)
  : _wire(wire), _address(address), _shadow(0xFFFF), _invertMask(0) {}

bool PCF8575::begin() {
  _wire.begin();
  _shadow = _readPort();
  return true;
}

uint16_t PCF8575::readAll() {
  uint16_t val = _readPort();
  return val ^ _invertMask;
}

void PCF8575::writeAll(uint16_t value) {
  _shadow = value;
  _writePort(value ^ _invertMask);
}

bool PCF8575::digitalRead(uint8_t pin) {
  if (pin > 15) return false;
  return (readAll() >> pin) & 0x1;
}

void PCF8575::digitalWrite(uint8_t pin, bool level) {
  if (pin > 15) return;
  if (level) _shadow |= (1u << pin);
  else       _shadow &= ~(1u << pin);
  writeAll(_shadow);
}

void PCF8575::toggle(uint8_t pin) {
  if (pin > 15) return;
  _shadow ^= (1u << pin);
  writeAll(_shadow);
}

void PCF8575::pinMode(uint8_t pin, PinMode mode) {
  if (pin > 15) return;
  if (mode == PinMode::Input)  _shadow |= (1u << pin);
  else                          _shadow &= ~(1u << pin);
  writeAll(_shadow);
}

void PCF8575::writeMask(uint16_t mask, uint16_t value) {
  _shadow = (_shadow & ~mask) | (value & mask);
  writeAll(_shadow);
}

void PCF8575::invert(uint16_t mask, bool invert) {
  if (invert)      _invertMask |= mask;
  else             _invertMask &= ~mask;

  _writePort(_shadow ^ _invertMask);
}

void PCF8575::_writePort(uint16_t port) {
  uint8_t lo = port & 0xFF;
  uint8_t hi = port >> 8;
  _wire.beginTransmission(_address);
  _wire.write(lo);
  _wire.write(hi);
  _wire.endTransmission();
}

uint16_t PCF8575::_readPort() {
  _wire.requestFrom(int(_address), 2);
  uint8_t lo = _wire.read();
  uint8_t hi = _wire.read();
  return (uint16_t)hi << 8 | lo;
}

void PCF8575::onInterrupt(uint8_t irqPin, void (*handler)(), int mode) {
  ::pinMode(irqPin, INPUT_PULLUP);
  ::attachInterrupt(digitalPinToInterrupt(irqPin), handler, mode);
}

}
}
}