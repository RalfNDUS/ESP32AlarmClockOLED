#include <Arduino.h>
#include "oled.h"
#include <Wire.h>
#include <U8g2lib.h>
#include <Ticker.h>
#include <cmath>
#include "main.h"
#include "global.h"
#include "timeHelper.h"

#ifdef SH1106_OLED
  static U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(OLED_ROTATION, U8X8_PIN_NONE);
#else
  static U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(OLED_ROTATION, U8X8_PIN_NONE);
#endif

static U8G2LOG u8log;
static const uint8_t U8LOG_WIDTH=32, U8LOG_HEIGHT=10;
static uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];

static constexpr unsigned long DISPLAY_REFRESH_MS = 50;

// // Minuten (0..1439) aus Stundenbruch
// static int hmToMinutes(double h) {
//   if (isnan(h)) return -1;
//   int hh = (int)floor(h);
//   int mm = (int)floor((h - hh) * 60.0 + 0.5);
//   return hh*60 + (mm%60);
// }

void drawDayNightIcon(int x, int y, bool dayIcon) {
  u8g2.setFont(u8g2_font_open_iconic_weather_2x_t); // 32x32 px
  u8g2.setFontMode(1); // transparent

  const uint16_t GLYPH_SUN  = 69;
  const uint16_t GLYPH_MOON = 66;

  // y ist Baseline → +32, um oben auszurichten
  //int yPos = (dayIcon) ? 22 : 32;
  u8g2.drawGlyph(110, 18, dayIcon ? GLYPH_SUN : GLYPH_MOON);
}

void drawAlarmIcon(const int num) {
  u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t); // 32x32 px
  u8g2.setFontMode(1); // transparent

  const uint16_t GLYPH_ALARM  = 65;
  int yPos = (num == 1) ? 51 : 63;
  // y ist Baseline → +32, um oben auszurichten
  u8g2.drawGlyph(86, yPos, GLYPH_ALARM);
}

void drawAlarmTime(const int alarmNo, const char* alarmTime) {
  u8g2.setFont(u8g2_font_6x12_mf);
  u8g2.setFontMode(1); // transparent
  int yPos = (alarmNo == 1) ? 50 : 62;
  u8g2.drawStr(96, yPos, alarmTime);
}

void logPrint(const String& msg) {
    u8log.print(msg);
}
void logPrintln(const String& msg) {
    u8log.println(msg);
}

void initOLED() {
  DEBUG_PRINT("Initialize OLED-Display ... ");

  Wire.begin(I2C_SDA, I2C_SCL, 400000);
  u8g2.begin();
  u8g2.setFont(u8g2_font_tinyunicode_tr);
  u8log.begin(u8g2, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8log.setLineHeightOffset(0);
  u8log.setRedrawMode(0);

  DEBUG_PRINTLN("done.");
}

void refreshOLED() {
  auto& s = appState();

  if (millis() < s.nextDisplayRefresh) return;

  char line[24];
  int xPos = 0;

  snprintf(line, sizeof(line), "%.1f%cC", s.outdoorTemp, 0xB0);
  xPos = (s.outdoorTemp > 0.0 && s.outdoorTemp < 10.0) ? 30 : 22;

  uint8_t contrast = (s.isDaytime) ? 255 : 1;
  u8g2.setContrast(contrast);

  u8g2.firstPage();
  do {

    u8g2.setFont(u8g2_font_logisoso30_tf);
    u8g2.drawStr(14, 32, s.currentTimeString);
    u8g2.setFont(u8g2_font_logisoso18_tf);
    u8g2.drawStr(xPos, 62, line);
    //if (s.udpActive) u8g2.drawDisc(3, 3, 2, U8G2_DRAW_ALL);

    if (millis() < s.nextUdpSendIndicatorHide) {
      u8g2.drawDisc(3, 3, 2, U8G2_DRAW_ALL);
      s.updIndicatorCycles--;
    }
    drawDayNightIcon(110, 27, s.isDaytime);
    drawAlarmTime(1, s.alarm1TimeStr.c_str());
    drawAlarmTime(2, s.alarm2TimeStr.c_str());
    if (s.alarm1Enabled) drawAlarmIcon(1);
    if (s.alarm2Enabled) drawAlarmIcon(2);

  } while (u8g2.nextPage());

  s.nextDisplayRefresh = millis() + DISPLAY_REFRESH_MS;
}
