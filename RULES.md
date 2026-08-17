# PROJECT RULES

## Linear Counter
Version: 1.0

---

# 1. General Rules

- Always keep the project modular.
- One module should have one responsibility.
- Never write everything inside `main.cpp`.
- Every module must have its own `.h` and `.cpp` file.
- Keep the code clean, readable and well commented.
- Follow the existing project structure.
- Never duplicate code.
- Reuse functions whenever possible.

- After writing files update the memory.md as per modification and then only move to the next 
- never go to complex solving way
---

# 2. Documentation Rules

Documentation is mandatory.

After every completed feature or update:

✔ Update MEMORY.md

✔ Update CHANGELOG.md

If architecture changes:

✔ Update ARCHITECTURE.md

If a new design decision is made:

✔ Update DECISIONS.md

If a new project phase starts:

✔ Update PHASES.md

Documentation must always match the latest source code.

---

# 3. MEMORY.md Rules

MEMORY.md is the project's live status file.

It must always contain:

- Current Phase
- Current Module
- Current File
- Completed Files
- Pending Files
- Locked Hardware
- Locked Pin Mapping
- Locked Decisions
- Next Task

MEMORY.md must be updated after every coding session.

---

# 4. Coding Rules

Always use:

- Classes
- Header files
- Source files
- Modular programming
- Meaningful variable names
- Functions for reusable code

Never:

- Hardcode GPIO numbers
- Duplicate variables
- Create unnecessary global variables
- Write large functions
- Leave unused code

---

# 5. Display Rules

Use only:

- TFT_eSPI

Do not use:

- LVGL
- SquareLine
- Custom widget libraries

Display responsibilities:

- Draw UI
- Update UI
- No machine logic
- No motor logic

Display must never control hardware.

---

# 6. Motor Rules

Motor module is responsible only for:

- STEP
- DIR
- ENABLE
- RPM
- Motion

Motor module must never:

- Draw on display
- Read buttons
- Calculate UI values

---

# 7. Button Rules

Always use:

INPUT_PULLUP

Always debounce buttons.

Button module must only report button events.

No display logic.

No motor logic.

---

# 8. RPM Rules

RPM comes only from the potentiometer.

The RPM module is responsible for:

- Reading ADC
- Averaging samples
- Converting ADC to RPM
- Calculating Frequency

No display logic.

No motor logic.

---

# 9. Counter Rules

Counter module is responsible only for:

- Cycle Count
- Reset Count
- Return Count

One complete motor revolution equals one complete machine cycle.

---

# 10. Main.cpp Rules

main.cpp should only:

- Initialize modules
- Call update functions
- Coordinate modules

Never place application logic directly inside main.cpp.

---

# 11. Pin Rules

GPIO assignments are locked. Never modify any pin unless explicitly approved.

## Display SPI Pins (Do Not Use for Other Devices)

| ESP32 Pin | Purpose |
|-----------|---------|
| GPIO23    | MOSI    |
| GPIO19    | MISO    |
| GPIO18    | SCK     |
| GPIO5     | TFT CS  |
| GPIO2     | TFT DC  |
| GPIO4     | TFT RESET |

## Motor Driver

| ESP32 Pin | Function |
|-----------|----------|
| GPIO16    | STEP     |
| GPIO17    | DIR      |
| GPIO21    | ENABLE   |

## Buttons

| ESP32 Pin | Function |
|-----------|----------|
| GPIO25    | Start    |
| GPIO26    | Stop     |
| GPIO27    | Reset    |

## Potentiometers

| ESP32 Pin | Function |
|-----------|----------|
| GPIO34    | RPM Potentiometer |
| GPIO35    | Reserved for future (Load Cell / Second Pot / Analog Input) |

## Outputs

| ESP32 Pin | Function |
|-----------|----------|
| GPIO14    | Buzzer   |
| GPIO13    | Green LED |
| GPIO12    | Red LED  |

---

# 12. Libraries

Allowed

- TFT_eSPI
- AccelStepper
- Arduino Framework

Future (when required)

- HX711

Avoid adding new libraries unless absolutely necessary.

---

# 13. Error Handling

Always check:

- Invalid values
- Null states
- Range limits

Avoid:

- Crashes
- Infinite loops
- Blocking code

---

# 14. Timing Rules

Never use:

delay()

Always use:

millis()

All code must be non-blocking.

---

# 15. Project Workflow

Every feature must follow this order:

Plan

↓

Update Documentation

↓

Write Code

↓

Compile

↓

Fix Errors

↓

Test

↓

Update MEMORY.md

↓

Update CHANGELOG.md

↓

Commit

---

# 16. AI Development Rules

The AI assistant must:

- Follow the existing architecture.
- Never modify locked hardware without approval.
- Never modify locked GPIO assignments.
- Never rewrite completed modules unless requested.
- Prefer extending existing modules instead of replacing them.
- Keep code simple and maintainable.
- Generate compilable code.
- Respect the project folder structure.
- Update documentation recommendations after major milestones.

---

# 17. Locked Project Decisions

Project Name

Linear Counter

Display Library

TFT_eSPI

Controller

ESP32-WROOM-32

Display

3.5" SPI ILI9488

Motor

High Torque NEMA17

Driver

TB6600

Power Supply

24V SMPS

Architecture

Modular

UI

Single Screen

Coding Style

Object Oriented

Framework

PlatformIO

---

# 18. Definition of Done

A task is complete only when:

- Code compiles successfully.
- Function works as expected.
- No compiler errors remain.
- Documentation is updated.
- MEMORY.md reflects the latest project status.
- CHANGELOG.md contains the completed work.

Otherwise, the task is considered incomplete.