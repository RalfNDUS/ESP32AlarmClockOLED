#include "main.h"
#include <Arduino.h>
#include "alarm.h"
#include "display.h"
#include "global.h"
#include "wifi_mgr.h"
#include "time_sync.h"
#include "udp_comm_client.h"
#include "web_server.h"

void setup()
{
  #ifdef APP_DEBUG
    Serial.begin(115200); 
    while (!Serial)
      delay(10);
    DEBUG_PRINTLN("ready.");
  #endif
  
  initBuzzer();
  loadState();
  initDisplay();

  writeStr(tDate, "Set WiFi...");
  wifiConnect();
  writeStr(tDate, "Get time...");
  timeSetupOrDie();
  writeStr(tDate, "Start Web..");
  webServerStart();
  udpBegin();  
  showButtons(true);
}

void loop()
{
  myNex.NextionListen(); // This function must be called repeatedly to response touch events
                         // from Nextion touch panel. Actually, you should place it in your loop function.
  refreshTime();
  handleAlarm();
  refreshDisplay();
  udpListen();
  checkTempOutOfDate();
}

// set button
void trigger0()
{
  auto &s = appState();
  DEBUG_PRINT("Button Set pressed: ");

  if (!s.alarmTriggered) {
    DEBUG_PRINTLN("Going to settings page. ");
    int alarm1_hh = s.alarm1Time / 100;
    int alarm1_mm = s.alarm1Time % 100;
    int alarm1_enabled = (s.alarm1Enabled == 1) ? 1 : 0;
    int alarm2_hh = s.alarm2Time / 100;
    int alarm2_mm = s.alarm2Time % 100;
    int alarm2_enabled = (s.alarm2Enabled == 1) ? 1 : 0;

    setDisplayPage(settingsPage);
    writeNum(nAlarm1HH, alarm1_hh);
    writeNum(nAlarm1MM, alarm1_mm);
    writeNum(nAlarm1Active, alarm1_enabled);
    writeNum(nAlarm2HH, alarm2_hh);
    writeNum(nAlarm2MM, alarm2_mm);
    writeNum(nAlarm2Active, alarm2_enabled);
  } else {
    DEBUG_PRINTLN("End Alarm.");
    endAlarm();
  }
}

// dim+ button
void trigger1()
{
  incBrightness();
}

// dim- button
void trigger2()
{
  decBrightness();
}

// save button
void trigger3()
{
  auto &s = appState();

  int alarm1_hh = myNex.readNumber(nAlarm1HH);
  int alarm1_mm = myNex.readNumber(nAlarm1MM);
  int alarm1_active = myNex.readNumber(nAlarm1Active);
  int alarm2_hh = myNex.readNumber(nAlarm2HH);
  int alarm2_mm = myNex.readNumber(nAlarm2MM);
  int alarm2_active = myNex.readNumber(nAlarm2Active);

  if ((alarm1_hh >= 0 && alarm1_hh < 24) && (alarm1_mm >= 0 && alarm1_mm < 60) && (alarm2_hh >= 0 && alarm2_hh < 24) && (alarm2_mm >= 0 && alarm2_mm < 60))
  {
    s.alarm1Time = alarm1_hh * 100 + alarm1_mm;
    s.alarm1Enabled = (alarm1_active == 1) ? true : false;
    s.alarm2Time = alarm2_hh * 100 + alarm2_mm;
    s.alarm2Enabled = (alarm2_active == 1) ? true : false;
    saveState();

    DEBUG_PRINTF("Alarm 1: %02d:%02d %d\tAlarm 2: %02d:%02d %d\n", alarm1_hh, alarm1_mm, alarm1_active, alarm2_hh, alarm2_mm, alarm2_active);
    DEBUG_PRINTF("currentTime: %d\t", s.currentTime);
    DEBUG_PRINTF("Alarm1: %d\t", s.alarm1Time);
    DEBUG_PRINTF("Alarm2: %d\t", s.alarm2Time);
    DEBUG_PRINTF("lastTriggeredAlarm: %d\n", s.lastTriggeredAlarm);
    setDisplayPage(mainPage);
    forceDisplayRefresh();

  } else {
    writeStr(tSetError, "Invalid alarm settings!");
  }
}
