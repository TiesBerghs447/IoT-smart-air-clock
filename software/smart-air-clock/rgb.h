#ifndef RGB_H
#define RGB_H

#include <Arduino.h>

class RGB
{
public:
    void begin();

    void setColor(
        uint8_t red,
        uint8_t green,
        uint8_t blue
    );

    void setRed();
    void setGreen();
    void setBlue();

    void setYellow();
    void setPurple();
    void setCyan();
    void setWhite();

    void off();

private:
    static const uint8_t RED_PIN = 40;
    static const uint8_t GREEN_PIN = 41;
    static const uint8_t BLUE_PIN = 42;
};

#endif