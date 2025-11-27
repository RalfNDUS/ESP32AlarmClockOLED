#include <Arduino.h>
#include "main.h"
#include <time.h>
#include "global.h"
#include "time_helper.h"

static constexpr unsigned long TIME_REFRESH_MS = 50;

static void ntpSetupOnce() {
  auto& s = appState();

  struct tm lt;
  configTzTime(TZ_EU_BERLIN, "pool.ntp.org", "time.cloudflare.com", "time.google.com");
  bool ok = false;
  for (int i=0; i<20 && !(ok = getLocalTime(&lt, 200)); ++i) {
    delay(200);
  }
  if (!ok) {
    DEBUG_PRINTLN("Set local time failed! Restart in 3 sec ...");
    delay(3000);
    ESP.restart();
  }

  // Save current hour for triggering next update
  s.lastUpdateHour = lt.tm_hour;

  DEBUG_PRINTF("Successful setting local time. (%0d)\n", s.lastUpdateHour);
}

void timeSetupOrDie() {
  ntpSetupOnce();
} 

// void scheduleTimeRefresh() {
//   ntpTicker.detach();
//   ntpTicker.attach(NTP_REFRESH_SEC, ntpSetupOnce);
// }

bool checkTimeUpdateRequired() {
  auto& s = appState();
  time_t now = time(nullptr);

  struct tm lt;
  localtime_r(&now, &lt);
  if (lt.tm_hour != s.lastUpdateHour) {
    s.timeUpdateRequired = true;
    return true;
  } else 
    return false;
}


void refreshTime() {
  auto& s = appState();
  if (millis() < s.nextTimeRefresh ) return;

  time_t now = time(nullptr);
  struct tm lt;
  localtime_r(&now, &lt);
  s.isDaytime = isDaytime(lt);

  s.currentTime = lt.tm_hour * 100 + lt.tm_min;
  snprintf(s.currentTimeString, sizeof(s.currentTimeString),
                  "%02d:%02d", lt.tm_hour, lt.tm_min);
  snprintf(s.currentDateString, sizeof(s.currentDateString),
                  "%02d.%02d.%04d", lt.tm_mday, lt.tm_mon + 1, lt.tm_year + 1900);   
                  
  s.nextTimeRefresh = millis() + TIME_REFRESH_MS;
}
