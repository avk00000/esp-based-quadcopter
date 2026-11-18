#include <math.h>
#include "utils/filters.h"
LowPassFilter::LowPassFilter(float tau) : _tau(tau > 0.0f ? tau : 0.001f), _value(0.0f), _initialized(false) {}
void LowPassFilter::reset(float value) { _value = value; _initialized = true; }
float LowPassFilter::update(float input, float dt) {
    if (!isfinite(input) || !isfinite(dt) || dt <= 0.0f) return _value;
    if (!_initialized) { reset(input); return _value; }
    const float alpha = dt / (_tau + dt); _value += alpha * (input - _value); return _value;
}
