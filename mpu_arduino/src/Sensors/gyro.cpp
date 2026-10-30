#include "gyro.h"
#include <Wire.h>
#include <Arduino.h>

void Gyro::check_sensor_state(State &state, bool &promptPrinted,bool &gyro_ready_for_streaming) {
switch (state) {
    case STATE_PROMPT: {
      if (!promptPrinted) {
        Serial.println("[REPLY]Calibrate Gyroscope? (y/n)");
        promptPrinted = true;
      }

      while (Serial.available() == 0) { }
      String reply = Serial.readStringUntil('\n');
      char response = reply.length() > 0 ? reply[0] : 0;
        if (response == 'y' || response == 'Y') {
          state = STATE_CALIBRATING;
        } else if (response == 'n' || response == 'N') {
          state = STATE_STREAMING;
        }
      
      while(Serial.available()>0){
        Serial.read();
      } // flush out the extra enter 
      break;
    }

    case STATE_CALIBRATING: {
      bool calibrated = calibrate();
      if (calibrated){
        Serial.println("Gyroscope calibration complete.");
        Serial.println("Bias: X=" + String(bias_x) + ", Y=" + String(bias_y) + ", Z=" + String(bias_z));
        state = STATE_STREAMING;
      }
      break;
    }

    case STATE_STREAMING: {
      gyro_ready_for_streaming=true;
      break;
    }
  }

}

bool Gyro::initialize()
{
    
    Wire.begin();
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x1B); // GYRO_CONFIG register
    Wire.write(0x08); //set gyro to +/-500 deg/s , LSB sensitivity is 65.5 LSB/deg/s
    Wire.endTransmission(true);

    initialized = true; // Set the initialized flag to true
    return initialized;

}

bool Gyro::calibrate(){
    int numSamples = 200;
    Serial.println("Calibrating Gyroscope...");
    Serial.println("[REPLY]Please keep the device STATIONARY and LEVEL. Press Enter when done.");
    while (Serial.available() == 0) { }
    long sumX = 0, sumY = 0, sumZ = 0;
    for (int i = 0; i < numSamples; i++) {
      int16_t x, y, z;
      getGyroRaw(x, y, z);
      sumX += x; sumY += y; sumZ += z;
      delay(10);
    }
    bias_x = (float)sumX / numSamples;
    bias_y = (float)sumY / numSamples;
    bias_z = (float)sumZ / numSamples;
    return true; // Calibration successful;
}

void Gyro::getGyroRaw(int16_t &gx, int16_t &gy, int16_t &gz) //changing value of the argument by reference.
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x43); // starting with register 0x43 (GYRO_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 6, true); // request a total of 6 registers

    gx = Wire.read() << 8 | Wire.read(); // as an integer
    gy = Wire.read() << 8 | Wire.read();
    gz = Wire.read() << 8 | Wire.read();

}

void Gyro::getGyroCorrected(float &gx, float &gy, float &gz) //changing value of the argument by reference.
{
  int16_t gxraw, gyraw, gzraw;
  // Raw but scaled 
    getGyroRaw(gxraw, gyraw, gzraw);
    gx=(gxraw-bias_x)/65.0; // Convert from Raw value to degrees/second value by dividing with LSB sensitivity of 65 LSB/(deg/s)
    gy=(gyraw-bias_y)/65.0;
    gz=(gzraw-bias_z)/65.0;

     
}





 