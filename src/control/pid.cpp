#include <math.h>
#include "control/pid.h"
#include "config.h"
static float clampValue(float x, float limit) {
    if (limit <= 0.0f) return x;
    return x > limit ? limit : (x < -limit ? -limit : x);
}
void resetPID(PIDState& s) { s.integral = 0.0f; s.previousMeasurement = 0.0f; s.hasPreviousMeasurement = false; }
float updatePID(const PIDConfig& c, PIDState& s, float setpoint, float measurement, float dt) {
    if (!isfinite(dt) || dt < ATTITUDE_MIN_DT_S || dt > CONTROL_MAX_DT_S || !isfinite(setpoint) || !isfinite(measurement)) {
        resetPID(s); return 0.0f;
    }
    const float error = setpoint - measurement;
    const float derivative = s.hasPreviousMeasurement ? -(measurement - s.previousMeasurement) / dt : 0.0f;
    const float candidate = clampValue(s.integral + error * dt, c.integralLimit);
    const float trial = c.kp * error + c.ki * candidate + c.kd * derivative;
    if (trial == clampValue(trial, c.outputLimit) || (trial > c.outputLimit && error < 0.0f) || (trial < -c.outputLimit && error > 0.0f)) s.integral = candidate;
    s.previousMeasurement = measurement; s.hasPreviousMeasurement = true;
    return clampValue(c.kp * error + c.ki * s.integral + c.kd * derivative, c.outputLimit);
}
