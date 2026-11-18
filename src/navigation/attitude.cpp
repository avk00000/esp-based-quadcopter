#include <math.h>
#include "sensors/imu.h"
#include "navigation/attitude.h"
#include "config.h"

static float wrapDegrees(float angle) {
    while (angle >= 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

void initAttitude()
{
    attitude.roll = 0.0f;
    attitude.pitch = 0.0f;
    attitude.yaw = 0.0f;
}

void updateAttitude(float dt)
{
    if (!isfinite(dt) || dt < ATTITUDE_MIN_DT_S || dt > ATTITUDE_MAX_DT_S) return;

    const float accelMagnitude = sqrtf(imu.ax * imu.ax + imu.ay * imu.ay + imu.az * imu.az);
    float accelRoll =
        atan2f(imu.ay, imu.az) * 180.0f / PI;

    float accelPitch =
        atan2f(
            -imu.ax,
            sqrtf(imu.ay * imu.ay +
                  imu.az * imu.az)
        ) * 180.0f / PI;

    const float gyroRoll = attitude.roll + imu.gx * dt;
    const float gyroPitch = attitude.pitch + imu.gy * dt;
    const float alpha = expf(-dt / ATTITUDE_TIME_CONSTANT_S);

    // Accelerometer correction is useful only when measured magnitude is close to 1 g.
    if (accelMagnitude > 0.80f && accelMagnitude < 1.20f) {
        attitude.roll = alpha * gyroRoll + (1.0f - alpha) * accelRoll;
        attitude.pitch = alpha * gyroPitch + (1.0f - alpha) * accelPitch;
    } else {
        attitude.roll = gyroRoll;
        attitude.pitch = gyroPitch;
    }
    attitude.yaw = wrapDegrees(attitude.yaw + imu.gz * dt);
}
