#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <Arduino.h>

struct MagRaw
{
    int16_t mx;
    int16_t my;
    int16_t mz;
};

struct MagScaled
{
    float mx;
    float my;
    float mz;
};

struct MagCalibration
{
    float offsetX;
    float offsetY;
    float offsetZ;

    float scaleX;
    float scaleY;
    float scaleZ;
};

extern MagRaw magRaw;
extern MagScaled mag;
extern MagCalibration magCalib;

bool initMagnetometer();

void readRawMag();

void calibrateMagnetometer();

void applyMagCalibration();

#endif