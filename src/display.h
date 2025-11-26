// display.h
#pragma once

#include <EasyNextionLibrary.h> 

extern const char* const tTime;
extern const char* const tDate;
extern const char* const tTemp;
extern const char* const tSetError;
extern const char* const btnSet;
extern const char* const btnDimPlus;
extern const char* const btnDimMinus;
extern const char* const nAlarm1HH;
extern const char* const nAlarm1MM;
extern const char* const nAlarm1Active;
extern const char* const nAlarm2HH;
extern const char* const nAlarm2MM;
extern const char* const nAlarm2Active;

extern const byte mainPage;
extern const byte settingsPage;

extern size_t brightnessIndex;
extern EasyNex myNex;

void setDisplayPage(uint8_t page);
void invertDisplayPage0(bool inverted);
void setBrightnessIndex(uint8_t brightnessIndex);
void incBrightness();
void decBrightness();
void setVisibility(const char* element, bool visible);
void showMessage(String msg);
void writeStr(const char* element, String msg);
void writeNum(const char* element, int num);
void showButtons(bool visible);
void initDisplay();
void refreshDisplay();
void forceDisplayRefresh();
