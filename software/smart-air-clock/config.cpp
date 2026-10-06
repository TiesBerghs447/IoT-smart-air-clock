#include "config.h"

Config deviceConfig;

void loadDefaultConfig()
{
    deviceConfig.co2GoodMax = 800;
    deviceConfig.co2WarningMax = 1200;

    deviceConfig.uploadInterval = 30;
    deviceConfig.refreshInterval = 5;

    deviceConfig.audioEnabled = true;
    deviceConfig.audioVolume = 20;

    deviceConfig.alarmEnabled = true;
    deviceConfig.alarmHour = 7;
    deviceConfig.alarmMinute = 0;

    deviceConfig.displayEnabled = true;

    deviceConfig.greenEnabled = true;
    deviceConfig.orangeEnabled = true;
    deviceConfig.redEnabled = true;

    deviceConfig.deviceEnabled = true;
}