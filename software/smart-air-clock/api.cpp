#include "api.h"
#include "config.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* API_KEY =
    "smartairclock2026api123456789012345";

bool downloadConfig()
{
    HTTPClient http;
    
    http.begin(
        "https://smart-air-clock.infy.click/settings/get_config.php"
    );

    int responseCode =
        http.GET();

    Serial.print("Config HTTP code: ");
    Serial.println(responseCode);

    if (responseCode != 200)
    {
        http.end();
        return false;
    }

    String payload =
        http.getString();

    StaticJsonDocument<2048> doc;

    DeserializationError error =
        deserializeJson(
            doc,
            payload
        );

    if (error)
    {
        Serial.println("JSON fout");
        http.end();
        return false;
    }

    deviceConfig.co2GoodMax =
        doc["co2_good_max"];

    deviceConfig.co2WarningMax =
        doc["co2_warning_max"];

    deviceConfig.uploadInterval =
        doc["upload_interval"];

    deviceConfig.refreshInterval =
        doc["refresh_interval"];

    deviceConfig.audioEnabled =
        doc["audio_enabled"];

    deviceConfig.audioVolume =
        doc["audio_volume"];

    deviceConfig.alarmEnabled =
        doc["alarm_enabled"];

    deviceConfig.alarmHour =
        doc["alarm_hour"];

    deviceConfig.alarmMinute =
        doc["alarm_minute"];

    deviceConfig.displayEnabled =
        doc["display_enabled"];

    deviceConfig.greenEnabled =
        doc["green_enabled"];

    deviceConfig.orangeEnabled =
        doc["orange_enabled"];

    deviceConfig.redEnabled =
        doc["red_enabled"];

    deviceConfig.deviceEnabled =
        doc["device_enabled"];

    http.end();

    return true;
}

bool uploadMeasurement(
    float temperature,
    float humidity,
    float pressure,
    int co2
)
{
    HTTPClient http;

    //http.begin(
    //    "https://smart-air-clock.infy.click/data/save_data.php"
    //);
    http.begin(
"https://smart-air-clock.infy.click/data/esp_test.php"
);
    http.addHeader(
        "Content-Type",
        "application/x-www-form-urlencoded"
    );

    http.addHeader(
        "X-API-KEY",
        API_KEY
    );

    String payload =
        "temperature=" +
        String(temperature, 2);

    payload +=
        "&humidity=" +
        String(humidity, 2);

    payload +=
        "&pressure=" +
        String(pressure, 2);

    payload +=
        "&co2=" +
        String(co2);

    Serial.println("Payload:");
    Serial.println(payload);

    int responseCode =
        http.POST(payload);

    Serial.print("HTTP code: ");
    Serial.println(responseCode);

    String response =
        http.getString();

    Serial.print("Response: ");
    Serial.println(response);

    http.end();

    return responseCode == 200;
}