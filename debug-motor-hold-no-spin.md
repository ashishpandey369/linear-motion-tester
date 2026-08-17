# Debug Session: motor-hold-no-spin
- **Status**: [OPEN]
- **Issue**: The stepper driver energizes the motor and the shaft resists manual force, but the motor does not rotate.
- **Debug Server**: Pending initialization
- **Log File**: .dbg/trae-debug-log-motor-hold-no-spin.ndjson

## Reproduction Steps
1. Power the ESP32 and TB6600 system.
2. Turn the RPM potentiometer above zero.
3. Observe that the motor holds torque but does not spin.

## Hypotheses & Verification
| ID | Hypothesis | Likelihood | Effort | Evidence |
|----|------------|------------|--------|----------|
| A | The firmware is enabling the driver but never generating effective STEP pulses. | High | Low | Pending |
| B | STEP pulses are generated in firmware, but the pulse polarity or timing is incompatible with the TB6600 input. | High | Medium | Pending |
| C | The machine state is not reaching or staying in the running path long enough to sustain stepping. | Medium | Low | Pending |
| D | The TB6600 signal wiring mode does not match the firmware output style. | Medium | Medium | Pending |
| E | The configured microstep or target-speed ramp is causing a stall condition right at startup. | Low | Medium | Pending |

## Log Evidence
- User observation after matching firmware to current wiring: motor still holds at idle, and increasing the potentiometer releases the hold instead of producing rotation.
- This behavior is consistent with ENA polarity being inverted for the attached TB6600 board: the run path is likely driving ENA into the disable state.
- After inverting ENA polarity, user reports the opposite behavior: with potentiometer turned up, the motor now holds; with the potentiometer down, the motor releases.
- This confirms the enable path is now behaving consistently with the run/idle state, but the motor still does not rotate or vibrate.
- User-provided runtime evidence:
  - `[DEBUG][C] setRPM targetRPM=300 targetSPS=8000.00`
  - `[DEBUG][B] stepping enabled halfPeriodUs=1852 stepsPerSecond=270.00`
  - `[DEBUG][C] start requested rpm=50 freq=0.83`
  - `[DEBUG][A] pulses=36 running=1 enabled=1 ...`
- These lines confirm that the firmware reaches the running path, enables the driver, and generates pulses.
- User rewired the TB6600 to sink-drive mode while keeping the same GPIO numbers:
  - `5V -> PUL+`, `GPIO22 -> PUL-`
  - `5V -> DIR+`, `GPIO27 -> DIR-`
  - `5V -> ENA+`, `GPIO21 -> ENA-`
- Applying minimal fix: restore firmware polarity for sink-drive TB6600 inputs while preserving the same GPIO pins.

## Verification Conclusion
- Hypothesis A: Rejected. Firmware is generating step pulses while running.
- Hypothesis B: Confirmed. Pulses are generated in firmware, but the TB6600 is not converting them into motion with the current electrical drive arrangement.
- Hypothesis C: Rejected by user report because the motor state changes with the potentiometer.
- Hypothesis D: Confirmed for ENA polarity. Current pin mapping is kept, but the TB6600 enable input behavior was opposite to the original firmware assumption.
- Current fix direction: match firmware polarity to the corrected sink-drive wiring and verify motion on hardware.
