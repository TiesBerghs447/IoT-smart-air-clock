#include "netwerk.h"
#include "config.h"
#include "api.h"
#include "globals.h"
#include "sensors.h"
#include "rgb.h"
#include "display.h"
#include "buttons.h"
#include "alarm.h"

#include <time.h>

RGB rgb;

static unsigned long lastRgbUpdate = 0;
static unsigned long lastConfigSync = 0;

String ledMode;

int remoteRed;
int remoteGreen;
int remoteBlue;

void setup()
{
    Serial.begin(115200);

    loadDefaultConfig();

    connectWiFi();

    downloadConfig();
    lastConfigSync = millis();
    Serial.println("CONFIG:");

    Serial.print("co2GoodMax=");
    Serial.println(deviceConfig.co2GoodMax);

    Serial.print("co2WarningMax=");
    Serial.println(deviceConfig.co2WarningMax);

    Serial.print("uploadInterval=");
    Serial.println(deviceConfig.uploadInterval);

    initSensors();

    rgb.begin();

    configTime(
        0,
        0,
        "pool.ntp.org"
    );

    setenv(
        "TZ",
        "CET-1CEST,M3.5.0,M10.5.0/3",
        1
    );

    tzset();

    struct tm timeinfo;

    while(!getLocalTime(&timeinfo))
    {
        Serial.println("Wachten op tijd...");
        delay(500);
    }

    Serial.println("Tijd ontvangen");
    initDisplay();
    initButtons();
    initAlarm();
}

void loop()
{
    static unsigned long lastDisplayUpdate = 0;
    static unsigned long lastSensorUpdate = 0;
    static unsigned long lastUpload = 0;

    unsigned long now = millis();

    // ==================================================
    // BUTTONS
    // ==================================================

    updateButtons();

    // ==================================================
    // ALARM MENU
    // ==================================================

    if(
        setLongPressed() &&
        getAlarmMenuState() ==
        ALARM_NORMAL
    )
    {
        setAlarmMenuState(
            ALARM_LIST
        );
    }

    // Alarm lijst

    if(
        getAlarmMenuState() ==
        ALARM_LIST
    )
    {
        if(upPressed())
        {
            nextAlarm();
        }

        if(setPressed())
        {
            startEditAlarm();

            setAlarmMenuState(
                ALARM_SET_HOUR
            );
        }

        if(powerPressed())
        {
            setAlarmMenuState(
                ALARM_NORMAL
            );
        }

        if(
            powerLongPressed()
            &&
            !isNewAlarmSelected()
        )
        {
            setAlarmMenuState(
                ALARM_DELETE
            );
        }
    }


    // Uur instellen

    if(
        getAlarmMenuState() ==
        ALARM_SET_HOUR
    )
    {
        if(
            upPressed() ||
            upHeld()
        )
        {
            increaseEditHour();
        }

        if(setPressed())
        {
            setAlarmMenuState(
                ALARM_SET_MINUTE
            );
        }

        if(powerPressed())
        {
            setAlarmMenuState(
                ALARM_LIST
            );
        }
    }

    // Minuten instellen

    if(
        getAlarmMenuState() ==
        ALARM_SET_MINUTE
    )
    {
        if(
            upPressed() ||
            upHeld()
        )
        {
            increaseEditMinute();
        }

        if(setPressed())
        {
            setAlarmMenuState(
                ALARM_SET_ENABLED
            );
        }

        if(powerPressed())
        {
            setAlarmMenuState(
                ALARM_LIST
            );
        }
    }

    // Aan / Uit

    if(
        getAlarmMenuState() ==
        ALARM_SET_ENABLED
    )
    {
        if(upPressed())
        {
            toggleEditEnabled();
        }

        if(setPressed())
        {
            saveEditedAlarm();

            setAlarmMenuState(
                ALARM_LIST
            );
        }

        if(powerPressed())
        {
            setAlarmMenuState(
                ALARM_LIST
            );
        }
    }
    if(
        getAlarmMenuState() ==
        ALARM_DELETE
    )
    {
        if(upPressed())
        {
            Serial.println("DELETE CONFIRMED");

            removeAlarm(
                getSelectedAlarm()
            );

            setAlarmMenuState(
                ALARM_LIST
            );
        }

        if(powerPressed())
        {
            Serial.println("DELETE CANCELLED");

            setAlarmMenuState(
                ALARM_LIST
            );
        }
    }

    // ==================================================
    // ALARM ENGINE
    // ==================================================

    updateAlarm();

    // STOP

    if(isAlarmRinging())
    {
        if(powerPressed())
        {
            stopAlarm();
        }

        if(upPressed())
        {
            startSnooze();
        }
    }
static AlarmMenuState lastState =
    ALARM_NORMAL;

if(
    getAlarmMenuState() !=
    lastState
)
{
    Serial.print("STATE = ");
    Serial.println(
        getAlarmMenuState()
    );

    lastState =
        getAlarmMenuState();
}
    // ==================================================
    // ALARM RGB
    // ==================================================

    if(isAlarmRinging())
    {
        static unsigned long alarmBlinkTimer = 0;
        static bool blinkState = false;

        if(
            millis() -
            alarmBlinkTimer >=
            500
        )
        {
            alarmBlinkTimer =
                millis();

            blinkState =
                !blinkState;

            if(blinkState)
            {
                rgb.setRed();
            }
            else
            {
                rgb.off();
            }
        }
    }

    // ==================================================
    // DISPLAY
    // ==================================================

    if(
        now - lastDisplayUpdate >=
        100
    )
    {
        if(
            getAlarmMenuState() ==
            ALARM_NORMAL
        )
        {
            updateDisplay(
                sensorData.temperature,
                sensorData.humidity,
                sensorData.co2
            );
        }
        else
        {
            updateAlarmDisplay();
        }

        lastDisplayUpdate =
            now;
    }

    // ==================================================
    // SENSOREN
    // ==================================================

    if(
        now - lastSensorUpdate >=
        (unsigned long)max(
            1,
            deviceConfig.uploadInterval
        ) * 1000UL
    )
    {
        updateSensors();

        lastSensorUpdate =
            now;
    }

    // ==================================================
    // CONFIG DOWNLOAD
    // ==================================================

    if(
        now - lastConfigSync >=
        (unsigned long)max(
            15,
            deviceConfig.configSyncInterval
        ) * 1000UL
    )
    {
        downloadConfig();

        lastConfigSync =
            now;
    }

    // ==================================================
    // RGB DOWNLOAD
    // ==================================================

    if(
        now - lastRgbUpdate >=
        1000
    )
    {
        downloadLedState(
            ledMode,
            remoteRed,
            remoteGreen,
            remoteBlue
        );

        lastRgbUpdate =
            now;
    }

    // ==================================================
    // NORMALE RGB MODUS
    // ==================================================

    if(!isAlarmRinging())
    {
        if(ledMode == "remote")
        {
            rgb.setColor(
                remoteRed,
                remoteGreen,
                remoteBlue
            );
        }
        else
        {
            if(
                sensorData.co2 <
                deviceConfig.co2GoodMax
            )
            {
                rgb.setGreen();
            }
            else if(
                sensorData.co2 <
                deviceConfig.co2WarningMax
            )
            {
                rgb.setYellow();
            }
            else
            {
                rgb.setRed();
            }
        }
    }

    // ==================================================
    // UPLOAD
    // ==================================================

    if(
        now - lastUpload >=
        (unsigned long)max(
            1,
            deviceConfig.uploadInterval
        ) * 1000UL
    )
    {
        bool success =
            uploadMeasurement(
                sensorData.temperature,
                sensorData.humidity,
                sensorData.pressure,
                sensorData.co2
            );

        Serial.print(
            "Upload resultaat: "
        );

        Serial.println(
            success
        );

        lastUpload =
            now;
    }
}