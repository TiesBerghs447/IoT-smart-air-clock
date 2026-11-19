#include "buttons.h"

#include <Arduino.h>

#define POWER_PIN 4   
#define SET_PIN   5
#define UP_PIN    6

#define DEBOUNCE_TIME 20

static bool powerPressEvent = false;
static bool powerLongEvent = false;

static bool setPressEvent = false;
static bool setLongEvent = false;

static bool upPressEvent = false;
static bool upHoldEvent = false;

static bool powerLongTriggered = false;
static bool setLongTriggered = false;

static unsigned long powerDownTime =    0;
static unsigned long setDownTime = 0;
static unsigned long upDownTime = 0;

static bool powerWasDown = false;
static bool setWasDown = false;
static bool upWasDown = false;

static bool powerStableState = HIGH;
static bool setStableState = HIGH;
static bool upStableState = HIGH;

static bool powerLastReading = HIGH;
static bool setLastReading = HIGH;
static bool upLastReading = HIGH;

static unsigned long powerDebounceTime = 0;
static unsigned long setDebounceTime = 0;
static unsigned long upDebounceTime = 0;


void initButtons()
{
    pinMode(POWER_PIN, INPUT_PULLUP);
    pinMode(SET_PIN, INPUT_PULLUP);
    pinMode(UP_PIN, INPUT_PULLUP);
}

void updateButtons()
{
    unsigned long now = millis();

    bool powerReading = digitalRead(POWER_PIN);
    bool setReading = digitalRead(SET_PIN);
    bool upReading = digitalRead(UP_PIN);

    // =========================
    // POWER DEBOUNCE
    // =========================

    if(powerReading != powerLastReading)
    {
        powerDebounceTime = now;
        powerLastReading = powerReading;
    }

    if(
        powerReading != powerStableState &&
        now - powerDebounceTime >= DEBOUNCE_TIME
    )
    {
        powerStableState = powerReading;
    }
    // =========================
    // SET DEBOUNCE
    // =========================

    if(setReading != setLastReading)
    {
        setDebounceTime = now;
        setLastReading = setReading;
    }

    if(
        setReading != setStableState &&
        now - setDebounceTime >= DEBOUNCE_TIME
    )
    {
        setStableState = setReading;
    }
    // =========================
    // UP DEBOUNCE
    // =========================

    if(upReading != upLastReading)
    {
        upDebounceTime = now;
        upLastReading = upReading;
    }

    if(
        upReading != upStableState &&
        now - upDebounceTime >= DEBOUNCE_TIME
    )
    {
        upStableState = upReading;
    }

    bool powerDown =
        (powerStableState == LOW);

    bool setDown =
        (setStableState == LOW);

    bool upDown =
        (upStableState == LOW);

    // =========================
    // POWER
    // =========================

    if(powerDown && !powerWasDown)
    {
        powerDownTime = now;
        powerLongTriggered = false;
    }

    if(
        powerDown &&
        powerWasDown &&
        !powerLongTriggered &&
        now - powerDownTime >= 2000
    )
    {
        powerLongEvent = true;
        powerLongTriggered = true;
    }

    if(!powerDown && powerWasDown)
    {
        if(
            !powerLongTriggered &&
            now - powerDownTime < 2000
        )
        {
            powerPressEvent = true;
        }
    }

    powerWasDown = powerDown;

    // =========================
    // SET
    // =========================

    if(setDown && !setWasDown)
    {
        setDownTime = now;
        setLongTriggered = false;
    }

    if(
        setDown &&
        setWasDown &&
        !setLongTriggered &&
        now - setDownTime >= 1000
    )
    {
        setLongEvent = true;
        setLongTriggered = true;
    }

    if(!setDown && setWasDown)
    {
        if(
            !setLongTriggered &&
            now - setDownTime < 1000
        )
        {
            setPressEvent = true;
        }
    }

    setWasDown = setDown;

    // =========================
    // UP
    // =========================

    static unsigned long lastRepeat = 0;

    if(upDown && !upWasDown)
    {
        upDownTime = now;
        upPressEvent = true;
        lastRepeat = now;
    }

    if(upDown && upWasDown)
    {
        if(now - upDownTime >= 500)
        {
            if(now - lastRepeat >= 75)
            {
                upHoldEvent = true;
                lastRepeat = now;
            }
        }
    }

    if(!upDown && upWasDown)
    {
        lastRepeat = 0;
    }

    upWasDown = upDown;
}

bool powerPressed()
{
    bool result = powerPressEvent;
    powerPressEvent = false;

    return result;
}

bool powerLongPressed()
{
    bool result = powerLongEvent;
    powerLongEvent = false;

    return result;
}

bool setPressed()
{
    bool result = setPressEvent;
    setPressEvent = false;

    return result;
}

bool setLongPressed()
{
    bool result = setLongEvent;
    setLongEvent = false;

    return result;
}

bool upPressed()
{
    bool result = upPressEvent;
    upPressEvent = false;

    return result;
}

bool upHeld()
{
    bool result = upHoldEvent;
    upHoldEvent = false;

    return result;
}