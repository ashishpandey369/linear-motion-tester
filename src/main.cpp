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
    // New DC Motor
    // GPIO34 = existing potentiometer
    // GPIO13 = BC547 PWM control
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
    // Buttons
    // -------------------------------------------------

    buttons.update();

    // -------------------------------------------------
    // DC MOTOR
    //
    // Reads the existing potentiometer on GPIO34
    // and controls the motor through GPIO13.
    // -------------------------------------------------

    dcMotor.update();

    // -------------------------------------------------
    // DISPLAY UPDATE
    // -------------------------------------------------

    if (millis() - previousMillis >= 100)
    {
        previousMillis = millis();

        uint8_t motorSpeed = dcMotor.getSpeedPercent();

        // -------------------------------------------------
        // Temporary display representation
        //
        // This is NOT measured motor RPM yet.
        // We currently have no RPM sensor on the DC motor.
        // -------------------------------------------------

        dcDisplayRPM = motorSpeed;

        dcDisplayFrequency = dcDisplayRPM / 60.0f;

        display.updateRPM(dcDisplayRPM);
        display.updateFrequency(dcDisplayFrequency);

        // -------------------------------------------------
        // STATUS
        // -------------------------------------------------

        if (motorSpeed == 0)
        {
            display.updateStatus("READY");
            display.updateButton("START");
        }
        else
        {
            display.updateStatus("RUNNING");
            display.updateButton("STOP");
        }

        // -------------------------------------------------
        // SERIAL DEBUG
        // -------------------------------------------------

        Serial.print("Motor Speed : ");
        Serial.print(motorSpeed);

        Serial.print("%   Display RPM : ");
        Serial.print(dcDisplayRPM);

        Serial.print("   Frequency : ");
        Serial.print(dcDisplayFrequency);

        Serial.println(" Hz");
    }
}