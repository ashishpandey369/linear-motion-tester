#include "motor.h"
#include "pins.h"
#include "config.h"

#include <driver/gpio.h>
#include <math.h>

Motor motor;

namespace
{
    hw_timer_t *stepTimer = nullptr;
    portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
    Motor *motorInstance = nullptr;
    // #region debug-point A:pulse-report-state
    unsigned long lastMotorDebugReportMs = 0;
    // #endregion
}

Motor::Motor()
    : generatedPulses(0),
      stepPinHigh(MOTOR_STEP_IDLE_STATE == HIGH),
      running(false),
      driverEnabled(false),
      targetRPM(0),
      targetStepsPerSecond(0.0f),
      currentStepsPerSecond(0.0f),
      lastRampUpdate(0)
{
}

void Motor::begin()
{
    pinMode(MOTOR_STEP, OUTPUT);
    pinMode(MOTOR_DIR, OUTPUT);
    if (MOTOR_USE_ENABLE_PIN)
    {
        pinMode(MOTOR_EN, OUTPUT);
    }
    else
    {
        pinMode(MOTOR_EN, INPUT);
    }

    digitalWrite(MOTOR_STEP, MOTOR_STEP_IDLE_STATE);
    digitalWrite(MOTOR_DIR, MOTOR_DIRECTION_CW);
    disableDriver();

    motorInstance = this;
    stepTimer = timerBegin(0, 80, true);
    timerAttachInterrupt(stepTimer, &Motor::onStepTimer, true);
    timerAlarmWrite(stepTimer, 1000, true);
    timerAlarmDisable(stepTimer);
    lastRampUpdate = millis();
}

void Motor::start()
{
    running = true;
    lastRampUpdate = millis();
}

void Motor::stop()
{
    running = false;
    targetRPM = 0;
    targetStepsPerSecond = 0.0f;
    currentStepsPerSecond = 0.0f;
    applyStepRate(0.0f);
    disableDriver();
}

void Motor::setRPM(uint16_t rpm)
{
    targetRPM = constrain(rpm, MOTOR_MIN_RPM, MOTOR_MAX_RPM);
    targetStepsPerSecond = (static_cast<float>(targetRPM) * STEPS_PER_REV) / 60.0f;

    // #region debug-point C:target-rpm
    static uint16_t lastLoggedTargetRPM = UINT16_MAX;
    if (targetRPM != lastLoggedTargetRPM)
    {
        lastLoggedTargetRPM = targetRPM;
        Serial.printf("[DEBUG][C] setRPM targetRPM=%u targetSPS=%.2f\n", targetRPM, targetStepsPerSecond);
    }
    // #endregion
}

void Motor::update()
{
    if (!running)
    {
        return;
    }

    unsigned long now = millis();
    float deltaSeconds = (now - lastRampUpdate) / 1000.0f;

    if (deltaSeconds <= 0.0f || now - lastRampUpdate < MOTOR_RAMP_INTERVAL_MS)
    {
        return;
    }

    lastRampUpdate = now;

    float maxDelta = MOTOR_ACCEL_STEPS_PER_SEC2 * deltaSeconds;

    if (currentStepsPerSecond < targetStepsPerSecond)
    {
        currentStepsPerSecond = min(currentStepsPerSecond + maxDelta, targetStepsPerSecond);
    }
    else if (currentStepsPerSecond > targetStepsPerSecond)
    {
        currentStepsPerSecond = max(currentStepsPerSecond - maxDelta, targetStepsPerSecond);
    }

    applyStepRate(currentStepsPerSecond);
}

bool Motor::isRunning()
{
    return running;
}

uint16_t Motor::getCurrentRPM()
{
    return static_cast<uint16_t>(lroundf((currentStepsPerSecond * 60.0f) / STEPS_PER_REV));
}

float Motor::getCurrentFrequency()
{
    return static_cast<float>(getCurrentRPM()) / 60.0f;
}

uint32_t Motor::consumeGeneratedPulses()
{
    uint32_t pulses = 0;

    portENTER_CRITICAL(&timerMux);
    pulses = generatedPulses;
    generatedPulses = 0;
    portEXIT_CRITICAL(&timerMux);

    // #region debug-point A:pulse-report-state
    unsigned long now = millis();
    if (now - lastMotorDebugReportMs >= 500)
    {
        lastMotorDebugReportMs = now;
        Serial.printf("[DEBUG][A] pulses=%lu running=%u enabled=%u currentRPM=%u targetRPM=%u currentSPS=%.2f targetSPS=%.2f\n",
                      static_cast<unsigned long>(pulses),
                      running ? 1 : 0,
                      driverEnabled ? 1 : 0,
                      getCurrentRPM(),
                      targetRPM,
                      currentStepsPerSecond,
                      targetStepsPerSecond);
    }
    // #endregion

    return pulses;
}

void IRAM_ATTR Motor::onStepTimer()
{
    if (motorInstance == nullptr || !motorInstance->running)
    {
        gpio_set_level(static_cast<gpio_num_t>(MOTOR_STEP), MOTOR_STEP_IDLE_STATE);
        return;
    }

    motorInstance->stepPinHigh = !motorInstance->stepPinHigh;
    gpio_set_level(static_cast<gpio_num_t>(MOTOR_STEP), motorInstance->stepPinHigh ? HIGH : LOW);

    bool activeLevelHigh = (MOTOR_STEP_ACTIVE_STATE == HIGH);
    if (motorInstance->stepPinHigh == activeLevelHigh)
    {
        motorInstance->generatedPulses++;
    }
}

void Motor::applyStepRate(float stepsPerSecond)
{
    portENTER_CRITICAL(&timerMux);

    if (!running || stepsPerSecond <= 0.5f)
    {
        stepPinHigh = (MOTOR_STEP_IDLE_STATE == HIGH);
        gpio_set_level(static_cast<gpio_num_t>(MOTOR_STEP), MOTOR_STEP_IDLE_STATE);
        timerAlarmDisable(stepTimer);
        portEXIT_CRITICAL(&timerMux);
        disableDriver();
        // #region debug-point B:disable-stepping
        Serial.printf("[DEBUG][B] stepping disabled running=%u requestedSPS=%.2f\n",
                      running ? 1 : 0,
                      stepsPerSecond);
        // #endregion
        return;
    }

    uint32_t halfPeriodMicros = static_cast<uint32_t>(lroundf(500000.0f / stepsPerSecond));
    if (halfPeriodMicros < MOTOR_MIN_HALF_PERIOD_US)
    {
        halfPeriodMicros = MOTOR_MIN_HALF_PERIOD_US;
    }

    timerAlarmWrite(stepTimer, halfPeriodMicros, true);
    timerAlarmEnable(stepTimer);
    portEXIT_CRITICAL(&timerMux);

    enableDriver();

    // #region debug-point B:step-timing
    static uint32_t lastLoggedHalfPeriodMicros = 0;
    if (halfPeriodMicros != lastLoggedHalfPeriodMicros)
    {
        lastLoggedHalfPeriodMicros = halfPeriodMicros;
        Serial.printf("[DEBUG][B] stepping enabled halfPeriodUs=%lu stepsPerSecond=%.2f\n",
                      static_cast<unsigned long>(halfPeriodMicros),
                      stepsPerSecond);
    }
    // #endregion
}

void Motor::enableDriver()
{
    if (MOTOR_USE_ENABLE_PIN)
    {
        digitalWrite(MOTOR_EN, MOTOR_ENABLE_ACTIVE_STATE);
    }
    driverEnabled = true;
}

void Motor::disableDriver()
{
    if (MOTOR_USE_ENABLE_PIN)
    {
        digitalWrite(MOTOR_EN, MOTOR_ENABLE_INACTIVE_STATE);
    }
    driverEnabled = false;
}
