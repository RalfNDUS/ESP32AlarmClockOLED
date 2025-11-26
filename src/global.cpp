#include "main.h"
#include "global.h"
#include <Preferences.h>

static AppState _state;
AppState& appState() { return _state; }

static Preferences prefs;

String formatTime(int hhmm) {
    int h = hhmm / 100;
    int m = hhmm % 100;

    char buffer[6];          // "HH:MM"
    sprintf(buffer, "%02d:%02d", h, m);

    return String(buffer);
}

void loadState() {
  auto& s = _state;
  prefs.begin("state", true);
  s.brightnessIndex = prefs.getInt("dim", s.brightnessIndex);
  s.alarm1Time = prefs.getInt("a1_t", s.alarm1Time);
  s.alarm1TimeStr = formatTime(s.alarm1Time);
  s.alarm1Enabled = prefs.getBool("a1_e", s.alarm1Enabled);
  s.alarm2Time  = prefs.getInt("a2_t", s.alarm2Time);
  s.alarm2TimeStr = formatTime(s.alarm2Time);
  s.alarm2Enabled= prefs.getBool("a2_e", s.alarm2Enabled);
  prefs.end();
  DEBUG_PRINTF_P(PSTR("Preferences loaded from NVS.\n"));
}

void saveState() {
  auto& s = _state;
  prefs.begin("state", false);
  prefs.putInt ("dim", s.brightnessIndex);
  prefs.putInt ("a1_t", s.alarm1Time);
  prefs.putBool("a1_e", s.alarm1Enabled);
  prefs.putInt ("a2_t", s.alarm2Time);
  prefs.putBool("a2_e", s.alarm2Enabled);
  prefs.end();
  DEBUG_PRINTF_P(PSTR("Preferences saved in NVS.\n"));

  // read after save
  loadState();
}