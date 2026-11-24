#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

#include "netwerk.h"

// Hoe vaak we opnieuw proberen te verbinden als de wifi weg is
static const unsigned long WIFI_RETRY_INTERVAL_MS = 15000;

bool connectWiFi()
{
    WiFiManager wm;
    wm.setConnectTimeout(20);          // max 20 s proberen met opgeslagen wifi
    wm.setConfigPortalTimeout(120);    // setup-portaal max 2 min open, daarna lokaal verder

    Serial.println(
        "Verbinden met WiFi..."
    );

    bool result =
        wm.autoConnect(
            "SmartAirClock"
        );

    // Automatisch opnieuw verbinden als de router even wegvalt
    WiFi.setAutoReconnect(true);

    if(!result)
    {
        // Vroeger: ESP.restart(). Dan werkt de klok/wekker niet zonder wifi.
        // Nu draait het toestel lokaal verder; maintainWiFi() blijft proberen
        // en metingen worden gebufferd tot de verbinding terug is.
        Serial.println(
            "WiFi mislukt, verder in offline modus"
        );

        WiFi.mode(WIFI_STA);
        WiFi.begin();   // opgeslagen gegevens blijven proberen

        return false;
    }

    Serial.println(
        "WiFi verbonden"
    );

    Serial.print(
        "IP: "
    );

    Serial.println(
        WiFi.localIP()
    );

    return true;
}

bool wifiConnected()
{
    return
        WiFi.status() ==
        WL_CONNECTED;
}

void maintainWiFi()
{
    static unsigned long lastAttempt = 0;
    static bool wasConnected = true;

    bool connected = wifiConnected();

    if(connected)
    {
        if(!wasConnected)
        {
            Serial.println("WiFi hersteld");
        }

        wasConnected = true;
        return;
    }

    if(wasConnected)
    {
        Serial.println("WiFi verbinding verloren");
        wasConnected = false;
        lastAttempt = millis();
        return;
    }

    if(millis() - lastAttempt >= WIFI_RETRY_INTERVAL_MS)
    {
        Serial.println("WiFi opnieuw verbinden...");
        WiFi.reconnect();
        lastAttempt = millis();
    }
}
