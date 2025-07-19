#include "RTC.h"
#include <RTClib.h>
#include <Preferences.h>

namespace Calibration {
namespace RTC {


static TwoWire *_wire = &Wire;

static RTC_DS3231 rtc;

static Preferences rtcPrefs;

static uint16_t calWindowDays = 90;

bool rtcBegin() {
  _wire->begin();
  if (!rtc.begin()) {
    return false;
  }
  return !rtc.lostPower();
}

bool rtcBegin(uint8_t sdaPin, uint8_t sclPin, TwoWire &wire) {
  _wire = &wire;
  _wire->begin(sdaPin, sclPin);
  if (!rtc.begin()) {
    return false;
  }
  return !rtc.lostPower();
}

bool rtcTimeSet() {
  return !rtc.lostPower();
}

void setDateTime(int year, int month, int day, int hour, int minute, int second) {
  rtc.adjust(DateTime(year, month, day, hour, minute, second));
}

String getDateTimeString() {
  DateTime now = rtc.now();
  char buf[20];
  sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
          now.year(), now.month(), now.day(),
          now.hour(), now.minute(), now.second());
  return String(buf);
}

void storeCalibrationDate() {
  DateTime now = rtc.now();
  rtcPrefs.begin("rtc-cal", false);
  rtcPrefs.putUInt("cal_year", now.year());
  rtcPrefs.putUChar("cal_month", now.month());
  rtcPrefs.putUChar("cal_day", now.day());
  rtcPrefs.end();
}

void setCalibrationWindowDays(uint16_t days) {
  calWindowDays = days;
}

bool isCalibrationExpired() {
  if (!rtcTimeSet()) return true;
  rtcPrefs.begin("rtc-cal", true);
  uint16_t year = rtcPrefs.getUInt("cal_year", 0);
  uint8_t month = rtcPrefs.getUChar("cal_month", 0);
  uint8_t day = rtcPrefs.getUChar("cal_day", 0);
  rtcPrefs.end();
  if (year == 0 || month == 0 || day == 0) return true;
  DateTime calDate(year, month, day);
  DateTime now = rtc.now();
  TimeSpan diff = now - calDate;
  return diff.days() > calWindowDays;
}

}
}