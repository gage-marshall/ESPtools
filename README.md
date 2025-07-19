# ESPtools Library

A modular Arduino library for ESP32-based production test fixtures with a focus on extensibility, connectivity, and ISO 9001 calibration compliance.



## `ADS1256`

High-resolution 24-bit ADC driver for precision measurements, SPI interfaced. Includes single-ended and differential read support.

- `ADS1256.cpp`
- `ADS1256.h`

## `ADS1115`

16-bit ADC driver for general measurements, I²C interfaced. Includes single-ended and differential read support.

- `ADS1115.cpp`
- `ADS1115.h`

## `ButtonManager`

Manages GPIO button inputs, debouncing, detection of isPressed, wasReleased, wasPressed

- `ButtonManager.cpp`
- `ButtonManager.h`

## `CD74HC4067`

16-channel analog/digital mux control.

- `CD74HC4067.cpp`
- `CD74HC4067.h`

## `ESPtools`

Header file aggregating all module interfaces.

- `ESPtools.h`

## `EventBus`

Decoupled MQTT publish/subscribe-style event handler between components. Can be configured as a local MQTT broker to initiate commands to MQTTClient based on user input.

- `EventBus.cpp`
- `EventBus.h`

## `LCD`

Driver for character LCDs via I²C using PCF8574 I/O expander.

- `LCD.cpp`
- `LCD.h`

## `MQTTClient`

Publishes sensor or system data to a broker.

- `MQTTClient.cpp`
- `MQTTClient.h`

## `PCA9548A`

I²C multiplexer for connecting multiple I²C devices.

- `PCA9548A.cpp`
- `PCA9548A.h`

## `PCF8575`

16-bit GPIO expander over I²C, supports I/O pin extension.

- `PCF8575.cpp`
- `PCF8575.h`

## `RTC`

Real-Time Clock abstraction with calibration date tracking (ISO 9001 friendly).

- `RTC.cpp`
- `RTC.h`

## `UARTBridge`

Serial bridge for tunneling data between UART and MQTT.

- `UARTBridge.cpp`
- `UARTBridge.h`

## `WiFiEnterprise`

Provides easy WPA2 Enterprise network support

- `WiFiEnterprise.cpp`
- `WiFiEnterprise.h`



# Requirements

- Adafruit's RTClib (https://github.com/adafruit/RTClib) is required for RTC/calibration integration

- Nicholas O'Leary's PubSubClient (https://github.com/knolleary/pubsubclient) is required for EventBus/MQTT integration

- Matthias Hertel's LiquidCrystal_PCF8574 (https://github.com/mathertel/LiquidCrystal_PCF8574) is required for LCD integration



# Usage Examples



## ADC Interfacing

```c++
#include <Wire.h>
#include <SPI.h>

#include "ADS1115.h"
#include "ADS1256.h"

ESPtools::ADC::ADS1115 ads1115(Wire, 0x48);			// Default Wire interface

ESPtools::ADC::ADS1256 ads1256(SPI, 23, 4, 34);		// CS 23, DRDY 4, RST 34

void setup() {
  Serial.begin(115200);
  
  // Initialize ADS1115
  Wire.begin();
  ads1115.reset();
  ads1115.setGain(ADS1115::GAIN_1X);       			// ±4.096v range
  ads1115.setSampleRate(ADS1115::SPS_128); 			// 128 SPS

  //Initialize ADS1256
  SPI.begin();  
  ads1256.reset();
  ads1256.selfCalibrate();                  		// internal offset/gain cal
  ads1256.setGain(ADS1256::GAIN_2X);        		// ±2.5v on 5v ref
  ads1256.setSampleRate(ADS1256::SPS_1000); 		// 1000 SPS
  ads1256.setChannel(0);
}

void loop() {
  // Read single-ended channel 0 on ADS1115
  float v1115 = ads1115.readVoltage(0);
  Serial.print("ADS1115 CH0: ");
  Serial.print(v1115, 6);
  Serial.print(" V\t");

  // Read single-ended channel 0 on ADS1256
  float v1256 = ads1256.readVoltage(0);
  Serial.print("ADS1256 CH0: ");
  Serial.print(v1256, 6);
  Serial.println(" V");

  delay(500);
}

```



## MQTT, EventBus Local Broker, UART Bridge, Button Manager

```C++
#include <WiFi.h>
#include <PubSubClient.h>

#include "ButtonManager.h"
#include "EventBus.h"
#include "UARTBridge.h"
#include "MQTTClient.h"

const char* WIFI_SSID   = "WIFISSID";     // Wi-Fi SSID
const char* WIFI_PASS   = "PASSWORD";     //Wi-Fi password
const char* MQTT_BROKER = "127.0.0.1";    //Broker address
const uint16_t MQTT_PORT = 1883;

constexpr uint8_t BUTTON_IDX = 0;
constexpr uint8_t BUTTON_PIN = 0;         // Onboard button at GPIO0
constexpr uint8_t LED_PIN    = 2;         // Onboard LED
constexpr uint32_t UART_BAUD = 115200;

WiFiClient   wifiClient;
PubSubClient mqttClient(wifiClient);

bool ledState = false;

void onLedEvent(const String &payload) {
  if (payload == "ON") {
    digitalWrite(LED_PIN, HIGH);
    ledState = true;
  }
  else if (payload == "OFF") {
    digitalWrite(LED_PIN, LOW);
    ledState = false;
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
 
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("Wi-Fi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  Serial.println("Connected!");

  // Set up a button
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  ESPtools::Input::configureButton(BUTTON_IDX, BUTTON_PIN);

  // EventBus local broker 
  ESPtools::EventBus::begin();
  ESPtools::EventBus::subscribe("device/led", onLedEvent);

  // UARTBridge setup
  ESPtools::UART::begin(Serial1, UART_BAUD);

  // MQTTClient creation and subscribe
  ESPtools::MQTT::begin(mqttClient, MQTT_BROKER, MQTT_PORT);
  if (!ESPtools::MQTT::connect("DUT")) {
    Serial.println("MQTT connect failed");
  }

  ESPtools::MQTT::subscribe("device/led");

  Serial.println("Setup complete.");
}

void loop() {
  // Button presses feed command to EventBus local broker
  ESPtools::Input::pollButtons();
  if (ESPtools::Input::wasReleased(BUTTON_IDX)) {
    String cmd = ledState ? "OFF" : "ON";
    ESPtools::EventBus::publish("device/led", cmd);
  }

  // USB Serial feeds colon-separated command to EventBus local broker 
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    int sp = line.indexOf(':');
    if (sp > 0) {
      String topic   = line.substring(0, sp);
      String payload = line.substring(sp + 1);
      ESPtools::EventBus::publish(topic, payload);
    }
  }

  // UARTBridge feeds colon-separated command to EventBus local broker 
  ESPtools::UART::loop();

  // Main MQTT loop
  ESPtools::MQTT::loop();

  delay(10);
}

```



## LCD Interfacing

```C++
#include <Wire.h>
#include "LCD.h"

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Wire.begin();
  if (!ESPtools::Display::lcdBegin()) {
    Serial.println("LCD init failed");
    while (1);
  }
  ESPtools::Display::backlight(true);
  ESPtools::Display::clear();
}

void loop() {
  ESPtools::Display::writeLine(0, "Line 0");
  ESPtools::Display::writeCentered(2, "Line 2");
  ESPtools::Display::scrollLine(1, "Test of long scrolling text on line 1!", 500);

for (uint8_t pct = 0; pct <= 100; pct += 5) {
    ESPtools::Display::drawProgressBar(3, pct);
    delay(100);
  }

  // Pause with full progress bar shown
  delay(1000);
  
  ESPtools::Display::writeLine(3, "");  // blanks row 3
}
```



## WPA2 Enterprise Connection

```C++
#include <WiFi.h>
#include "WiFiEnterprise.h"

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  // SSID, identity, password:
  if (ESPtools::WiFi::connect("ENTERPRISE_SSID", "user@domain.com", "password")) {
    Serial.println("Connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed!");
  }
}

void loop() {}
```



## RTC Interface With ISO 9001 Calibration Support

```C++
#include <Wire.h>
#include "RTC.h"

using namespace Calibration::RTC;

static const uint16_t CAL_WINDOW_DAYS = 90;   // Audit interval

unsigned long lastReport = 0;
const unsigned long REPORT_INTERVAL = 5000;   // ms

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  // Initialize RTC
  Wire.begin();
  if (!rtcBegin()) {
    Serial.println("RTC init failed!");
    while (1) { delay(1000); }
  }

  // Set a time if we've lost power
  if (!rtcTimeSet()) {
    Serial.println("RTC time not set, syncing from compile time.");
    setDateTime(2025, 7, 19, 21, 0, 0);
  }

  // ISO 9001 calibration check example
  setCalibrationWindowDays(CAL_WINDOW_DAYS);
  Serial.println();
  Serial.printf("RTC ready. Calibration window = %u days.\n", CAL_WINDOW_DAYS);
  Serial.println("Send 'cal' to store today as new calibration date.\n");
}

void loop() {
  // Set calibration date over serial
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.equalsIgnoreCase("cal")) {
      storeCalibrationDate();
      Serial.println("Calibration date stored.");
    }
  }

  // Calibration status check example
  if (millis() - lastReport >= REPORT_INTERVAL) {
    lastReport = millis();
    Serial.print("Now:  ");
    Serial.println(getDateTimeString());

    bool expired = isCalibrationExpired();
    Serial.print("Calibration status: ");
    Serial.println(expired ? "EXPIRED" : "OK");
    Serial.println();
  }
}

```



## Muxes and Expanders

```C++
#include <Wire.h>
#include "CD74HC4067.h"

// S0, S1, S2, S3, enPin
ESPtools::Mux::CD74HC4067 mux(14, 12, 13, 15, -1);

void setup() {
  Serial.begin(115200);
  mux.begin();

  Serial.println("Scanning channels:");
  for (uint8_t ch = 0; ch < 16; ch++) {
    mux.selectChannel(ch);
    int raw = mux.readAnalog(A0);        // Read from A0
    float volts = raw * (3.3 / 4095.0);
    Serial.printf("  CH%2u → %4d (%.2f V)\n", ch, raw, volts);
    delay(100);
  }
  mux.disable();  // Disconnect all channels
}

void loop() {}
```



```C++
#include <Wire.h>
#include "PCA9548A.h"

// Address 0x70, using default Wire
ESPtools::Mux::PCA9548A i2cMux(0x70, Wire);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  if (!i2cMux.begin()) {
    Serial.println("PCA9548A init failed!");
    while (1);
  }

  Serial.println("Scanning I2C buses via PCA9548A:");
  for (uint8_t bus = 0; bus < 8; bus++) {
    i2cMux.selectBus(bus);
    Serial.printf(" Bus %u mask=0x%02X = ", bus, i2cMux.enabledMask());
    for (uint8_t addr = 1; addr < 128; addr++) {
      Wire.beginTransmission(addr);
      if (Wire.endTransmission() == 0) {
        Serial.printf("0x%02X ", addr);
      }
    }
    Serial.println();
    delay(200);
  }
  i2cMux.disableAll(); // Disconnect all buses
}

void loop() {}

```



```C++
#include <Wire.h>
#include "PCF8575.h"

using namespace ESPtools::IOExpander::PCF8575;

// I2C address of PCF8575 (0x20–0x27)
PCF8575 expander(0x20, Wire);

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Wire.begin();
  if (!expander.begin()) {
    Serial.println("PCF8575 init failed!");
    while (1);
  }
  Serial.println("PCF8575 ready!");

  // Configure lower 8 bits as outputs, upper 8 bits as inputs
  for (uint8_t pin = 0; pin < 8; pin++) {
    expander.pinMode(pin, PinMode::Output);
    expander.digitalWrite(pin, false);
  }
  for (uint8_t pin = 8; pin < 16; pin++) {
    expander.pinMode(pin, PinMode::Input);
  }
}

void loop() {
  // Toggle pins 0-7
  static bool state = false;
  state = !state;
  for (uint8_t pin = 0; pin < 8; pin++) {
    expander.digitalWrite(pin, state);
  }
  Serial.printf("Outputs 0–7 set to %s\n", state ? "HIGH" : "LOW");

  // Read pins 8-15
  for (uint8_t pin = 8; pin < 16; pin++) {
    bool level = expander.digitalRead(pin);
    Serial.printf("Input %u: %s  ", pin, level ? "HIGH" : "LOW");
  }
  Serial.println("\n");

  delay(1000);
}
```



# License

```
Copyright 2025 Gage Marshall

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

