#include "sensors.h"
#include "globals.h"

float temperature = 0;
float humidity = 0;
float pressure = 0;
int co2ppm = 0;

void initSensors()
{
    // later:
    // AHT20
    // BMP280
    // MH-Z19B
}

void readSensors()
{
    // tijdelijke testwaarden

    temperature = 22.5;
    humidity = 48;
    pressure = 1013;
    co2ppm = 720;
}