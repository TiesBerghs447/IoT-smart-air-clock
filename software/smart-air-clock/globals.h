#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

typedef struct
{
    float temperature;
    float humidity;
    float pressure;
    uint16_t co2;
    int light;
    bool presence;
} SensorData;

extern SensorData sensorData;

#pragma once

enum PowerState
{
    POWER_ON,
    POWER_DISPLAY_OFF,
    POWER_OFF
};

extern PowerState powerState;

#endif