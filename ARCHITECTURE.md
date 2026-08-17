                    USER
                      │
        ┌─────────────┴─────────────┐
        │                           │
   Push Buttons              RPM Potentiometer
        │                           │
        └─────────────┬─────────────┘
                      │
                      ▼
               Input Layer
        (buttons.cpp, rpm.cpp)
                      │
                      ▼
              Machine Layer
              (machine.cpp)
                      │
      ┌───────────────┼────────────────┐
      │               │                │
      ▼               ▼                ▼
 Display Layer    Motor Layer    Counter Layer
(display.cpp)    (motor.cpp)    (counter.cpp)
      │               │                │
      └───────────────┼────────────────┘
                      │
                      ▼
              Hardware Layer
        ESP32 • TFT • TB6600 • Buzzer

Linear Counter
│
├── docs/
│
├── include/
│   ├── config.h
│   ├── pins.h
│   ├── display.h
│   ├── buttons.h
│   ├── rpm.h
│   ├── motor.h
│   ├── counter.h
│   ├── machine.h
│   └── buzzer.h
│
├── src/
│   ├── main.cpp
│   ├── display.cpp
│   ├── buttons.cpp
│   ├── rpm.cpp
│   ├── motor.cpp
│   ├── counter.cpp
│   ├── machine.cpp
│   └── buzzer.cpp
│
├── lib/
├── data/
├── test/
└── platformio.ini

Power ON
    │
    ▼
ESP32 Boot
    │
    ▼
Initialize Modules
    │
    ├── Display
    ├── Buttons
    ├── RPM
    ├── Motor
    ├── Counter
    └── Buzzer
    │
    ▼
Draw Home Screen
    │
    ▼
Idle (READY)
    │
    ▼
Read Buttons & Potentiometer
    │
    ▼
Machine Logic
    │
    ├── Start
    ├── Stop
    ├── Reset
    └── Update RPM
    │
    ▼
Update Motor
    │
    ▼
Update Counter
    │
    ▼
Update Display
    │
    ▼
Repeat Loop



Power ON
    │
    ▼
ESP32 Boot
    │
    ▼
Initialize Modules
    │
    ├── Display
    ├── Buttons
    ├── RPM
    ├── Motor
    ├── Counter
    └── Buzzer
    │
    ▼
Draw Home Screen
    │
    ▼
Idle (READY)
    │
    ▼
Read Buttons & Potentiometer
    │
    ▼
Machine Logic
    │
    ├── Start
    ├── Stop
    ├── Reset
    └── Update RPM
    │
    ▼
Update Motor
    │
    ▼
Update Counter
    │
    ▼
Update Display
    │
    ▼
Repeat Loop

Module Responsibilities
main.cpp

Only starts the project.

Setup

↓

Initialize Modules

↓

Call Updates

No business logic.

machine.cpp

This is the brain.

Responsible for:

Start machine
Stop machine
Reset machine
Machine state
Calling other modules

Everything passes through here.

display.cpp

Responsible only for:

Draw screen
Update RPM
Update Frequency
Update Counter
Update Status
Update Button

It never controls the motor.

motor.cpp

Responsible only for:

STEP pulses
DIR
ENABLE
Speed
Start
Stop

It never draws on the display.

rpm.cpp

Responsible only for:

Read potentiometer
Average ADC
Convert to RPM
Calculate frequency
buttons.cpp

Responsible only for:

Read buttons
Debounce
Return button events
counter.cpp

Responsible only for:

Increment cycle count
Reset count
Return count

Remember:

1 complete motor revolution = 1 complete linear cycle

buzzer.cpp

Responsible only for:

Short beep
Long beep
Error beep
Data Flow
Potentiometer
      │
      ▼
 rpm.cpp
      │
      ▼
machine.cpp
      │
      ▼
motor.cpp
      │
      ▼
counter.cpp
      │
      ▼
display.cpp
Hardware Architecture
             24V SMPS
                 │
      ┌──────────┴──────────┐
      │                     │
      ▼                     ▼
   TB6600 Driver      Buck Converter
      │                 24V → 5V
      │                     │
      ▼                     ▼
 High Torque NEMA17      ESP32
                               │
      ┌────────────────────────┼─────────────────────┐
      │                        │                     │
      ▼                        ▼                     ▼
 TFT Display             Push Buttons         Potentiometer
      │                        │                     │
      └──────────────┬─────────┴────────────┬────────┘
                     ▼                      ▼
                 Buzzer                 LEDs
Why this architecture?
✅ Simple enough for a student project.
✅ Professional enough to scale.
✅ Every module has one responsibility.
✅ Easy to debug.
✅ Easy to test.
✅ Easy to add future features like a load cell or data logging.
✅ No unnecessary complexity.
🔒 Architecture Decision

For your ARCHITECTURE.md, I'd define the project architecture as:

Layered Modular Architecture (LMA)

with the layers:

Hardware Layer – ESP32, TB6600, TFT, buttons, potentiometer, buzzer.
Driver Layer – display.cpp, motor.cpp, buttons.cpp, rpm.cpp, counter.cpp, buzzer.cpp.
Machine Layer – machine.cpp (application logic and state management).
Application Layer – main.cpp (startup and scheduler).