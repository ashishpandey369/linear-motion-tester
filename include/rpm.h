#ifndef RPM_H
#define RPM_H

#include <Arduino.h>

class RPM
{
public:
    void begin();
    void update();
    void reset();
    uint16_t getRPM();
    float getFrequency();
    uint16_t readAverageADC();

private:
    uint16_t currentRPM = 0;
    float currentFrequency = 0.0f;
    uint16_t rawADC = 0;
    float filteredADC = 0.0f;
};

extern RPM rpm;

#endif
