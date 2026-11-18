#include "drivers/esc.h"
#include "config.h"

namespace {
const int8_t motorPins[4] = {MOTOR_1_PIN, MOTOR_2_PIN, MOTOR_3_PIN, MOTOR_4_PIN};
const uint8_t channels[4] = {0, 1, 2, 3};
uint32_t pulseToDuty(uint16_t us) {
    const uint32_t maxDuty = (1UL << ESC_PWM_BITS) - 1;
    return (static_cast<uint32_t>(us) * ESC_PWM_HZ * maxDuty) / 1000000UL;
}
}

bool initESCs() {
#if !FLIGHT_OUTPUTS_ENABLED
    return false;
#else
    for (uint8_t i = 0; i < 4; ++i) {
        if (motorPins[i] < 0 || !ledcSetup(channels[i], ESC_PWM_HZ, ESC_PWM_BITS)) { stopESCs(); return false; }
        ledcAttachPin(motorPins[i], channels[i]);
        ledcWrite(channels[i], pulseToDuty(ESC_MIN_US));
    }
    return true;
#endif
}

void stopESCs() {
    for (uint8_t i = 0; i < 4; ++i) ledcWrite(channels[i], pulseToDuty(ESC_MIN_US));
}

bool writeMotorUs(uint8_t motor, uint16_t pulseUs) {
#if !FLIGHT_OUTPUTS_ENABLED
    (void)motor; (void)pulseUs; return false;
#else
    if (motor >= 4 || pulseUs < ESC_MIN_US || pulseUs > ESC_MAX_US) return false;
    ledcWrite(channels[motor], pulseToDuty(pulseUs)); return true;
#endif
}
