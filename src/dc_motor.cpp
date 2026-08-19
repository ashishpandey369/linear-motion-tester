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
    pinMode(DC_MOTOR_PWM_PIN, OUTPUT);

    // BTS7960 is configured for one-direction operation externally:
    // R_EN = HIGH, L_EN = HIGH, RPWM is controlled by this GPIO.
    // LPWM is intentionally unused.
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(DC_MOTOR_PWM_PIN, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, 0);

    speedPercent = 0;
    machineRunning = false;
}

void DCMotor::update(bool running)
{
    machineRunning = running;

    // Reverse the potentiometer response:
    // turning the pot toward its previous maximum now reduces speed,
    // while turning it toward its previous minimum increases speed.
    uint16_t potValue = analogRead(DC_MOTOR_POT_PIN);

    uint8_t percent = map(
        potValue,
        0,
        4095,
        100,
        0);

    setSpeed(percent);
}

void DCMotor::setSpeed(uint8_t percent)
{
    speedPercent = constrain(percent, 0, 100);
    applyOutput();
}

void DCMotor::applyOutput()
{
    // The potentiometer may select speed, but the motor is allowed
    // to run only while the machine is in RUNNING state.
    if (!machineRunning || speedPercent == 0)
    {
        ledcWrite(PWM_CHANNEL, 0);
        return;
    }

    // 100% requested speed = full 8-bit PWM duty (255/255).
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
}

uint8_t DCMotor::getSpeedPercent()
{
    return speedPercent;
}
