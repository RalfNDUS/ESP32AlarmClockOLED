#pragma once

#ifdef OLED_ROTATED
  #define OLED_ROTATION U8G2_R2
#else 
  #define OLED_ROTATION U8G2_R0
#endif

void logPrint(const String& msg);
void logPrintln(const String& msg);
void initOLED();
void refreshOLED();
