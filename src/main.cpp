#include "main.h"
#include <Arduino.h>
#include "alarm.h"
#include "oled.h"
#include "global.h"
#include "wifi_mgr.h"
#include "time_sync.h"
#include "udp_comm_client.h"
#include "web_server.h"

#define APP_DEBUG

void setup()
{
  delay(1000);
  #ifdef APP_DEBUG
    // Serial.begin(115200); 
    // while (!Serial)
    //   delay(10);
    // DEBUG_PRINTLN("ready.");
  #endif
  delay(2000);
  initBuzzer();
  loadState();
  initOLED();
  // initDisplay();

  // writeStr(tDate, "Set WiFi...");
  wifiConnect();
  // writeStr(tDate, "Get time...");
  timeSetupOrDie();
  // writeStr(tDate, "Start Web..");
  webServerStart();
  udpBegin();  
  //showButtons(true);
}

void loop()
{
  //myNex.NextionListen(); // This function must be called repeatedly to response touch events
                         // from Nextion touch panel. Actually, you should place it in your loop function.
  refreshTime();
  handleAlarm();
  refreshOLED();
  udpListen();
  checkTempOutOfDate();
}
