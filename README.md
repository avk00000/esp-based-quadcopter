# ESP32 Quadcopter Flight Controller (development firmware)

Original Arduino/PlatformIO firmware for an ESP32-WROOM-32 based Quad-X project. It borrows engineering goals such as modular drivers, explicit failsafes, and deterministic control timing, but it is not ArduPilot and does not contain or port the ArduPilot codebase.

## Current state

The project contains MPU6050 and QMC5883L drivers, a complementary roll/pitch estimator, gyro-integrated yaw, a tilt-compensated heading helper, PWM receiver capture with signal timeout, PID, a conventional Quad-X mixer, and an ESP32 LEDC ESC driver.

**This firmware is for bench development only. It is not flight-ready.** Receiver capture and motor outputs are disabled by default in `include/config.h`. The receiver protocol, channel mapping, sensor/frame orientation, motor numbering, motor rotation, arming logic, and failsafe action must be physically verified before connecting or enabling propulsion. No flight mode or closed-loop motor-control path is armed by this build.

## Build

Open this folder in PlatformIO and build the `esp32dev` environment from `platformio.ini`. The project uses the Arduino framework. Hardware validation is required after a successful build; compilation does not establish sensor correctness or safe actuator behavior.

## Bring-up order

1. Keep all propellers removed.
2. Verify I2C wiring, sensor identity, raw and scaled sensor output, and calibration while the frame is still.
3. Validate roll and pitch signs with slow, single-axis physical rotations. Yaw is gyro-integrated only and will drift; the magnetometer heading helper is intentionally not fused until axis alignment is confirmed.
4. Identify the FS-R6B receiver output actually wired to the ESP32 (PWM, PPM, or another supported signal) and verify channel order, pulse widths, loss behavior, and GPIO electrical levels. The current receiver module supports five separate PWM inputs, not PPM or serial receiver protocols.
5. Confirm the exact motor GPIOs, Quad-X numbering, and spin directions. Test each ESC/motor individually with props removed before considering a mixer test.
6. Add and validate explicit arming, disarming, low-throttle, receiver-loss, sensor-fault, and watchdog behavior before any closed-loop actuator integration.

See [`docs/hardware-validation.md`](docs/hardware-validation.md) for the configuration gates and test procedure.

