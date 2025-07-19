#ifndef ESPTOOLS_ADS1115_H
#define ESPTOOLS_ADS1115_H

#include <Arduino.h>
#include <Wire.h>

namespace ESPtools {
namespace ADC {

/**
 * ADS1115 4-channel 16-bit I2C ADC driver in ESPtools framework.
 * API styled to mirror ADS1256 for consistency.
 *
 * PGA Gain Codes (Config Register bits [11:9]):
 *   GAIN_TWOTHIRDS = 0  // ±6.144V
 *   GAIN_1X        = 1  // ±4.096V
 *   GAIN_2X        = 2  // ±2.048V
 *   GAIN_4X        = 3  // ±1.024V
 *   GAIN_8X        = 4  // ±0.512V
 *   GAIN_16X       = 5  // ±0.256V
 *
 * Data Rate Codes (Config Register bits [7:5]):
 *   SPS_860 = 7  // 860 SPS
 *   SPS_475 = 6  // 475 SPS
 *   SPS_250 = 5  // 250 SPS
 *   SPS_128 = 4  // 128 SPS (default)
 *   SPS_64  = 3  // 64 SPS
 *   SPS_32  = 2  // 32 SPS
 *   SPS_16  = 1  // 16 SPS
 *   SPS_8   = 0  // 8 SPS
 */
 
class ADS1115 {
public:
  // Gain constants
  static constexpr uint8_t GAIN_TWOTHIRDS 	= 0;
  static constexpr uint8_t GAIN_1X		= 1;
  static constexpr uint8_t GAIN_2X       	= 2;
  static constexpr uint8_t GAIN_4X      	= 3;
  static constexpr uint8_t GAIN_8X      	= 4;
  static constexpr uint8_t GAIN_16X     	= 5;

  // Data rate constants
  static constexpr uint8_t SPS_860 = 7;
  static constexpr uint8_t SPS_475 = 6;
  static constexpr uint8_t SPS_250 = 5;
  static constexpr uint8_t SPS_128 = 4;
  static constexpr uint8_t SPS_64  = 3;
  static constexpr uint8_t SPS_32  = 2;
  static constexpr uint8_t SPS_16  = 1;
  static constexpr uint8_t SPS_8   = 0;

  /**
   * Constructor: configure I2C bus and optional alert pin.
   * @param wire    TwoWire instance (e.g., Wire)
   * @param address I2C address (default 0x48)
   * @param drdyPin ALERT/RDY pin (optional, -1 if unused)
   */
  ADS1115(TwoWire &wire = Wire, uint8_t address = 0x48, int8_t drdyPin = -1);

  /**
   * Initialize I2C, apply default gain/rate, return true on success.
   */
  bool begin();

  /**
   * Check if conversion ready. If drdyPin < 0, always return true after wait.
   */
  bool isReady();

  /**
   * Set PGA gain (use GAIN_ constants).
   */
  void setGain(uint8_t gainCode);

  /**
   * Set data rate (use SPS_ constants).
   */
  void setSampleRate(uint8_t rateCode);

  /**
   * Select ADC channel (0..3) for next conversion.
   */
  void setChannel(uint8_t channel);

  /**
   * Read raw ADC value, averaging n conversions.
   */
  int32_t read(uint8_t samples = 1);

  /**
   * Read voltage (volts) on given channel, averaging samples.
   */
  float readVoltage(uint8_t channel, uint8_t samples = 1);
  
  /**
   * Configure internal MUX for a P-N differential channel.
   * only the standard pairs are supported.
   *   (0,1), (0,3), (1,3), (2,3).
   */
  void setDifferential(uint8_t posChannel, uint8_t negChannel);

  /**
   * Read raw signed code from a differential pair.
   */
  int32_t readDifferential(uint8_t posChannel, uint8_t negChannel, uint8_t samples = 50);

  /**
   * Read differential voltage (in volts).
   */
  float readDifferentialVoltage(uint8_t posChannel, uint8_t negChannel, uint8_t samples = 50);

  /**
   * Read device ID (returns I2C address, since ADS1115 has no ID reg).
   */
  uint8_t readID();

  /**
   * Perform simple I2C test, read MSB of conversion register.
   */
  uint8_t testI2C();

  /**
   * Reset config to defaults.
   */
  void reset();

private:
  TwoWire &_wire;
  uint8_t  _address;
  int8_t   _drdyPin;
  uint16_t _configReg;
  uint8_t  _currentChannel;

  static constexpr uint8_t REG_CONVERSION = 0x00;
  static constexpr uint8_t REG_CONFIG     = 0x01;

  void writeRegister(uint8_t reg, uint16_t value);
  uint16_t readRegister(uint8_t reg);
  void waitForConversion();
};

}
}

#endif
