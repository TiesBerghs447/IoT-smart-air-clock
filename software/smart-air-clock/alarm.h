#ifndef ALARM_H
#define ALARM_H

struct Alarm
{
    bool used;
    bool enabled;

    int hour;
    int minute;
};

enum AlarmMenuState
{
    ALARM_NORMAL,

    ALARM_LIST,

    ALARM_SET_HOUR,

    ALARM_SET_MINUTE,

    ALARM_SET_ENABLED,

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

#endif