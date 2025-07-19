#include "ADS1256.h"

namespace ESPtools {
namespace ADC {

ADS1256::ADS1256(SPIClass &spi, int8_t csPin, int8_t drdyPin, int8_t rstPin)
  : _spi(spi),
    _csPin(csPin),
    _drdyPin(drdyPin),
    _rstPin(rstPin)
{

  _config.gain             = 0;
  _config.drateCode        = 0x06;
  _config.referenceVoltage = 2.5f;
  _config.commandSpeed     = 1000000UL;
  _config.readSpeed        = 1000000UL;
  _config.idSpeed          = 1000000UL;
  _config.testSpeed        = 1000000UL;
  _config.spiMode          = SPI_MODE1;
}

bool ADS1256::begin() {
  pinMode(_csPin, OUTPUT);
  if (_drdyPin >= 0) pinMode(_drdyPin, INPUT);
  pinMode(_rstPin, OUTPUT);
  digitalWrite(_csPin, HIGH);
  digitalWrite(_rstPin, HIGH);
  delay(100);

  reset(); delay(10);
  setGain(_config.gain); setSampleRate(_config.drateCode);
  uint8_t adcon = readRegister(0x02);
  adcon |= (1<<3);
  _spi.beginTransaction(SPISettings(_config.commandSpeed, MSBFIRST, _config.spiMode));
  digitalWrite(_csPin, LOW);
  _spi.transfer(0x50 | 0x02);
  _spi.transfer(0x00);
  _spi.transfer(adcon);
  digitalWrite(_csPin, HIGH);
  _spi.endTransaction();
  delay(10);
  selfCalibrate(); delay(50);
  return true;
}

bool ADS1256::isReady() {
  return (_drdyPin >= 0) ? (digitalRead(_drdyPin) == LOW) : true;
}

void ADS1256::setGain(uint8_t gainCode) {
  _config.gain = gainCode & 0x07;
  _spi.beginTransaction(SPISettings(_config.commandSpeed, MSBFIRST, _config.spiMode));
  digitalWrite(_csPin, LOW);
  _spi.transfer(0x52);
  _spi.transfer(0x00);
  _spi.transfer(_config.gain);
  digitalWrite(_csPin, HIGH);
  _spi.endTransaction(); delay(10);
}

void ADS1256::setSampleRate(uint8_t drateCode) {
  _config.drateCode = drateCode;
  _spi.beginTransaction(SPISettings(_config.commandSpeed, MSBFIRST, _config.spiMode));
  digitalWrite(_csPin, LOW);
  _spi.transfer(0x53);
  _spi.transfer(0x00);
  _spi.transfer(_config.drateCode);
  digitalWrite(_csPin, HIGH);
  _spi.endTransaction(); delay(10);
}

void ADS1256::setChannel(uint8_t channel) {
  _spi.beginTransaction(SPISettings(_config.commandSpeed, MSBFIRST, _config.spiMode));
  digitalWrite(_csPin, LOW);
  _spi.transfer(0x51);
  _spi.transfer(0x00);
  _spi.transfer((channel << 4) | 0x08);
  digitalWrite(_csPin, HIGH);
  _spi.endTransaction(); delayMicroseconds(10);
  sendCommand(0xFC); sendCommand(0x00); delayMicroseconds(10);
}

int32_t ADS1256::read(uint8_t samples) {
  int64_t total = 0;
  for (uint8_t i = 0; i < samples; ++i) {
    unsigned long timeout = millis() + 1000;
    while (digitalRead(_drdyPin) == HIGH) {
      if (millis() > timeout) return 0;
      delayMicroseconds(10);
    }

    _spi.beginTransaction(SPISettings(_config.readSpeed, MSBFIRST, _config.spiMode));
    digitalWrite(_csPin, LOW);
    _spi.transfer(0x01);
    delayMicroseconds(10);


    uint32_t raw24 = ((uint32_t)_spi.transfer(0) << 16)
                   | ((uint32_t)_spi.transfer(0) << 8)
                   |  (uint32_t)_spi.transfer(0);
    digitalWrite(_csPin, HIGH);
    _spi.endTransaction();


    if (raw24 & 0x800000) {
      raw24 |= 0xFF000000;
    }
    int32_t signed24 = (int32_t)raw24;

    total += signed24;
    delayMicroseconds(100);
  }


  return (int32_t)(total / samples);
}

float ADS1256::readVoltage(uint8_t channel, uint8_t samples) {
  setChannel(channel);
  int32_t signedRaw = read(samples);

  float vref   = _config.referenceVoltage;
  float fsMax  = float((1 << 23) - 1);

  float pga    = PGA_FACTORS[_config.gain];


  return (float(signedRaw) / fsMax) * (vref / pga);
}

void ADS1256::setDifferential(uint8_t posChannel, uint8_t negChannel) {
  const uint8_t MUX = 0x01;
  uint8_t value = ((posChannel & 0x07) << 4) | (negChannel & 0x07);
  _spi.beginTransaction(SPISettings(_config.commandSpeed, MSBFIRST, _config.spiMode));
  digitalWrite(_csPin, LOW);
  _spi.transfer(0x50 | MUX);
  _spi.transfer(0x00);
  _spi.transfer(value);
  digitalWrite(_csPin, HIGH);
  _spi.endTransaction();
  delayMicroseconds(10);

  sendCommand(0xFC);
  sendCommand(0x00);
  delayMicroseconds(10);
}

int32_t ADS1256::readDifferential(uint8_t posChannel, uint8_t negChannel, uint8_t samples) {
  setDifferential(posChannel, negChannel);
  return read(samples);
}

float ADS1256::readDifferentialVoltage(uint8_t posChannel,
                                       uint8_t negChannel,
                                       uint8_t samples) {
  int32_t signedRaw = readDifferential(posChannel, negChannel, samples);
  float    full     = float((1 << 23) - 1);
  float    pga      = float(1 << _config.gain);
  float    vref     = _config.referenceVoltage;

  return (float(signedRaw) / full) * (vref / pga);
}

uint8_t ADS1256::readID() {
  _spi.beginTransaction(SPISettings(_config.idSpeed, MSBFIRST, _config.spiMode)); digitalWrite(_csPin, LOW);
  _spi.transfer(0x20); _spi.transfer(0); delayMicroseconds(10);
  uint8_t id=_spi.transfer(0);
  digitalWrite(_csPin, HIGH); _spi.endTransaction(); return id;
}

uint8_t ADS1256::testSPI() {
  _spi.beginTransaction(SPISettings(_config.testSpeed, MSBFIRST, _config.spiMode)); digitalWrite(_csPin, LOW);
  uint8_t r = _spi.transfer(0xFF);
  digitalWrite(_csPin, HIGH); _spi.endTransaction(); return r;
}

void ADS1256::reset() {
  digitalWrite(_rstPin, LOW); delay(10); digitalWrite(_rstPin, HIGH); delay(10);
  sendCommand(0xFE); delay(10);
}

void ADS1256::selfCalibrate() { sendCommand(0xF0); }

void ADS1256::sendCommand(uint8_t cmd) {
  _spi.beginTransaction(SPISettings(_config.commandSpeed, MSBFIRST, _config.spiMode));
  digitalWrite(_csPin, LOW); _spi.transfer(cmd); digitalWrite(_csPin, HIGH);
  _spi.endTransaction(); delayMicroseconds(10);
}

uint8_t ADS1256::readRegister(uint8_t reg) {

  _spi.beginTransaction(SPISettings(_config.commandSpeed, MSBFIRST, _config.spiMode));
  digitalWrite(_csPin, LOW);
  _spi.transfer(0x10 | (reg & 0x0F));
  _spi.transfer(0x00);
  delayMicroseconds(10);
  uint8_t val = _spi.transfer(0x00);
  digitalWrite(_csPin, HIGH);
  _spi.endTransaction();
  return val;
}

uint8_t ADS1256::readADCON() {

  return readRegister(0x02);
}

}
}