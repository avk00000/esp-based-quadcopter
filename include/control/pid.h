#pragma once
struct PIDConfig { float kp, ki, kd, integralLimit, outputLimit; };
struct PIDState { float integral; float previousMeasurement; bool hasPreviousMeasurement; };
void resetPID(PIDState& state);
float updatePID(const PIDConfig& config, PIDState& state, float setpoint, float measurement, float dt);
