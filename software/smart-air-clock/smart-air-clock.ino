#include "netwerk.h"
#include "config.h"
#include "api.h"
#include "globals.h"
#include "sensors.h"

void setup()
{
    Serial.begin(115200);

    loadDefaultConfig();

    connectWiFi();

    downloadConfig();

    initSensors();
}

void loop()
{
    updateSensors();
    
    if(deviceConfig.audioEnabled)
    {
        // DFPlayer actief
    }

    if(deviceConfig.greenEnabled)
    {
        // groene LED
    }

    if(deviceConfig.deviceEnabled == false)
    {
        // deep sleep
    }
    if(sensorData.co2 <
    deviceConfig.co2GoodMax)
    {
        // groen
    }
    else if(sensorData.co2 <
            deviceConfig.co2WarningMax)
    {
        // oranje
    }
    else
    {
        // rood
    }
    Serial.println("Meting:");

    Serial.print("Temp: ");
    Serial.println(sensorData.temperature);

    Serial.print("Humidity: ");
    Serial.println(sensorData.humidity);

    Serial.print("Pressure: ");
    Serial.println(sensorData.pressure);

    Serial.println("Upload starten...");

    bool success =
    uploadMeasurement(
    sensorData.temperature,
    sensorData.humidity,
    sensorData.pressure,
    sensorData.co2
    );

    Serial.print("Upload resultaat: ");
    Serial.println(success);
    delay(30000);
}