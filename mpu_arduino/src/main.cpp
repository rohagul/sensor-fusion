#include <Arduino.h>
#include "Sensors/I2C.h"
#include "Sensors/accel.h"
#include "Sensors/gyro.h"
#include "Sensors/mag.h"

Accelerometer accel;
Magnetometer mag;
Gyro gyro;

bool accelInitialized = false; // Flag to track if the accelerometer is initialized
bool magInitialized = false;   // Flag to track if the magnetometer is initialized
bool gyroInitialized = false;  // Flag to track if the gyroscope is initialized
bool accelCalibrated = false; // Flag to track if the accelerometer is calibrated
bool magCalibrated = false;   // Flag to track if the magnetometer is calibrated
bool gyroCalibrated = false;  // Flag to track if the gyroscope is calibrated
bool accel_ready_for_streaming = false;
bool gyro_ready_for_streaming = false;
bool mag_ready_for_streaming = false;

float ax, ay, az; // Accelerometer readings
float gx, gy, gz; // Gyroscope readings
float mx, my, mz; // Magnetometer readings
bool send_data=false;
bool sent_data=false;
unsigned long now;
unsigned long then=0;
void setup() {

Serial.begin(9600); 
scanI2C(); 
enableBypass();

//Initialize the Accelerometer
bool accelInitialized = accel.initialize();
if (accelInitialized) {
    Serial.println("Accelerometer initialized successfully.");
} else {
    Serial.println("Failed to initialize accelerometer.");
}

//Initialize the Magnetometer
bool magInitialized = mag.initialize();
if (magInitialized) {
    Serial.println("Magnetometer initialized successfully.");
} else {
    Serial.println("Failed to initialize magnetometer.");
}

//Initialize the Gyroscope
bool gyroInitialized = gyro.initialize();
if (gyroInitialized) {
    Serial.println("Gyroscope initialized successfully.");
} else {
    Serial.println("Failed to initialize gyroscope.");
}
}

void loop() {

while(!accel_ready_for_streaming){
accel.check_sensor_state(accel.state, accel.promptPrinted,accel_ready_for_streaming);
}

while(!mag_ready_for_streaming){
    mag.check_sensor_state(mag.state, mag.prompPrinted,mag_ready_for_streaming);
}

while(!gyro_ready_for_streaming){
    gyro.check_sensor_state(gyro.state, gyro.promptPrinted,gyro_ready_for_streaming);
}

/*
Find Orientation of the device using the accelerometer, magnetometer and gyroscope data
*/

if(sent_data==false){
Serial.println("[REPLY]Send IMU data? (y/n)");
while (Serial.available() == 0) { }
      String reply = Serial.readStringUntil('\n');
      char response = reply.length() > 0 ? reply[0] : 0;
        if (response == 'y' || response == 'Y') {
            send_data=true;
        } else if (response == 'n' || response == 'N') {
            send_data=false;
        }
  
while(Serial.available()>0){Serial.read();} //flush out extra enter 

while(send_data){
accel.getAccelCorrected(ax, ay, az);
mag.getMagCorrected(mx, my, mz);
gyro.getGyroCorrected(gx, gy, gz);

now = millis();

if(now-then>=10){
Serial.println(now);
Serial.print("[DATA_IMU]");
Serial.print(","); 
Serial.print(ax);
Serial.print(",");
Serial.print(ay);
Serial.print(",");  
Serial.print(az);
Serial.print(",");
Serial.print(gx);
Serial.print(",");
Serial.print(gy);
Serial.print(",");
Serial.print(gz);
Serial.print(",");
Serial.print(mx);
Serial.print(",");
Serial.print(my);
Serial.print(",");
Serial.println(mz);

then=now;
if(now > 7200000){send_data=false; sent_data=true;}
}
}

}
}