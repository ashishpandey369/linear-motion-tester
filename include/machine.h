#ifndef MACHINE_H
#define MACHINE_H

#include <Arduino.h>

enum MachineState {
    STATE_READY,
    STATE_RUNNING,
    STATE_STOPPED
};

class Machine {
public:
    void begin();
    void update();
    
    void start();
    void stop();
    void reset();
    
    MachineState getState() const;
    
private:
    MachineState state;
    bool armed;
    unsigned long lastDisplayUpdate;
    unsigned long runStartTimeMs;
    unsigned long accumulatedRunMs;
};

extern Machine machine;

#endif
