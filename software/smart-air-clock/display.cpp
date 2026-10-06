#include "display.h"
#include "globals.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET);

void initDisplay()
{
    Wire.begin(20, 21);

    if (!display.begin(
            SSD1306_SWITCHCAPVCC,
            SCREEN_ADDRESS))
    {
        Serial.println("OLED niet gevonden");

        while (true)
        {
            delay(100);
        }
    }

    display.clearDisplay();
    display.display();
}

void updateDisplay()
{
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("Smart Air Clock");

    display.setCursor(0, 16);
    display.print("T: ");
    display.print(sensorData.temperature);
    display.println(" C");

    display.setCursor(0, 28);
    display.print("H: ");
    display.print(sensorData.humidity);
    display.println(" %");

    display.setCursor(0, 40);
    display.print("P: ");
    display.print(sensorData.pressure);
    display.println(" hPa");

    display.setCursor(0, 52);
    display.print("CO2: ");
    display.print(sensorData.co2);
    display.println(" ppm");

    display.display();
}