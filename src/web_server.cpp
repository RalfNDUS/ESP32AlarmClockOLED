#include <ESPAsyncWebServer.h>
#include <Arduino.h>
#include "main.h"
#include "web_ui.h"
#include "global.h"
#include "alarm.h"

static AsyncWebServer server(80);

void webServerStart() {
  DEBUG_PRINT("Starting Web Server ... ");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
    DEBUG_PRINTLN("Web Server request received: GET /");
    req->send(200, "text/html; charset=utf-8", htmlIndex());
  });

  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request) {
    DEBUG_PRINTLN("Web Server request received: GET /test");
    auto& s = appState();
    if (!s.alarmTriggered) {
      s.alarmTriggered = true;  
    }
    request->redirect("/");
  });

  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request) {
    DEBUG_PRINTLN("Web Server request received: GET /stop");
    endAlarm();
    request->redirect("/");
  });

  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request) {
    DEBUG_PRINTLN("Web Server request received: GET /set");
    auto& s = appState();

    int a1h = s.alarm1Time / 100;
    int a1m  = s.alarm1Time % 100;
    int a2h = s.alarm2Time / 100;
    int a2m  = s.alarm2Time % 100;
    if (request->hasParam("a1h")) a1h = constrain(request->getParam("a1h")->value().toInt(), 0, 23);
    if (request->hasParam("a1m")) a1m = constrain(request->getParam("a1m")->value().toInt(), 0, 59);
    if (request->hasParam("a2h")) a2h = constrain(request->getParam("a2h")->value().toInt(), 0, 23);
    if (request->hasParam("a2m")) a2m = constrain(request->getParam("a2m")->value().toInt(), 0, 59);
    bool a1e = request->hasParam("a1e");
    bool a2e = request->hasParam("a2e");

    if ((a1h >= 0 && a1h < 24) && (a1m >= 0 && a1m < 60) && (a2h >= 0 && a2h < 24) && (a2m >= 0 && a2m < 60))
    {
      s.alarm1Time = a1h * 100 + a1m;
      s.alarm1Enabled = (a1e) ? true : false;
      s.alarm2Time = a2h * 100 + a2m;
      s.alarm2Enabled = (a2e) ? true : false;
      saveState();

      DEBUG_PRINTF("Alarm 1: %02d:%02d %d\tAlarm 2: %02d:%02d %d\n", a1h, a1m, a1e, a2h, a2m, a2e);
      DEBUG_PRINTF("Alarm1: %d\t", s.alarm1Time);
      DEBUG_PRINTF("Alarm2: %d\t", s.alarm2Time);
      DEBUG_PRINTF("lastTriggeredAlarm: %d\n", s.lastTriggeredAlarm);
    }
    request->redirect("/");
  });

  server.begin();
  DEBUG_PRINTLN("done.");
}
