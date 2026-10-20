#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

#include "netwerk.h"

void connectWiFi()
{
    WiFiManager wm;

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