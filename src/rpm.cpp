#include "rpm.h"
#include "config.h"
#include "pins.h"

#include <math.h>

RPM rpm;

/****************************************************
 *                  INITIALIZE
 ****************************************************/
void RPM::begin()
{
    pinMode(POT_RPM, INPUT);
    analogReadResolution(12);
    analogSetPinAttenuation(POT_RPM, ADC_11db);
    reset();
}

/****************************************************
 *            READ AVERAGE ADC
 ****************************************************/
uint16_t RPM::readAverageADC()
{
    uint32_t total = 0;

    for (uint8_t i = 0; i < ADC_SAMPLE_COUNT; i++)
    {
        total += analogRead(POT_RPM);
    }

    return total / ADC_SAMPLE_COUNT;
}

/****************************************************
 *                  RESET RPM
 ****************************************************/
void RPM::reset()
{
    currentRPM = 0;
    currentFrequency = 0.0f;
    rawADC = 0;
    filteredADC = 0.0f;
}

/****************************************************
 *                 UPDATE TARGET
 ****************************************************/
void RPM::update()
{
    rawADC = readAverageADC();

    if (rawADC <= ADC_DEADZONE)
    {
        rawADC = 0;
    }

    if (rawADC == 0 && filteredADC <= ADC_ZERO_SNAP)
    {
        filteredADC = 0.0f;
    }
    else
    {
        filteredADC += (rawADC - filteredADC) * ADC_SMOOTHING_ALPHA;
    }

    uint16_t effectiveADC = static_cast<uint16_t>(lroundf(filteredADC));

    if (effectiveADC <= ADC_DEADZONE)
    {
        currentRPM = 0;
        currentFrequency = 0.0f;
        return;
    }

    uint16_t mappedRPM = map(effectiveADC,
                             ADC_DEADZONE,
                             ADC_MAX,
                             MOTOR_MIN_RPM,
                             MOTOR_MAX_RPM);

    mappedRPM = constrain(mappedRPM, MOTOR_MIN_RPM, MOTOR_MAX_RPM);

    if (abs(static_cast<int>(mappedRPM) - static_cast<int>(currentRPM)) <= RPM_HYSTERESIS)
    {
        mappedRPM = currentRPM;
    }

    currentRPM = mappedRPM;
    currentFrequency = static_cast<float>(currentRPM) / 60.0f;
}

/****************************************************
 *                  GET RPM
 ****************************************************/
uint16_t RPM::getRPM()
{
    return currentRPM;
}

/****************************************************
 *             GET FREQUENCY
 ****************************************************/
float RPM::getFrequency()
{
    return currentFrequency;
}
