#include "sensors.h"
#include "globals.h"

#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

#define SDA_PIN 8
#define SCL_PIN 9

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

void initSensors()
{
    Wire.begin(SDA_PIN, SCL_PIN);

    aht.begin();
    bmp.begin(0x77);

    if (!aht.begin())
    {
        Serial.println("AHT20 NIET GEVONDEN");
        while (true)
        {
            delay(100);
        }
    }

    Serial.println("AHT20 OK");

    if (!bmp.begin(0x77))
    {
        Serial.println("BMP280 NIET GEVONDEN");
        while (true)
        {
            delay(100);
        }
    }

    Serial.println("BMP280 OK");
    Serial.println();
}

void updateSensors()
{
    sensors_event_t humidity;
    sensors_event_t temperature;

    aht.getEvent(
        &humidity,
        &temperature
    );

    sensorData.temperature =
        temperature.temperature;

    sensorData.humidity =
        humidity.relative_humidity;

    sensorData.pressure =
        bmp.readPressure() / 100.0;

    sensorData.co2 =
        720; // testwaarde tot MH-Z19B binnen is
}