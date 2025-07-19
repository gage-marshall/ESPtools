#ifndef LCD_H
#define LCD_H

#include <Arduino.h>

namespace ESPtools {
namespace Display {

bool lcdBegin();
void writeLine(uint8_t row, const String& text);
void clear();
void backlight(bool on);

void scrollLine(uint8_t row, const String& text, uint16_t speedMs = 300);
void writeCentered(uint8_t row, const String& text);
void setCursorVisible(bool visible);
void setCursorBlinking(bool blinking);
void drawProgressBar(uint8_t row, uint8_t percent);

}
}

#endif
