#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

#define SDA_PIN 8
#define SCL_PIN 9

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

void setup()
{
    Serial.begin(115200);

    delay(3000);

    Serial.println();
    Serial.println("=== Smart Air Clock Sensor Test ===");

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
    Serial.println();
}

void loop()
{
    sensors_event_t humidity;
    sensors_event_t temperature;

    aht.getEvent(
        &humidity,
        &temperature
    );

    float bmpTemp =
        bmp.readTemperature();

    float pressure =
        bmp.readPressure() / 100.0;

    Serial.println("----- METING -----");

    Serial.print("AHT20 Temp: ");
    Serial.print(
        temperature.temperature
    );
    Serial.println(" C");

    Serial.print("AHT20 Humidity: ");
    Serial.print(
        humidity.relative_humidity
    );
    Serial.println(" %");

    Serial.print("BMP280 Temp: ");
    Serial.print(
        bmpTemp
    );
    Serial.println(" C");

    Serial.print("BMP280 Pressure: ");
    Serial.print(
        pressure
    );
    Serial.println(" hPa");

    Serial.println();

    delay(3000);
}