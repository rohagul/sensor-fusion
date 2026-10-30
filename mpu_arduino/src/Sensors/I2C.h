#ifndef I2C_H
#define I2C_H
#define MPU_ADDR 0x68
#define QMC_ADDR 0x2C
#include <Wire.h>
#include <Arduino.h>

void scanI2C();
void enableBypass();
#endif