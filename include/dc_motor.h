#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include <Arduino.h>

class DCMotor
{
public:
    DCMotor();

    void begin();
    void update(bool machineRunning);

    void setSpeed(uint8_t percent);
    void stop();

    uint8_t getSpeedPercent();

private:
    uint8_t speedPercent;
    bool machineRunning;
    void applyOutput();
};

extern DCMotor dcMotor;

#endif
