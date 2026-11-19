#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "alarm.h"

void initDisplay();

void updateDisplay(
    float temperature,
    float humidity,
    int co2,
    int light
);

void updateAlarmDisplay();

void showBootMessage(const String& message);

void clearDisplay();

void setDisplayContrast(
    uint8_t contrast
);

#endif