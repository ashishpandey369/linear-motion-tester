#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
public:
    void begin();
    void beepShort();
    void beepLong();
    void beepError();
};

extern Buzzer buzzer;

#endif
