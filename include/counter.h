#ifndef COUNTER_H
#define COUNTER_H

#include <Arduino.h>

class Counter
{
public:
    void begin();
    void addPulses(uint32_t pulses);
    void reset();
    uint32_t getCount();

private:
    uint32_t cycleCount = 0;
    uint32_t pulseRemainder = 0;
};

extern Counter counter;

#endif
