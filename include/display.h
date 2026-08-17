#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <TFT_eSPI.h>

class Display
{
public:
    // Initialization
    void begin();

    // Draw complete home screen
    void drawHomeScreen();

    // Individual UI updates
    void updateFrequency(float frequency);
    void updateRPM(uint16_t rpm);
    void updateRunningTime(uint32_t totalSeconds, uint16_t millisPart);
    void updateStatus(const String &status);
    void updateButton(const String &text);

    // Refresh complete screen
    void refresh();

private:
    TFT_eSPI tft = TFT_eSPI();

    // Drawing Functions
    void drawHeader();
    void drawHeaderStatusBadge();
    void drawTopSection();
    void drawBottomSection();
    void drawButton();

    // Font helpers (avoid accidentally leaving a FreeFont active)
    void useBuiltin(uint8_t font) { tft.setFreeFont(nullptr); tft.setTextFont(font); }

    // Cached Values
    float currentFrequency = 0.0f;
    uint16_t currentRPM = 0;
    uint32_t runningTimeSeconds = 0;
    uint16_t runningTimeMillis = 0;
    String machineStatus = "READY";
    String buttonText = "START";

    // Dirty-change tracking: only redraw when value actually changes
    float lastFrequency = -999.0f;
    uint16_t lastRPM = 0xFFFF;
    // Per-zone dirty tracking for running time (avoids full-box flicker)
    uint8_t  lastMinutes = 0xFF;
    uint8_t  lastSeconds = 0xFF;
    uint16_t lastMillis  = 0xFFFF;
    String lastStatus = "";
    String lastButton = "";
};

// Global Display Object
extern Display display;

#endif