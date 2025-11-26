#include <WiFi.h>
#include <ESPmDNS.h>
#include "global.h"
#include "main.h"

#ifdef ESP32_C3_MINI
  #include <esp_wifi.h>
#endif

void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) 
{
  DEBUG_PRINT("[WiFi-event] ");
  switch (event) {
    case ARDUINO_EVENT_WIFI_READY:               DEBUG_PRINTLN("WiFi interface ready"); break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:           DEBUG_PRINTLN("Completed scan for access points"); break;
    case ARDUINO_EVENT_WIFI_STA_START:           DEBUG_PRINTLN("WiFi client started"); break;
    case ARDUINO_EVENT_WIFI_STA_STOP:            DEBUG_PRINTLN("WiFi clients stopped"); break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:       DEBUG_PRINTLN("Connected to access point"); break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:    
      // DEBUG_PRINT("Disconnected from WiFi access point. Reason: "); 
      // DEBUG_PRINT(info.wifi_sta_disconnected.reason);
      // DEBUG_PRINTLN(" -> Trying to Reconnect ...");
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE: DEBUG_PRINTLN("Authentication mode of access point has changed"); break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      DEBUG_PRINTF("Got IP-Address: ");
      DEBUG_PRINTLN(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:        DEBUG_PRINTLN("Lost IP address and IP address is reset to 0"); break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:          DEBUG_PRINTLN("WiFi Protected Setup (WPS): succeeded in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_FAILED:           DEBUG_PRINTLN("WiFi Protected Setup (WPS): failed in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:          DEBUG_PRINTLN("WiFi Protected Setup (WPS): timeout in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_PIN:              DEBUG_PRINTLN("WiFi Protected Setup (WPS): pin code in enrollee mode"); break;
    case ARDUINO_EVENT_WIFI_AP_START:           DEBUG_PRINTLN("WiFi access point started"); break;
    case ARDUINO_EVENT_WIFI_AP_STOP:            DEBUG_PRINTLN("WiFi access point  stopped"); break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:    DEBUG_PRINTLN("Client connected"); break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED: DEBUG_PRINTLN("Client disconnected"); break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:   DEBUG_PRINTLN("Assigned IP address to client"); break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:  DEBUG_PRINTLN("Received probe request"); break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:         DEBUG_PRINTLN("AP IPv6 is preferred"); break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:        DEBUG_PRINTLN("STA IPv6 is preferred"); break;
    case ARDUINO_EVENT_ETH_GOT_IP6:             DEBUG_PRINTLN("Ethernet IPv6 is preferred"); break;
    case ARDUINO_EVENT_ETH_START:               DEBUG_PRINTLN("Ethernet started"); break;
    case ARDUINO_EVENT_ETH_STOP:                DEBUG_PRINTLN("Ethernet stopped"); break;
    case ARDUINO_EVENT_ETH_CONNECTED:           DEBUG_PRINTLN("Ethernet connected"); break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:        DEBUG_PRINTLN("Ethernet disconnected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP:              DEBUG_PRINTLN("Obtained IP address"); break;
    default:                                    break;
  }
}

void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
#ifdef ESP32_C3_MINI
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
#endif
  //WiFi.onEvent(onWiFiEvent);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  DEBUG_PRINTF("Connecting %s ...\n", WIFI_SSID);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 30000)
    delay(500);

  if (WiFi.status() == WL_CONNECTED)
  {
    DEBUG_PRINTLN("Connected to WiFi AP");
    DEBUG_PRINT("IP: ");
    DEBUG_PRINTLN(WiFi.localIP().toString());
  }
  else
  { 
    DEBUG_PRINTLN("Connection failed!. Restart in 3 sec ...");
    delay(3000);
    ESP.restart();
  }
  if (MDNS.begin(MDNS_NAME))
  {
    DEBUG_PRINTF("mDNS aktiv: http://%s.local/\n", MDNS_NAME);
  }
}
