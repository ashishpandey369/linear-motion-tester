#include <Arduino.h>

#include "config.h"
#include "pins.h"
#include "display.h"
#include "buttons.h"
#include "rpm.h"
#include "dc_motor.h"

// =====================================================
// DEVELOPMENT CHECKPOINT
// Date: 2026-08-18
// Time: 01:03 IST
// Change: Restore intended 0-200 RPM display scale.
// The DC motor command remains 0-100% from the pot.
// Display RPM is a commanded/display scale, not measured RPM.
// Frequency is derived from displayed RPM / 60.
// Existing NEMA17/TB6600 and display architecture untouched.
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

    // GPIO34 = existing potentiometer
    // GPIO13 = DC motor PWM
    dcMotor.begin();

    Serial.println("DC Motor initialized");
    Serial.println("Potentiometer : GPIO34");
    Serial.println("Motor PWM     : GPIO13");
    Serial.println("Display scale : 0-200 RPM");
    Serial.println("--------------------------------");
}

void loop()
{
    buttons.update();
    dcMotor.update();

    uint8_t motorSpeed = dcMotor.getSpeedPercent();
    bool machineRunning = (motorSpeed > 0);

    // =================================================
    // STOPWATCH: follows the potentiometer-driven state.
    // =================================================

    if (machineRunning && !stopwatchRunning)
    {
        runStartMillis = millis();
        stopwatchRunning = true;
        Serial.println("[WATCH] Started");
    }
    else if (!machineRunning && stopwatchRunning)
    {
        accumulatedRunTimeMs += millis() - runStartMillis;
        stopwatchRunning = false;
        Serial.println("[WATCH] Paused");
    }

    // =================================================
    // RESET
    // =================================================

    if (buttons.resetPressed())
    {
        accumulatedRunTimeMs = 0;

        if (machineRunning)
        {
            runStartMillis = millis();
            stopwatchRunning = true;
        }
        else
        {
            stopwatchRunning = false;
        }

        Serial.println("[WATCH] Reset");
    }

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

        // Map the existing 0-100% pot command to 0-200 RPM.
        dcDisplayRPM = map(
            motorSpeed,
            0,
            100,
            DC_DISPLAY_MIN_RPM,
            DC_DISPLAY_MAX_RPM
        );

        // Display frequency = revolutions per second.
        dcDisplayFrequency = dcDisplayRPM / 60.0f;

        display.updateRPM(dcDisplayRPM);
        display.updateFrequency(dcDisplayFrequency);
        display.updateRunningTime(totalSeconds, millisPart);

        if (machineRunning)
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
        Serial.print(" Hz | Time: ");
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
