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
#define BTN_RESET 16

/****************************************************
 *             POTENTIOMETERS
 ****************************************************/
#define POT_RPM 34
#define POT_AUX 35

/****************************************************
 *             OUTPUTS
 ****************************************************/
#define BUZZER_PIN 14

#define LED_GREEN 13
#define LED_RED 12

// =====================================================
// 5V DC MOTOR
// =====================================================

#define DC_MOTOR_PWM_PIN 13
#define DC_MOTOR_POT_PIN 34

#endif
