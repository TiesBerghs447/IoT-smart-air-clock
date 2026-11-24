#ifndef API_H
#define API_H
#include <Arduino.h>
#include <time.h>

#include "config.h"
#include "alarm.h"

bool downloadConfig();

// timestamp = 0  -> server gebruikt "nu" als meettijdstip.
// timestamp > 0  -> Unix-tijd van een gebufferde (offline) meting.
bool uploadMeasurement(
    float temperature,
    float humidity,
    float pressure,
    int co2,
    int light,
    bool presence,
    time_t timestamp = 0
);

bool downloadLedState(
    String& mode,
    int& red,
    int& green,
    int& blue
);

bool deleteAlarmServer(int id);

#define ALARM_UPLOAD_REJECTED -2

// Stuurt een alarm dat op het toestel gemaakt/gewijzigd is naar de server.
// Geeft het server-id terug, -1 bij netwerkfout,
// ALARM_UPLOAD_REJECTED als de server het alarm weigert.
int uploadAlarmServer(const Alarm& alarm);

#endif
