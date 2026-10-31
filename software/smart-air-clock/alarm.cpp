#include "alarm.h"
#include "audio.h"

#include <Arduino.h>
#include <time.h>
#include <Preferences.h>

#define MAX_ALARM_SOUND 5

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

static bool alarmAudioPlaying = false;

static int activeSoundId = 1;
static int editSoundId = 1;

const int soundTrackMap[] =
{
    0,
    1, // WakeUp
    3, // RickAstly
    4, // Bird
    5, // StarDestroyer
    6  // Piano
};

extern bool deleteAlarmServer(int id);

void initAlarm()
{
     loadAlarms();

    alarmRinging = false;

    alarmSnoozed = false;

    alarmAudioPlaying = false;

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

    alarms[alarmCount].id = -1;

    alarms[alarmCount].soundID = 1;

    alarmCount++;

    saveAlarms();

    return true;
}

void removeAlarm(int index)
{
    Serial.println("REMOVE ALARM CALLED");

    if(
        index < 0 ||
        index >= alarmCount
    )
    {
        return;
    }

    if(
        index < 0 ||
        index >= alarmCount
    )
    {
        return;
    }

    int alarmId =
        getAlarmId(index);

    Serial.print("ALARM ID = ");
    Serial.println(alarmId);

    if(alarmId > 0)
    {
        bool success =
            deleteAlarmServer(alarmId);

        Serial.print("Delete result: ");
        Serial.println(success);
    }

    for(
        int i = index;
        i < alarmCount - 1;
        i++
    )
    {
        alarms[i] = alarms[i + 1];
    }

    alarmCount--;

    alarms[alarmCount].used = false;

    saveAlarms();

    if(selectedAlarm >= alarmCount)
    {
        if(alarmCount > 0)
        {
            selectedAlarm = alarmCount - 1;
        }
        else
        {
            selectedAlarm = 0;
        }
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

    alarmAudioPlaying = false;

    audioStop();
}

void startSnooze()
{
    audioStop();

    alarmAudioPlaying = false;

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

            audioPlay(
                soundTrackMap[activeSoundId]
            );

            alarmAudioPlaying = true;
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

                if(!alarmAudioPlaying)
                {
                    activeSoundId =
                        alarms[i].soundID;

                    audioPlay(
                        soundTrackMap[activeSoundId]
                    );

                    alarmAudioPlaying = true;
                }

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

    if(alarmCount >= 5)
    {
        if(selectedAlarm >= alarmCount)
        {
            selectedAlarm = 0;
        }
    }
    else
    {
        if(selectedAlarm > alarmCount)
        {
            selectedAlarm = 0;
        }
    }
}

void previousAlarm()
{
    selectedAlarm--;

    if(alarmCount >= 5)
    {
        if(selectedAlarm < 0)
        {
            selectedAlarm = alarmCount - 1;
        }
    }
    else
    {
        if(selectedAlarm < 0)
        {
            selectedAlarm = alarmCount;
        }
    }
}

bool isNewAlarmSelected()
{
    if(alarmCount >= 5)
    {
        return false;
    }

    return selectedAlarm == alarmCount;
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

        editSoundId =
            alarms[selectedAlarm].soundID;
    }
    else
    {
        if(alarmCount >= 5)
        {
            return;
        }

        editHour = 7;
        editMinute = 0;
        editEnabled = true;
        editSoundId = 1;
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

void increaseEditSoundId()
{
    editSoundId++;

    if(editSoundId > MAX_ALARM_SOUND)
    {
        editSoundId = 1;
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
            
        alarms[selectedAlarm].soundID =
            editSoundId;
    }
    else
    {
        addAlarm(
            editHour,
            editMinute
        );

        alarms[alarmCount - 1].enabled =
            editEnabled;

        alarms[alarmCount - 1].soundID =
            editSoundId;
    }

    saveAlarms();
}

void saveAlarms()
{
    prefs.begin("alarms", false);

    prefs.putInt("count", alarmCount);

    for(int i = 0; i < alarmCount; i++)
    {
        char key[16];
        char idKey[16];
        char soundKey[16];

        sprintf(key, "alarm%d", i);
        sprintf(idKey, "id%d", i);
        sprintf(soundKey, "sound%d", i);

        uint32_t packed =
            (alarms[i].hour << 16) |
            (alarms[i].minute << 8) |
            alarms[i].enabled;

        prefs.putUInt(key, packed);
        prefs.putInt(idKey, alarms[i].id);
        prefs.putInt(soundKey, alarms[i].soundID);
    }

    prefs.end();
}

void loadAlarms()
{
    prefs.begin("alarms", true);

    alarmCount = prefs.getInt("count", 0);

    if (alarmCount > 5)
    {
        alarmCount = 5;
    }

    for (int i = 0; i < alarmCount; i++)
    {
        char key[16];
        char idKey[16];
        char soundKey[16];

        sprintf(key, "alarm%d", i);
        sprintf(idKey, "id%d", i);
        sprintf(soundKey, "sound%d", i);

        uint32_t packed = prefs.getUInt(key, 0);

        alarms[i].used = true;

        alarms[i].id =
            prefs.getInt(idKey, -1);

        alarms[i].soundID =
            prefs.getInt(soundKey, 1);

        alarms[i].hour =
            (packed >> 16) & 0xFF;

        alarms[i].minute =
            (packed >> 8) & 0xFF;

        alarms[i].enabled =
            packed & 0xFF;
    }

    prefs.end();

    Serial.println("Alarmen geladen:");

    for (int i = 0; i < alarmCount; i++)
    {
        Serial.printf(
            "A%d ID=%d %02d:%02d %s SOUND=%d\n",
            i + 1,
            alarms[i].id,
            alarms[i].hour,
            alarms[i].minute,
            alarms[i].enabled ? "ON" : "OFF",
            alarms[i].soundID
        );
    }
}

void clearAlarms()
{
    alarmCount = 0;
    saveAlarms();
}

void setAlarmId(
    int index,
    int id
)
{
    if(
        index < 0 ||
        index >= alarmCount
    )
    {
        return;
    }

    alarms[index].id = id;
}

int getAlarmId(
    int index
)
{
    if(
        index < 0 ||
        index >= alarmCount
    )
    {
        return -1;
    }

    return alarms[index].id;
}

int getEditSoundId()
{
    return editSoundId;
}

void setAlarmSoundId(
    int index,
    int soundId
)
{
    if(
        index < 0 ||
        index >= alarmCount
    )
    {
        return;
    }

    alarms[index].soundID =
        soundId;
}