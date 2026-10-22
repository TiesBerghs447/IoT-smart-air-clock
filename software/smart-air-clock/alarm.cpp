#include "alarm.h"

#include <Arduino.h>
#include <time.h>
#include <Preferences.h>

static Preferences prefs;

static Alarm alarms[5];

static int alarmCount = 0;

static bool alarmRinging = false;

static bool alarmSnoozed = false;

static int snoozeHour = 0;
static int snoozeMinute = 0;

static AlarmMenuState menuState =
    ALARM_NORMAL;

static int selectedAlarm = 0;

static int editHour = 7;
static int editMinute = 0;
static bool editEnabled = true;

static int lastTriggeredMinute = -1;

void initAlarm()
{
     loadAlarms();

    alarmRinging = false;

    alarmSnoozed = false;

    menuState =
        ALARM_NORMAL;

    selectedAlarm = 0;
}

bool addAlarm(
    int hour,
    int minute
)
{
    if(alarmCount >= 5)
    {
        return false;
    }

    alarms[alarmCount].used = true;

    alarms[alarmCount].enabled = true;

    alarms[alarmCount].hour = hour;

    alarms[alarmCount].minute = minute;

    alarmCount++;

    saveAlarms();

    return true;
}

void removeAlarm(
    int index
)
{
    if(
        index < 0 ||
        index >= alarmCount
    )
    {
        return;
    }

    for(
        int i = index;
        i < alarmCount - 1;
        i++
    )
    {
        alarms[i] =
            alarms[i + 1];
    }

    alarmCount--;

    alarms[alarmCount].used = false;

    saveAlarms();

    if(
        selectedAlarm >
        alarmCount
    )
    {
        selectedAlarm =
            alarmCount;
    }
}

void enableAlarm(
    int index,
    bool enabled
)
{
    if(
        index < 0 ||
        index >= alarmCount
    )
    {
        return;
    }

    alarms[index].enabled =
        enabled;

    saveAlarms();
}

Alarm getAlarm(
    int index
)
{
    Alarm emptyAlarm;

    emptyAlarm.used = false;
    emptyAlarm.enabled = false;
    emptyAlarm.hour = 0;
    emptyAlarm.minute = 0;

    if(
        index < 0 ||
        index >= alarmCount
    )
    {
        return emptyAlarm;
    }

    return alarms[index];
}

int getAlarmCount()
{
    return alarmCount;
}

bool isAlarmRinging()
{
    return alarmRinging;
}

bool isAlarmSnoozed()
{
    return alarmSnoozed;
}

void stopAlarm()
{
    alarmRinging = false;

    alarmSnoozed = false;
}

void startSnooze()
{
    struct tm timeinfo;

    if(
        !getLocalTime(
            &timeinfo
        )
    )
    {
        return;
    }

    snoozeHour =
        timeinfo.tm_hour;

    snoozeMinute =
        timeinfo.tm_min + 5;

    if(
        snoozeMinute >= 60
    )
    {
        snoozeMinute -= 60;

        snoozeHour++;
    }

    if(
        snoozeHour >= 24
    )
    {
        snoozeHour = 0;
    }

    alarmRinging = false;

    alarmSnoozed = true;
}

void updateAlarm()
{
    struct tm timeinfo;

    if(
        !getLocalTime(
            &timeinfo
        )
    )
    {
        return;
    }

    if(alarmSnoozed)
    {
        if(
            timeinfo.tm_hour ==
                snoozeHour
            &&
            timeinfo.tm_min ==
                snoozeMinute
        )
        {
            alarmSnoozed = false;

            alarmRinging = true;
        }
    }

    for(
        int i = 0;
        i < alarmCount;
        i++
    )
    {
        if(
            !alarms[i].enabled
        )
        {
            continue;
        }

        if(
            alarms[i].hour ==
                timeinfo.tm_hour
            &&
            alarms[i].minute ==
                timeinfo.tm_min
        )
        {
            int currentMinute =
                timeinfo.tm_hour * 60 +
                timeinfo.tm_min;

            if(
                currentMinute !=
                lastTriggeredMinute
            )
            {
                alarmRinging = true;

                lastTriggeredMinute =
                    currentMinute;
            }
        }
    }
}

AlarmMenuState getAlarmMenuState()
{
    return menuState;
}

void setAlarmMenuState(
    AlarmMenuState state
)
{
    menuState = state;
}

int getSelectedAlarm()
{
    return selectedAlarm;
}

void nextAlarm()
{
    selectedAlarm++;

    if(
        selectedAlarm >
        alarmCount
    )
    {
        selectedAlarm = 0;
    }
}

void previousAlarm()
{
    selectedAlarm--;

    if(
        selectedAlarm < 0
    )
    {
        selectedAlarm =
            alarmCount;
    }
}

bool isNewAlarmSelected()
{
    return
        selectedAlarm ==
        alarmCount;
}

int getEditHour()
{
    return editHour;
}

int getEditMinute()
{
    return editMinute;
}

bool getEditEnabled()
{
    return editEnabled;
}

void startEditAlarm()
{
    if(
        selectedAlarm < alarmCount
    )
    {
        editHour =
            alarms[selectedAlarm].hour;

        editMinute =
            alarms[selectedAlarm].minute;

        editEnabled =
            alarms[selectedAlarm].enabled;
    }
    else
    {
        editHour = 7;
        editMinute = 0;
        editEnabled = true;
    }
}

void increaseEditHour()
{
    editHour++;

    if(editHour > 23)
    {
        editHour = 0;
    }
}

void increaseEditMinute()
{
    editMinute++;

    if(editMinute > 59)
    {
        editMinute = 0;
    }
}

void toggleEditEnabled()
{
    editEnabled =
        !editEnabled;
}
void saveEditedAlarm()
{
    if(selectedAlarm < alarmCount)
    {
        alarms[selectedAlarm].hour =
            editHour;

        alarms[selectedAlarm].minute =
            editMinute;

        alarms[selectedAlarm].enabled =
            editEnabled;
    }
    else
    {
        addAlarm(
            editHour,
            editMinute
        );

        alarms[alarmCount - 1].enabled =
            editEnabled;
    }

    saveAlarms();
}

void saveAlarms()
{
    prefs.begin(
        "alarms",
        false
    );

    prefs.putInt(
        "count",
        alarmCount
    );

    for(
        int i = 0;
        i < alarmCount;
        i++
    )
    {
        char key[16];

        sprintf(
            key,
            "alarm%d",
            i
        );

        uint32_t packed =
            (alarms[i].hour << 16) |
            (alarms[i].minute << 8) |
            alarms[i].enabled;

        prefs.putUInt(
            key,
            packed
        );
    }

    prefs.end();
}

void loadAlarms()
{
    prefs.begin(
        "alarms",
        true
    );

    alarmCount =
        prefs.getInt(
            "count",
            0
        );

    if(alarmCount > 5)
    {
        alarmCount = 5;
    }

    for(
        int i = 0;
        i < alarmCount;
        i++
    )
    {
        char key[16];

        sprintf(
            key,
            "alarm%d",
            i
        );

        uint32_t packed =
            prefs.getUInt(
                key,
                0
            );

        alarms[i].used = true;

        alarms[i].hour =
            (packed >> 16) & 0xFF;

        alarms[i].minute =
            (packed >> 8) & 0xFF;

        alarms[i].enabled =
            packed & 0xFF;
    }

    prefs.end();

    Serial.println("Alarmen geladen:");

    for(int i = 0; i < alarmCount; i++)
    {
        Serial.printf(
            "A%d %02d:%02d %s\n",
            i + 1,
            alarms[i].hour,
            alarms[i].minute,
            alarms[i].enabled ? "ON" : "OFF"
        );
    }
}

void clearAlarms()
{
    alarmCount = 0;
    saveAlarms();
}