#include "netwerk.h"
#include "config.h"
#include "api.h"
#include "globals.h"
#include "sensors.h"
#include "rgb.h"
#include "display.h"
#include "buttons.h"
#include "alarm.h"
#include "audio.h"
#include "microfoon.h"

#include <time.h>

RGB rgb;

static unsigned long lastRgbUpdate = 0;
static unsigned long lastConfigSync = 0;

String ledMode;

int remoteRed;
int remoteGreen;
int remoteBlue;

bool displayEnabled = true;
bool powerOffMode = false;

int clapCount = 0;
unsigned long lastClapTime = 0;

static unsigned long lastPresenceTime = 0;

void setup()
{
    Serial.begin(115200);

    initDisplay();

    showBootMessage("Smart Air Clock");
    showBootMessage("Booting...");

    showBootMessage("Loading config");
    loadDefaultConfig();

    showBootMessage("Connecting WiFi");
    connectWiFi();

    showBootMessage("Downloading config");
    downloadConfig();
    lastConfigSync = millis();
    Serial.println("CONFIG:");

    Serial.print("co2GoodMax=");
    Serial.println(deviceConfig.co2GoodMax);

    Serial.print("co2WarningMax=");
    Serial.println(deviceConfig.co2WarningMax);

    Serial.print("uploadInterval=");
    Serial.println(deviceConfig.uploadInterval);

    showBootMessage("Init sensors");
    initSensors();

    showBootMessage("Init RGB");
    rgb.begin();

    showBootMessage("Sync time");
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

    showBootMessage("Wachten op tijd...");
    while(!getLocalTime(&timeinfo))
    {
        Serial.println("Wachten op tijd...");
        delay(500);
    }

    showBootMessage("Tijd ontvangen");
    Serial.println("Tijd ontvangen");
    initButtons();
    audioInit();
    initAlarm();
}

void loop()
{
    static unsigned long lastDisplayUpdate = 0;
    static unsigned long lastSensorUpdate = 0;
    static unsigned long lastUpload = 0;

    unsigned long now = millis();

    updatePresence();

    // ==================================================
    // BUTTONS
    // ==================================================

    updateButtons();

    bool powerPress = powerPressed();
    bool powerLong = powerLongPressed();

    bool setPress = setPressed();
    bool setLong = setLongPressed();

    bool upPress = upPressed();
    bool upHold = upHeld();

    // ==================================================
    // POWER MANAGEMENT
    // ==================================================

    if(powerOffMode)
    {
        Serial.println("POWER OFF MODE");

        if(powerPress)
        {
            Serial.println("WAKE UP");
            powerOffMode = false;
            displayEnabled = true;
            setAlarmMenuState(ALARM_NORMAL);
        }
    }
    else if(
        getAlarmMenuState() ==
        ALARM_NORMAL
    )
    {
        if(powerLong)
        {
            powerOffMode = true;
            displayEnabled = false;

            setAlarmMenuState(
                ALARM_NORMAL
            );

            clearDisplay();
            rgb.off();

            stopAlarm();
        }
        else if(powerPress)
        {
            displayEnabled =
                !displayEnabled;

            if(!displayEnabled)
            {
                clearDisplay();
            }
        }
    }

    // ==================================================
    // ALARM MENU
    // ==================================================

    if(
        setLong &&
        getAlarmMenuState() ==
        ALARM_NORMAL &&
        !powerOffMode
    )
    {
        setAlarmMenuState(
            ALARM_LIST
        );
    }

    // Alarm lijst

    else if(
        getAlarmMenuState() ==
        ALARM_LIST
    )
    {
        if(upPress)
        {
            nextAlarm();
        }

        if(setPress)
        {
            startEditAlarm();

            Serial.println("SET PRESSED IN ALARM_LIST");

            setAlarmMenuState(
                ALARM_SET_HOUR
            );
        }

        if(powerPress)
        {
            setAlarmMenuState(
                ALARM_NORMAL
            );
        }

        if(
            powerLong
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

    else if(
        getAlarmMenuState() ==
        ALARM_SET_HOUR
    )
    {
        if(
            upPress ||
            upHold
        )
        {
            increaseEditHour();
        }

        if(setPress)
        {
            setAlarmMenuState(
                ALARM_SET_MINUTE
            );
        }

        if(powerPress)
        {
            setAlarmMenuState(
                ALARM_LIST
            );
        }
    }

    // Minuten instellen

    else if(
        getAlarmMenuState() ==
        ALARM_SET_MINUTE
    )
    {
        if(
            upPress ||
            upHold
        )
        {
            increaseEditMinute();
        }

        if(setPress)
        {
            setAlarmMenuState(
                ALARM_SET_ENABLED
            );
        }

        if(powerPress)
        {
            setAlarmMenuState(
                ALARM_LIST
            );
        }
    }

    // Aan / Uit

    else if(
        getAlarmMenuState() ==
        ALARM_SET_ENABLED
    )
    {
        if(upPress)
        {
            toggleEditEnabled();
        }

        if(setPress)
        {
            setAlarmMenuState(
                ALARM_SET_SOUND
            );
        }
        
        if(powerPress)
        {
            setAlarmMenuState(
                ALARM_LIST
            );
        }
    }
    
    // Sound kiezen

    else if(
        getAlarmMenuState() ==
        ALARM_SET_SOUND
    )
    {
        if(
            upPress ||
            upHold
        )
        {
            increaseEditSoundId();
        }

        if(setPress)
        {
            saveEditedAlarm();

            setAlarmMenuState(
                ALARM_LIST
            );
        }

        if(powerPress)
        {
            setAlarmMenuState(
                ALARM_SET_ENABLED
            );
        }
    }

    else if(
        getAlarmMenuState() ==
        ALARM_DELETE
    )
    {
        if(upPress)
        {
            Serial.println("DELETE CONFIRMED");

            removeAlarm(
                getSelectedAlarm()
            );

            setAlarmMenuState(
                ALARM_LIST
            );
        }

        if(powerPress)
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

    if(!powerOffMode)
    {
        updateAlarm();
    }

    // STOP

    if(!powerOffMode && isAlarmRinging())
    {
        if(setPress)
        {
            stopAlarm();
        }

        if(upPress)
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

    if(!powerOffMode && isAlarmRinging())
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
            if(displayEnabled)
            {
                updateDisplay(
                    sensorData.temperature,
                    sensorData.humidity,
                    sensorData.co2,
                    sensorData.light
                );
            }
        }
        else if(displayEnabled)
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

    if(!isAlarmRinging() && !powerOffMode)
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
                sensorData.co2,
                sensorData.light,
                sensorData.presence
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
    static unsigned long
        lastBrightnessUpdate = 0;

    if(
        millis() -
        lastBrightnessUpdate >=
        1000
    )
    {
        int light =
            readLightLevel();

        uint8_t contrast =
            map(
                light,
                0,
                4095,
                20,
                255
            );

        contrast =
            constrain(
                contrast,
                20,
                255
            );

        Serial.print("LIGHT=");
        Serial.print(light);

        Serial.print(" CONTRAST=");
        Serial.println(contrast);

        setDisplayContrast(contrast);

        lastBrightnessUpdate =
            millis();
    }
    
    if(isAlarmRinging())
    {
        int sound =
            readSoundLevel();

        static bool clapDetected = false;

        if(sound > 5000 && !clapDetected)
        {
            clapDetected = true;

            clapCount++;

            lastClapTime =
                millis();

            Serial.print(
                "CLAP "
            );

            Serial.println(
                clapCount
            );
        }

        if(sound < 2000)
        {
            clapDetected = false;
        }
    }
    static unsigned long lastRadarPrint = 0;

    if(sensorData.presence)
    {
        lastPresenceTime = millis();

        if(!displayEnabled)
        {
            displayEnabled = true;
        }

        // LED terug aan
        if(!isAlarmRinging() && !powerOffMode)
        {
            if(ledMode == "remote")
            {
                rgb.setColor(
                    remoteRed,
                    remoteGreen,
                    remoteBlue
                );
            }
        }
    }

    if(
        millis() - lastPresenceTime >=
        60000UL
    )
    {
        if(displayEnabled)
        {
            displayEnabled = false;
            clearDisplay();

            Serial.println(
                "Display uit: geen aanwezigheid"
            );
        }

        // LED uit
        rgb.off();
    }
    
}