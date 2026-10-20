#include "display.h"
#include "alarm.h"

#include <Wire.h>
#include <time.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SDA_PIN 8
#define SCL_PIN 9

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    -1
);

const char* weekdagen[] =
{
    "Zondag",
    "Maandag",
    "Dinsdag",
    "Woensdag",
    "Donderdag",
    "Vrijdag",
    "Zaterdag"
};

void initDisplay()
{
    Wire.begin(
        SDA_PIN,
        SCL_PIN
    );

    if(
        !display.begin(
            SSD1306_SWITCHCAPVCC,
            0x3C
        )
    )
    {
        Serial.println(
            "OLED NIET GEVONDEN"
        );

        while(true)
        {
            delay(100);
        }
    }

    display.clearDisplay();
    display.display();

    Serial.println("OLED OK");
}

void updateDisplay(
    float temperature,
    float humidity,
    int co2
)
{
    struct tm timeinfo;

    char tijd[9] =
        "--:--:--";

    char datum[11] =
        "--/--/----";

    const char* weekdag =
        "---";

    if(
        getLocalTime(
            &timeinfo
        )
    )
    {
        sprintf(
            tijd,
            "%02d:%02d:%02d",
            timeinfo.tm_hour,
            timeinfo.tm_min,
            timeinfo.tm_sec
        );

        sprintf(
            datum,
            "%02d-%02d-%04d",
            timeinfo.tm_mday,
            timeinfo.tm_mon + 1,
            timeinfo.tm_year + 1900
        );

        weekdag =
            weekdagen[
                timeinfo.tm_wday
            ];
    }

    display.clearDisplay();

    display.setTextColor(
        SSD1306_WHITE
    );

    display.setTextSize(2);

    display.setCursor(0, 0);
    display.println(tijd);

    display.setTextSize(1);

    display.setCursor(0, 20);
    display.println(weekdag);

    display.setCursor(0, 30);
    display.println(datum);

    display.setCursor(0, 42);

    display.print("T: ");
    display.print(
        temperature,
        1
    );
    display.print("C");

    display.setCursor(70, 42);

    display.print("H: ");
    display.print(
        humidity,
        0
    );
    display.print("%");

    display.setCursor(0, 54);

    display.print("CO2: ");
    display.print(co2);
    display.print("ppm");

    display.display();
}

void updateAlarmDisplay()
{
    display.clearDisplay();

    display.setTextColor(
        SSD1306_WHITE
    );

    switch(
        getAlarmMenuState()
    )
    {
        case ALARM_LIST:
        {
            display.setTextSize(1);

            display.setCursor(0, 0);
            display.println("ALARMEN");

            int selected =
                getSelectedAlarm();

            if(isNewAlarmSelected())
            {
                display.setCursor(0, 20);
                display.println("+ NIEUW");

                display.setCursor(0, 40);
                display.println("SET=MAKEN");
            }
            else
            {
                Alarm alarm =
                    getAlarm(
                        selected
                    );

                char tijd[6];

                sprintf(
                    tijd,
                    "%02d:%02d",
                    alarm.hour,
                    alarm.minute
                );

                display.setTextSize(2);

                display.setCursor(
                    0,
                    18
                );

                display.println(
                    tijd
                );

                display.setTextSize(1);

                display.setCursor(
                    0,
                    50
                );

                display.print("A");
                display.print(
                    selected + 1
                );

                display.print(" ");

                if(alarm.enabled)
                {
                    display.print("ON");
                }
                else
                {
                    display.print("OFF");
                }
            }

            break;
        }

        case ALARM_SET_HOUR:
        {
            display.setTextSize(1);

            display.setCursor(0,0);
            display.println("UUR");

            display.setTextSize(3);

            display.setCursor(25,20);

            char uur[3];

            sprintf(
                uur,
                "%02d",
                getEditHour()
            );

            display.println(uur);

            break;
        }

        case ALARM_SET_MINUTE:
        {
            display.setTextSize(1);

            display.setCursor(0,0);
            display.println("MINUTEN");

            display.setTextSize(3);

            display.setCursor(25,20);

            char minuten[3];

            sprintf(
                minuten,
                "%02d",
                getEditMinute()
            );

            display.println(minuten);

            break;
        }

        case ALARM_SET_ENABLED:
        {
            display.setTextSize(1);

            display.setCursor(0,0);
            display.println("ACTIEF?");

            display.setTextSize(2);

            display.setCursor(15,25);

            if(getEditEnabled())
            {
                display.println("ON");
            }
            else
            {
                display.println("OFF");
            }

            break;
        }
        case ALARM_DELETE:
        {
            display.setTextSize(1);

            display.setCursor(0, 0);
            display.println("VERWIJDEREN?");

            if(isNewAlarmSelected())
            {
                display.setCursor(0, 20);
                display.println("GEEN ALARM");
            }
            else
            {
                Alarm alarm =
                    getAlarm(
                        getSelectedAlarm()
                    );

                char tijd[6];

                sprintf(
                    tijd,
                    "%02d:%02d",
                    alarm.hour,
                    alarm.minute
                );

                display.setTextSize(2);

                display.setCursor(
                    0,
                    20
                );

                display.println(tijd);

                display.setTextSize(1);

                display.setCursor(
                    0,
                    50
                );

                display.println(
                    "SET=JA POWER=NEE"
                );
            }

            break;
        }
        default:
            break;
    }

    display.display();
}