#ifndef API_H
#define API_H

#include "config.h"

bool downloadConfig();
bool uploadMeasurement(
    float temperature,
    float humidity,
    float pressure,
    int co2
);

#endif