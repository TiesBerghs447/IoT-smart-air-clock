#ifndef BUTTONS_H
#define BUTTONS_H

void initButtons();
void updateButtons();

bool powerPressed();
bool powerLongPressed();

bool setPressed();
bool setLongPressed();

bool upPressed();
bool upHeld();

#endif