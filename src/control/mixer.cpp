#include "control/mixer.h"
static float clamp01(float x) { return x < 0.0f ? 0.0f : (x > 1.0f ? 1.0f : x); }
MotorOutputs mixQuadX(float throttle, float roll, float pitch, float yaw) {
    MotorOutputs out;
    out.m1 = clamp01(throttle + pitch + roll - yaw);
    out.m2 = clamp01(throttle + pitch - roll + yaw);
    out.m3 = clamp01(throttle - pitch - roll - yaw);
    out.m4 = clamp01(throttle - pitch + roll + yaw);
    return out;
}
