#include "dc_motor.h"
#include "pins.h"

DCMotor dcMotor;

namespace
{
    constexpr uint8_t PWM_CHANNEL = 0;
    constexpr uint32_t PWM_FREQUENCY = 1000;
    constexpr uint8_t PWM_RESOLUTION = 8;
}

DCMotor::DCMotor()
    : speedPercent(0)
{
}

void DCMotor::begin()
{
    pinMode(DC_MOTOR_PWM_PIN, OUTPUT);

    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(DC_MOTOR_PWM_PIN, PWM_CHANNEL);

    ledcWrite(PWM_CHANNEL, 0);

    speedPercent = 0;
}

void DCMotor::update()
{
    uint16_t potValue = analogRead(DC_MOTOR_POT_PIN);

    uint8_t percent = map(
        potValue,
        0,
        4095,
        0,
        100);

    setSpeed(percent);
}

void DCMotor::setSpeed(uint8_t percent)
{
    percent = constrain(percent, 0, 100);

    speedPercent = percent;

    uint8_t duty = map(
        percent,
        0,
        100,
        0,
        255);

    ledcWrite(PWM_CHANNEL, duty);
}

void DCMotor::stop()
{
    speedPercent = 0;

    ledcWrite(PWM_CHANNEL, 0);
}

uint8_t DCMotor::getSpeedPercent()
{
    return speedPercent;
}