#ifndef BUFFER_H
#define BUFFER_H

#include <Arduino.h>
#include <time.h>

// Offline buffer: metingen die niet geüpload konden worden (geen wifi of
// server down) worden hier bewaard en later automatisch verstuurd, met hun
// echte meettijdstip. Ringbuffer in RAM: als hij vol is, valt de oudste
// meting weg.

struct BufferedMeasurement
{
    float temperature;
    float humidity;
    float pressure;
    uint16_t co2;
    int light;
    bool presence;
    time_t timestamp;   // Unix-tijd van de meting
};

void bufferPush(const BufferedMeasurement& m);
bool bufferPeek(BufferedMeasurement& m);   // oudste meting, zonder te verwijderen
void bufferPop();                          // oudste meting verwijderen
int bufferCount();

#endif
