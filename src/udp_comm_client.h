#pragma once
void udpBegin();
void udpListen();         // in loop() aufrufen
void udpSendHello();    // um Temp-Pull zu triggern
void checkTempOutOfDate();
