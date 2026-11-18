#pragma once
struct MotorOutputs { float m1, m2, m3, m4; };
// Conventional Quad-X example. Verify motor positions and rotation directions first.
MotorOutputs mixQuadX(float throttle, float roll, float pitch, float yaw);
