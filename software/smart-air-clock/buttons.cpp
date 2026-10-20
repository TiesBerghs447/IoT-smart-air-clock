#include "buttons.h"

#include <Arduino.h>

#define POWER_PIN 4
#define SET_PIN   5
#define UP_PIN    6

static bool powerPressEvent = false;
static bool powerLongEvent = false;

static bool setPressEvent = false;
static bool setLongEvent = false;

static bool upPressEvent = false;
static bool upHoldEvent = false;

static bool powerLongTriggered = false;
static bool setLongTriggered = false;

static unsigned long powerDownTime = 0;
static unsigned long setDownTime = 0;
static unsigned long upDownTime = 0;

static bool powerWasDown = false;
static bool setWasDown = false;
static bool upWasDown = false;

void initButtons()
{
    pinMode(POWER_PIN, INPUT_PULLUP);
    pinMode(SET_PIN, INPUT_PULLUP);
    pinMode(UP_PIN, INPUT_PULLUP);
}

void updateButtons()
{
    bool powerDown =
        digitalRead(POWER_PIN) == LOW;

    bool setDown =
        digitalRead(SET_PIN) == LOW;

    bool upDown =
        digitalRead(UP_PIN) == LOW;

    unsigned long now = millis();

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

    if(upDown && !upWasDown)
    {
        upDownTime = now;
        upPressEvent = true;
    }

    if(upDown && upWasDown)
    {
        static unsigned long lastRepeat = 0;

        if(now - upDownTime >= 600)
        {
            if(now - lastRepeat >= 100)
            {
                upHoldEvent = true;
                lastRepeat = now;
            }
        }
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