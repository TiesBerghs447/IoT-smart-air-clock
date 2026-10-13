#include "rgb.h"

void RGB::begin()
{
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    off();
}

void RGB::setColor(
    uint8_t red,
    uint8_t green,
    uint8_t blue
)
{
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
}

void RGB::setRed()
{
    setColor(255, 0, 0);
}

void RGB::setGreen()
{
    setColor(0, 255, 0);
}

void RGB::setBlue()
{
    setColor(0, 0, 255);
}

void RGB::setYellow()
{
    setColor(255, 255, 0);
}

void RGB::setPurple()
{
    setColor(255, 0, 255);
}

void RGB::setCyan()
{
    setColor(0, 255, 255);
}

void RGB::setWhite()
{
    setColor(255, 255, 255);
}

void RGB::off()
{
    setColor(0, 0, 0);
}