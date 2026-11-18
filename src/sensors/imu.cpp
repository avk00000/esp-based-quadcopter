#include <Wire.h>
#include "sensors/imu.h"
#include "config.h"


//====================================================
// Global Objects
//====================================================

IMURaw imuRaw;
IMUScaled imu;
Attitude attitude;
IMUCalibration calib;

//====================================================
// Initialize MPU6050
//====================================================

void initIMU()
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(PWR_MGMT_1);
    Wire.write(0x00);        // Wake up MPU6050
    Wire.endTransmission();

    delay(100);

    Serial.println("MPU6050 Initialized");
}

//====================================================
// Read Raw IMU Data
//====================================================

void readRawIMU()
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(ACCEL_XOUT_H);
    Wire.endTransmission(false);

    if (Wire.requestFrom(MPU_ADDR, 14) != 14)
    {
        Serial.println("MPU6050 Read Error");
        return;
    }

    imuRaw.ax = (Wire.read() << 8) | Wire.read();
    imuRaw.ay = (Wire.read() << 8) | Wire.read();
    imuRaw.az = (Wire.read() << 8) | Wire.read();

    imuRaw.temp = (Wire.read() << 8) | Wire.read();

    imuRaw.gx = (Wire.read() << 8) | Wire.read();
    imuRaw.gy = (Wire.read() << 8) | Wire.read();
    imuRaw.gz = (Wire.read() << 8) | Wire.read();
}
int16_t readWord(uint8_t reg)
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);

    Wire.requestFrom(MPU_ADDR, (uint8_t)2);

    int16_t value = (Wire.read() << 8) | Wire.read();

    return value;
}

void printFactoryOffsets()
{
    Serial.println("===== Factory Offset Registers =====");

    Serial.print("XA_OFFSET : ");
    Serial.println(readWord(0x06));

    Serial.print("YA_OFFSET : ");
    Serial.println(readWord(0x08));

    Serial.print("ZA_OFFSET : ");
    Serial.println(readWord(0x0A));

    Serial.println();
}
uint8_t readByte(uint8_t reg)
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);

    Wire.requestFrom(MPU_ADDR, (uint8_t)1);

    return Wire.read();
}

void printRegisters()
{
    Serial.println("\n===== MPU6050 Registers =====");

    Serial.print("WHO_AM_I      : 0x");
    Serial.println(readByte(0x75), HEX);

    Serial.print("PWR_MGMT_1    : 0x");
    Serial.println(readByte(0x6B), HEX);

    Serial.print("PWR_MGMT_2    : 0x");
    Serial.println(readByte(0x6C), HEX);

    Serial.print("SMPLRT_DIV    : 0x");
    Serial.println(readByte(0x19), HEX);

    Serial.print("CONFIG        : 0x");
    Serial.println(readByte(0x1A), HEX);

    Serial.print("GYRO_CONFIG   : 0x");
    Serial.println(readByte(0x1B), HEX);

    Serial.print("ACCEL_CONFIG  : 0x");
    Serial.println(readByte(0x1C), HEX);

    Serial.print("ACCEL_CONFIG2 : 0x");
    Serial.println(readByte(0x1D), HEX);

    Serial.print("INT_ENABLE    : 0x");
    Serial.println(readByte(0x38), HEX);

    Serial.println("==============================");
}
//====================================================
// Apply Calibration
//====================================================

void applyCalibration()
{
    imu.ax = imuRaw.ax - calib.ax_offset;
    imu.ay = imuRaw.ay - calib.ay_offset;
    imu.az = imuRaw.az - calib.az_offset - Z_AXIS_BIAS;

    imu.gx = imuRaw.gx - calib.gx_offset;
    imu.gy = imuRaw.gy - calib.gy_offset;
    imu.gz = imuRaw.gz - calib.gz_offset;
}

//====================================================
// Verify Calibration
//====================================================

bool verifyCalibration()
{
    float ax = 0;
    float ay = 0;
    float az = 0;

    float gx = 0;
    float gy = 0;
    float gz = 0;

    for (int i = 0; i < VERIFY_SAMPLES; i++)
    {
        readRawIMU();
        applyCalibration();

        ax += imu.ax;
        ay += imu.ay;
        az += imu.az;

        gx += imu.gx;
        gy += imu.gy;
        gz += imu.gz;

        delay(2);
    }

    ax /= VERIFY_SAMPLES;
    ay /= VERIFY_SAMPLES;
    az /= VERIFY_SAMPLES;

    gx /= VERIFY_SAMPLES;
    gy /= VERIFY_SAMPLES;
    gz /= VERIFY_SAMPLES;

    Serial.println();
    Serial.println("========== Verification ==========");

    Serial.print("AX : "); Serial.println(ax);
    Serial.print("AY : "); Serial.println(ay);
    Serial.print("AZ : "); Serial.println(az);

    Serial.print("GX : "); Serial.println(gx);
    Serial.print("GY : "); Serial.println(gy);
    Serial.print("GZ : "); Serial.println(gz);

    if (fabs(ax) > 100) return false;
    if (fabs(ay) > 100) return false;
    if (fabs(az - 16384.0f) > 150) return false;

    if (fabs(gx) > 5) return false;
    if (fabs(gy) > 5) return false;
    if (fabs(gz) > 5) return false;

    return true;
}

//====================================================
// Calibrate IMU
//====================================================

void calibrateIMU()
{
    while (true)
    {
        Serial.println();
        Serial.println("======================================");
        Serial.println("       MPU6050 CALIBRATION");
        Serial.println(" Place drone on a level surface");
        Serial.println(" Do NOT touch the drone");
        Serial.println("======================================");

        delay(CALIBRATION_WAIT_MS);

        // Discard initial unstable samples
        for (int i = 0; i < 100; i++)
        {
            readRawIMU();
            delay(2);
        }

        int32_t ax_sum = 0;
        int32_t ay_sum = 0;
        int32_t az_sum = 0;

        int32_t gx_sum = 0;
        int32_t gy_sum = 0;
        int32_t gz_sum = 0;

        for (int i = 0; i < CALIBRATION_SAMPLES; i++)
        {
            readRawIMU();

            ax_sum += imuRaw.ax;
            ay_sum += imuRaw.ay;
            az_sum += imuRaw.az;

            gx_sum += imuRaw.gx;
            gy_sum += imuRaw.gy;
            gz_sum += imuRaw.gz;

            delay(CALIBRATION_DELAY_MS);
        }

        calib.ax_offset = (float)ax_sum / CALIBRATION_SAMPLES;
        calib.ay_offset = (float)ay_sum / CALIBRATION_SAMPLES;
        calib.az_offset =
    ((float)az_sum / CALIBRATION_SAMPLES)
    - 16384.0f
    - Z_AXIS_BIAS;

        calib.gx_offset = (float)gx_sum / CALIBRATION_SAMPLES;
        calib.gy_offset = (float)gy_sum / CALIBRATION_SAMPLES;
        calib.gz_offset = (float)gz_sum / CALIBRATION_SAMPLES;

        Serial.println();
        Serial.println("========== OFFSETS ==========");

        Serial.print("AX : "); Serial.println(calib.ax_offset);
        Serial.print("AY : "); Serial.println(calib.ay_offset);
        Serial.print("AZ : "); Serial.println(calib.az_offset);

        Serial.print("GX : "); Serial.println(calib.gx_offset);
        Serial.print("GY : "); Serial.println(calib.gy_offset);
        Serial.print("GZ : "); Serial.println(calib.gz_offset);

        if (verifyCalibration())
        {
            Serial.println();
            Serial.println("======================================");
            Serial.println(" Calibration Successful");
            Serial.println("======================================");
            return;
        }

        Serial.println();
        Serial.println("Calibration verification failed.");
        Serial.println("Retrying...");
        delay(1000);
    }
}
void scaleIMU()
{
    imu.ax = (imuRaw.ax - calib.ax_offset) / 16384.0f;
    imu.ay = (imuRaw.ay - calib.ay_offset) / 16384.0f;
    imu.az = (imuRaw.az - calib.az_offset - Z_AXIS_BIAS) / 16384.0f;
    imu.gx = (imuRaw.gx - calib.gx_offset) / 131.0f;
    imu.gy = (imuRaw.gy - calib.gy_offset) / 131.0f;
    imu.gz = (imuRaw.gz - calib.gz_offset) / 131.0f;
}