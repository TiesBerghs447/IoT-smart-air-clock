#include "api.h"
#include "config.h"
#include "alarm.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>

const char* API_KEY =
    "smartairclock2026api123456789012345";

bool downloadLedState(
    String& mode,
    int& red,
    int& green,
    int& blue
)
{
    HTTPClient http;

    http.begin(
        "https://iot-smart-air-clock-1.onrender.com/data/get_commando.php"
    );

    int responseCode =
        http.GET();

    if(responseCode != 200)
    {
        http.end();
        return false;
    }

    String payload =
        http.getString();

    StaticJsonDocument<256> doc;

    if(
        deserializeJson(
            doc,
            payload
        )
    )
    {
        http.end();
        return false;
    }

    mode =
        doc["led_mode"]
        .as<String>();

    red =
        doc["red"];

    green =
        doc["green"];

    blue =
        doc["blue"];

    http.end();

    return true;
}

bool downloadConfig()
{
    HTTPClient http;

    http.begin(
        "https://iot-smart-air-clock-1.onrender.com/settings/get_device_config.php"
    );

    http.addHeader(
        "X-API-KEY",
        API_KEY
    );

    int responseCode =
        http.GET();

    Serial.print(
        "Config HTTP code: "
    );

    Serial.println(
        responseCode
    );

    if(responseCode != 200)
    {
        http.end();
        return false;
    }

    String payload =
        http.getString();

    StaticJsonDocument<4096> doc;

    DeserializationError error =
        deserializeJson(
            doc,
            payload
        );

    if(error)
    {
        Serial.println(
            "JSON fout"
        );

        http.end();

        return false;
    }

    deviceConfig.co2GoodMax =
        doc["co2_good_max"];

    deviceConfig.co2WarningMax =
        doc["co2_warning_max"];

    deviceConfig.uploadInterval =
        doc["upload_interval"];

    deviceConfig.configSyncInterval =
        doc["config_sync_interval"] | 60;

    deviceConfig.audioEnabled =
        doc["audio_enabled"];

    deviceConfig.audioVolume =
        doc["audio_volume"];

    deviceConfig.displayEnabled =
        doc["display_enabled"];

    deviceConfig.greenEnabled =
        doc["green_enabled"];

    deviceConfig.orangeEnabled =
        doc["orange_enabled"];

    deviceConfig.redEnabled =
        doc["red_enabled"];

    // =========================
    // Alarm synchronisatie
    // =========================

    if(doc.containsKey("alarms"))
    {
        clearAlarms();

        JsonArray alarms =
            doc["alarms"].as<JsonArray>();

        for(JsonObject alarm : alarms)
        {
            Serial.print("SERVER ALARM ID: ");
            Serial.println(alarm["id"].as<int>());
        }

        for(JsonObject alarm : alarms)
        {
            int id =
                alarm["id"];

            int hour =
                alarm["hour"];

            int minute =
                alarm["minute"];

            bool enabled =
                alarm["enabled"];

            addAlarm(hour, minute);

            int index =
                getAlarmCount() - 1;

            setAlarmId(index, id);

            setAlarmId(index, id);

            saveAlarms();

            enableAlarm(
                index,
                enabled
            );
        }
    }

        Serial.print(
            "Alarmen geladen: "
        );

        Serial.println(
            getAlarmCount()
        );

        for(int i = 0; i < getAlarmCount(); i++)
        {
            Alarm alarm =
                getAlarm(i);

            Serial.printf(
                "Alarm %d: %02d:%02d %s\n",
                i + 1,
                alarm.hour,
                alarm.minute,
                alarm.enabled ? "ON" : "OFF"
            );
        }
    

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

    http.begin(
        "https://iot-smart-air-clock-1.onrender.com/data/save_data.php"
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
        String(
            temperature,
            2
        );

    payload +=
        "&humidity=" +
        String(
            humidity,
            2
        );

    payload +=
        "&pressure=" +
        String(
            pressure,
            2
        );

    payload +=
        "&co2=" +
        String(
            co2
        );

    Serial.println(
        "Payload:"
    );

    Serial.println(
        payload
    );

    int responseCode =
        http.POST(
            payload
        );

    Serial.print(
        "HTTP code: "
    );

    Serial.println(
        responseCode
    );

    String response =
        http.getString();

    Serial.println("CONFIG JSON:");
    Serial.println(payload);

    Serial.print(
        "Response: "
    );

    Serial.println(
        response
    );

    http.end();

    return
        responseCode == 200;
}

bool deleteAlarmServer(int id)
{
    HTTPClient http;

    http.begin(
        "https://iot-smart-air-clock-1.onrender.com/settings/delete_alarm.php"
    );

    http.addHeader(
        "Content-Type",
        "application/x-www-form-urlencoded"
    );

    http.addHeader(
        "X-API-KEY",
        API_KEY
    );

    String data =
        "id=" + String(id);

    Serial.print("DELETE ID: ");
    Serial.println(id);

    int code =
        http.POST(data);

    Serial.print("DELETE CODE: ");
    Serial.println(code);

    String response =
        http.getString();

    Serial.print("DELETE RESPONSE: ");
    Serial.println(response);

    http.end();

    return code == 200;
}