#include "netwerk.h"
#include "config.h"
#include "api.h"
#include "globals.h"
#include "sensors.h"
#include "rgb.h"

RGB rgb;

static unsigned long lastRgbUpdate = 0;
static unsigned long lastConfigSync = 0;

String ledMode;

int remoteRed;
int remoteGreen;
int remoteBlue;

void setup()
{
    Serial.begin(115200);

    loadDefaultConfig();

    connectWiFi();

    downloadConfig();
    lastConfigSync = millis();
    Serial.println("CONFIG:");

Serial.print("co2GoodMax=");
Serial.println(deviceConfig.co2GoodMax);

Serial.print("co2WarningMax=");
Serial.println(deviceConfig.co2WarningMax);

Serial.print("uploadInterval=");
Serial.println(deviceConfig.uploadInterval);

    initSensors();

    rgb.begin();
}

void loop()
{
    updateSensors();
    if (millis() - lastConfigSync >=
        (unsigned long)max(15, deviceConfig.configSyncInterval) * 1000UL)
    {
        downloadConfig();
        lastConfigSync = millis();
    }

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
    if(millis() - lastRgbUpdate > 1000)
    {
    downloadLedState(
        ledMode,
        remoteRed,
        remoteGreen,
        remoteBlue
    );

    lastRgbUpdate = millis();
    }

    if(ledMode == "remote")
    {
        rgb.setColor(
            remoteRed,
            remoteGreen,
            remoteBlue
        );
    }
    else
    {
        if(sensorData.co2 <
        deviceConfig.co2GoodMax)
        {
            rgb.setGreen();
        }
        else if(sensorData.co2 <
                deviceConfig.co2WarningMax)
        {
            rgb.setYellow();
        }
        else
        {
            rgb.setRed();
        }
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
    unsigned long waitStarted = millis();
    unsigned long uploadInterval =
        (unsigned long)max(1, deviceConfig.uploadInterval) * 1000UL;

    while (millis() - waitStarted < uploadInterval)
    {
        if (millis() - lastConfigSync >=
            (unsigned long)max(15, deviceConfig.configSyncInterval) * 1000UL)
        {
            downloadConfig();
            lastConfigSync = millis();
        }

        delay(100);
    }
}