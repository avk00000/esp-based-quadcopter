# Hardware mapping and validation gates

## Configuration that must be established

`include/config.h` deliberately sets `FLIGHT_OUTPUTS_ENABLED` and `RECEIVER_INPUTS_ENABLED` to `0`, and all receiver and motor GPIO values to `-1`. Keep those values until the following are known from the physical setup:

- The receiver's actual output format and whether the selected ESP32 pins receive independent PWM channel signals. The capture module accepts five independent PWM inputs only.
- The roll, pitch, throttle, yaw, and auxiliary channel assignment, each channel's pulse range, and transmitter failsafe output.
- ESP32 pin assignments, including whether each pin is input-capable/output-capable and whether any board peripherals conflict.
- MPU6050 and QMC5883L axes relative to the airframe, including any required sign changes.
- Quad-X motor number/position and each motor's direction, checked against the chosen mixer convention.

Do not enable outputs as a substitute for validating these mappings. The current firmware has no arming state machine and no integrated receiver-to-PID-to-mixer-to-ESC control loop.

## Bench checks (propellers removed)

1. Power the flight controller from a current-limited source where practical. Check for heating or wiring faults.
2. Confirm stationary accelerometer magnitude is near 1 g and calibrated gyro rates remain near zero. Calibration assumes the board is still and the sensor is level during startup.
3. Rotate slowly around each physical axis and confirm roll/pitch sign and scale. Repeat at several angles; do not tune control gains from a single orientation.
4. Log magnetometer values through a full rotation, away from motors, steel, and current-carrying wires. Confirm each sensor axis and heading convention before using `calculateMagHeading()`.
5. With receiver input still disabled, identify the receiver protocol and measure channel pulse widths independently. Test transmitter-off behavior and verify the receiver's failsafe pulses are rejected or explicitly handled.
6. Before any ESC signal test, check motor outputs with a logic analyzer or oscilloscope. Then test each ESC and motor separately, with propellers removed, and confirm stop/minimum pulse behavior.

## Algorithm notes

- The attitude filter uses gyro integration with a time-constant-based accelerometer correction. Accelerometer correction is suppressed when measured acceleration magnitude is outside 0.8–1.2 g; this is a basic heuristic, not a vibration-rejection solution.
- `attitude.yaw` is gyro-integrated and is not an absolute heading. It wraps to [-180, 180) degrees.
- The magnetometer helper assumes its inputs are in a known right-handed body frame. The sensor's current calibration corrects hard-iron offsets and per-axis scale only; board-to-frame alignment and magnetic declination are not applied.
- The mixer equations are an example convention only. Motor order and signs must be checked before use.
- The PID helper rejects invalid or excessively long time steps and clamps integral and output terms. Gains are not provided or tuned.
