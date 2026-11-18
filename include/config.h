#ifndef CONFIG_H
#define CONFIG_H

//========================
// I2C Pins
//========================
#define SDA_PIN             21
#define SCL_PIN             22

//========================
// MPU6050
//========================
#define MPU_ADDR            0x68

#define PWR_MGMT_1          0x6B
#define ACCEL_XOUT_H        0x3B
#define Z_AXIS_BIAS 6205.0f

// Actuator and receiver I/O are deliberately disabled until the actual wiring
// and receiver protocol have been verified. Set these only after bench checks.
#define FLIGHT_OUTPUTS_ENABLED 0
#define RECEIVER_INPUTS_ENABLED 0

// PWM receiver channel inputs (set to GPIO numbers after confirming wiring).
#define RC_ROLL_PIN       -1
#define RC_PITCH_PIN      -1
#define RC_THROTTLE_PIN   -1
#define RC_YAW_PIN        -1
#define RC_AUX_PIN        -1

// ESC signal outputs (set to GPIO numbers after confirming wiring).
#define MOTOR_1_PIN       -1
#define MOTOR_2_PIN       -1
#define MOTOR_3_PIN       -1
#define MOTOR_4_PIN       -1

#define ESC_PWM_HZ        250
#define ESC_PWM_BITS      16
#define ESC_MIN_US        1000
#define ESC_MAX_US        2000
#define RC_MIN_US         1000
#define RC_MAX_US         2000
#define RC_TIMEOUT_US     100000UL

#define ATTITUDE_TIME_CONSTANT_S 0.50f
#define ATTITUDE_MIN_DT_S        0.0001f
#define ATTITUDE_MAX_DT_S        0.050f
#define CONTROL_MAX_DT_S         0.020f
//========================
// Magnetometer
//========================
#define QMC5883_ADDR 0x0D

#define QMC_X_LSB 0x00

#define QMC_CONTROL_1 0x09

#define QMC_CONTROL_2 0x0A

//========================
// Calibration
//========================
#define CALIBRATION_SAMPLES     5000

#define CALIBRATION_DELAY_MS    2

#define CALIBRATION_WAIT_MS     5000
// Maximum allowed gyro reading while calibrating.
// If exceeded, calibration restarts.
#define GYRO_MOVEMENT_THRESHOLD  100

// Number of samples used to verify calibration
#define VERIFY_SAMPLES           200

#endif
