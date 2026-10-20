#include "sensors.h"
#include "globals.h"

#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

#include "MHZ19.h"
#include "MHZ19_ESP32.h"

#define SDA_PIN 8
#define SCL_PIN 9

#define MHZ19_UART_RX 16
#define MHZ19_UART_TX 17

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

HardwareSerial MHZ19_UART(2);

MHZ19_ESP32_Platform uartPlatform(
    &MHZ19_UART,
    1000
);

MHZ19 co2UART(&uartPlatform);

static uint16_t lastValidCo2 = 400;

void initSensors()
{
    Wire.begin(SDA_PIN, SCL_PIN);

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

    MHZ19_UART.begin(
        9600,
        SERIAL_8N1,
        MHZ19_UART_RX,
        MHZ19_UART_TX
    );

    delay(5000);

    co2UART.begin();

    co2UART.setABCFunction(false);

    Serial.println("MH-Z19 OK");
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

    uint16_t co2Ppm = 0;
    int16_t co2Temp = 0;

    if (
        co2UART.readCO2WithRetry(
            &co2Ppm,
            &co2Temp,
            3
        )
    )
    {
        if (
            co2Ppm >= 350 &&
            co2Ppm <= 3000
        )
        {
            lastValidCo2 = co2Ppm;
        }
    }

    sensorData.co2 = lastValidCo2;

    Serial.print("CO2: ");
    Serial.print(sensorData.co2);
    Serial.println(" ppm");

    Serial.print("Temperatuur: ");
    Serial.print(sensorData.temperature);
    Serial.println(" °C");

    Serial.print("Vochtigheid: ");
    Serial.print(sensorData.humidity);
    Serial.println(" %");

    Serial.print("Druk: ");
    Serial.print(sensorData.pressure);
    Serial.println(" hPa");

    Serial.println();
}