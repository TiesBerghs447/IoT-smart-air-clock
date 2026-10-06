#ifndef GLOBALS_H
#define GLOBALS_H

struct SensorData
{
    float temperature;
    float humidity;
    float pressure;
    int co2;
};

extern SensorData sensorData;

#endif