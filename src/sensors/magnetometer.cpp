#include <Wire.h>
#include "sensors/magnetometer.h"
#include "config.h"

int16_t mxMin = 32767;
int16_t myMin = 32767;
int16_t mzMin = 32767;

int16_t mxMax = -32768;
int16_t myMax = -32768;
int16_t mzMax = -32768;
MagRaw magRaw;

MagCalibration magCalib;

MagScaled mag;
bool initMagnetometer()
{
    
magCalib.offsetX = 917.5f;
magCalib.offsetY = 25.5f;
magCalib.offsetZ = -46.0f;

magCalib.scaleX = 0.9883f;
magCalib.scaleY = 0.9932f;
magCalib.scaleZ = 1.0191f;

    // Set/Reset register
    Wire.beginTransmission(QMC5883_ADDR);
    Wire.write(0x0B);
    Wire.write(0x01);

    if (Wire.endTransmission() != 0)
    {
        Serial.println("QMC5883L not found");
        return false;
    }

    delay(10);

    // Continuous mode
    Wire.beginTransmission(QMC5883_ADDR);
    Wire.write(0x09);
    Wire.write(0x1D);

    if (Wire.endTransmission() != 0)
    {
        Serial.println("Configuration Failed");
        return false;
    }

    delay(10);

    Serial.println("QMC5883L Initialized");

    return true;
}
void readRawMag()
{
    Wire.beginTransmission(QMC5883_ADDR);
    Wire.write(0x00);

    if (Wire.endTransmission() != 0)
        return;

    if (Wire.requestFrom(QMC5883_ADDR, 6) != 6)
        return;

    magRaw.mx = (int16_t)(Wire.read() | (Wire.read() << 8));
    magRaw.my = (int16_t)(Wire.read() | (Wire.read() << 8));
    magRaw.mz = (int16_t)(Wire.read() | (Wire.read() << 8));
}
void calibrateMagnetometer()
{
    Serial.println();
    Serial.println("========================================");
    Serial.println("     MAGNETOMETER CALIBRATION");
    Serial.println("========================================");
    Serial.println();

    Serial.println("Rotate the drone in ALL directions.");
    Serial.println("Roll, Pitch, Yaw, Upside Down");
    Serial.println("Finish with Figure-8 motions.");
    Serial.println();

    Serial.println("Starting in 5 seconds...");
    delay(5000);

    // Reset min/max values
    mxMin = myMin = mzMin = 32767;
    mxMax = myMax = mzMax = -32768;

    unsigned long start = millis();
    unsigned long lastPrint = millis();

    while (millis() - start < 90000)
    {
        readRawMag();

        // Update minimum values
        if (magRaw.mx < mxMin) mxMin = magRaw.mx;
        if (magRaw.my < myMin) myMin = magRaw.my;
        if (magRaw.mz < mzMin) mzMin = magRaw.mz;

        // Update maximum values
        if (magRaw.mx > mxMax) mxMax = magRaw.mx;
        if (magRaw.my > myMax) myMax = magRaw.my;
        if (magRaw.mz > mzMax) mzMax = magRaw.mz;

        // Print every second
        if (millis() - lastPrint >= 1000)
        {
            lastPrint = millis();

            Serial.println("--------------------------------");

            Serial.print("Time Left : ");
            Serial.print((90000 - (millis() - start)) / 1000);
            Serial.println(" sec");

            Serial.print("X : ");
            Serial.print(mxMin);
            Serial.print(" -> ");
            Serial.println(mxMax);

            Serial.print("Y : ");
            Serial.print(myMin);
            Serial.print(" -> ");
            Serial.println(myMax);

            Serial.print("Z : ");
            Serial.print(mzMin);
            Serial.print(" -> ");
            Serial.println(mzMax);

            Serial.println("--------------------------------");
        }

        delay(20);
    }

    // Calculate offsets
    magCalib.offsetX = (mxMax + mxMin) / 2.0f;
    magCalib.offsetY = (myMax + myMin) / 2.0f;
    magCalib.offsetZ = (mzMax + mzMin) / 2.0f;

    // Calculate ranges
    float rangeX = mxMax - mxMin;
    float rangeY = myMax - myMin;
    float rangeZ = mzMax - mzMin;

    float avgRange = (rangeX + rangeY + rangeZ) / 3.0f;

    // Calculate scale factors
    magCalib.scaleX = avgRange / rangeX;
    magCalib.scaleY = avgRange / rangeY;
    magCalib.scaleZ = avgRange / rangeZ;

    Serial.println();
    Serial.println("========================================");
    Serial.println("     CALIBRATION COMPLETE");
    Serial.println("========================================");

    Serial.println();

    Serial.println("Minimum Values");
    Serial.print("MX Min : ");
    Serial.println(mxMin);
    Serial.print("MY Min : ");
    Serial.println(myMin);
    Serial.print("MZ Min : ");
    Serial.println(mzMin);

    Serial.println();

    Serial.println("Maximum Values");
    Serial.print("MX Max : ");
    Serial.println(mxMax);
    Serial.print("MY Max : ");
    Serial.println(myMax);
    Serial.print("MZ Max : ");
    Serial.println(mzMax);

    Serial.println();

    Serial.println("Ranges");
    Serial.print("Range X : ");
    Serial.println(rangeX);

    Serial.print("Range Y : ");
    Serial.println(rangeY);

    Serial.print("Range Z : ");
    Serial.println(rangeZ);

    Serial.println();

    Serial.println("Offsets");
    Serial.print("Offset X : ");
    Serial.println(magCalib.offsetX);

    Serial.print("Offset Y : ");
    Serial.println(magCalib.offsetY);

    Serial.print("Offset Z : ");
    Serial.println(magCalib.offsetZ);

    Serial.println();

    Serial.println("Scale Factors");
    Serial.print("Scale X : ");
    Serial.println(magCalib.scaleX, 4);

    Serial.print("Scale Y : ");
    Serial.println(magCalib.scaleY, 4);

    Serial.print("Scale Z : ");
    Serial.println(magCalib.scaleZ, 4);

    Serial.println("========================================");
}
void applyMagCalibration()
{
    mag.mx = (magRaw.mx - magCalib.offsetX) * magCalib.scaleX;

    mag.my = (magRaw.my - magCalib.offsetY) * magCalib.scaleY;

    mag.mz = (magRaw.mz - magCalib.offsetZ) * magCalib.scaleZ;
}
