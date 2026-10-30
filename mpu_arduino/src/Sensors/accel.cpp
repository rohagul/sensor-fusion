#include "accel.h"
#include <Wire.h>
#include <Arduino.h>



void Accelerometer::check_sensor_state(State &state, bool &promptPrinted, bool &accel_ready_for_streaming) {      
switch (state) {

    case STATE_PROMPT: {
      if (!promptPrinted) {
        Serial.println("[REPLY]Calibrate Accelerometer? (y/n)");
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
        Serial.println("Accelerometer calibration complete.");
        Serial.println("Bias: X=" + String(bias_x) + ", Y=" + String(bias_y) + ", Z=" + String(bias_z));
        Serial.println("Scale: X=" + String(scale_x) + ", Y=" + String(scale_y) + ", Z=" + String(scale_z));
        state = STATE_STREAMING;
      }
      break;
    }

    case STATE_STREAMING: {
      accel_ready_for_streaming=true;
      break;
    }
  }

}

bool Accelerometer::initialize() {
    
    Wire.begin();
    
    Wire.beginTransmission(MPU_ADDR); //start communication with MPU6050
    Wire.write(0x6B); // Queue the PWR_MGMT_1 register (6B hex)
    Wire.write(0x01); // Set the register bits as 00000001 to wake up the MPU6050 , 1 BECAUSE clock selection is X-axis gyro.
    Wire.endTransmission(true); //end the transmission and release the I2C bus

    Wire.beginTransmission(MPU_ADDR); 
    Wire.write(0x19); // SMPLRT_DIV register
    Wire.write(0x00);
    Wire.endTransmission(true); //Sample rate 1k Hz

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x1A); // CONFIG register
    Wire.write(0x00); //set Digital Low Pass Filter to 260 Hz
    Wire.endTransmission(true);

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x1C); // ACCEL_CONFIG register
    Wire.write(0x00); //set accelerometer to +/-2g , LSB sensitivity is 16384 LSB/g
    Wire.endTransmission(true);

    initialized = true; // Set the initialized flag to true
    return initialized;

}

bool Accelerometer::calibrate() {
     
    int numSamples = 200;
    long sumX, sumY, sumZ;   // declared once, reused across all six positions
    int16_t rawAccelxposavg, rawAccelxnegavg, rawAccelyposavg, rawAccelynegavg, rawAccelzposavg, rawAccelznegavg;

    Serial.println("Calibrating Accelerometer...");

    Serial.println("[REPLY]Please keep the device LEVEL. Press Enter when done.");
    
    while (Serial.available()==0) {
        // Wait for user input
    }   
    sumX = 0; sumY = 0; sumZ = 0;
    for (int i = 0; i < numSamples; i++) {
      int16_t x, y, z;
      getAccelRaw(x, y, z);
      sumX += x; sumY += y; sumZ += z;
      delay(10);
    }
    rawAccelzposavg = sumZ / numSamples;   
 
    while(Serial.available()>0){Serial.read();}

    Serial.println("[REPLY]Please keep the device RIGHT. Press Enter when done.");
    while (Serial.available()==0) {
        // Wait for user input
    }   
    sumX = 0; sumY = 0; sumZ = 0;
    for (int i = 0; i < numSamples; i++) {
      int16_t x, y, z;
      getAccelRaw(x, y, z);
      sumX += x; sumY += y; sumZ += z;
      delay(10);
    }
    rawAccelxnegavg = sumX / numSamples;  
 
    while(Serial.available()>0){Serial.read();}

    Serial.println("[REPLY]Please keep the device LEFT. Press Enter when done.");
    while (Serial.available()==0) {
        // Wait for user input
    }   
    sumX = 0; sumY = 0; sumZ = 0;
    for (int i = 0; i < numSamples; i++) {
      int16_t x, y, z;
      getAccelRaw(x, y, z);
      sumX += x; sumY += y; sumZ += z;
      delay(10);
    }
    rawAccelxposavg = sumX / numSamples;   
 

    while(Serial.available()>0){Serial.read();}

    Serial.println("[REPLY]Please keep the device NOSEUP. Press Enter when done.");
    while (Serial.available()==0) {
        // Wait for user input
    }
    sumX = 0; sumY = 0; sumZ = 0;
    for (int i = 0; i < numSamples; i++) {
      int16_t x, y, z;
      getAccelRaw(x, y, z);
      sumX += x; sumY += y; sumZ += z;
      delay(10);
    }
    rawAccelyposavg = sumY / numSamples;   // Z, not X - fixed
 
    while(Serial.available()>0){Serial.read();}

    Serial.println("[REPLY]Please keep the device NOSEDOWN. Press Enter when done.");
    while(Serial.available()==0) {
        // Wait for user input
    }
    sumX = 0; sumY = 0; sumZ = 0;
    for (int i = 0; i < numSamples; i++) {
      int16_t x, y, z;
      getAccelRaw(x, y, z);
      sumX += x; sumY += y; sumZ += z;
      delay(10);
    }
    rawAccelynegavg = sumY/ numSamples;   // Z, not X - fixed
 
  
    while(Serial.available()>0){Serial.read();}

    Serial.println("[REPLY]Please keep the device BACK. Press Enter when done.");
    while(Serial.available()==0) {
        // Wait for user input
    }

    sumX = 0; sumY = 0; sumZ = 0;
    for (int i = 0; i < numSamples; i++) {
      int16_t x, y, z;
      getAccelRaw(x, y, z);
      sumX += x; sumY += y; sumZ += z;
      delay(10);
    }
    rawAccelznegavg = sumZ / numSamples;   // Z, not X - fixed
 
    while(Serial.available()>0){Serial.read();}
    
     bias_x=(rawAccelxposavg + rawAccelxnegavg)/2;
     bias_y=(rawAccelyposavg + rawAccelynegavg)/2;
     bias_z=(rawAccelzposavg + rawAccelznegavg)/ 2;

     scale_x=(rawAccelxposavg - rawAccelxnegavg)/2;
     scale_y=(rawAccelyposavg - rawAccelynegavg)/2;
     scale_z=(rawAccelzposavg - rawAccelznegavg)/2;
    
    calibrated = true; // Set the calibrated flag to true
    return calibrated ; // Calibration successful;
}

void Accelerometer::getAccelRaw(int16_t &ax, int16_t &ay, int16_t &az) //changing value of the argument by reference.
{
  // Raw but scaled 
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 6, true); // request a total of 6 registers

    ax = Wire.read() << 8 | Wire.read(); // as an integer
    ay = Wire.read() << 8 | Wire.read();
    az = Wire.read() << 8 | Wire.read();

}


void Accelerometer::getAccelCorrected(float &ax, float &ay, float &az) //changing value of the argument by reference.
{
  int16_t axraw, ayraw, azraw;
  // Raw but scaled 
    getAccelRaw(axraw, ayraw, azraw);
    ax=(axraw-bias_x)/scale_x*9.81/16384.0;
    ay=(ayraw-bias_y)/scale_y*9.81/16384.0;
    az=(azraw-bias_z)/scale_z*9.81/16384.0;

    
}

