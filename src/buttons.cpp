#include "buttons.h"
#include "pins.h"
#include "config.h"

Buttons buttons;

/****************************************************
 *                INITIALIZE BUTTONS
 ****************************************************/
void Buttons::begin()
{
    pinMode(BTN_START, INPUT_PULLUP);
    pinMode(BTN_STOP, INPUT_PULLUP);
    pinMode(BTN_RESET, INPUT_PULLUP);
}

/****************************************************
 *                  UPDATE BUTTONS
 ****************************************************/
void Buttons::update()
{
    unsigned long now = millis();

    // START
    bool startReading = digitalRead(BTN_START);
    if (startReading != lastStartReading)
    {
        lastDebounceStart = now;
        lastStartReading = startReading;
    }
    if ((now - lastDebounceStart) > BUTTON_DEBOUNCE && startReading != stableStartReading)
    {
        stableStartReading = startReading;
        if (stableStartReading == LOW)
        {
            startState = true;
        }
    }

    // STOP
    bool stopReading = digitalRead(BTN_STOP);
    if (stopReading != lastStopReading)
    {
        lastDebounceStop = now;
        lastStopReading = stopReading;
    }
    if ((now - lastDebounceStop) > BUTTON_DEBOUNCE && stopReading != stableStopReading)
    {
        stableStopReading = stopReading;
        if (stableStopReading == LOW)
        {
            stopState = true;
        }
    }

    // RESET
    bool resetReading = digitalRead(BTN_RESET);
    if (resetReading != lastResetReading)
    {
        lastDebounceReset = now;
        lastResetReading = resetReading;
    }
    if ((now - lastDebounceReset) > BUTTON_DEBOUNCE && resetReading != stableResetReading)
    {
        stableResetReading = resetReading;
        if (stableResetReading == LOW)
        {
            resetState = true;
        }
    }
}

/****************************************************
 *              START BUTTON EVENT
 ****************************************************/
bool Buttons::startPressed()
{
    if (startState)
    {
        startState = false;
        return true;
    }

    return false;
}

/****************************************************
 *               STOP BUTTON EVENT
 ****************************************************/
bool Buttons::stopPressed()
{
    if (stopState)
    {
        stopState = false;
        return true;
    }

    return false;
}

/****************************************************
 *              RESET BUTTON EVENT
 ****************************************************/
bool Buttons::resetPressed()
{
    if (resetState)
    {
        resetState = false;
        return true;
    }

    return false;
}
