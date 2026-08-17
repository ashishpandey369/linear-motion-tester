#ifndef LED_H
#define LED_H

#include <Arduino.h>

class LED {
public:
    void begin();
    void setRunning();
    void setStopped();
    void setReady();
    void setError();
    
private:
    void write(bool green, bool red);
};

extern LED led;

#endif
