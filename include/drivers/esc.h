#pragma once
#include <Arduino.h>
bool initESCs();
void stopESCs();
bool writeMotorUs(uint8_t motor, uint16_t pulseUs);
