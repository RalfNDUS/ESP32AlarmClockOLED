#include "main.h"
#include <Arduino.h>
#include "global.h"
#include <InterruptButton.h>

InterruptButton btn(BUTTON_PIN, LOW);

unsigned long nextBuzzerToogle = 0;
//bool displayInverted = false;
//unsigned long invertDisplayTicks = 0;

void endAlarm() {
  auto& s = appState();
  s.alarmTriggered = false;
  digitalWrite(BUZZER_PIN, LOW);
//  invertDisplayPage0(false);
//  displayInverted = false;
//  invertDisplayTicks = 0;
  DEBUG_PRINTF_P(PSTR("Alarm suspended.\n"));
}

void toogleBuzzer() {
    bool status = !digitalRead(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, status);
    // if (invertDisplayTicks == 0) {
    //   displayInverted = !displayInverted;
    //   invertDisplayPage0(displayInverted);
    //   invertDisplayTicks = 6;
    // } else {
    //   invertDisplayTicks--;
    // } 
}

void initBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  DEBUG_PRINTF_P(PSTR("Buzzer on GPIO%d activated.\n"), BUZZER_PIN);
  // Button
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

    if (digitalRead(BUTTON_PIN) == LOW) {
      // press button to end alarm
      DEBUG_PRINTLN("End Alarm.");
      endAlarm();
    } else if (millis() > nextBuzzerToogle) {
      toogleBuzzer();
      nextBuzzerToogle = millis() + (random(10, 30) * 10);
    }
  }
}

