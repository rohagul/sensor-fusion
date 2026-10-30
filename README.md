# Sensor Fusion

A ground-up Attitude and Heading Reference System (AHRS) built on an Arduino UNO and a GY-87 IMU module. The goal is to understand and implement every layer of orientation estimation by hand — sensor drivers, calibration, and filtering — rather than relying on existing libraries.

> **Status:** Acceleration, gyro, and magnetometer calibration is complete.; the complementary filter is under active development.

---

## Hardware

| Component | Part | 
|---|---|
| MCU | Arduino UNO (ATmega328P, 2 KB SRAM) |
| Accel / Gyro | MPU6050 | 
| Magnetometer | QMC5883P (I²C `0x2C`) |
| Barometer | BMP085 | 

All sensors come on a single **GY-87** breakout.

---

## Architecture

The project is split into two codebases. src/ contains main code for sending and recieving serial data from Arduino as well as calibration for magnetometer to avoid large computational expense on the MC. mpu_arduino/ consists of the Arduino firmware. time_series/ contains main code for generating Allan Variance plots for the sensors. test/ contains code to test the ellipsoid linear programming problem. Calibration of accelerometer and gyro remain on the arduino. 

## Development Environment

- **Firmware:** PlatformIO in VS Code, target ATmega328P (AVR)
- **Host tool:** C++ with [Eigen](https://eigen.tuxfamily.org/), wildcard Makefile, WSL2
- **USB passthrough:** [usbipd-win](https://github.com/dorssel/usbipd-win), device appears as `/dev/ttyACM0`
- **Serial:** 9600 baud, `VMIN=0` / `VTIME=5`

### Attaching the board in WSL2

`attach` must be repeated every WSL session:

```powershell
usbipd bind --busid <BUSID>            # one-time (add --force here if needed)
usbipd attach --wsl --busid <BUSID>    # every session
```

## References

- J. Farrell, *Aided Navigation: GPS with High Rate Sensors* — complementary filter (§7.3.1), AHRS (Ch. 10)
- D. Titterton & J. Weston, *Strapdown Inertial Navigation Technology* — calibration procedures (Ch. 8)
- Y. Bar-Shalom et al., *Estimation with Applications to Tracking and Navigation*
- S. Boyd & L. Vandenberghe, *Convex Optimization*
