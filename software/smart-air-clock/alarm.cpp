#include "alarm.h"
#include "audio.h"
#include "config.h"

#include <Arduino.h>
#include <time.h>
#include <Preferences.h>

// Na zoveel tijd stopt een niet-afgezet alarm vanzelf
// (bv. wanneer niemand thuis is).
#define ALARM_MAX_RING_MS (10UL * 60UL * 1000UL)

// Sluimertijd in seconden
#define SNOOZE_SECONDS (5 * 60)

static Preferences prefs;

static Alarm alarms[MAX_ALARMS];

static int alarmCount = 0;

static bool alarmRinging = false;

static bool alarmSnoozed = false;

// Sluimeren op basis van Unix-tijd i.p.v. uur:minuut vergelijken:
// werkt ook als die ene minuut gemist wordt en over middernacht.
static time_t snoozeUntil = 0;

static unsigned long ringStartedAt = 0;

static AlarmMenuState menuState =
    ALARM_NORMAL;

static int selectedAlarm = 0;

static int editHour = 7;
static int editMinute = 0;
static bool editEnabled = true;

static int lastTriggeredMinute = -1;

static bool alarmAudioPlaying = false;

static int activeSoundId = DEFAULT_SOUND_ID;
static int editSoundId = DEFAULT_SOUND_ID;

extern bool deleteAlarmServer(int id);

// =========================
// Geluiden
// =========================

struct SoundInfo
{
    int id;            // id op website/database
    int track;         // bestandsnummer op SD-kaart (000X....mp3)
    const char* name;  // tekst op OLED
};

// Moet overeenkomen met alarm_sound_options() in alarms.php
// en met de bestanden in de map songs/.

//1 = smoke alarm
//2 = wake up
//3 = batterij
//4 = rick asley
//5 = bird
//6 = star destroyer
//7 = piano

static const SoundInfo sounds[] =
{
    { 1, 2, "WakeUp" },   // website "Wake-up geluid"  -> SD-track 2
    { 2, 4, "Rick"   },   // website "Rickroll"        -> SD-track 4
    { 4, 6, "Star"   },   // website "Star Destroyer"  -> SD-track 6
    { 5, 5, "Vogels" },   // website "Vogels"          -> SD-track 5
    { 7, 7, "Piano"  }    // website "Piano"           -> SD-track 7
};
static const int SOUND_COUNT =
    sizeof(sounds) / sizeof(sounds[0]);

static int soundIndex(int soundId)
{
    for(int i = 0; i < SOUND_COUNT; i++)
    {
        if(sounds[i].id == soundId)
        {
            return i;
        }
    }

    return 0;
}

int normalizeSoundId(int soundId)
{
    return sounds[soundIndex(soundId)].id;
}

int nextSoundId(int soundId)
{
    return sounds[(soundIndex(soundId) + 1) % SOUND_COUNT].id;
}

int soundTrack(int soundId)
{
    return sounds[soundIndex(soundId)].track;
}

const char* soundName(int soundId)
{
    return sounds[soundIndex(soundId)].name;
}

static void startRinging(int soundId)
{
    alarmRinging = true;
    ringStartedAt = millis();
    activeSoundId = normalizeSoundId(soundId);

    // Audio uit via dashboard -> alleen de rode LED knippert.
    if(deviceConfig.audioEnabled)
    {
        audioSetVolume(deviceConfig.audioVolume);
        audioPlay(soundTrack(activeSoundId));
        alarmAudioPlaying = true;
    }
}

// =========================
// Beheer
// =========================

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
    if(alarmCount >= MAX_ALARMS)
    {
        return false;
    }

    alarms[alarmCount].used = true;

    alarms[alarmCount].enabled = true;

    alarms[alarmCount].dirty = true;

    alarms[alarmCount].hour = hour;

    alarms[alarmCount].minute = minute;

    alarms[alarmCount].id = -1;

    alarms[alarmCount].soundID = DEFAULT_SOUND_ID;

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

void replaceAlarms(const Alarm* list, int count)
{
    alarmCount = constrain(count, 0, MAX_ALARMS);

    for(int i = 0; i < alarmCount; i++)
    {
        alarms[i] = list[i];
        alarms[i].used = true;
        alarms[i].dirty = false;
    }

    if(selectedAlarm > alarmCount)
    {
        selectedAlarm = 0;
    }

    saveAlarms();
}

void markAlarmSynced(int index, int id)
{
    if(
        index < 0 ||
        index >= alarmCount
    )
    {
        return;
    }

    alarms[index].id = id;
    alarms[index].dirty = false;
}

Alarm getAlarm(
    int index
)
{
    Alarm emptyAlarm;

    emptyAlarm.id = -1;
    emptyAlarm.used = false;
    emptyAlarm.enabled = false;
    emptyAlarm.dirty = false;
    emptyAlarm.hour = 0;
    emptyAlarm.minute = 0;
    emptyAlarm.soundID = DEFAULT_SOUND_ID;

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

    // Altijd stoppen met rinkelen, ook als de tijd even niet beschikbaar is.
    alarmRinging = false;

    time_t now = time(nullptr);

    if(now < 100000)   // tijd nog niet gesynchroniseerd
    {
        alarmSnoozed = false;
        return;
    }

    snoozeUntil = now + SNOOZE_SECONDS;

    alarmSnoozed = true;
}

void updateAlarm()
{
    // Rinkelt al te lang -> automatisch stoppen
    if(
        alarmRinging &&
        millis() - ringStartedAt >= ALARM_MAX_RING_MS
    )
    {
        Serial.println("Alarm automatisch gestopt (timeout)");
        stopAlarm();
    }

    struct tm timeinfo;

    if(
        !getLocalTime(
            &timeinfo,
            10
        )
    )
    {
        return;
    }

    if(
        alarmSnoozed &&
        time(nullptr) >= snoozeUntil
    )
    {
        alarmSnoozed = false;

        startRinging(activeSoundId);
    }

    int currentMinute =
        timeinfo.tm_hour * 60 +
        timeinfo.tm_min;

    // Elke alarmminuut maar één keer afgaan.
    if(currentMinute == lastTriggeredMinute)
    {
        return;
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
            lastTriggeredMinute =
                currentMinute;

            // Nieuw alarm annuleert een lopende sluimer.
            alarmSnoozed = false;

            startRinging(alarms[i].soundID);

            break;
        }
    }
}

// =========================
// Menu
// =========================

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

    if(alarmCount >= MAX_ALARMS)
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

    if(alarmCount >= MAX_ALARMS)
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
    if(alarmCount >= MAX_ALARMS)
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
            normalizeSoundId(alarms[selectedAlarm].soundID);
    }
    else
    {
        if(alarmCount >= MAX_ALARMS)
        {
            return;
        }

        editHour = 7;
        editMinute = 0;
        editEnabled = true;
        editSoundId = DEFAULT_SOUND_ID;
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
    // Alleen geldige geluiden (dezelfde als op de website).
    editSoundId =
        nextSoundId(editSoundId);
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

        // Moet nog naar de server, anders overschrijft de sync het.
        alarms[selectedAlarm].dirty = true;
    }
    else
    {
        if(
            !addAlarm(
                editHour,
                editMinute
            )
        )
        {
            return;   // lijst vol
        }

        alarms[alarmCount - 1].enabled =
            editEnabled;

        alarms[alarmCount - 1].soundID =
            editSoundId;
    }

    saveAlarms();
}

// =========================
// Flash-opslag
// =========================

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

        // bit 0 = enabled, bit 1 = dirty
        uint32_t packed =
            (alarms[i].hour << 16) |
            (alarms[i].minute << 8) |
            (alarms[i].dirty ? 0x02 : 0) |
            (alarms[i].enabled ? 0x01 : 0);

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

    alarmCount = constrain(alarmCount, 0, MAX_ALARMS);

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
            normalizeSoundId(prefs.getInt(soundKey, DEFAULT_SOUND_ID));

        alarms[i].hour =
            constrain((int)((packed >> 16) & 0xFF), 0, 23);

        alarms[i].minute =
            constrain((int)((packed >> 8) & 0xFF), 0, 59);

        alarms[i].enabled =
            packed & 0x01;

        alarms[i].dirty =
            packed & 0x02;
    }

    prefs.end();

    Serial.println("Alarmen geladen:");

    for (int i = 0; i < alarmCount; i++)
    {
        Serial.printf(
            "A%d ID=%d %02d:%02d %s SOUND=%s\n",
            i + 1,
            alarms[i].id,
            alarms[i].hour,
            alarms[i].minute,
            alarms[i].enabled ? "ON" : "OFF",
            soundName(alarms[i].soundID)
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
        normalizeSoundId(soundId);
}
