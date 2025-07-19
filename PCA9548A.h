#ifndef ESPTOOLS_PCA9548A_H
#define ESPTOOLS_PCA9548A_H

#include <Arduino.h>
#include <Wire.h>

namespace ESPtools {
namespace Mux {

/**
 * PCA9548A 8-channel I2C multiplexer driver.
 * Allows switching between 8 downstream I2C buses.
 */
class PCA9548A {
public:
  /**
   * Constructor
   * @param address I2C address of PCA9548A (0x70-0x77)
   * @param wire    TwoWire instance (default Wire)
   */
  PCA9548A(uint8_t address = 0x70, TwoWire &wire = Wire);

  /**
   * Initialize I2C bus for the multiplexer.
   * @return true if device acknowledged
   */
  bool begin();

  /**
   * Select a single downstream bus (0-7). All others disabled.
   * @param bus index of bus to enable
   */
  void selectBus(uint8_t bus);

  /**
   * Disable all downstream buses.
   */
  void disableAll();

  /**
   * Get current enabled bus mask.
   * @return 8-bit mask of enabled channels (bit0=bus0)
   */
  uint8_t enabledMask() const;

private:
  TwoWire &_wire;
  uint8_t _address;
  uint8_t _mask;
};

}
}

#endif
