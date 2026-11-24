#ifndef ALARM_H
#define ALARM_H

#define MAX_ALARMS 5

// Geluid-ID's zijn dezelfde als op de website (alarms.php) en in de database.
// Zo betekent soundID 5 overal "Vogels". De vertaling naar het MP3-bestand
// op de SD-kaart gebeurt alleen in soundTrack().
#define DEFAULT_SOUND_ID 1

struct Alarm
{
    int id;          // id in de database, <= 0 = nog niet op de server

    bool used;
    bool enabled;
    bool dirty;      // lokaal gewijzigd, nog niet naar de server gestuurd

    int hour;
    int minute;

    int soundID;
};

// Geluid-hulpfuncties
int normalizeSoundId(int soundId);   // ongeldige id -> DEFAULT_SOUND_ID
int nextSoundId(int soundId);        // volgende geluid in de lijst
int soundTrack(int soundId);         // MP3-tracknummer op de SD-kaart
const char* soundName(int soundId);  // korte naam voor het OLED-scherm

// Vervangt alle alarmen in één keer (gebruikt door de server-sync).
void replaceAlarms(const Alarm* list, int count);

// Markeert een alarm als gesynchroniseerd met de server.
void markAlarmSynced(int index, int id);

enum AlarmMenuState
{
    ALARM_NORMAL,

    ALARM_LIST,

    ALARM_SET_HOUR,

    ALARM_SET_MINUTE,

    ALARM_SET_ENABLED,
    
    ALARM_SET_SOUND,

    ALARM_DELETE
};

void initAlarm();

void updateAlarm();

void updateAlarmMenu();

bool addAlarm(
    int hour,
    int minute
);

void removeAlarm(
    int index
);

void enableAlarm(
    int index,
    bool enabled
);

Alarm getAlarm(
    int index
);

int getAlarmCount();

bool isAlarmRinging();

bool isAlarmSnoozed();

void stopAlarm();

void startSnooze();

AlarmMenuState getAlarmMenuState();

void setAlarmMenuState(
    AlarmMenuState state
);

int getSelectedAlarm();

void nextAlarm();

void previousAlarm();

bool isNewAlarmSelected();

int getEditHour();

int getEditMinute();

bool getEditEnabled();

void startEditAlarm();

void increaseEditHour();

void increaseEditMinute();

void toggleEditEnabled();

void saveEditedAlarm();

void saveAlarms();

void loadAlarms();

void clearAlarms();

void setAlarmId(
    int index,
    int id
);

int getAlarmId(
    int index
);

int getEditSoundId();
void increaseEditSoundId();

void setAlarmSoundId(
    int index,
    int soundId
);

#endif