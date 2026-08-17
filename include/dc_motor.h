#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include <Arduino.h>

class DCMotor
{
public:
    DCMotor();

    void begin();
    void update();

    void setSpeed(uint8_t percent);
    void stop();

    uint8_t getSpeedPercent();

private:
    uint8_t speedPercent;
};

extern DCMotor dcMotor;

#endif