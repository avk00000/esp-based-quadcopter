#include <Arduino.h>
#include <Wire.h>

#include "config.h"

#include "sensors/imu.h"
#include "sensors/magnetometer.h"

#include "navigation/attitude.h"

static uint32_t previousTimeUs = 0;
static uint32_t previousTelemetryUs = 0;
static bool magnetometerReady = false;

void setup()
{
    Serial.begin(115200);

    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);

    initIMU();
    magnetometerReady = initMagnetometer();

    delay(100);

    calibrateIMU();
    initAttitude();

    previousTimeUs = micros();

    Serial.println();
    Serial.println("===============================================");
    Serial.println("        SENSOR INTEGRATION TEST");
    Serial.print("Magnetometer: ");
    Serial.println(magnetometerReady ? "ready" : "not detected");
    Serial.println("Motor output is disabled in config.h.");
    Serial.println("===============================================");
    Serial.println();
}

void loop()
{
    const uint32_t currentTimeUs = micros();
    const float dt = (currentTimeUs - previousTimeUs) * 1e-6f;
    previousTimeUs = currentTimeUs;

    //-----------------------------
    // Read Sensors
    //-----------------------------
    readRawIMU();
    scaleIMU();

    if (magnetometerReady) {
        readRawMag();
        applyMagCalibration();
    }

    //-----------------------------
    // Attitude Estimation
    //-----------------------------
    updateAttitude(dt);

    // Keep serial activity out of the sensor update cadence as much as possible.
    if (currentTimeUs - previousTelemetryUs >= 100000UL) {
        previousTelemetryUs = currentTimeUs;
        Serial.print("dt_us="); Serial.print(static_cast<uint32_t>(dt * 1000000.0f));
        Serial.print(" acc_g="); Serial.print(imu.ax, 3); Serial.print(',');
        Serial.print(imu.ay, 3); Serial.print(','); Serial.print(imu.az, 3);
        Serial.print(" gyro_dps="); Serial.print(imu.gx, 2); Serial.print(',');
        Serial.print(imu.gy, 2); Serial.print(','); Serial.print(imu.gz, 2);
        Serial.print(" attitude_deg="); Serial.print(attitude.roll, 2); Serial.print(',');
        Serial.print(attitude.pitch, 2); Serial.print(','); Serial.println(attitude.yaw, 2);
    }
}
