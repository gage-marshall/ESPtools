#include "LCD.h"
#include <LiquidCrystal_PCF8574.h>

#define LCD_I2C_ADDR    0x27
#define LCD_ROWS        4
#define LCD_COLS        20
#define LCD_BACKLIGHT   255

LiquidCrystal_PCF8574 lcd(LCD_I2C_ADDR);

namespace ESPtools {
namespace Display {

bool lcdBegin() {
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.setBacklight(LCD_BACKLIGHT);
  lcd.clear();
  return true;
}

void writeLine(uint8_t line, const String &text) {
  String padded = text;
  int padCount = (int)LCD_COLS - (int)padded.length();
  if (padCount > 0) {
    for (int i = 0; i < padCount; ++i) {
      padded += ' ';
    }
  }
  lcd.setCursor(0, line);
  lcd.print(padded);
}

void clear() {
  lcd.clear();
}

void backlight(bool on) {
  lcd.setBacklight(on ? LCD_BACKLIGHT : 0);
}

void scrollLine(uint8_t row, const String& text, uint16_t speedMs) {
  if (row >= LCD_ROWS || text.length() <= LCD_COLS) {
    writeLine(row, text);
    return;
  }

  for (size_t i = 0; i <= text.length() - LCD_COLS; ++i) {
    lcd.setCursor(0, row);
    lcd.print(text.substring(i, i + LCD_COLS));
    delay(speedMs);
  }
}

void writeCentered(uint8_t line, const String &text) {
  int total = LCD_COLS;
  int tlen  = (int)text.length();
  int left  = (total - tlen) / 2;
  if (left < 0) left = 0;
  int right = total - left - tlen;
  if (right < 0) right = 0;

  String lineStr;
  for (int i = 0; i < left; ++i)  lineStr += ' ';
  lineStr += text;
  for (int i = 0; i < right; ++i) lineStr += ' ';

  lcd.setCursor(0, line);
  lcd.print(lineStr);
}

void setCursorVisible(bool visible) {
  if (visible) lcd.cursor();
  else lcd.noCursor();
}

void setCursorBlinking(bool blinking) {
  if (blinking) lcd.blink();
  else lcd.noBlink();
}

void drawProgressBar(uint8_t row, uint8_t percent) {
  if (row >= LCD_ROWS || percent > 100) return;

  lcd.setCursor(0, row);

  const uint8_t totalBlocks = LCD_COLS;
  uint8_t filledBlocks = (percent * totalBlocks) / 100;

  String bar = "";
  for (uint8_t i = 0; i < totalBlocks; ++i) {
    bar += (i < filledBlocks) ? char(255) : '-';
  }

  lcd.print(bar);
}

}
}