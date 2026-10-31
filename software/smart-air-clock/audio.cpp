#include "audio.h"

#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

static HardwareSerial mp3Serial(1);
static DFRobotDFPlayerMini dfPlayer;

static bool audioPlaying = false;
static uint16_t currentTrack = 1;

void audioInit()
{
    Serial.println("[AUDIO] Init start");

    mp3Serial.begin(9600, SERIAL_8N1, 18, 17);

    delay(3000); 

    bool ok = dfPlayer.begin(mp3Serial);

    Serial.print("[AUDIO] begin() = ");
    Serial.println(ok);

    if (!ok)
    {
        Serial.println("[AUDIO] DFPlayer FAIL");
        return;
    }

    Serial.println("[AUDIO] DFPlayer OK");

    Serial.print("Files=");
    Serial.println(dfPlayer.readFileCounts());

    Serial.print("Volume=");
    Serial.println(dfPlayer.readVolume());

    dfPlayer.volume(20);
}

void audioPlay(uint16_t track)
{
    currentTrack = track;

    dfPlayer.play(track);

    audioPlaying = true;

    Serial.print("[AUDIO] Play track ");
    Serial.println(track);
}

void audioStop()
{
    dfPlayer.stop();

    audioPlaying = false;

    Serial.println("[AUDIO] Stop");
}

void audioPause()
{
    dfPlayer.pause();

    Serial.println("[AUDIO] Pause");
}

void audioResume()
{
    dfPlayer.start();

    Serial.println("[AUDIO] Resume");
}

void audioSetVolume(uint8_t volume)
{
    if (volume > 30)
    {
        volume = 30;
    }

    dfPlayer.volume(volume);

    Serial.print("[AUDIO] Volume ");
    Serial.println(volume);
}

void audioUpdate()
{
    if (!audioPlaying)
    {
        return;
    }

    if (dfPlayer.available())
    {
        uint8_t type = dfPlayer.readType();
        int value = dfPlayer.read();

        switch (type)
        {
            case DFPlayerPlayFinished:
            {
                Serial.print("[AUDIO] Track ");
                Serial.print(value);
                Serial.println(" klaar, opnieuw starten");

                delay(100);

                dfPlayer.play(currentTrack);

                break;
            }

            case DFPlayerError:
            {
                Serial.print("[AUDIO] Error ");
                Serial.println(value);

                break;
            }

            default:
                break;
        }
    }
}