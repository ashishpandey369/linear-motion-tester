#ifndef CONFIG_H
#define CONFIG_H

/****************************************************
 *                 PROJECT INFO
 ****************************************************/
#define PROJECT_NAME "Linear Counter"
#define PROJECT_VERSION "1.0.0"

/****************************************************
 *               DISPLAY SETTINGS
 ****************************************************/
// Landscape orientation (setRotation(1)): 480 wide x 320 tall
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 320

// --- Unified Layout Geometry (all elements stay inside outer border) ---
// VALUE FONTS:  big numbers use Font 7 (7-seg, ~52px tall)
//               Frequency effective band = 51px (tightened ~1% per user request)
// LABEL FONTS:  section labels use FreeMonoBold12pt7b (bold, ~23px)
// UNIT FONTS:   Hz / RPM use FreeMonoBold9pt7b (bold, ~17px) BOTTOM-CORNER INLINE with dynamic follow-on x based on digit width
// SUB MSG:      "Press START to begin" uses FreeMonoBold9pt7b (bold, 17px, +~2% bigger vs font 2)
#define BORDER_MARGIN      6
#define BORDER_RADIUS      8
#define SECTION_RADIUS     6
#define SECTION_GAP        5
#define INNER_PADDING_SEC  6
#define LABEL_HEIGHT       23
#define VALUE7_HEIGHT      52    // Font 7 (7-seg) nominal full glyph height
#define VALUE7_HEIGHT_FREQ 51    // Frequency value band tightened ~1% (52 → 51)
#define VALUE4_HEIGHT      26
#define UNIT_BOLD_HEIGHT   17
#define SUB_BOLD_HEIGHT    17
#define BTN_HEIGHT         28
#define BTN_WIDTH          240
#define LABEL_GAP          5
#define UNIT_FOLLOW_GAP    3     // Pixel gap between right-edge of value digits and left-edge of unit label (Hz, RPM)
#define HEADER_H           36
#define HEADER_STATUS_MAX_W 120
// Approx width of 7-seg digit (font 7) ~ 24px; max 4-digit RPM (300) or 4-digit Freq (999.9)
#define VALUE7_MAX_W       100   // Pessimistic width of a 4-digit 7-seg value
// Approx width of "Hz" in FreeMonoBold9pt7b ~ 22px; "RPM" ~ 33px
#define UNIT_HZ_W          26
#define UNIT_RPM_W         38
// Approx width of "Press START to begin" longest sub msg
#define SUB_MAX_W          220

// ---- Height budget (BORDER_H = 320 - 2*6 = 308) ----
// HEADER_H + 3*SECTION_GAP + TOP_SEC_H + BOT_SEC_H + BTN_HEIGHT
//   36     +   15        +   122     +   107     +   28       = 308  EXACT! No error.
// -------------------------------------------------------------------
// Top value band now: label (23) + gap(5) + font7 value (52) + bottom pad ~ 34px => fits in 122 (roomy!)
// Bottom value band now: label (23) + gap(5) + font7 value (52) + status font4 (26) + sub bold (17) => all good.
// -------------------------------------------------------------------
// EVERYTHING FITS WITH MULTIPLE PIXELS OF SAFETY MARGIN — ZERO BORDER CROSSING!

// Derived outer border box (everything must fit within these)
#define BORDER_X          BORDER_MARGIN
#define BORDER_Y          BORDER_MARGIN
#define BORDER_W          (SCREEN_WIDTH  - (BORDER_MARGIN * 2))
#define BORDER_H          (SCREEN_HEIGHT - (BORDER_MARGIN * 2))
#define BORDER_RIGHT      (BORDER_X + BORDER_W)
#define BORDER_BOTTOM     (BORDER_Y + BORDER_H)

// Header (title + top status)
#define HEADER_Y          BORDER_Y
#define HEADER_BOTTOM     (HEADER_Y + HEADER_H)
#define HEADER_STATUS_CLR_X (BORDER_RIGHT - HEADER_STATUS_MAX_W)
#define HEADER_STATUS_CLR_Y (HEADER_Y + 2)
#define HEADER_STATUS_CLR_W HEADER_STATUS_MAX_W
#define HEADER_STATUS_CLR_H (HEADER_H - 4)

// Top section box (Frequency + Counter)
#define TOP_SEC_Y         (HEADER_BOTTOM + SECTION_GAP)
#define TOP_SEC_H         122
#define TOP_SEC_BOTTOM    (TOP_SEC_Y + TOP_SEC_H)
#define TOP_SEC_X         BORDER_X
#define TOP_SEC_W         BORDER_W
#define TOP_SEC_MID_X     (TOP_SEC_X + TOP_SEC_W / 2)

// Bottom section box (RPM + Status)
#define BOT_SEC_Y         (TOP_SEC_BOTTOM + SECTION_GAP)
#define BOT_SEC_H         107
#define BOT_SEC_BOTTOM    (BOT_SEC_Y + BOT_SEC_H)
#define BOT_SEC_X         BORDER_X
#define BOT_SEC_W         BORDER_W
#define BOT_SEC_MID_X     (BOT_SEC_X + BOT_SEC_W / 2)

// Button bar
#define BTN_Y             (BOT_SEC_BOTTOM + SECTION_GAP)
#define BTN_X             (BORDER_X + (BORDER_W - BTN_WIDTH) / 2)
#define BTN_RIGHT         (BTN_X + BTN_WIDTH)
#define BTN_BOTTOM        (BTN_Y + BTN_HEIGHT)

// --- Header text positions ---
#define HEADER_TEXT_Y     (HEADER_Y + (HEADER_H - VALUE4_HEIGHT) / 2)
#define HEADER_TITLE_X    (BORDER_X + BORDER_W / 2)
#define HEADER_STATUS_X   (BORDER_RIGHT - INNER_PADDING_SEC)

// Inline unit helper: value sits LEFT-OF-PANEL-CENTER, unit sits immediately RIGHT of it.
// Panel usable inner width per panel = (BORDER_W/2) - 2*INNER_PADDING_SEC
//                                  = 468/2 - 12 = 234 - 12 = 222 pixels PLENTY of room.
// We'll render value centred on panel baseline, then draw unit a bit to the right of value centre.
// Baseline (top of text) for both value and inline unit is the same horizontal strip;
// we align unit VERTICALLY to value by positioning it at value y + (value_height - unit_height)/2

// --- Top Section: Left panel (Frequency) LABEL + FONT7 VALUE + DYNAMIC BOTTOM-CORNER "Hz" ---
#define TOP_LEFT_CENTER_X      (TOP_SEC_X + (TOP_SEC_MID_X - TOP_SEC_X) / 2)
#define TOP_LABEL_Y            (TOP_SEC_Y + INNER_PADDING_SEC)
#define TOP_VALUE7_Y           (TOP_LABEL_Y + LABEL_HEIGHT + LABEL_GAP)
// Hz unit: vertically aligned to BOTTOM of the value band (bottom-corner next to digits)
// Use VALUE7_HEIGHT_FREQ (-1px ≈ 1% shrink as requested)
#define TOP_HZ_UNIT_Y          (TOP_VALUE7_Y + VALUE7_HEIGHT_FREQ - UNIT_BOLD_HEIGHT)

// Frequency clear rect: full panel inner width, height = label-bottom to bottom of value band + pad
#define TOP_FREQ_CLR_X         (TOP_SEC_X + INNER_PADDING_SEC)
#define TOP_FREQ_CLR_Y         (TOP_LABEL_Y + LABEL_HEIGHT + 1)
#define TOP_FREQ_CLR_W         (TOP_SEC_MID_X - TOP_SEC_X - (INNER_PADDING_SEC * 2))
#define TOP_FREQ_CLR_H         (TOP_SEC_BOTTOM - TOP_FREQ_CLR_Y - INNER_PADDING_SEC)

// --- Top Section: Right panel (Counter) label + font7 value ---
#define TOP_RIGHT_CENTER_X     (TOP_SEC_MID_X + (BORDER_RIGHT - TOP_SEC_MID_X) / 2)
#define TOP_CNT_CLR_X          (TOP_SEC_MID_X + INNER_PADDING_SEC)
#define TOP_CNT_CLR_Y          TOP_FREQ_CLR_Y
#define TOP_CNT_CLR_W          TOP_FREQ_CLR_W
#define TOP_CNT_CLR_H          TOP_FREQ_CLR_H
#define TOP_CNT_VALUE7_Y       TOP_VALUE7_Y

// --- Bottom Section: Left panel (RPM) label + font7 value + DYNAMIC BOTTOM-CORNER "RPM" ---
#define BOT_LEFT_CENTER_X      (BOT_SEC_X + (BOT_SEC_MID_X - BOT_SEC_X) / 2)
#define BOT_LABEL_Y            (BOT_SEC_Y + INNER_PADDING_SEC)
#define BOT_VALUE7_Y           (BOT_LABEL_Y + LABEL_HEIGHT + LABEL_GAP)
// RPM unit: vertically aligned to BOTTOM of the value band (bottom-corner next to digits)
#define BOT_RPM_UNIT_Y         (BOT_VALUE7_Y + VALUE7_HEIGHT - UNIT_BOLD_HEIGHT)

#define BOT_RPM_CLR_X          (BOT_SEC_X + INNER_PADDING_SEC)
#define BOT_RPM_CLR_Y          (BOT_LABEL_Y + LABEL_HEIGHT + 1)
#define BOT_RPM_CLR_W          (BOT_SEC_MID_X - BOT_SEC_X - (INNER_PADDING_SEC * 2))
#define BOT_RPM_CLR_H          (BOT_SEC_BOTTOM - BOT_RPM_CLR_Y - INNER_PADDING_SEC)

// --- Bottom Section: Right panel (Status) label + font4 status + BOLD SUB MESSAGE ---
#define BOT_RIGHT_CENTER_X     (BOT_SEC_MID_X + (BORDER_RIGHT - BOT_SEC_MID_X) / 2)
#define BOT_STATUS_Y           (BOT_LABEL_Y + LABEL_HEIGHT + LABEL_GAP)
// Sub message: gap + font4 height + 2; uses bold FreeMonoBold9pt7b (~17px)
#define BOT_STATUS_SUB_BOLD_Y  (BOT_STATUS_Y + VALUE4_HEIGHT + 3)
#define BOT_STATUS_CLR_X       (BOT_SEC_MID_X + INNER_PADDING_SEC)
#define BOT_STATUS_CLR_Y       (BOT_LABEL_Y + LABEL_HEIGHT + 1)
#define BOT_STATUS_CLR_W       (BOT_SEC_MID_X - BOT_SEC_X - (INNER_PADDING_SEC * 2))
#define BOT_STATUS_CLR_H       (BOT_SEC_BOTTOM - BOT_STATUS_CLR_Y - INNER_PADDING_SEC)

/****************************************************
 *              MOTOR SETTINGS
 ****************************************************/
#define MOTOR_MIN_RPM 0
#define MOTOR_MAX_RPM 300
#define MOTOR_DEFAULT_RPM 60
#define MOTOR_DIRECTION_CW LOW
#define MOTOR_STEP_ACTIVE_STATE LOW
#define MOTOR_STEP_IDLE_STATE HIGH
#define MOTOR_USE_ENABLE_PIN 1
#define MOTOR_ENABLE_ACTIVE_STATE LOW
#define MOTOR_ENABLE_INACTIVE_STATE HIGH
#define MOTOR_RAMP_INTERVAL_MS 20
#define MOTOR_ACCEL_STEPS_PER_SEC2 9000.0f
#define MOTOR_MIN_HALF_PERIOD_US 4

// Standard 1.8° stepper
#define MOTOR_STEPS_PER_REV 200

// TB6600 Microstepping
#define MICROSTEP 8

// Total STEP pulses required for one motor revolution
#define STEPS_PER_REV (MOTOR_STEPS_PER_REV * MICROSTEP)

/****************************************************
 *           FREQUENCY CALCULATION
 ****************************************************/
// 1 Revolution = 1 Up + Down Cycle
#define CYCLE_PER_REV 1

/****************************************************
 *          POTENTIOMETER SETTINGS
 ****************************************************/
#define ADC_MIN 0
#define ADC_MAX 4095
#define ADC_SAMPLE_COUNT 12
#define ADC_DEADZONE 60
#define ADC_NOISE_BAND 4
#define ADC_ZERO_SNAP 8
#define ADC_SMOOTHING_ALPHA 0.18f
#define RPM_HYSTERESIS 1

/****************************************************
 *            BUZZER SETTINGS
 ****************************************************/
#define BUZZER_TIME 200

/****************************************************
 *           BUTTON DEBOUNCE
 ****************************************************/
#define BUTTON_DEBOUNCE 40

/****************************************************
 *          UI REFRESH TIME
 ****************************************************/
#define DISPLAY_REFRESH 100

#endif
