#include "sensors.h"
#include "globals.h"
#include "display.h"

#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

#include "MHZ19.h"
#include "MHZ19_ESP32.h"

#include <ld2410.h>

#define SDA_PIN 8
#define SCL_PIN 9

#define MHZ19_UART_RX 16
#define MHZ19_UART_TX 15

#define LIGHT_SENSOR_PIN 7

#define LD2410_RX_PIN 1
#define LD2410_TX_PIN 2

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

HardwareSerial MHZ19_UART(0);

MHZ19_ESP32_Platform uartPlatform(
    &MHZ19_UART,
    1000
);

MHZ19 co2UART(&uartPlatform);

HardwareSerial radarSerial(2);
ld2410 radar;

bool radarConnected = false;

static uint16_t lastValidCo2 = 400;

// Vroeger: while(true) -> toestel bleef voor altijd hangen (bv. door een
// losse draad). Nu: melding tonen en na 10 s herstarten; vaak is het
// probleem dan weg (I2C-bus terug vrij).
static void sensorFatal(const char* message)
{
    Serial.println(message);
    showBootMessage(message);
    showBootMessage("Herstart in 10 s");

    delay(10000);
    ESP.restart();
}

bool radarAvailable()
{
    return radarConnected;
}

void initSensors()
{
    Wire.begin(SDA_PIN, SCL_PIN);

    if (!aht.begin())
    {
        sensorFatal("AHT20 NIET GEVONDEN");
    }

    Serial.println("AHT20 OK");

    if (!bmp.begin(0x77))
    {
        sensorFatal("BMP280 NIET GEVONDEN");
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

    pinMode(LIGHT_SENSOR_PIN, INPUT);

    radarSerial.begin(
        256000,
        SERIAL_8N1,
        LD2410_RX_PIN,
        LD2410_TX_PIN
    );

    radarConnected =
        radar.begin(radarSerial);

    if(radarConnected)
    {
        Serial.println("LD2410 OK");
    }
    else
    {
        Serial.println("LD2410 NIET GEVONDEN");
    }
}

void updateSensors()
{
    sensors_event_t humidity;
    sensors_event_t temperature;

    // Alleen geldige waarden overnemen. Een mislukte I2C-lezing gaf vroeger
    // NaN of onzin, die de server dan weigerde (HTTP 400).
    if(
        aht.getEvent(
            &humidity,
            &temperature
        ) &&
        !isnan(temperature.temperature) &&
        !isnan(humidity.relative_humidity)
    )
    {
        sensorData.temperature =
            temperature.temperature;

        sensorData.humidity =
            constrain(humidity.relative_humidity, 0.0f, 100.0f);
    }
    else
    {
        Serial.println("AHT20 leesfout, vorige waarde behouden");
    }

    float pressure =
        bmp.readPressure() / 100.0;

    if(
        !isnan(pressure) &&
        pressure >= 300 &&
        pressure <= 1200
    )
    {
        sensorData.pressure = pressure;
    }
    else
    {
        Serial.println("BMP280 leesfout, vorige waarde behouden");
    }

    // Pas uploaden als alle I2C-waarden minstens één keer echt gelezen zijn
    // (anders zou de eerste upload druk = 0 hPa bevatten).
    if(
        sensorData.pressure >= 300 &&
        sensorData.humidity > 0
    )
    {
        sensorData.valid = true;
    }

    sensorData.light =
        readLightLevel();
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

    Serial.print("Presence: ");
    if(sensorData.presence)
    {
        Serial.println("JA");
    }
    else
    {
        Serial.println("NEE");
    }


    Serial.println();
}

int readLightLevel()
{
    long sum = 0;

    for(int i = 0; i < 10; i++)
    {
        sum += analogRead(
            LIGHT_SENSOR_PIN
        );
    }

    return sum / 10;
}

void updatePresence()
{
    if(radarConnected)
    {
        radar.read();

        sensorData.presence =
            radar.movingTargetDetected();
    }
    else
    {
        sensorData.presence = false;
    }
}