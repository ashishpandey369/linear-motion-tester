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
    : speedPercent(0),
      machineRunning(false)
{
}

void DCMotor::begin()
{
    pinMode(DC_MOTOR_STBY_PIN, OUTPUT);
    pinMode(DC_MOTOR_PWM_PIN, OUTPUT);
    pinMode(DC_MOTOR_AIN1_PIN, OUTPUT);
    pinMode(DC_MOTOR_AIN2_PIN, OUTPUT);

    // Default to forward direction, but keep the driver in standby.
    digitalWrite(DC_MOTOR_AIN1_PIN, HIGH);
    digitalWrite(DC_MOTOR_AIN2_PIN, LOW);
    digitalWrite(DC_MOTOR_STBY_PIN, LOW);

    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(DC_MOTOR_PWM_PIN, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, 0);

    speedPercent = 0;
    machineRunning = false;
}

void DCMotor::update(bool running)
{
    machineRunning = running;

    // The potentiometer always determines the requested speed,
    // but the motor output is allowed only while the machine is RUNNING.
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
    speedPercent = constrain(percent, 0, 100);
    applyOutput();
}

void DCMotor::applyOutput()
{
    if (!machineRunning || speedPercent == 0)
    {
        digitalWrite(DC_MOTOR_STBY_PIN, LOW);
        ledcWrite(PWM_CHANNEL, 0);
        return;
    }

    digitalWrite(DC_MOTOR_STBY_PIN, HIGH);

    uint8_t duty = map(
        speedPercent,
        0,
        100,
        0,
        255);

    ledcWrite(PWM_CHANNEL, duty);
}

void DCMotor::stop()
{
    machineRunning = false;
    ledcWrite(PWM_CHANNEL, 0);
    digitalWrite(DC_MOTOR_STBY_PIN, LOW);
}

uint8_t DCMotor::getSpeedPercent()
{
    return speedPercent;
}
