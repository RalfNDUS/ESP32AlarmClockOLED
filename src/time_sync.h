#pragma once
void timeSetupOrDie();      // initialer Sync (reboot on fail)
//void scheduleTimeRefresh(); // Ticker setzt für periodisches setup
bool checkTimeUpdateRequired(); // get every hour a NTP time update
void refreshTime();