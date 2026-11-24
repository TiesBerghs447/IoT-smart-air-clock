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
#include "buffer.h"

#include <time.h>
#include <esp_task_wdt.h>
#include <esp_arduino_version.h>

// ==================================================
// INSTELLINGEN
// ==================================================

// Sensoren vaker lezen dan uploaden: het scherm en de LED reageren dan
// sneller, de server krijgt nog steeds één meting per uploadInterval.
static const unsigned long SENSOR_INTERVAL_MS = 5000;

// LED-commando van het dashboard ophalen. Elke HTTPS-request blokkeert de
// loop een fractie van een seconde, dus niet sneller dan nodig.
static const unsigned long LED_POLL_INTERVAL_MS = 2000;

// Eén gebufferde meting per keer doorsturen, zodat de loop vlot blijft.
static const unsigned long BUFFER_FLUSH_INTERVAL_MS = 2000;

// Scherm/LED uit als de radar zo lang niemand ziet
static const unsigned long NO_PRESENCE_TIMEOUT_MS = 60000UL;

// Watchdog: herstart het toestel als loop() langer dan dit vastzit
static const uint32_t WDT_TIMEOUT_S = 30;

RGB rgb;

static unsigned long lastRgbUpdate = 0;
static unsigned long lastConfigSync = 0;
static bool forceConfigSync = false;

String ledMode = "co2";

int remoteRed;
int remoteGreen;
int remoteBlue;

// Door de gebruiker met POWER uitgezet (los van aanwezigheid)
bool displayOffByUser = false;
bool powerOffMode = false;

static unsigned long lastPresenceTime = 0;

static void startWatchdog()
{
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    esp_task_wdt_config_t config =
    {
        .timeout_ms = WDT_TIMEOUT_S * 1000,
        .idle_core_mask = 0,
        .trigger_panic = true
    };

    // In core 3.x staat de watchdog vaak al aan: dan herconfigureren.
    if(esp_task_wdt_reconfigure(&config) != ESP_OK)
    {
        esp_task_wdt_init(&config);
    }
#else
    esp_task_wdt_init(WDT_TIMEOUT_S, true);
#endif

    esp_task_wdt_add(NULL);   // loop-taak bewaken

    Serial.println("Watchdog actief");
}

// Zet de LED alleen als de kleur echt verandert
// (voorkomt flikkeren door elke loop opnieuw te schrijven).
static void setLed(uint8_t r, uint8_t g, uint8_t b)
{
    static int lastR = -1;
    static int lastG = -1;
    static int lastB = -1;

    if(r == lastR && g == lastG && b == lastB)
    {
        return;
    }

    rgb.setColor(r, g, b);

    lastR = r;
    lastG = g;
    lastB = b;
}

// Kleur volgens CO2-niveau, rekening houdend met de kleuren die
// op het dashboard aan/uit gezet zijn.
static void showCo2Color()
{
    if(sensorData.co2 < deviceConfig.co2GoodMax)
    {
        if(deviceConfig.greenEnabled) setLed(0, 255, 0);
        else                          setLed(0, 0, 0);
    }
    else if(sensorData.co2 < deviceConfig.co2WarningMax)
    {
        if(deviceConfig.orangeEnabled) setLed(255, 255, 0);
        else                           setLed(0, 0, 0);
    }
    else
    {
        if(deviceConfig.redEnabled) setLed(255, 0, 0);
        else                        setLed(0, 0, 0);
    }
}

// Meting uploaden; lukt het niet, dan in de offline buffer.
static void uploadOrBuffer()
{
    if(!sensorData.valid)
    {
        Serial.println("Nog geen geldige meting, upload overgeslagen");
        return;
    }

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

    if(success)
    {
        return;
    }

    time_t now = time(nullptr);

    // Zonder geldige klok weten we het meettijdstip niet -> niet bufferen
    if(now < 100000)
    {
        return;
    }

    BufferedMeasurement m;
    m.temperature = sensorData.temperature;
    m.humidity = sensorData.humidity;
    m.pressure = sensorData.pressure;
    m.co2 = sensorData.co2;
    m.light = sensorData.light;
    m.presence = sensorData.presence;
    m.timestamp = now;

    bufferPush(m);

    Serial.print("Gebufferd, wachtrij = ");
    Serial.println(bufferCount());
}

// Oudste gebufferde meting doorsturen zodra er weer verbinding is.
static void flushBuffer()
{
    BufferedMeasurement m;

    if(!wifiConnected() || !bufferPeek(m))
    {
        return;
    }

    bool success =
        uploadMeasurement(
            m.temperature,
            m.humidity,
            m.pressure,
            m.co2,
            m.light,
            m.presence,
            m.timestamp
        );

    if(success)
    {
        bufferPop();

        Serial.print("Buffer verstuurd, nog ");
        Serial.println(bufferCount());
    }
}

void setup()
{
    Serial.begin(115200);

    initDisplay();

    showBootMessage("Smart Air Clock");
    showBootMessage("Booting...");

    showBootMessage("Loading config");
    loadDefaultConfig();

    showBootMessage("Connecting WiFi");

    if(!connectWiFi())
    {
        showBootMessage("Offline modus");
    }

    // Alarmen uit flash laden VÓÓR de eerste sync. Anders is de lijst nog
    // leeg, worden lokale (nog niet geüploade) alarmen niet doorgestuurd
    // en overschrijft de serverlijst ze.
    initAlarm();

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

    // Max 15 s wachten. Vroeger eindeloos: zonder internet startte de klok
    // nooit op. De tijd wordt later alsnog automatisch gesynchroniseerd.
    unsigned long ntpStart = millis();

    while(
        !getLocalTime(&timeinfo, 500) &&
        millis() - ntpStart < 15000
    )
    {
        Serial.println("Wachten op tijd...");
    }

    if(getLocalTime(&timeinfo, 10))
    {
        showBootMessage("Tijd ontvangen");
        Serial.println("Tijd ontvangen");
    }
    else
    {
        showBootMessage("Nog geen tijd");
        Serial.println("Nog geen tijd, later opnieuw");
    }

    initButtons();
    audioInit();

    lastPresenceTime = millis();

    startWatchdog();
}

void loop()
{
    static unsigned long lastDisplayUpdate = 0;
    static unsigned long lastSensorUpdate = 0;
    static unsigned long lastUpload = 0;

    esp_task_wdt_reset();

    unsigned long now = millis();

    maintainWiFi();

    audioUpdate();   // laat het alarmgeluid herhalen tot het gestopt wordt

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

    // Wie op een knop drukt, is aanwezig.
    if(
        powerPress || powerLong ||
        setPress || setLong ||
        upPress || upHold
    )
    {
        lastPresenceTime = now;
    }

    // ==================================================
    // POWER MANAGEMENT
    // ==================================================

    if(powerOffMode)
    {
        // (vroeger stond hier een Serial.println die elke loop de
        //  seriële monitor vol spamde)
        if(powerPress)
        {
            Serial.println("WAKE UP");
            powerOffMode = false;
            displayOffByUser = false;
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
            Serial.println("POWER OFF MODE");

            powerOffMode = true;

            setAlarmMenuState(
                ALARM_NORMAL
            );

            stopAlarm();
        }
        else if(powerPress && !isAlarmRinging())
        {
            displayOffByUser =
                !displayOffByUser;
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

            // Meteen naar de server sturen i.p.v. te wachten op de
            // volgende sync (anders kon die het alarm overschrijven).
            forceConfigSync = true;

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
    // AANWEZIGHEID
    // ==================================================

    // Zonder radar weten we niet of er iemand is: dan altijd "aanwezig",
    // anders ging het scherm na 1 minuut voorgoed uit.
    if(sensorData.presence || !radarAvailable())
    {
        lastPresenceTime = now;
    }

    bool someonePresent =
        now - lastPresenceTime < NO_PRESENCE_TIMEOUT_MS;

    // ==================================================
    // DISPLAY
    // ==================================================

    // Eén plek die beslist of het scherm aan mag. Vroeger zetten meerdere
    // stukken code displayEnabled aan/uit, waardoor aanwezigheid het scherm
    // weer aanzette na POWER of zelfs in power-off modus.
    bool inMenu =
        getAlarmMenuState() != ALARM_NORMAL;

    bool screenOn =
        !powerOffMode &&
        (
            inMenu ||
            isAlarmRinging() ||
            (
                !displayOffByUser &&
                deviceConfig.displayEnabled &&
                someonePresent
            )
        );

    static bool lastScreenOn = true;

    if(screenOn != lastScreenOn)
    {
        if(!screenOn)
        {
            clearDisplay();
            Serial.println("Display uit");
        }

        lastScreenOn = screenOn;
    }

    if(
        screenOn &&
        now - lastDisplayUpdate >= 100
    )
    {
        if(inMenu)
        {
            updateAlarmDisplay();
        }
        else
        {
            updateDisplay(
                sensorData.temperature,
                sensorData.humidity,
                sensorData.co2,
                sensorData.light
            );
        }

        lastDisplayUpdate =
            now;
    }

    // ==================================================
    // SENSOREN
    // ==================================================

    if(
        now - lastSensorUpdate >=
        SENSOR_INTERVAL_MS
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
        forceConfigSync ||
        now - lastConfigSync >=
        (unsigned long)max(
            15,
            deviceConfig.configSyncInterval
        ) * 1000UL
    )
    {
        downloadConfig();

        forceConfigSync = false;

        lastConfigSync =
            now;
    }

    // ==================================================
    // RGB DOWNLOAD
    // ==================================================

    if(
        now - lastRgbUpdate >=
        LED_POLL_INTERVAL_MS
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
    // RGB LED
    // ==================================================

    // Eén beslissing per loop, in volgorde van belangrijkheid. Vroeger zette
    // de code eerst een kleur en daarna rgb.off() -> flikkeren, en het
    // knipperen van het alarm werd uitgeschakeld als niemand bewoog.
    if(powerOffMode)
    {
        setLed(0, 0, 0);
    }
    else if(isAlarmRinging())
    {
        bool blinkOn =
            (now / 500) % 2 == 0;

        if(blinkOn) setLed(255, 0, 0);
        else        setLed(0, 0, 0);
    }
    else if(!someonePresent)
    {
        setLed(0, 0, 0);
    }
    else if(ledMode == "remote")
    {
        setLed(
            remoteRed,
            remoteGreen,
            remoteBlue
        );
    }
    else
    {
        showCo2Color();
    }

    // ==================================================
    // UPLOAD + OFFLINE BUFFER
    // ==================================================

    if(
        now - lastUpload >=
        (unsigned long)max(
            5,
            deviceConfig.uploadInterval
        ) * 1000UL
    )
    {
        uploadOrBuffer();

        lastUpload =
            now;
    }

    static unsigned long lastBufferFlush = 0;

    if(
        bufferCount() > 0 &&
        now - lastBufferFlush >= BUFFER_FLUSH_INTERVAL_MS
    )
    {
        flushBuffer();

        lastBufferFlush = now;
    }

    // ==================================================
    // SCHERMHELDERHEID
    // ==================================================

    static unsigned long
        lastBrightnessUpdate = 0;

    if(
        now -
        lastBrightnessUpdate >=
        1000
    )
    {
        // Laatst gemeten waarde gebruiken i.p.v. opnieuw de ADC te lezen
        int light =
            sensorData.light;

        uint8_t contrast =
            constrain(
                map(
                    light,
                    0,
                    4095,
                    20,
                    255
                ),
                20,
                255
            );

        setDisplayContrast(contrast);

        lastBrightnessUpdate =
            now;
    }
}
