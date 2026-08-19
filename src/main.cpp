#include <Arduino.h>

#include "config.h"
#include "pins.h"
#include "display.h"
#include "buttons.h"
#include "rpm.h"
#include "dc_motor.h"

// =====================================================
// DEVELOPMENT CHECKPOINT
// Date: 2026-08-19
// Time: 06:08 IST
// Change: Timer control moved from potentiometer state
// to dedicated START / STOP / RESET buttons.
//
// Required behavior:
// - Pot controls motor speed and display RPM/frequency only.
// - Pot movement must NOT start the timer.
// - START starts/resumes the timer.
// - STOP freezes the timer at its current value.
// - RESET clears the timer to 00:00.000.
// - RESET is moved to GPIO32.
// - Existing NEMA17/TB6600 motor code remains untouched.
// =====================================================

#define DC_DISPLAY_MIN_RPM 0
#define DC_DISPLAY_MAX_RPM 200

uint16_t dcDisplayRPM = 0;
float dcDisplayFrequency = 0.0f;

// =====================================================
// RUNNING TIME / STOPWATCH
// =====================================================

uint32_t accumulatedRunTimeMs = 0;
unsigned long runStartMillis = 0;
bool stopwatchRunning = false;

unsigned long previousMillis = 0;

void setup()
{
    Serial.begin(115200);
    delay(100);

    Serial.println();
    Serial.println("--------------------------------");
    Serial.println(PROJECT_NAME);
    Serial.println(PROJECT_VERSION);
    Serial.println("--------------------------------");

    display.begin();
    display.drawHomeScreen();

    buttons.begin();
    rpm.begin();

    // GPIO34 = potentiometer
    // GPIO13 = DC motor PWM
    dcMotor.begin();

    Serial.println("DC Motor initialized");
    Serial.println("Potentiometer : GPIO34");
    Serial.println("Motor PWM     : GPIO13");
    Serial.println("Display scale : 0-200 RPM");
    Serial.println("Timer START   : GPIO25");
    Serial.println("Timer STOP    : GPIO26");
    Serial.println("Timer RESET   : GPIO32");
    Serial.println("--------------------------------");
}

void loop()
{
    buttons.update();
    dcMotor.update();

    // =================================================
    // MOTOR SPEED / DISPLAY
    // The potentiometer controls motor speed only.
    // It has NO control over the stopwatch.
    // =================================================

    uint8_t motorSpeed = dcMotor.getSpeedPercent();

    // =================================================
    // TIMER BUTTON LOGIC
    // =================================================

    // START: begin or resume timing.
    if (buttons.startPressed())
    {
        if (!stopwatchRunning)
        {
            runStartMillis = millis();
            stopwatchRunning = true;
            Serial.println("[WATCH] Started / Resumed");
        }
    }

    // STOP: freeze the current elapsed time.
    if (buttons.stopPressed())
    {
        if (stopwatchRunning)
        {
            accumulatedRunTimeMs += millis() - runStartMillis;
            stopwatchRunning = false;
            Serial.println("[WATCH] Stopped / Frozen");
        }
    }

    // RESET: clear elapsed time to zero.
    if (buttons.resetPressed())
    {
        accumulatedRunTimeMs = 0;
        runStartMillis = millis();
        stopwatchRunning = false;
        Serial.println("[WATCH] Reset to 00:00.000");
    }

    // =================================================
    // CALCULATE CURRENT RUNNING TIME
    // =================================================

    uint32_t currentRunTimeMs = accumulatedRunTimeMs;

    if (stopwatchRunning)
    {
        currentRunTimeMs += millis() - runStartMillis;
    }

    uint32_t totalSeconds = currentRunTimeMs / 1000;
    uint16_t millisPart = currentRunTimeMs % 1000;

    // =================================================
    // DISPLAY UPDATE
    // =================================================

    if (millis() - previousMillis >= 50)
    {
        previousMillis = millis();

        // Map pot command 0-100% to display range 0-200 RPM.
        dcDisplayRPM = map(
            motorSpeed,
            0,
            100,
            DC_DISPLAY_MIN_RPM,
            DC_DISPLAY_MAX_RPM
        );

        // Frequency = displayed revolutions per second.
        dcDisplayFrequency = dcDisplayRPM / 60.0f;

        display.updateRPM(dcDisplayRPM);
        display.updateFrequency(dcDisplayFrequency);
        display.updateRunningTime(totalSeconds, millisPart);

        // Status reflects the timer state, not the pot position.
        if (stopwatchRunning)
        {
            display.updateStatus("RUNNING");
            display.updateButton("STOP");
        }
        else
        {
            display.updateStatus("READY");
            display.updateButton("START");
        }

        Serial.print("Motor Command: ");
        Serial.print(motorSpeed);
        Serial.print("% | Display RPM: ");
        Serial.print(dcDisplayRPM);
        Serial.print(" | Frequency: ");
        Serial.print(dcDisplayFrequency, 2);
        Serial.print(" Hz | Timer: ");
        Serial.print(totalSeconds / 60);
        Serial.print(":");

        if ((totalSeconds % 60) < 10)
            Serial.print("0");
        Serial.print(totalSeconds % 60);
        Serial.print(".");

        if (millisPart < 100)
            Serial.print("0");
        if (millisPart < 10)
            Serial.print("0");
        Serial.println(millisPart);
    }
}
