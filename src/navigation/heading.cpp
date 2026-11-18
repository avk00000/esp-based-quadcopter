#include <math.h>
#include "navigation/heading.h"
#include <Arduino.h>

bool calculateMagHeading(float mx, float my, float mz,
                         float rollDegrees, float pitchDegrees, float& headingDegrees) {
    if (!isfinite(mx) || !isfinite(my) || !isfinite(mz) ||
        !isfinite(rollDegrees) || !isfinite(pitchDegrees)) return false;
    const float roll = rollDegrees * PI / 180.0f;
    const float pitch = pitchDegrees * PI / 180.0f;
    const float xh = mx * cosf(pitch) + mz * sinf(pitch);
    const float yh = mx * sinf(roll) * sinf(pitch) + my * cosf(roll) - mz * sinf(roll) * cosf(pitch);
    if (fabsf(xh) + fabsf(yh) < 1e-6f) return false;
    headingDegrees = atan2f(-yh, xh) * 180.0f / PI;
    if (headingDegrees < 0.0f) headingDegrees += 360.0f;
    return true;
}
