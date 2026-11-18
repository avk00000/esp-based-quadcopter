#ifndef IMU_H
#define IMU_H

#include <Arduino.h>

//====================================================
// Raw IMU Data (Directly from MPU6050)
//====================================================

struct IMURaw
{
    int16_t ax;
    int16_t ay;
    int16_t az;

    int16_t temp;

    int16_t gx;
    int16_t gy;
    int16_t gz;
};

//====================================================
// Corrected IMU Data (After Calibration)
//====================================================

struct IMUScaled
{
    float ax;      // g
    float ay;
    float az;

    float gx;      // deg/sec
    float gy;
    float gz;
};

//====================================================
// IMU Calibration Offsets
//====================================================

struct IMUCalibration
{
    // Accelerometer
    float ax_offset;
    float ay_offset;
    float az_offset;

    float ax_scale;
    float ay_scale;
    float az_scale;

    // Gyroscope
    float gx_offset;
    float gy_offset;
    float gz_offset;
};

//====================================================
// Drone Attitude
//====================================================

struct Attitude
{
    float roll;
    float pitch;
    float yaw;
};

//====================================================
// Global Variables
//====================================================

extern IMURaw imuRaw;
extern IMUScaled imu;
extern IMUCalibration calib;
extern Attitude attitude;

//====================================================
// Function Prototypes
//====================================================

void initIMU();

void readRawIMU();

void applyCalibration();

void calibrateIMU();

bool verifyCalibration();

void scaleIMU();
int16_t readWord(uint8_t reg);
void printFactoryOffsets();
uint8_t readByte(uint8_t reg);
void printRegisters();
#endif