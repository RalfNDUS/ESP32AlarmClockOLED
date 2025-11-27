#include <Arduino.h>
#include "oled.h"
#include <Wire.h>
#include <U8g2lib.h>
#include <Ticker.h>
#include <cmath>
#include "main.h"
#include "global.h"
#include "timeHelper.h"

static U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(OLED_ROTATION, U8X8_PIN_NONE);

static U8G2LOG u8log;
static const uint8_t U8LOG_WIDTH=32, U8LOG_HEIGHT=10;
static uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];

static constexpr unsigned long DISPLAY_REFRESH_MS = 50;

void logPrint(const String& msg) {
    u8log.print(msg);
}
void logPrintln(const String& msg) {
    u8log.println(msg);
}

void drawDayNightIcon(bool isDay) {
  u8g2.setFont(u8g2_font_open_iconic_weather_2x_t); // 32x32 px
  const uint16_t GLYPH_SUN  = 69;
  const uint16_t GLYPH_MOON = 66;
  u8g2.drawGlyph(110, 18, isDay ? GLYPH_SUN : GLYPH_MOON);
}

void drawAlarmIcons(const bool a1enabled, const bool a2enabled) {
  u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t); // 32x32 px
  const uint16_t GLYPH_ALARM  = 65;
  if (a1enabled) u8g2.drawGlyph(90, 51, GLYPH_ALARM);
  if (a2enabled) u8g2.drawGlyph(90, 63, GLYPH_ALARM);
}

void drawAlarmTimes(const char* alarmTime1, const char* alarmTime2) {
  u8g2.setFont(u8g2_font_finderskeepers_tn);
  int width = u8g2.getStrWidth(alarmTime1);
  int xPos = 126 - width;  
  u8g2.drawStr(xPos, 50, alarmTime1);
  u8g2.drawStr(xPos, 62, alarmTime2);
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

  //char line[24];
  int xPos = 0;

  //snprintf(line, sizeof(line), "%.1f%cC", s.outdoorTemp, 0xB0);
  xPos = (s.outdoorTemp > 0.0 && s.outdoorTemp < 10.0) ? 30 : 22;

  uint8_t contrast = (s.isDaytime) ? 255 : 1;
  u8g2.setContrast(contrast);

  u8g2.firstPage();
  do {
    // time
    u8g2.setFont(u8g2_font_logisoso30_tn);
    int width = u8g2.getStrWidth(s.currentTimeString);
    int xPos = 103 - width;
    u8g2.drawStr(xPos, 32, s.currentTimeString);

    // temperature
    u8g2.setFont(u8g2_font_logisoso18_tf);
    width = u8g2.getStrWidth(s.outdoorTempString);
    xPos = 83 - width;
    u8g2.drawStr(xPos, 62, s.outdoorTempString);
  
    // symbols
    if (millis() < s.nextUdpSendIndicatorHide) u8g2.drawDisc(3, 3, 2, U8G2_DRAW_ALL);  
    drawDayNightIcon(s.isDaytime);
    drawAlarmTimes(s.alarm1TimeStr.c_str(), s.alarm2TimeStr.c_str());
    drawAlarmIcons(s.alarm1Enabled, s.alarm2Enabled);
    if (millis() < s.nextUdpSendIndicatorHide) u8g2.drawDisc(3, 3, 2, U8G2_DRAW_ALL);  

  } while (u8g2.nextPage());

  s.nextDisplayRefresh = millis() + DISPLAY_REFRESH_MS;
}
