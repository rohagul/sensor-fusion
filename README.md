# Sensor Fusion

A ground-up Attitude and Heading Reference System (AHRS) built on an Arduino UNO and a GY-87 IMU module. The goal is to understand and implement every layer of orientation estimation by hand — sensor drivers, calibration, and filtering — rather than relying on existing libraries.

> **Status:** Acceleration, gyro, and magnetometer calibration is complete.; the complementary filter is under active development.

---

## Hardware

| Component | Part | 
|---|---|
| MCU | Arduino UNO  |
| Accel / Gyro | MPU6050 | 
| Magnetometer | QMC5883P  |
| Barometer | BMP085 | 

All sensors come on a single **GY-87** breakout.

---

## Architecture

The project is split into two codebases. src/ contains main code for sending and receiving serial data from Arduino as well as calibration for magnetometer to avoid large computational expense on the MCU. mpu_arduino/ consists of the Arduino firmware. time_series/ contains main code for generating Allan Variance plots for the sensors. test/ contains code to test the ellipsoid linear programming problem. Calibration of accelerometer and gyro remain on the arduino. 

## References

- J. Farrell, *Aided Navigation: GPS with High Rate Sensors* — complementary filter (§7.3.1), AHRS (Ch. 10)
- D. Titterton & J. Weston, *Strapdown Inertial Navigation Technology* — calibration procedures (Ch. 8)
- Y. Bar-Shalom et al., *Estimation with Applications to Tracking and Navigation*
- S. Boyd & L. Vandenberghe, *Convex Optimization*
