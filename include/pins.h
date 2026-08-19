#ifndef PINS_H
#define PINS_H

/****************************************************
 *              TFT DISPLAY (SPI)
 ****************************************************/
#define TFT_MOSI 23
#define TFT_MISO 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 4

/****************************************************
 *             TB6600 DRIVER
 ****************************************************/
#define MOTOR_STEP 22
#define MOTOR_DIR 27
#define MOTOR_EN 21

/****************************************************
 *             PUSH BUTTONS
 ****************************************************/
#define BTN_START 25
#define BTN_STOP 26
#define BTN_RESET 32

/****************************************************
 *             POTENTIOMETERS
 ****************************************************/
#define POT_RPM 34
#define POT_AUX 35

/****************************************************
 *          BTS7960 - 12V DC MOTOR
 ****************************************************/
// One-direction operation: RPWM is the only ESP32 control signal.
// R_EN and L_EN are held HIGH externally at the BTS7960 module.
#define DC_MOTOR_PWM_PIN 13
#define DC_MOTOR_POT_PIN 34

/****************************************************
 *       LEGACY OUTPUT DEFINITIONS
 ****************************************************/
// Retained for source compatibility. GPIO13 is now reserved for
// BTS7960 RPWM, so the legacy LED/buzzer hardware must not share
// these pins during motor operation.
#define BUZZER_PIN 14
#define LED_GREEN 13
#define LED_RED 12

#endif
