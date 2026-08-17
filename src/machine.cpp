#include "machine.h"
#include "buttons.h"
#include "rpm.h"
#include "motor.h"
#include "counter.h"
#include "display.h"
#include "buzzer.h"
#include "led.h"
#include "config.h"
#include "pins.h"

Machine machine;

void Machine::begin()
{
    state = STATE_READY;
    armed = false;
    lastDisplayUpdate = 0;
    runStartTimeMs = 0;
    accumulatedRunMs = 0;
    led.setReady();
}

void Machine::start()
{
    armed = true;

    // #region debug-point C:start-path
    Serial.printf("[DEBUG][C] start requested rpm=%u freq=%.2f\n", rpm.getRPM(), rpm.getFrequency());
    // #endregion

    if (rpm.getRPM() > 0)
    {
        motor.setRPM(rpm.getRPM());
        motor.start();
        runStartTimeMs = millis();
        state = STATE_RUNNING;
        led.setRunning();
    }
    else
    {
        state = STATE_READY;
        led.setReady();
    }
    buzzer.beepShort();
}

void Machine::stop()
{
    counter.addPulses(motor.consumeGeneratedPulses());
    accumulatedRunMs += (millis() - runStartTimeMs);
    armed = false;
    state = STATE_STOPPED;
    motor.stop();
    led.setStopped();
    buzzer.beepShort();
}

void Machine::reset()
{
    counter.addPulses(motor.consumeGeneratedPulses());
    motor.stop();
    counter.reset();
    rpm.reset();
    accumulatedRunMs = 0;
    runStartTimeMs = 0;
    armed = false;
    state = STATE_READY;
    led.setReady();
    buzzer.beepShort();
}

MachineState Machine::getState() const
{
    return state;
}

void Machine::update()
{
    buttons.update();
    rpm.update();

    if (buttons.resetPressed())
    {
        reset();
    }

    switch (state)
    {
    case STATE_READY:
        if (buttons.startPressed())
        {
            start();
        }
        if (rpm.getRPM() > 0)
        {
            start();
        }
        break;

    case STATE_RUNNING:
        if (buttons.stopPressed())
        {
            stop();
            break;
        }

        if (rpm.getRPM() == 0)
        {
            // #region debug-point C:zero-rpm-stop
            Serial.println("[DEBUG][C] rpm dropped to zero, stopping motor");
            // #endregion
            counter.addPulses(motor.consumeGeneratedPulses());
            accumulatedRunMs += (millis() - runStartTimeMs);
            motor.stop();
            state = STATE_READY;
            led.setReady();
            break;
        }

        motor.setRPM(rpm.getRPM());
        motor.update();
        counter.addPulses(motor.consumeGeneratedPulses());
        break;

    case STATE_STOPPED:
        if (buttons.startPressed())
        {
            start();
        }
        break;
    }

    if (millis() - lastDisplayUpdate >= DISPLAY_REFRESH)
    {
        lastDisplayUpdate = millis();

        unsigned long totalElapsedMs = accumulatedRunMs;
        if (state == STATE_RUNNING)
            totalElapsedMs += (millis() - runStartTimeMs);
        uint32_t elapsedSeconds = (uint32_t)(totalElapsedMs / 1000UL);
        uint16_t elapsedMillis  = (uint16_t)(totalElapsedMs % 1000UL);

        switch (state)
        {
        case STATE_READY:
            display.updateRPM(rpm.getRPM());
            display.updateFrequency(rpm.getFrequency());
            display.updateRunningTime(elapsedSeconds, elapsedMillis);
            display.updateStatus("READY");
            display.updateButton("START");
            break;

        case STATE_RUNNING:
            display.updateRPM(motor.getCurrentRPM());
            display.updateFrequency(motor.getCurrentFrequency());
            display.updateRunningTime(elapsedSeconds, elapsedMillis);
            display.updateStatus("RUNNING");
            display.updateButton("STOP");
            break;

        case STATE_STOPPED:
            display.updateRPM(rpm.getRPM());
            display.updateFrequency(rpm.getFrequency());
            display.updateRunningTime(elapsedSeconds, elapsedMillis);
            display.updateStatus("STOPPED");
            display.updateButton("START");
            break;
        }
    }
}
