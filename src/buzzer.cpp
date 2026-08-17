#include "buzzer.h"
#include "pins.h"
#include "config.h"

Buzzer buzzer;

void Buzzer::begin() {
    pinMode(BUZZER_PIN, OUTPUT);
    ::noTone(BUZZER_PIN);
}

void Buzzer::beepShort() {
    ::tone(BUZZER_PIN, 2000, BUZZER_TIME);
}

void Buzzer::beepLong() {
    ::tone(BUZZER_PIN, 2000, BUZZER_TIME * 3);
}

void Buzzer::beepError() {
    for (uint8_t i = 0; i < 3; i++) {
        ::tone(BUZZER_PIN, 2000, BUZZER_TIME);
        delay(50);
        ::noTone(BUZZER_PIN);
    }
}
