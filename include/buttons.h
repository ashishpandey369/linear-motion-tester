#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

class Buttons
{
public:
    void begin();

    void update();

    bool startPressed();

    bool stopPressed();

    bool resetPressed();

private:
    bool startState = false;
    bool stopState = false;
    bool resetState = false;
    bool lastStartReading = HIGH;
    bool lastStopReading = HIGH;
    bool lastResetReading = HIGH;
    bool stableStartReading = HIGH;
    bool stableStopReading = HIGH;
    bool stableResetReading = HIGH;
    unsigned long lastDebounceStart = 0;
    unsigned long lastDebounceStop = 0;
    unsigned long lastDebounceReset = 0;
};

extern Buttons buttons;

#endif
