#ifndef CALIBRATION_RTC_H
#define CALIBRATION_RTC_H

#include <Arduino.h>
#include <Wire.h>

namespace Calibration {
namespace RTC {

// Initialize RTC with optional custom I2C pins
// Default: Wire.begin() with board defaults
bool rtcBegin();
bool rtcBegin(uint8_t sdaPin, uint8_t sclPin, TwoWire &wire = Wire);

// Check if RTC has valid time set
bool rtcTimeSet();

// Set RTC date/time manually
void setDateTime(int year, int month, int day, int hour, int minute, int second);

// Get current RTC date and time as a formatted string
String getDateTimeString();

// Store current RTC date as the last calibration date
void storeCalibrationDate();

// Set how many days before calibration expires
void setCalibrationWindowDays(uint16_t days);

// Check if calibration is expired or RTC date is not set
bool isCalibrationExpired();

}
}

#endif
