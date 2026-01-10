#include "Defines.hpp"
#include "Backlight.hpp"
#include <Arduino.h>

void backlight_init() {
  ledcAttachChannel(LCD_BL, LCD_BL_FREQ, LCD_BL_RESOLUTION, 8);
  ledcWrite(LCD_BL, LCD_BL_MAX);
}

void set_backlight_level(int level) {
  ledcWrite(LCD_BL, level);
}
