#include "counter.h"
#include "config.h"

Counter counter;

void Counter::begin()
{
    cycleCount = 0;
    pulseRemainder = 0;
}

void Counter::addPulses(uint32_t pulses)
{
    if (pulses == 0)
    {
        return;
    }

    pulseRemainder += pulses;

    while (pulseRemainder >= STEPS_PER_REV)
    {
        pulseRemainder -= STEPS_PER_REV;
        cycleCount++;
    }
}

void Counter::reset()
{
    cycleCount = 0;
    pulseRemainder = 0;
}

uint32_t Counter::getCount()
{
    return cycleCount;
}
