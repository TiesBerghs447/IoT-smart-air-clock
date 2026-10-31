#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>

void audioInit();

void audioPlay(uint16_t track);

void audioStop();

void audioPause();

void audioResume();

void audioSetVolume(uint8_t volume);

void audioUpdate();

#endif