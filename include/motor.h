#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor
{
public:
    Motor();

    void begin();

    void start();
    void stop();
    void setRPM(uint16_t rpm);
    void update();
    bool isRunning();
    uint16_t getCurrentRPM();
    float getCurrentFrequency();
    uint32_t consumeGeneratedPulses();

private:
    volatile uint32_t generatedPulses;
    volatile bool stepPinHigh;
    volatile bool running;
    bool driverEnabled;
    uint16_t targetRPM;
    float targetStepsPerSecond;
    float currentStepsPerSecond;
    unsigned long lastRampUpdate;

    static void IRAM_ATTR onStepTimer();
    void applyStepRate(float stepsPerSecond);
    void enableDriver();
    void disableDriver();
};

extern Motor motor;

#endif
