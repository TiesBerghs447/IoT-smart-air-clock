#ifndef API_H
#define API_H
#include <Arduino.h>

#include "config.h"

bool downloadConfig();

bool uploadMeasurement(
    float temperature,
    float humidity,
    float pressure,
    int co2,
    int light,
    bool presence
);

bool downloadLedState(
    String& mode,
    int& red,
    int& green,
    int& blue
);

bool deleteAlarmServer(int id);


#endif