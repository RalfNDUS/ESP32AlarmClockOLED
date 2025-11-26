#include "udp_comm.h"
// #include <Arduino.h>
// #include <WiFiUdp.h>
// #include <ArduinoJson.h>
// #include "global.h"

// static WiFiUDP udp;
// static constexpr uint16_t UDP_PORT = 5005;
// static constexpr unsigned long UDP_SEND_INTERVALL_MS = 30000;
// static constexpr unsigned long UDP_SHOW_INDICATOR_MS = 500;

// unsigned long nextUdpPacketSend = 0;

// void udpBegin() {
//     udp.begin(0);          // Source-Port beliebig
//     udp.begin(UDP_PORT);   // zusätzlich Zielport lauschen
// }

// void sendUdpPacket() {
//     auto& s = appState();
//     if (s.udpActive || isnan(s.outdoorTemp)) return;

//     s.udpActive = true;
//     nextUdpPacketSend = millis() + UDP_SEND_INTERVALL_MS;
//     s.nextUdpSendIndicatorHide = millis() + UDP_SHOW_INDICATOR_MS;

//     char buf[32];
//     JsonDocument jsonDoc;
//     jsonDoc["tempC"] = s.outdoorTemp;
//     size_t len = serializeJson(jsonDoc, buf, sizeof(buf));

//     udp.beginPacket(IPAddress(255,255,255,255), UDP_PORT);
//     udp.write((uint8_t*)buf, len);
//     udp.endPacket();
//     Serial.printf("%09lu: Broadcast: %s\n", millis(), buf);
// }

// void maybeSendUdpPacket() {
//     //auto& s = appState();
//     if (millis() < nextUdpPacketSend) return;
//     sendUdpPacket();
// }

// void udpListen() {
//   int packetSize = udp.parsePacket();
//   if (packetSize <= 0) return;

//   char buf[64];
//   int n = udp.read(buf, sizeof(buf) - 1);
//   if (n < 0) return;
//   buf[n] = '\0';

//   if (strcmp(buf, "hello") == 0) {
//     Serial.printf("%09lu: Received 'hello' from %s\n", millis(), udp.remoteIP().toString().c_str());
//     sendUdpPacket(); // auf hello antworten
//   }
// }
