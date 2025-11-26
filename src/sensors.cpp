// sensors.cpp
#include "sensors.h"

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>

#include "global.h"

#ifdef ONE_WIRE_BUS
// nur in dieser Datei sichtbar (static = interne Bindung)
static OneWire oneWire(ONE_WIRE_BUS);
static DallasTemperature ds(&oneWire);
#endif

// nur hier intern benutzt -> static
static void ds1820_show_error(int code) {
    Serial.print("[DS1820 error] ");
    switch (code) {
        case DEVICE_DISCONNECTED_C:   Serial.println("DEVICE_DISCONNECTED_C"); break;
        case DEVICE_FAULT_OPEN_C:     Serial.println("DEVICE_FAULT_OPEN_C"); break;
        case DEVICE_FAULT_SHORTGND_C: Serial.println("DEVICE_FAULT_SHORTGND_C"); break;
        case DEVICE_FAULT_SHORTVDD_C: Serial.println("DEVICE_FAULT_SHORTVDD_C"); break;
        default:                      break;
    }
}

static constexpr unsigned long TEMP_POLL_MS    = 1000;

void sensorsInit() {
    auto& s = appState();  // falls du später noch was aus s brauchst

    #ifdef ONE_WIRE_BUS
        ds.begin();
        ds.setWaitForConversion(false);
        ds.setResolution(12);
        s.deviceCountDS1820 = ds.getDeviceCount();
        Serial.printf("%d DS1820 found and initialized.\n", s.deviceCountDS1820);
    #else
        Serial.println("ONE_WIRE_BUS < 0, DS18B20 disabled.");
    #endif
}

void sensorsPoll() {
    auto& s = appState();

    #ifdef ONE_WIRE_BUS
        if (millis() < s.nextSensorsPoll) return;

        // DS18B20: nicht-blockierend
        if (!s.tempReqPending) {
            ds.requestTemperatures();
            s.tempReqPending = true;
        } else {
            float t = ds.getTempCByIndex(0);
            Serial.print("outdoorTemp="); Serial.print(t); 
            Serial.printf("\tcurrentTime=%d\t", s.currentTime);
            Serial.printf("\tlastTriggeredAlarm=%d\n", s.lastTriggeredAlarm);
            if (t > -127.0f && t < 150.0f) {
                float last = s.outdoorTemp;
                s.outdoorTemp = roundf(t * 10.f) / 10.f;
                snprintf(s.outdoorTempString, sizeof(s.outdoorTempString),
                  "%.1f\xB0""C", s.outdoorTemp);
                // s.jsonDoc["tempC"] = s.outdoorTemp;  // für UDP
                // if (s.outdoorTemp != last) udpSendPacket();  // sofortiger Push
            } else {
                ds1820_show_error(ds.getTempCByIndex(0));
                s.outdoorTemp = NAN;
                snprintf(s.outdoorTempString, sizeof(s.outdoorTempString), "n/a");
                // s.jsonDoc["tempC"] = NAN;
            }
            s.tempReqPending = false;
        }
    #endif
        s.nextSensorsPoll = millis() + TEMP_POLL_MS;
}
