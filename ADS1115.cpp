#include "ADS1115.h"

namespace ESPtools {
namespace ADC {

ADS1115::ADS1115(TwoWire &wire, uint8_t address, int8_t drdyPin)
  : _wire(wire), _address(address), _drdyPin(drdyPin), _configReg(0x8583), _currentChannel(0) {}

bool ADS1115::begin() {
  _wire.begin();
  if (_drdyPin >= 0) pinMode(_drdyPin, INPUT);

  writeRegister(REG_CONFIG, _configReg);
  return true;
}

bool ADS1115::isReady() {
  if (_drdyPin >= 0) {
    return digitalRead(_drdyPin) == LOW;
  }
  waitForConversion();
  return true;
}

void ADS1115::setGain(uint8_t gainCode) {
  gainCode &= 0x07;
  _configReg = (_configReg & 0xF1FF) | (gainCode << 9);
  writeRegister(REG_CONFIG, _configReg);
}

void ADS1115::setSampleRate(uint8_t rateCode) {
  rateCode &= 0x07;
  _configReg = (_configReg & 0xFF1F) | (rateCode << 5);
  writeRegister(REG_CONFIG, _configReg);
}

void ADS1115::setChannel(uint8_t channel) {
  _currentChannel = channel & 0x03;
}

int32_t ADS1115::read(uint8_t samples) {
  int64_t total = 0;
  for (uint8_t i = 0; i < samples; ++i) {

    uint16_t cfg = 0x8000;
    cfg |= uint16_t(0x04 + _currentChannel) << 12;
    cfg |= _configReg & 0x0F9F;
    cfg |= _configReg & 0x00E0;
    _configReg = cfg;
    writeRegister(REG_CONFIG, _configReg);

    waitForConversion();
    uint16_t raw = readRegister(REG_CONVERSION);
    total += int16_t(raw);
  }
  return total / samples;
}

float ADS1115::readVoltage(uint8_t channel, uint8_t samples) {
  setChannel(channel);
  int32_t raw = read(samples);
  float fsr;
  switch ((_configReg >> 9) & 0x07) {
    case ADS1115::GAIN_TWOTHIRDS: fsr = 6.144f; break;
    case ADS1115::GAIN_1X:        fsr = 4.096f; break;
    case ADS1115::GAIN_2X:        fsr = 2.048f; break;
    case ADS1115::GAIN_4X:        fsr = 1.024f; break;
    case ADS1115::GAIN_8X:        fsr = 0.512f; break;
    case ADS1115::GAIN_16X:       fsr = 0.256f; break;
    default:                      fsr = 4.096f; break;
  }
  return raw * fsr / 32768.0f;
}

static uint8_t diffMuxCode(uint8_t p, uint8_t n) {
  if (p == 0 && n == 1) return 0b000;
  if (p == 0 && n == 3) return 0b001;
  if (p == 1 && n == 3) return 0b010;
  if (p == 2 && n == 3) return 0b011;
  return 0xFF;
}

void ADS1115::setDifferential(uint8_t posChannel, uint8_t negChannel) {
  uint8_t code = diffMuxCode(posChannel, negChannel);
  if (code == 0xFF) return;


  uint8_t gainCode  = (_configReg >> 9) & 0x07;
  uint8_t drateCode = (_configReg >> 5) & 0x07;

  uint16_t cfg = 0x8000
               | (uint16_t(code)     << 12)
               | (uint16_t(gainCode) << 9)
               | (1                   << 8)
               | (uint16_t(drateCode)<< 5)
               | (0                   << 4)
               | 0;


  Wire.beginTransmission(_address);
  Wire.write(0x01);
  Wire.write(uint8_t(cfg >> 8));
  Wire.write(uint8_t(cfg & 0xFF));
  Wire.endTransmission();
}

float ADS1115::readDifferentialVoltage(uint8_t posChannel,
                                       uint8_t negChannel,
                                       uint8_t samples) {
  int32_t raw = readDifferential(posChannel, negChannel, samples);

  uint8_t gainCode = (_configReg >> 9) & 0x07;
  float   fsr;
  switch (gainCode) {
    case GAIN_TWOTHIRDS: fsr = 6.144f; break;
    case GAIN_1X:        fsr = 4.096f; break;
    case GAIN_2X:        fsr = 2.048f; break;
    case GAIN_4X:        fsr = 1.024f; break;
    case GAIN_8X:        fsr = 0.512f; break;
    case GAIN_16X:       fsr = 0.256f; break;
    default:             fsr = 4.096f; break;
  }

  return float(raw) * (fsr / 32768.0f);
}

uint8_t ADS1115::testI2C() {
  _wire.beginTransmission(_address);
  _wire.write(REG_CONVERSION);
  if (_wire.endTransmission() != 0) return 0;
  _wire.requestFrom(int(_address), 1);
  return _wire.available() ? _wire.read() : 0;
}

void ADS1115::reset() {
  _configReg = 0x8583;
  writeRegister(REG_CONFIG, _configReg);
}


void ADS1115::writeRegister(uint8_t reg, uint16_t value) {
  _wire.beginTransmission(_address);
  _wire.write(reg);
  _wire.write(value >> 8);
  _wire.write(value & 0xFF);
  _wire.endTransmission();
}

uint16_t ADS1115::readRegister(uint8_t reg) {
  _wire.beginTransmission(_address);
  _wire.write(reg);
  _wire.endTransmission();
  _wire.requestFrom(int(_address), 2);
  uint16_t hi = _wire.read();
  uint16_t lo = _wire.read();
  return (hi << 8) | lo;
}

void ADS1115::waitForConversion() {
  uint8_t dr = (_configReg >> 5) & 0x07;
  switch (dr) {
    case SPS_8:   delay(125); break;
    case SPS_16:  delay(63);  break;
    case SPS_32:  delay(32);  break;
    case SPS_64:  delay(16);  break;
    case SPS_128: delay(8);   break;
    case SPS_250: delay(4);   break;
    case SPS_475: delay(3);   break;
    case SPS_860: delay(2);   break;
  }
}

}
}