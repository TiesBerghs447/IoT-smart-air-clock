#include "api.h"
#include "config.h"
#include "alarm.h"
#include "netwerk.h"
#include "secrets.h"   // API_BASE_URL en API_KEY (niet op GitHub)

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <esp_task_wdt.h>

// Maximale wachttijd per HTTP-request. Zonder limiet kan een trage of
// onbereikbare server de loop() lang blokkeren (knoppen reageren dan niet
// en de watchdog zou het toestel herstarten).
static const uint16_t HTTP_TIMEOUT_MS = 4000;

// Start een request naar API_BASE_URL + path, met timeout en API-sleutel.
static bool beginRequest(HTTPClient& http, const char* path)
{
    if(!wifiConnected())
    {
        return false;
    }

    // Meerdere requests na elkaar (bv. config + alarmen) mogen de
    // watchdog niet laten aflopen.
    esp_task_wdt_reset();

    String url = String(API_BASE_URL) + path;

    if(!http.begin(url))
    {
        return false;
    }

    http.setConnectTimeout(HTTP_TIMEOUT_MS);
    http.setTimeout(HTTP_TIMEOUT_MS);
    http.addHeader("X-API-KEY", API_KEY);

    return true;
}

bool downloadLedState(
    String& mode,
    int& red,
    int& green,
    int& blue
)
{
    HTTPClient http;

    if(!beginRequest(http, "/data/get_commando.php"))
    {
        return false;
    }

    int responseCode =
        http.GET();

    if(responseCode != 200)
    {
        http.end();
        return false;
    }

    String payload =
        http.getString();

    http.end();

    StaticJsonDocument<256> doc;

    if(
        deserializeJson(
            doc,
            payload
        )
    )
    {
        return false;
    }

    // "| standaardwaarde": als een veld ontbreekt, blijft de LED in CO2-modus
    // in plaats van een lege String of 0 te krijgen.
    mode =
        doc["led_mode"] | "co2";

    red =
        constrain((int)(doc["red"] | 0), 0, 255);

    green =
        constrain((int)(doc["green"] | 0), 0, 255);

    blue =
        constrain((int)(doc["blue"] | 0), 0, 255);

    return true;
}

// Alarmen die op het toestel gemaakt of gewijzigd zijn (dirty of nog geen id)
// eerst naar de server sturen. Anders zou de volgende configuratie-sync ze
// wissen, want de server is de bron van waarheid voor de alarmlijst.
static bool pushLocalAlarms()
{
    bool allPushed = true;
    bool changed = false;

    for(int i = 0; i < getAlarmCount(); i++)
    {
        Alarm alarm = getAlarm(i);

        if(alarm.id > 0 && !alarm.dirty)
        {
            continue;
        }

        int newId = uploadAlarmServer(alarm);

        if(newId > 0)
        {
            markAlarmSynced(i, newId);
            changed = true;
        }
        else if(newId == ALARM_UPLOAD_REJECTED)
        {
            // Server weigert (bv. al 5 alarmen): de serverlijst wint.
            Serial.println("Alarm geweigerd door server");
        }
        else
        {
            allPushed = false;   // netwerkfout: volgende keer opnieuw
        }
    }

    if(changed)
    {
        saveAlarms();
    }

    return allPushed;
}

bool downloadConfig()
{
    // Lokale wijzigingen eerst uploaden. Lukt dat niet (bv. offline),
    // dan overschrijven we de lokale alarmen deze ronde niet.
    bool localAlarmsSynced =
        pushLocalAlarms();

    HTTPClient http;

    if(!beginRequest(http, "/settings/get_device_config.php"))
    {
        return false;
    }

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

    http.end();

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

        return false;
    }

    // Elk veld krijgt de huidige waarde als terugval ("| ..."). Vroeger werd
    // een ontbrekend veld 0, waardoor bv. co2GoodMax = 0 -> LED altijd rood.
    deviceConfig.co2GoodMax =
        doc["co2_good_max"] | deviceConfig.co2GoodMax;

    deviceConfig.co2WarningMax =
        doc["co2_warning_max"] | deviceConfig.co2WarningMax;

    deviceConfig.uploadInterval =
        doc["upload_interval"] | deviceConfig.uploadInterval;

    deviceConfig.configSyncInterval =
        doc["config_sync_interval"] | deviceConfig.configSyncInterval;

    deviceConfig.audioEnabled =
        doc["audio_enabled"] | deviceConfig.audioEnabled;

    deviceConfig.audioVolume =
        doc["audio_volume"] | deviceConfig.audioVolume;

    deviceConfig.displayEnabled =
        doc["display_enabled"] | deviceConfig.displayEnabled;

    deviceConfig.greenEnabled =
        doc["green_enabled"] | deviceConfig.greenEnabled;

    deviceConfig.orangeEnabled =
        doc["orange_enabled"] | deviceConfig.orangeEnabled;

    deviceConfig.redEnabled =
        doc["red_enabled"] | deviceConfig.redEnabled;

    // Onzinnige waarden begrenzen
    deviceConfig.uploadInterval =
        constrain(deviceConfig.uploadInterval, 5, 3600);

    deviceConfig.configSyncInterval =
        constrain(deviceConfig.configSyncInterval, 15, 3600);

    deviceConfig.audioVolume =
        constrain(deviceConfig.audioVolume, 0, 30);

    if(deviceConfig.co2WarningMax <= deviceConfig.co2GoodMax)
    {
        deviceConfig.co2WarningMax =
            deviceConfig.co2GoodMax + 1;
    }

    // =========================
    // Alarm synchronisatie
    // =========================

    // Niet vervangen terwijl de gebruiker in het alarmmenu zit: dan zouden
    // de indexen onder zijn vingers verschuiven.
    bool userEditing =
        getAlarmMenuState() != ALARM_NORMAL;

    if(
        doc["alarms"].is<JsonArray>() &&
        localAlarmsSynced &&
        !userEditing
    )
    {
        JsonArray serverAlarms =
            doc["alarms"].as<JsonArray>();

        Alarm newAlarms[MAX_ALARMS];
        int count = 0;

        for(JsonObject item : serverAlarms)
        {
            if(count >= MAX_ALARMS)
            {
                break;
            }

            Alarm& alarm = newAlarms[count];

            alarm.used = true;
            alarm.id = item["id"] | -1;
            alarm.hour = constrain((int)(item["hour"] | 0), 0, 23);
            alarm.minute = constrain((int)(item["minute"] | 0), 0, 59);
            alarm.enabled = item["enabled"] | false;
            alarm.soundID = normalizeSoundId(item["soundID"] | DEFAULT_SOUND_ID);

            count++;
        }

        // Eén keer naar flash schrijven i.p.v. per alarm.
        replaceAlarms(newAlarms, count);
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
            "Alarm %d: %02d:%02d %s %s\n",
            i + 1,
            alarm.hour,
            alarm.minute,
            alarm.enabled ? "ON" : "OFF",
            soundName(alarm.soundID)
        );
    }

    return true;
}

bool uploadMeasurement(
    float temperature,
    float humidity,
    float pressure,
    int co2,
    int light,
    bool presence,
    time_t timestamp
)
{
    HTTPClient http;

    if(!beginRequest(http, "/data/save_data.php"))
    {
        return false;
    }

    http.addHeader(
        "Content-Type",
        "application/x-www-form-urlencoded"
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

    payload +=
        "&light=" +
        String(
            light
        );

    payload +=
        "&presence=" +
        String(
            presence ? 1 : 0
        );

    // Gebufferde meting: stuur het echte meettijdstip mee.
    if(timestamp > 0)
    {
        payload +=
            "&timestamp=" +
            String((unsigned long)timestamp);
    }

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

    if(!beginRequest(http, "/settings/delete_alarm.php"))
    {
        return false;
    }

    http.addHeader(
        "Content-Type",
        "application/x-www-form-urlencoded"
    );

    String data =
        "id=" + String(id);

    Serial.print("DELETE ID: ");
    Serial.println(id);

    int code =
        http.POST(data);

    Serial.print("DELETE CODE: ");
    Serial.println(code);

    http.end();

    return code == 200;
}

int uploadAlarmServer(const Alarm& alarm)
{
    HTTPClient http;

    if(!beginRequest(http, "/settings/save_alarm.php"))
    {
        return -1;
    }

    http.addHeader(
        "Content-Type",
        "application/x-www-form-urlencoded"
    );

    String data =
        "hour=" + String(alarm.hour) +
        "&minute=" + String(alarm.minute) +
        "&enabled=" + String(alarm.enabled ? 1 : 0) +
        "&soundID=" + String(alarm.soundID);

    // Bestaand alarm -> server werkt het bij i.p.v. een nieuw aan te maken.
    if(alarm.id > 0)
    {
        data += "&id=" + String(alarm.id);
    }

    int code =
        http.POST(data);

    String response =
        http.getString();

    http.end();

    Serial.print("ALARM UPLOAD CODE: ");
    Serial.println(code);

    if(code == 409)
    {
        return ALARM_UPLOAD_REJECTED;
    }

    if(code != 200)
    {
        return -1;
    }

    StaticJsonDocument<128> doc;

    if(deserializeJson(doc, response))
    {
        return -1;
    }

    return doc["id"] | -1;
}
