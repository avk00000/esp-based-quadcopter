#pragma once
class LowPassFilter {
public:
    LowPassFilter(float timeConstantSeconds = 0.1f);
    void reset(float value = 0.0f);
    float update(float input, float dt);
private:
    float _tau, _value;
    bool _initialized;
};
