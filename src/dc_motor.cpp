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

    // Fixed forward direction. The potentiometer controls speed only.
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
    // Potentiometer may request any speed, but the physical motor is
    // enabled only while the machine is in RUNNING state.
    if (!machineRunning || speedPercent == 0)
    {
        digitalWrite(DC_MOTOR_STBY_PIN, LOW);
        ledcWrite(PWM_CHANNEL, 0);
        return;
    }

    digitalWrite(DC_MOTOR_STBY_PIN, HIGH);

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
    digitalWrite(DC_MOTOR_STBY_PIN, LOW);
}

uint8_t DCMotor::getSpeedPercent()
{
    return speedPercent;
}
