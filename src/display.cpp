#include "display.h"
#include "config.h"

Display display;

/****************************************************
 *                  BEGIN DISPLAY
 ****************************************************/
void Display::begin()
{
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    drawHomeScreen();
}

/****************************************************
 *               DRAW HOME SCREEN
 ****************************************************/
void Display::drawHomeScreen()
{
    tft.fillScreen(TFT_BLACK);

    tft.drawRoundRect(BORDER_X, BORDER_Y, BORDER_W, BORDER_H,
                      BORDER_RADIUS, TFT_WHITE);

    drawHeader();
    drawTopSection();
    drawBottomSection();
    drawButton();

    lastFrequency = -999.0f;
    lastRPM = 0xFFFF;
    lastMinutes = 0xFF;
    lastSeconds = 0xFF;
    lastMillis  = 0xFFFF;
    lastStatus = "";
    lastButton = "";

    updateFrequency(0.0f);
    updateRunningTime(0, 0);
    updateRPM(0);
    updateStatus("READY");
    updateButton("START");
}

/****************************************************
 *          UPDATE FREQUENCY
 ****************************************************/
void Display::updateFrequency(float frequency)
{
    currentFrequency = frequency;

    if (abs(currentFrequency - lastFrequency) < 0.05f && lastFrequency != -999.0f)
        return;
    lastFrequency = currentFrequency;

    tft.fillRect(TOP_FREQ_CLR_X, TOP_FREQ_CLR_Y,
                 TOP_FREQ_CLR_W, TOP_FREQ_CLR_H, TFT_BLACK);

    String freqStr = String(currentFrequency, 1);

    useBuiltin(7);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString(freqStr, TOP_LEFT_CENTER_X, TOP_VALUE7_Y, 7);

    // --- DYNAMIC Hz PLACEMENT: bottom-right corner of digit block, follows digit count ---
    int16_t valueW = tft.textWidth(freqStr, 7);
    int16_t hzX = TOP_LEFT_CENTER_X + (valueW / 2) + UNIT_FOLLOW_GAP;

    tft.setFreeFont(&FreeMonoBold9pt7b);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("Hz", hzX, TOP_HZ_UNIT_Y, 1);

    tft.setFreeFont(nullptr);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

/****************************************************
 *   UPDATE RUNNING TIME — MM:SS + live .mmm digits + "ms" label
 *  Flicker-free: per-zone dirty redraw.
 *  Milliseconds block uses FreeMonoBold12pt7b (LABEL_HEIGHT-sized bold
 *  font, ~35% larger than the 9pt Hz/RPM unit labels — closest step up
 *  since bitmap fonts don't support fractional 5% sizing).
 ****************************************************/
void Display::updateRunningTime(uint32_t totalSeconds, uint16_t millisPart)
{
    runningTimeSeconds = totalSeconds;
    runningTimeMillis  = millisPart;

    uint8_t  minutes = (uint8_t)(totalSeconds / 60);
    uint8_t  seconds = (uint8_t)(totalSeconds % 60);
    uint16_t ms      = (uint16_t)(millisPart % 1000);

    bool firstDraw = (lastMinutes == 0xFF && lastSeconds == 0xFF && lastMillis == 0xFFFF);
    bool mmChanged = (minutes != lastMinutes) || firstDraw;
    bool ssChanged = (seconds != lastSeconds) || firstDraw;
    bool msChanged = (ms      != lastMillis)  || firstDraw;
    if (!mmChanged && !ssChanged && !msChanged)
        return;

    char mmStr[3], ssStr[3], colonStr[2];
    snprintf(mmStr, sizeof(mmStr), "%02u", minutes);
    snprintf(ssStr, sizeof(ssStr), "%02u", seconds);
    snprintf(colonStr, sizeof(colonStr), ":");

    useBuiltin(7);
    int16_t mmW    = tft.textWidth(mmStr, 7);
    int16_t colonW = tft.textWidth(colonStr, 7);
    int16_t ssW    = tft.textWidth(ssStr, 7);
    int16_t fullW  = mmW + colonW + ssW;

    int16_t blockX = TOP_RIGHT_CENTER_X - (fullW / 2);
    int16_t mmX    = blockX;
    int16_t colonX = mmX + mmW;
    int16_t ssX    = colonX + colonW;
    int16_t valueY = TOP_CNT_VALUE7_Y;

    int16_t zoneY = valueY;
    int16_t zoneH = VALUE7_HEIGHT;

    if (firstDraw) {
        tft.fillRect(TOP_CNT_CLR_X, TOP_CNT_CLR_Y,
                     TOP_CNT_CLR_W, TOP_CNT_CLR_H, TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(colonStr, colonX, valueY, 7);
        mmChanged = true;
        ssChanged = true;
        msChanged = true;
    }

    // ---- Zone 1: MM digits ----
    if (mmChanged) {
        tft.fillRect(mmX, zoneY, mmW, zoneH, TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(mmStr, mmX, valueY, 7);
    }

    // ---- Zone 2: SS digits ----
    if (ssChanged) {
        tft.fillRect(ssX, zoneY, ssW, zoneH, TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(ssStr, ssX, valueY, 7);
    }

    // ---- Zone 3: live .mmm + "ms" label (FreeMonoBold12pt7b, bigger bold font) ----
    if (msChanged || ssChanged || mmChanged) {
        char msValStr[8];
        snprintf(msValStr, sizeof(msValStr), ".%03u", ms);
        const char *msUnitStr = "ms";

        tft.setFreeFont(&FreeMonoBold12pt7b);
        int16_t msValW  = tft.textWidth(msValStr, 1);
        int16_t msUnitW = tft.textWidth(msUnitStr, 1);
        int16_t msBlockW = msValW + 2 + msUnitW;   // small spacer between value + "ms"
        int16_t msFontH = LABEL_HEIGHT;             // matches FreeMonoBold12pt7b nominal height

        // Vertically bottom-align the ms block to the Font7 value band
        int16_t msY = valueY + VALUE7_HEIGHT - msFontH;
        int16_t msX = ssX + ssW + UNIT_FOLLOW_GAP;

        if (msX + msBlockW <= TOP_CNT_CLR_X + TOP_CNT_CLR_W - 2) {
            // Clear only the ms block rect
            int16_t clrX = (ssChanged || mmChanged) ? (ssX + ssW) : msX;
            int16_t clrW = (ssChanged || mmChanged)
                               ? (UNIT_FOLLOW_GAP + msBlockW)
                               : msBlockW;
            tft.fillRect(clrX, msY, clrW, msFontH, TFT_BLACK);

            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.drawString(msValStr, msX, msY, 1);
            tft.drawString(msUnitStr, msX + msValW + 2, msY, 1);
        }
        tft.setFreeFont(nullptr);
    }

    lastMinutes = minutes;
    lastSeconds = seconds;
    lastMillis  = ms;
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

/****************************************************
 *               UPDATE RPM
 ****************************************************/
void Display::updateRPM(uint16_t rpm)
{
    currentRPM = rpm;

    if (currentRPM == lastRPM && lastRPM != 0xFFFF)
        return;
    lastRPM = currentRPM;

    tft.fillRect(BOT_RPM_CLR_X, BOT_RPM_CLR_Y,
                 BOT_RPM_CLR_W, BOT_RPM_CLR_H, TFT_BLACK);

    String rpmStr = String(currentRPM);

    useBuiltin(7);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString(rpmStr, BOT_LEFT_CENTER_X, BOT_VALUE7_Y, 7);

    // --- DYNAMIC RPM PLACEMENT: bottom-right corner of digit block, follows digit count ---
    int16_t valueW = tft.textWidth(rpmStr, 7);
    int16_t rpmX = BOT_LEFT_CENTER_X + (valueW / 2) + UNIT_FOLLOW_GAP;

    tft.setFreeFont(&FreeMonoBold9pt7b);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("RPM", rpmX, BOT_RPM_UNIT_Y, 1);

    tft.setFreeFont(nullptr);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

/****************************************************
 *             UPDATE STATUS
 ****************************************************/
void Display::updateStatus(const String &status)
{
    machineStatus = status;

    bool changed = (machineStatus != lastStatus);
    bool firstTime = (lastStatus == "");
    if (!changed && !firstTime)
        return;
    lastStatus = machineStatus;

    tft.fillRect(BOT_STATUS_CLR_X, BOT_STATUS_CLR_Y,
                 BOT_STATUS_CLR_W, BOT_STATUS_CLR_H, TFT_BLACK);

    uint16_t statusColor = TFT_WHITE;
    if (status == "READY" || status == "RUNNING")
        statusColor = TFT_GREEN;
    else if (status == "STOPPED")
        statusColor = TFT_RED;
    else if (status == "RESET")
        statusColor = TFT_YELLOW;

    useBuiltin(4);
    tft.setTextColor(statusColor, TFT_BLACK);
    tft.drawCentreString(status, BOT_RIGHT_CENTER_X, BOT_STATUS_Y, 4);

    tft.setFreeFont(&FreeMonoBold9pt7b);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    const char *sub = "";
    if (status == "READY")        sub = "Press START to begin";
    else if (status == "RUNNING") sub = "Machine Running";
    else if (status == "STOPPED") sub = "Machine Stopped";
    else if (status == "RESET")   sub = "Counter Reset";
    if (*sub)
        tft.drawCentreString(sub, BOT_RIGHT_CENTER_X, BOT_STATUS_SUB_BOLD_Y, 1);

    tft.setFreeFont(nullptr);
    drawHeaderStatusBadge();
}

/****************************************************
 *            UPDATE BUTTON
 ****************************************************/
void Display::updateButton(const String &text)
{
    buttonText = text;

    if (buttonText == lastButton && lastButton != "")
        return;
    lastButton = buttonText;

    uint16_t btnBg = TFT_RED;
    uint16_t btnFg = TFT_WHITE;
    if (text == "START") {
        btnBg = TFT_GREEN;     btnFg = TFT_WHITE;
    } else if (text == "STOP") {
        btnBg = TFT_RED;       btnFg = TFT_WHITE;
    } else if (text == "RESET") {
        btnBg = TFT_YELLOW;    btnFg = TFT_BLACK;
    }

    tft.fillRoundRect(BTN_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT,
                      SECTION_RADIUS, btnBg);

    useBuiltin(4);
    tft.setTextColor(btnFg, btnBg);
    tft.drawCentreString(buttonText,
                         BORDER_X + BORDER_W / 2,
                         BTN_Y + (BTN_HEIGHT - VALUE4_HEIGHT) / 2, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

/****************************************************
 *               REFRESH
 ****************************************************/
void Display::refresh()
{
    updateFrequency(currentFrequency);
    updateRunningTime(runningTimeSeconds, runningTimeMillis);
    updateRPM(currentRPM);
    updateStatus(machineStatus);
    updateButton(buttonText);
}

/****************************************************
 *                DRAW HEADER (full)
 ****************************************************/
void Display::drawHeader()
{
    tft.drawFastHLine(BORDER_X, HEADER_Y, BORDER_W, TFT_WHITE);
    tft.drawFastHLine(BORDER_X, HEADER_BOTTOM, BORDER_W, TFT_WHITE);

    useBuiltin(4);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("LINEAR COUNTER", HEADER_TITLE_X, HEADER_TEXT_Y, 4);

    drawHeaderStatusBadge();
}

/****************************************************
 *           DRAW HEADER STATUS BADGE ONLY
 ****************************************************/
void Display::drawHeaderStatusBadge()
{
    tft.fillRect(HEADER_STATUS_CLR_X, HEADER_STATUS_CLR_Y,
                 HEADER_STATUS_CLR_W, HEADER_STATUS_CLR_H, TFT_BLACK);

    uint16_t statusColor = TFT_WHITE;
    if (machineStatus == "READY" || machineStatus == "RUNNING")
        statusColor = TFT_GREEN;
    else if (machineStatus == "STOPPED")
        statusColor = TFT_RED;
    else if (machineStatus == "RESET")
        statusColor = TFT_YELLOW;

    useBuiltin(4);
    tft.setTextColor(statusColor, TFT_BLACK);
    tft.drawRightString(machineStatus, HEADER_STATUS_X, HEADER_TEXT_Y, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

/****************************************************
 *            DRAW TOP SECTION
 ****************************************************/
void Display::drawTopSection()
{
    tft.drawRoundRect(TOP_SEC_X, TOP_SEC_Y, TOP_SEC_W, TOP_SEC_H,
                      SECTION_RADIUS, TFT_WHITE);
    tft.drawFastVLine(TOP_SEC_MID_X, TOP_SEC_Y, TOP_SEC_H, TFT_WHITE);

    tft.setFreeFont(&FreeMonoBold12pt7b);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("CURRENT FREQUENCY", TOP_LEFT_CENTER_X, TOP_LABEL_Y, 1);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("RUNNING TIME", TOP_RIGHT_CENTER_X, TOP_LABEL_Y, 1);

    tft.setFreeFont(nullptr);
}

/****************************************************
 *          DRAW BOTTOM SECTION
 ****************************************************/
void Display::drawBottomSection()
{
    tft.drawRoundRect(BOT_SEC_X, BOT_SEC_Y, BOT_SEC_W, BOT_SEC_H,
                      SECTION_RADIUS, TFT_WHITE);
    tft.drawFastVLine(BOT_SEC_MID_X, BOT_SEC_Y, BOT_SEC_H, TFT_WHITE);

    tft.setFreeFont(&FreeMonoBold12pt7b);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("CURRENT RPM", BOT_LEFT_CENTER_X, BOT_LABEL_Y, 1);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("MACHINE STATUS", BOT_RIGHT_CENTER_X, BOT_LABEL_Y, 1);

    tft.setFreeFont(nullptr);
}

/****************************************************
 *             DRAW BUTTON AREA
 ****************************************************/
void Display::drawButton()
{
    tft.drawRoundRect(BTN_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT,
                      SECTION_RADIUS, TFT_WHITE);
}
