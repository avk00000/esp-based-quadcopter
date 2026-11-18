#pragma once
#include <stdbool.h>
// Tilt-compensated magnetic heading in degrees. Inputs must already be in a
// known, right-handed body frame; verify sensor-to-frame signs before use.
bool calculateMagHeading(float mx, float my, float mz,
                         float rollDegrees, float pitchDegrees,
                         float& headingDegrees);
