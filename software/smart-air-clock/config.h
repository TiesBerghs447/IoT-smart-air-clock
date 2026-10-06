#ifndef CONFIG_H
#define CONFIG_H

struct Config
{
    int co2GoodMax;
    int co2WarningMax;

    int uploadInterval;
    int refreshInterval;

    bool audioEnabled;
    int audioVolume;

    bool alarmEnabled;
    int alarmHour;
    int alarmMinute;

    bool displayEnabled;

    bool greenEnabled;
    bool orangeEnabled;
    bool redEnabled;

    bool deviceEnabled;
};

extern Config deviceConfig;

void loadDefaultConfig();

#endif