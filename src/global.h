#pragma once
#include <Arduino.h>
#include "secrets.h"
 
// ---- WLAN & Dienste ----
// use secrets.h to set WIFI_SSD and WIFI_PASSWORD
#ifndef WIFI_SSID
  #define WIFI_SSID     "<your_ssid>"
#endif
#ifndef WIFI_PASSWORD
  #define WIFI_PASSWORD "<your_wifi_password>"
#endif
#define TZ_EU_BERLIN "CET-1CEST,M3.5.0/2,M10.5.0/3"
#define MDNS_NAME    "esp32-alarmclock"

// ---- GPIO ----
constexpr int BUZZER_PIN = 3;
constexpr int BUTTON_PIN = 2;
#define I2C_SDA 8
#define I2C_SCL 9

// Zentraler, gemeinsam genutzter Zustand
struct AppState {

  // Time-related
  unsigned int currentTime = 0;   // Military format HHMM
  char currentTimeString[6];
  char currentDateString[12];
  bool isDaytime = false;
  unsigned long nextTimeRefresh = 0;
  int lastUpdateHour = -1;
  bool timeUpdateRequired = false;

  // Alarm Clock-related
  unsigned int alarm1Time = 0;
  String alarm1TimeStr = "";
  bool alarm1Enabled = false;
  unsigned int alarm2Time = 0;
  String alarm2TimeStr = "";
  bool alarm2Enabled = false;
  bool alarmTriggered = false;
  unsigned int lastTriggeredAlarm = 0;
  
  // Display-related
  unsigned long nextDisplayRefresh = 0;
  uint8_t brightnessIndex = 1;

  // Sensor-reöated
  float outdoorTemp = NAN;   // DS18B20 (Außen)
  char outdoorTempString[12];
  float outdoorTempMin = NAN;
  float outdoorTempMax = NAN;
  bool  tempReqPending = false;
  unsigned long nextSensorsPoll = 0;

  // UDP-related
  bool udpActive = false;
  unsigned long nextUdpSendIndicatorHide = 0;
  unsigned long tempWatchdogTimer = 0;

};

AppState& appState(); // Singleton-Zugriff
void loadState();
void saveState();
