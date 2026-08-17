#include "led.h"
#include "pins.h"

LED led;

void LED::begin() {
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    write(false, false);
}

void LED::setRunning() {
    write(true, false);
}

void LED::setStopped() {
    write(false, true);
}

void LED::setReady() {
    write(true, true);
}

void LED::setError() {
    write(false, true);
}

void LED::write(bool green, bool red) {
    digitalWrite(LED_GREEN, green ? HIGH : LOW);
    digitalWrite(LED_RED, red ? HIGH : LOW);
}
