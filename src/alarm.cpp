#include "main.h"
#include <Arduino.h>
#include "global.h"

unsigned long nextBuzzerToogle = 0;

void endAlarm() {
  auto& s = appState();
  s.alarmTriggered = false;
  digitalWrite(BUZZER_PIN, LOW);
  DEBUG_PRINTF_P(PSTR("Alarm suspended.\n"));
}

void toogleBuzzer() {
    bool status = !digitalRead(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, status);
}

void initBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  DEBUG_PRINTF_P(PSTR("Buzzer on GPIO%d activated.\n"), BUZZER_PIN);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  DEBUG_PRINTF_P(PSTR("Button bound to GPIO%d.\n"), BUTTON_PIN);
}

void handleAlarm() {
  auto& s = appState();

  // trigger alarm when current time equals alarm1 or alarm2
  if (!s.alarmTriggered && s.currentTime != s.lastTriggeredAlarm) {

    if ((s.alarm1Enabled && s.currentTime == s.alarm1Time) ||  
      (s.alarm2Enabled && s.currentTime == s.alarm2Time)) {
        s.alarmTriggered = true;  
        s.lastTriggeredAlarm = s.currentTime;
        DEBUG_PRINTF("lastTriggeredAlarm=%d\n", s.lastTriggeredAlarm);
    }
  }  

  // Handle buzzer on active alarm
  if (s.alarmTriggered){

    // press button to end alarm
    if (digitalRead(BUTTON_PIN) == LOW) {
      DEBUG_PRINTLN("End Alarm.");
      endAlarm();
    } else if (millis() > nextBuzzerToogle) {
      toogleBuzzer();
      nextBuzzerToogle = millis() + (random(10, 30) * 10);
    }
  }
}

