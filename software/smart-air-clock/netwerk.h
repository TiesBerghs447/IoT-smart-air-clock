#ifndef NETWERK_H
#define NETWERK_H

bool connectWiFi();
bool wifiConnected();

// Elke loop() aanroepen: probeert opnieuw te verbinden als de wifi wegvalt.
void maintainWiFi();

#endif
