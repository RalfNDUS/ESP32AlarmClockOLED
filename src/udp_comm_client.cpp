#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "global.h"

static WiFiUDP udp;
static constexpr uint16_t UDP_PORT = 5005;
static constexpr unsigned long UDP_SHOW_INDICATOR_MS = 200;
static constexpr unsigned long TEMP_WATCHDOG_MS = 120000;

void udpSendHello() {
  const char *helloMsg = "hello";
  udp.beginPacket(IPAddress(255,255,255,255), UDP_PORT);
  udp.write((const uint8_t*)helloMsg, strlen(helloMsg));
  udp.endPacket();
  Serial.printf("%09lu: Sent UDP hello message\n", millis());
}

void udpBegin() {
  udp.begin(UDP_PORT);
}

void udpListen() {
  auto& s = appState();

  int packetSize = udp.parsePacket();
  if (packetSize <= 0) return;

  char buf[64];
  int n = udp.read(buf, sizeof(buf) - 1);
  if (n < 0) return;
  buf[n] = '\0';

  if (strcmp(buf, "hello") == 0) {
    Serial.printf("%09lu: Received 'hello' from %s\n", millis(), udp.remoteIP().toString().c_str());
    return;
  }

  Serial.printf("%09lu: Received from %s: %s\n", millis(), udp.remoteIP().toString().c_str(), buf);

  JsonDocument json;
  if (deserializeJson(json, buf) == DeserializationError::Ok) {
    s.outdoorTemp = json["tempC"] | NAN;
    snprintf(s.outdoorTempString, sizeof(s.outdoorTempString),
                  "%.1f\xB0""C", s.outdoorTemp);
    s.tempWatchdogTimer = millis() + TEMP_WATCHDOG_MS;
    //s.updIndicatorCycles = 16;
    s.nextUdpSendIndicatorHide = millis() + 400;
  }
}

void checkTempOutOfDate() {
  auto& s = appState();
  if (millis() < s.tempWatchdogTimer) return;

  s.tempWatchdogTimer = millis() + 10000;
  s.outdoorTemp = NAN;
  // force temerature update
  udpSendHello();
}

