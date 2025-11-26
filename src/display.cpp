// display.cpp
#include "main.h"
#include "global.h"
#include "display.h"
#include <EasyNextionLibrary.h> 

#define NEXTION_BAUD 19200

static constexpr unsigned long DISPLAY_REFRESH_MS = 100;

// Brightness levels
static constexpr uint8_t BRIGHTNESS_LEVELS[] = {60, 30, 15, 5, 1, 0};
static constexpr size_t NUM_LEVELS = sizeof(BRIGHTNESS_LEVELS) / sizeof(BRIGHTNESS_LEVELS[0]);

EasyNex myNex(Serial2);  // das echte Objekt

const byte mainPage = 0;
const byte settingsPage = 1;

const char* const tTime = "t0.txt";
const char* const tDate = "t1.txt";
const char* const tTemp = "t2.txt";
const char* const tAlarm1 = "t3.txt";
const char* const tAlarm2 = "t4.txt";
const char* const tSetError = "t4.txt";
const char* const iconWiFi = "p0";
const char* const iconDaytime = "p1";
const char* const iconNighttime = "p2";
const char* const activeAlarm1 = "p3";
const char* const activeAlarm2 = "p4";
const char* const btnSet = "b0";
const char* const btnDimPlus = "b1";
const char* const btnDimMinus = "b2";
const char* const nAlarm1HH = "n0.val";
const char* const nAlarm1MM = "n1.val";
const char* const nAlarm1Active = "c0.val";
const char* const nAlarm2HH = "n2.val";
const char* const nAlarm2MM = "n3.val";
const char* const nAlarm2Active = "c1.val";

void setDisplayPage(uint8_t page) {
    auto& s = appState();
    char cmd[10]; 
    snprintf(cmd, sizeof(cmd), "page %d", page);
    myNex.writeStr(cmd); 
    s.nextDisplayRefresh = 0;   // force display update
    s.currentPage = page;
}

void invertDisplayPage0(bool inverted) {
    int bco = (inverted) ? 65535 : 0;
    int pco = (inverted) ? 31 : 31;

    myNex.writeNum("page0.bco", bco);
    myNex.writeNum("t0.bco", bco);
    myNex.writeNum("t1.bco", bco);
    myNex.writeNum("t2.bco", bco);
    myNex.writeNum("t3.bco", bco);
    myNex.writeNum("t4.bco", bco);
    myNex.writeNum("t4.pco", pco);
    myNex.writeNum("b0.bco", bco);
    myNex.writeNum("b1.bco", bco);
    myNex.writeNum("b2.bco", bco);
}

void setBrightnessIndex(uint8_t brightnessIndex) {
    uint8_t brightness = BRIGHTNESS_LEVELS[brightnessIndex];
    char cmd[10]; 
    snprintf(cmd, sizeof(cmd), "dim=%d", brightness);
    myNex.writeStr(cmd); 
}

void incBrightness() {
    auto& s = appState();
    int brightnessIndex = s.brightnessIndex - 1;
    if (brightnessIndex < 0) brightnessIndex = 0;
    s.brightnessIndex = brightnessIndex;
    setBrightnessIndex(brightnessIndex);
    saveState();
}

void decBrightness() {
    auto& s = appState();
    int brightnessIndex = s.brightnessIndex + 1;
    if (brightnessIndex > NUM_LEVELS - 1) brightnessIndex = NUM_LEVELS - 1;
    s.brightnessIndex = brightnessIndex;
    setBrightnessIndex(brightnessIndex);
    saveState();
}

void setVisibility(const char* element, bool visible) {
    String cmd = "vis ";
    cmd += element;
    cmd += ",";
    cmd += (visible ? "1" : "0");
    myNex.writeStr(cmd.c_str());
}

void showMessage(String msg) {
    myNex.writeStr(tDate, msg);
}

void writeStr(const char* element, String str) {
    myNex.writeStr(element, str);
}

void writeNum(const char* element, int num) {
    myNex.writeNum(element, num);
}

void showButtons(bool visible) {
    setVisibility(btnSet, visible);      // hide set button
    setVisibility(btnDimPlus, visible);  // hide dim+ button
    setVisibility(btnDimMinus, visible); // hide dim- button
}

void initDisplay() {
    DEBUG_PRINT("Initialize Nextion-Display ... ");

    myNex.begin(NEXTION_BAUD);    // Begin the object, default 9600
    delay(1000);            // wait for display startup
    setDisplayPage(mainPage);
    setBrightnessIndex(appState().brightnessIndex);   // max. brightness
    showButtons(false);
    myNex.writeStr(tTime, "XXXX");
    myNex.writeStr(tTemp, "XXXXX");
    invertDisplayPage0(false);

    DEBUG_PRINTLN("done.");
}

void refreshDisplay() {
    auto& s = appState();

    if (millis() < s.nextDisplayRefresh) return;
    
    if (s.alarmTriggered)
        myNex.writeStr("dim=100");
    else
        setBrightnessIndex(s.brightnessIndex);

    if (s.currentPage == 0) {
        //DEBUG_PRINT("Display refresh ... ");
        if (s.isDaytime) {
            setVisibility(iconDaytime, true);
            setVisibility(iconNighttime, false);
        } else {
            setVisibility(iconDaytime, false);
            setVisibility(iconNighttime, true);
        }
        myNex.writeStr(tTime, s.currentTimeString);
        myNex.writeStr(tDate, s.currentDateString);
        myNex.writeStr(tTemp, s.outdoorTempString);
        myNex.writeStr(tAlarm1, s.alarm1TimeStr);
        myNex.writeStr(tAlarm2, s.alarm2TimeStr);
        setVisibility(activeAlarm1, s.alarm1Enabled);
        setVisibility(activeAlarm2, s.alarm2Enabled);

        if (s.updIndicatorCycles > 0) {
            setVisibility(iconWiFi, true);
            s.updIndicatorCycles--;
        } else
            setVisibility(iconWiFi, false);
    }
    s.nextDisplayRefresh = millis() + DISPLAY_REFRESH_MS;
}

void forceDisplayRefresh() {
    auto& s = appState();
    s.nextDisplayRefresh = 0;
}