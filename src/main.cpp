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
// Time: 13:42 IST
// Change: Integrate N20 DC motor through TB6612FNG.
//
// Required behavior:
// - Potentiometer controls requested motor speed / display RPM/frequency.
// - Pot movement alone NEVER starts the motor or timer.
// - START starts/resumes the timer and enables the N20 motor.
// - STOP freezes the timer and immediately stops the N20 motor.
// - RESET clears the timer to 00:00.000 and keeps the N20 motor stopped.
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
    dcMotor.begin();

    Serial.println("N20 + TB6612FNG initialized");
    Serial.println("Motor STBY     : GPIO33");
    Serial.println("Motor PWMA     : GPIO13");
    Serial.println("Motor AIN1     : GPIO12");
    Serial.println("Motor AIN2     : GPIO14");
    Serial.println("Motor Pot      : GPIO34");
    Serial.println("Timer START    : GPIO25");
    Serial.println("Timer STOP     : GPIO26");
    Serial.println("Timer RESET    : GPIO32");
    Serial.println("Display scale  : 0-200 RPM");
    Serial.println("--------------------------------");
}

void loop()
{
    buttons.update();

    // =================================================
    // TIMER BUTTON LOGIC
    // The timer state is the machine RUNNING state.
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

    // RESET: clear elapsed time to zero and keep machine stopped.
    if (buttons.resetPressed())
    {
        accumulatedRunTimeMs = 0;
        runStartMillis = millis();
        stopwatchRunning = false;
        dcMotor.stop();
        Serial.println("[WATCH] Reset to 00:00.000");
    }

    // =================================================
    // N20 MOTOR
    // The potentiometer sets the requested speed, but
    // dcMotor will physically drive the motor ONLY when
    // stopwatchRunning == true.
    // =================================================

    dcMotor.update(stopwatchRunning);
    uint8_t motorSpeed = dcMotor.getSpeedPercent();

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

        // Pot command 0-100% maps to display range 0-200 RPM.
        // This is the requested/commanded value, not encoder-measured RPM.
        dcDisplayRPM = map(
            motorSpeed,
            0,
            100,
            DC_DISPLAY_MIN_RPM,
            DC_DISPLAY_MAX_RPM
        );

        dcDisplayFrequency = dcDisplayRPM / 60.0f;

        display.updateRPM(dcDisplayRPM);
        display.updateFrequency(dcDisplayFrequency);
        display.updateRunningTime(totalSeconds, millisPart);

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
        Serial.print("% | Motor Output: ");
        Serial.print(stopwatchRunning ? "ON" : "OFF");
        Serial.print(" | Display RPM: ");
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
