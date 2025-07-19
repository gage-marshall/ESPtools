#ifndef ESPTOOLS_ADS1256_H
#define ESPTOOLS_ADS1256_H

#include <Arduino.h>
#include <SPI.h>

namespace ESPtools {
namespace ADC {

/**
 * ADS1256 driver with extended features: init status, ready check,
 * scaled voltage reading, dynamic gain and sample rate.
 *
 * PGA Gain Codes (ADCON register, bits [2:0]):
 *   GAIN_1X   = 0x00 (×1)
 *   GAIN_2X   = 0x01 (×2)
 *   GAIN_4X   = 0x02 (×4)
 *   GAIN_8X   = 0x03 (×8)
 *   GAIN_16X  = 0x04 (×16)
 *   GAIN_32X  = 0x05 (×32)
 *   GAIN_64X  = 0x06 (×64)
 *
 * Data Rate Codes (DRATE register):
 *   SPS_30000 = 0xF0 (30000 SPS)
 *   SPS_15000 = 0xE0 (15000 SPS)
 *   SPS_7500  = 0xD0 (7500 SPS)
 *   SPS_3750  = 0xC0 (3750 SPS)
 *   SPS_2000  = 0xB0 (2000 SPS)
 *   SPS_1000  = 0xA0 (1000 SPS)
 *   SPS_500   = 0x92 (500 SPS)
 *   SPS_100   = 0x82 (100 SPS)
 *   SPS_60    = 0x72 (60 SPS)
 *   SPS_50    = 0x63 (50 SPS)
 *   SPS_30    = 0x53 (30 SPS)
 *   SPS_25    = 0x43 (25 SPS)
 *   SPS_15    = 0x33 (15 SPS)
 *   SPS_10    = 0x23 (10 SPS)
 *   SPS_5     = 0x13 (5 SPS)
 *   SPS_2_5   = 0x03 (2.5 SPS)
 */
 
class ADS1256 {
public:
  // Gain and data-rate constants
  static constexpr uint8_t GAIN_1X   = 0x00;
  static constexpr uint8_t GAIN_2X   = 0x01;
  static constexpr uint8_t GAIN_4X   = 0x02;
  static constexpr uint8_t GAIN_8X   = 0x03;
  static constexpr uint8_t GAIN_16X  = 0x04;
  static constexpr uint8_t GAIN_32X  = 0x05;
  static constexpr uint8_t GAIN_64X  = 0x06;

  static constexpr uint8_t SPS_30000 = 0xF0;
  static constexpr uint8_t SPS_15000 = 0xE0;
  static constexpr uint8_t SPS_7500  = 0xD0;
  static constexpr uint8_t SPS_3750  = 0xC0;
  static constexpr uint8_t SPS_2000  = 0xB0;
  static constexpr uint8_t SPS_1000  = 0xA0;
  static constexpr uint8_t SPS_500   = 0x92;
  static constexpr uint8_t SPS_100   = 0x82;
  static constexpr uint8_t SPS_60    = 0x72;
  static constexpr uint8_t SPS_50    = 0x63;
  static constexpr uint8_t SPS_30    = 0x53;
  static constexpr uint8_t SPS_25    = 0x43;
  static constexpr uint8_t SPS_15    = 0x33;
  static constexpr uint8_t SPS_10    = 0x23;
  static constexpr uint8_t SPS_5     = 0x13;
  static constexpr uint8_t SPS_2_5   = 0x03;
  
  static constexpr float PGA_FACTORS[8] = {
  	1.0f,   // code 0 ×1
  	2.0f,   // code 1 ×2
  	4.0f,   // code 2 ×4
  	8.0f,   // code 3 ×8
  	16.0f,  // code 4 ×16
  	32.0f,  // code 5 x32
  	64.0f,  // code 6 ×64
};


  /**
   * Constructor: configure SPI bus and control pins.
   * @param spi      SPIClass reference (HSPI or VSPI)
   * @param csPin    Chip select pin
   * @param drdyPin  Data ready (DRDY) pin, or -1 if unused
   * @param rstPin   Reset pin
   */
  ADS1256(SPIClass &spi, int8_t csPin, int8_t drdyPin, int8_t rstPin);

  /**
   * Initialize ADS1256 hardware. Set pin modes, reset, apply default gain/rate,
   * and self-calibrate. Returns true on success.
   */
  bool begin();

  /**
   * Check DRDY pin for data-ready (LOW). If drdyPin < 0, always returns true.
   */
  bool isReady();

  /**
   * Set PGA gain code (use GAIN_ constants).
   * @param gainCode 3-bit gain value (0x00-0x06)
   */
  void setGain(uint8_t gainCode);

  /**
   * Set data rate code (use SPS_ constants).
   * @param drateCode 8-bit DRATE register value
   */
  void setSampleRate(uint8_t drateCode);

  /**
   * Select input channel for conversion (0-7).
   * @param channel Channel number
   */
  void setChannel(uint8_t channel);

  /**
   * Read raw ADC data, averaging specified number of samples.
   * @param samples Number of conversions to average (default 50)
   * @return Averaged raw code (signed 24-bit)
   */
  int32_t read(uint8_t samples = 50);

  /**
   * Read voltage on channel (in volts), averaging samples.
   * @param channel Channel number
   * @param samples Number of samples to average
   * @return Measured voltage
   */
  float readVoltage(uint8_t channel, uint8_t samples = 50);
  
  /**
   * Configure P-N differential conversion.
   *  positive input = posChannel, negative = negChannel.
   */
  void setDifferential(uint8_t posChannel, uint8_t negChannel);

  /**
   * Read differential conversion (raw signed code).
   */
  int32_t readDifferential(uint8_t posChannel,
                           uint8_t negChannel,
                           uint8_t samples = 50);

  /**
   * Read a differential conversion (volts).
   */
  float readDifferentialVoltage(uint8_t posChannel,
                                uint8_t negChannel,
                                uint8_t samples = 50);

  /**
   * Read device ID register. Returns ID byte.
   */
  uint8_t readID();

  /**
   * Send a test command (0xFF) over SPI. Returns response byte.
   */
  uint8_t testSPI();

  /**
   * Perform hardware reset sequence and sync.
   */
  void reset();

  /**
   * Perform self-calibration (calls SELFCAL command).
   */
  void selfCalibrate();
  
  /**
  * Set reference voltage (default 2.5 V).
  */
  void setReferenceVoltage(float vref) {
  _config.referenceVoltage = vref;
  }

  /**
   * Read back a register for debugging.
   */
  uint8_t readRegister(uint8_t reg);
  
  // Quick method to read ADCON register.
  uint8_t readADCON();

  /**
   * Send a single SPI command byte.
   */
  void sendCommand(uint8_t cmd);
  
private:
  SPIClass &_spi;
  int8_t _csPin, _drdyPin, _rstPin;
  struct Config {
    uint32_t commandSpeed;
    uint32_t readSpeed;
    uint32_t idSpeed;
    uint32_t testSpeed;
    uint8_t  spiMode;
    uint8_t  gain;
    uint8_t  drateCode;
    float    referenceVoltage;
  } _config;

};

}
}

#endif
