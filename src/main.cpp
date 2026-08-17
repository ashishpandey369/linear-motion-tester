#include <Arduino.h>

#include "config.h"
#include "pins.h"
#include "display.h"
#include "buttons.h"
#include "rpm.h"
#include "dc_motor.h"

// =====================================================
// DC MOTOR DISPLAY VALUES
// =====================================================

uint16_t dcDisplayRPM = 0;
float dcDisplayFrequency = 0.0f;

// =====================================================
// RUNNING TIME / STOPWATCH
// =====================================================

uint32_t accumulatedRunTimeMs = 0;
unsigned long runStartMillis = 0;

bool stopwatchRunning = false;

// =====================================================
// DISPLAY UPDATE
// =====================================================

unsigned long previousMillis = 0;

// =====================================================
// SETUP
// =====================================================

void setup()
{
    Serial.begin(115200);

    delay(100);

    Serial.println();
    Serial.println("--------------------------------");
    Serial.println(PROJECT_NAME);
    Serial.println(PROJECT_VERSION);
    Serial.println("--------------------------------");

    // -------------------------------------------------
    // Display
    // -------------------------------------------------

    display.begin();
    display.drawHomeScreen();

    // -------------------------------------------------
    // Buttons
    // -------------------------------------------------

    buttons.begin();

    // -------------------------------------------------
    // Existing RPM module
    // DO NOT MODIFY
    // -------------------------------------------------

    rpm.begin();

    // -------------------------------------------------
    // DC Motor
    // GPIO34 = existing potentiometer
    // GPIO13 = PWM
    // -------------------------------------------------

    dcMotor.begin();

    Serial.println("DC Motor initialized");
    Serial.println("Potentiometer : GPIO34");
    Serial.println("Motor PWM     : GPIO13");

    Serial.println("--------------------------------");
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
    // -------------------------------------------------
    // Update buttons
    // -------------------------------------------------

    buttons.update();

    // -------------------------------------------------
    // DC MOTOR
    //
    // Potentiometer controls motor speed.
    // -------------------------------------------------

    dcMotor.update();

    // -------------------------------------------------
    // Get current motor state
    // -------------------------------------------------

    uint8_t motorSpeed = dcMotor.getSpeedPercent();

    bool machineRunning = (motorSpeed > 0);

    // =================================================
    // STOPWATCH STATE
    // =================================================

    if (machineRunning && !stopwatchRunning)
    {
        // Machine has just started running
        runStartMillis = millis();
        stopwatchRunning = true;

        Serial.println("[WATCH] Started");
    }

    else if (!machineRunning && stopwatchRunning)
    {
        // Machine has just stopped
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
            // Restart stopwatch from zero while machine
            // is already running.
            runStartMillis = millis();
            stopwatchRunning = true;
        }
        else
        {
            stopwatchRunning = false;
        }

        Serial.println("[WATCH] Reset");
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

        // -------------------------------------------------
        // Temporary RPM representation
        // -------------------------------------------------

        dcDisplayRPM = motorSpeed;

        dcDisplayFrequency =
            dcDisplayRPM / 60.0f;

        // -------------------------------------------------
        // Update display
        // -------------------------------------------------

        display.updateRPM(dcDisplayRPM);

        display.updateFrequency(
            dcDisplayFrequency
        );

        display.updateRunningTime(
            totalSeconds,
            millisPart
        );

        // -------------------------------------------------
        // Machine status
        // -------------------------------------------------

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

        // -------------------------------------------------
        // Serial debug
        // -------------------------------------------------

        Serial.print("Motor Speed: ");
        Serial.print(motorSpeed);

        Serial.print("% | RPM: ");
        Serial.print(dcDisplayRPM);

        Serial.print(" | Time: ");

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