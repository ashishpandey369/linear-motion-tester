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
 *          TB6612FNG - N20 DC MOTOR
 ****************************************************/
#define DC_MOTOR_STBY_PIN 33
#define DC_MOTOR_PWM_PIN 13
#define DC_MOTOR_AIN1_PIN 12
#define DC_MOTOR_AIN2_PIN 14
#define DC_MOTOR_POT_PIN 34

/****************************************************
 *       LEGACY OUTPUT DEFINITIONS
 ****************************************************/
// These definitions are retained so the legacy buzzer/LED
// source files continue to compile. GPIO12/13/14 are now
// reserved for TB6612FNG motor control and must not be used
// for the legacy LED/buzzer hardware during motor operation.
#define BUZZER_PIN 14
#define LED_GREEN 13
#define LED_RED 12

#endif
