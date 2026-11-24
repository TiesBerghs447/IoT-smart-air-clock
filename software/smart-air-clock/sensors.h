#ifndef SENSORS_H
#define SENSORS_H

void initSensors();
void updateSensors();
int readLightLevel();
void updatePresence();

// false als de LD2410-radar niet gevonden werd
bool radarAvailable();

#endif