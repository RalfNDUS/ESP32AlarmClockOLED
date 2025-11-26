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

  // server.on("/data", HTTP_GET, [](AsyncWebServerRequest *req){
  //   DEBUG_PRINTLN("Web Server request received: GET /data");
  //   time_t now = time(nullptr);
  //   struct tm lt;
  //   localtime_r(&now, &lt);
  //   bool valid = now > 8;

  //   String currentTime = valid ? (String(pad2(lt.tm_hour)) + ":" + pad2(lt.tm_min) + ":" + pad2(lt.tm_sec))
  //                              : String("Zeit wird synchronisiert…");
  //   String currentDate = valid ? (String(pad2(lt.tm_mday) + "." + pad2(lt.tm_mon + 1) + "." + String(1900 + lt.tm_year)))
  //                              : String("Datum wird synchronisiert…");

  //   String json = "{";
  //   json += "\"time\":\"" + currentTime + "\",";
  //   json += "\"date\":\"" + currentDate + "\",";
  //   float t = isnan(appState().outdoorTemp) ? NAN : appState().outdoorTemp;
  //   json += "\"outdoorTemp\":" + String(t, 1);

  //   #if defined(ESP32ALARMCLOCK) && !defined(NO_BME280) 
  //     json +=  + ",";
  //     float i = isnan(appState().indoorTemp) ? NAN : appState().indoorTemp;
  //     float p = isnan(appState().indoorPres) ? NAN : appState().indoorPres;
  //     json += "\"indoorTemp\":" + String(i, 1) + ",";
  //     json += "\"indoorPres\":" + String(p, 0);
  //   #endif
  //   json += "}";
  //   req->send(200, "application/json", json);
  // });

  server.begin();
  DEBUG_PRINTLN("done.");
}
