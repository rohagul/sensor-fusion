#include "mag.h"
#include <Wire.h>
#include <Arduino.h>


void Magnetometer::check_sensor_state(State &state, bool &promptPrinted, bool &mag_ready_for_streaming) {      
switch (state) {

    case STATE_PROMPT: {
      if (!promptPrinted) {
        Serial.println("[REPLY]Calibrate Magnetometer? (y/n)");
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
        Serial.println("Magnetometer calibration complete.");
        state = STATE_STREAMING;
      }
      break;
    }

    case STATE_STREAMING: {
      mag_ready_for_streaming=true;
      break;
    }
  }

}

bool Magnetometer::initialize(){
    
    Wire.begin();
    
    Wire.beginTransmission(QMC_ADDR); //start communication with QMC5883L
    Wire.write(0x0A); // select configuration register A
    Wire.write(0xC3); 
    Wire.endTransmission(true); // end the transmission and release the I2C bus

    Wire.beginTransmission(QMC_ADDR); // start communication with QMC5883L
    Wire.write(0x0B);
    Wire.write(0x08);
    Wire.endTransmission(true);

    initialized = true; // Set the initialized flag to true
    return initialized;
}

bool Magnetometer::calibrate(){
    int16_t mx,my,mz;
    int numSamples=500;

    Serial.println("Calibrating Magnetometer...");
    Serial.println("Rotate on all three axes.");
    Serial.println("[REPLY]Press Enter when Starting.");

    while(Serial.available()==0){}
    Serial.println("Sending Data");

    for (int i = 0; i < numSamples; i++) {
    getMagRaw(mx, my, mz);
    Serial.print("[DATA_MAG_CAL]");
    Serial.print(mx);
    Serial.print(",");
    Serial.print(my);
    Serial.print(",");
    Serial.println(mz);
    delay(100);
  
  }
    Serial.println("Data Sent!");
    
    while(Serial.available()>0){Serial.read();}

    if (Serial.available()) {

    String line = Serial.readStringUntil('\n');
    line.trim();

    if (line.startsWith("A,")) {

        line.remove(0, 2);

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {

                int comma = line.indexOf(',');

                if (comma == -1) {
                    A_inv[i][j] = line.toFloat();
                    line = "";
                } else {
                    A_inv[i][j] = line.substring(0, comma).toFloat();
                    line.remove(0, comma + 1);
                }
            }
        }
     
    }
    
    else if (line.startsWith("B,")) {

        line.remove(0, 2);

        for (int i = 0; i < 3; i++) {

            int comma = line.indexOf(',');

            if (comma == -1) {
                B[i] = line.toFloat();
                line = "";
            } else {
                B[i] = line.substring(0, comma).toFloat();
                line.remove(0, comma + 1);
            }
        }

        
    }
}

    calibrated=true;
    return calibrated; // Calibration successful;

}
void Magnetometer::getMagRaw(int16_t &mx, int16_t &my, int16_t &mz) //changing value of the argument by reference.
{
    Wire.beginTransmission(QMC_ADDR); // start communication with QMC5883L
    Wire.write(0x01);
    Wire.endTransmission(false);
    Wire.requestFrom(QMC_ADDR, 6, true);

    mx = Wire.read() | Wire.read() << 8;
    my = Wire.read() | Wire.read() << 8;
    mz = Wire.read() | Wire.read() << 8;


}
void Magnetometer::getMagCorrected(float &mx, float &my, float &mz) //changing value of the argument by reference.
{
  int16_t mxraw, myraw, mzraw;
  // Raw but scaled 
    getMagRaw(mxraw, myraw, mzraw);
    mxraw=(mxraw*1.0/3750.0); // Convert from Raw value to Gauss value by dividing with LSB sensitivity of 3750 LSB/Gauss
    myraw=(myraw*1.0/3750.0);
    mzraw=(mzraw*1.0/3750.0);
    float m[3] = {mxraw, myraw, mzraw};
    float centered_m[3] = {m[0] - B[0], m[1] - B[1], m[2] - B[2]};
    float m_corrected[3];
    for (int i = 0; i < 3; i++) {
        m_corrected[i] = 0;
        for (int j = 0; j < 3; j++) {
            m_corrected[i] += A_inv[i][j] * centered_m[j];
        }
    }
    mx=m_corrected[0];
    my=m_corrected[1];
    mz=m_corrected[2];  
     

}

