#ifndef ESPTOOLS_CD74HC4067_H
#define ESPTOOLS_CD74HC4067_H

#include <Arduino.h>

namespace ESPtools {
namespace Mux {

/**
 * CD74HC4067 16-channel analog multiplexer/demultiplexer driver.
 */
class CD74HC4067 {
public:
  /**
   * Constructor
   * @param s0Pin   GPIO connected to address bit S0
   * @param s1Pin   GPIO connected to address bit S1
   * @param s2Pin   GPIO connected to address bit S2
   * @param s3Pin   GPIO connected to address bit S3
   * @param enPin   Optional enable pin (active LOW), -1 if unused
   */
  CD74HC4067(uint8_t s0Pin,
             uint8_t s1Pin,
             uint8_t s2Pin,
             uint8_t s3Pin,
             int8_t enPin = -1);

  /**
   * Initialize multiplexer pins.
   * Call in setup().
   */
  void begin();

  /**
   * Select channel 0..15 for the common SIG pin to connect.
   * @param channel Channel number (0-15)
   */
  void selectChannel(uint8_t channel);

  /**
   * Read analog value from the given Arduino pin after selecting channel.
   * @param analogPin analog input pin connected to SIG
   * @return analogRead() result
   */
  int readAnalog(uint8_t analogPin);

  /**
   * Disable all channel switches (if enable pin is used).
   * After disable(), no channel is connected.
   */
  void disable();

private:
  uint8_t _s0, _s1, _s2, _s3;
  int8_t  _en;
};

}
}

#endif
