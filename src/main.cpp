#include "main.h"
#include <Arduino.h>
#include "alarm.h"
#include "oled.h"
#include "global.h"
#include "wifi_mgr.h"
#include "time_sync.h"
#include "udp_comm_client.h"
#include "web_server.h"

void setup()
{
  delay(1000);
  #ifdef APP_DEBUG
    Serial.begin(115200); 
    while (!Serial)
      delay(10);
    DEBUG_PRINTLN("ready.");
  #endif
  delay(2000);
  initBuzzer();
  loadState();
  initOLED();
  wifiConnect();
  timeSetupOrDie();
  webServerStart();
  udpBegin();  
}

void loop()
{
  refreshTime();
  handleAlarm();
  refreshOLED();
  udpListen();
  checkTempOutOfDate();
}
