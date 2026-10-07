#include <Arduino.h>
#include <WiFi.h>

#include "netwerk.h"

const char* WIFI_SSID = "berghs";
const char* WIFI_PASSWORD = "babbelroos";

void connectWiFi()
{
    Serial.println("Verbinden met WiFi...");

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );

    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi verbonden");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

bool wifiConnected()
{
    return WiFi.status() == WL_CONNECTED;
}