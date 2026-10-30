#include "I2C.h"

void enableBypass(){
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x37); // enable I2C bypass mode to access the magnetometer directly
    Wire.write(0x02);
    Wire.endTransmission(true); 

    Wire.beginTransmission(QMC_ADDR);
    int byte_error = Wire.endTransmission();
    if(byte_error == 0){
        Serial.println("MPU6050 I2C Bypass Completed");
    } else {
        Serial.print("Error enabling I2C Bypass:");
        Serial.println(byte_error);
    }
}
void scanI2C() {
  Serial.println("Scanning I2C bus for DevID");
  int found = 0;

  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Device found at 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
      found++;
    }
  }

  if (found == 0) {
    Serial.println("No I2C devices found.");
  } else {
    Serial.print(found);
    Serial.println(" device(s) found.");
  }
}
