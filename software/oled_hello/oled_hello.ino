#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    -1
);

void setup()
{
    Wire.begin(8, 9);

    if(!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C
    ))
    {
        Serial.println("OLED niet gevonden");

        while(true);
    }

    display.clearDisplay();

    display.setTextSize(2);
    display.setTextColor(WHITE);

    display.setCursor(0, 0);
    display.println("Hallo");

    display.display();
}

void loop()
{
}