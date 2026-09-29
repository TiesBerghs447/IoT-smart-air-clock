#include "sensors.h"
#include "display.h"

void setup()
{
    Serial.begin(115200);

    initSensors();
    initDisplay();
}

void loop()
{
    readSensors();

    updateDisplay();

    delay(2000);
}