//#define APP_DEBUG       // uncomment for serial debug

#ifdef APP_DEBUG
  #define DEBUG_PRINT(x)         Serial.print(x)
  #define DEBUG_PRINTLN(x)       Serial.println(x)
  #define DEBUG_PRINTF(x...)     Serial.printf(x)
  #define DEBUG_PRINTF_P(x...)   Serial.printf_P(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(x...)
  #define DEBUG_PRINTF_P(x...)
#endif
