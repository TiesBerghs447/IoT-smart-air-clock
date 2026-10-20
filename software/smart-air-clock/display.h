#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "alarm.h"

void initDisplay();

void updateDisplay(
    float temperature,
    float humidity,
    int co2
);

void updateAlarmDisplay();

#endif