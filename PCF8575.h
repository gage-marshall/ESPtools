#ifndef ESPTOOLS_PCF8575_H
#define ESPTOOLS_PCF8575_H

#include <Arduino.h>
#include <Wire.h>

namespace ESPtools {
namespace IOExpander {
namespace PCF8575 {

// Pin direction
enum class PinMode { Input, Output };

/**
 * PCF8575 16-bit I/O expander driver (instance-based, supports multiple devices).
 */
class PCF8575 {
public:
  /**
   * Constructor
   * @param address I2C address of PCF8575 (e.g., 0x20-0x27)
   * @param wire    TwoWire instance (default Wire)
   */
  PCF8575(uint8_t address, TwoWire &wire = Wire);

  /**
   * Initialize I2C and read initial port state into shadow register.
   */
  bool begin();

  /**
   * Read full 16-bit port value.
   * @return port bits [15:0]
   */
  uint16_t readAll();

  /**
   * Write full 16-bit port value.
   */
  void writeAll(uint16_t value);

  /**
   * Read single pin state.
   * @param pin index 0..15
   * @return true if high, false if low
   */
  bool digitalRead(uint8_t pin);

  /**
   * Set single pin state.
   * @param pin index 0..15
   * @param level true = high, false = low
   */
  void digitalWrite(uint8_t pin, bool level);

  /**
   * Toggle single pin state.
   * @param pin index 0..15
   */
  void toggle(uint8_t pin);

  /**
   * Set pin direction: Input (releases line) or Output (drives via shadow register).
   */
  void pinMode(uint8_t pin, PinMode mode);

  /**
   * Write only masked bits, only bits set in mask are updated to corresponding bits in value.
   */
  void writeMask(uint16_t mask, uint16_t value);

  /**
   * Invert polarity on specified mask bits for read/write operations.
   */
  void invert(uint16_t mask, bool invert = true);
  
  /**
   * Attach interrupt handler to INT line.
   * irqPin   = GPIO connected to PCF8575 INT
   * handler  = ISR function
   * mode     = FALLING, RISING, or CHANGE
   */
  void onInterrupt(uint8_t irqPin, void (*handler)(), int mode);

private:
  TwoWire &_wire;
  uint8_t  _address;
  uint16_t _shadow;
  uint16_t _invertMask;

  // Perform raw I2C read or write
  void _writePort(uint16_t port);
  uint16_t _readPort();
};

}
}
}

#endif
