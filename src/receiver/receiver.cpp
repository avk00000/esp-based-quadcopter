#include "receiver/receiver.h"
#include "config.h"

namespace {
struct Capture { volatile uint32_t riseUs, lastUs; volatile uint16_t pulseUs; } captures[5] = {};
const int8_t pins[5] = {RC_ROLL_PIN, RC_PITCH_PIN, RC_THROTTLE_PIN, RC_YAW_PIN, RC_AUX_PIN};
void IRAM_ATTR captureEdge(void* arg) {
    Capture* c = static_cast<Capture*>(arg);
    const uint32_t now = micros();
    if (digitalRead(pins[c - captures]) == HIGH) c->riseUs = now;
    else if (c->riseUs != 0) {
        const uint32_t width = now - c->riseUs;
        if (width >= 800 && width <= 2200) { c->pulseUs = width; c->lastUs = now; }
    }
}
float bipolar(uint16_t pulse) {
    float v = (static_cast<float>(pulse) - RC_MIN_US) / (RC_MAX_US - RC_MIN_US);
    if (v < 0.0f) v = 0.0f; if (v > 1.0f) v = 1.0f;
    return (v - 0.5f) * 2.0f;
}
}

bool initReceiver() {
#if !RECEIVER_INPUTS_ENABLED
    return false;
#else
    for (uint8_t i = 0; i < 5; ++i) {
        if (pins[i] < 0) return false;
        pinMode(pins[i], INPUT);
        attachInterruptArg(digitalPinToInterrupt(pins[i]), captureEdge, &captures[i], CHANGE);
    }
    return true;
#endif
}

bool readReceiver(ReceiverChannels& out) {
    uint16_t p[5]; uint32_t last[5];
    noInterrupts();
    for (uint8_t i = 0; i < 5; ++i) { p[i] = captures[i].pulseUs; last[i] = captures[i].lastUs; }
    interrupts();
    const uint32_t now = micros();
    for (uint8_t i = 0; i < 5; ++i)
        if (p[i] < RC_MIN_US || p[i] > RC_MAX_US || last[i] == 0 || now - last[i] > RC_TIMEOUT_US) {
            out = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false}; return false;
        }
    out.roll = bipolar(p[0]); out.pitch = bipolar(p[1]);
    out.throttle = (p[2] - RC_MIN_US) / static_cast<float>(RC_MAX_US - RC_MIN_US);
    out.yaw = bipolar(p[3]); out.aux = bipolar(p[4]); out.valid = true;
    return true;
}

bool receiverSignalValid() { ReceiverChannels c; return readReceiver(c); }
