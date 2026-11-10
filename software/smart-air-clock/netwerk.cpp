#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

#include "netwerk.h"

void connectWiFi()
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

    if(!result)
    {
        Serial.println(
            "WiFi mislukt"
        );

        ESP.restart();
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
}

bool wifiConnected()
{
    return
        WiFi.status() ==
        WL_CONNECTED;
}