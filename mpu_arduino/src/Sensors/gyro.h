#ifndef GYRO_H
#define ACCEL_H

#include <stdint.h>

#define MPU_ADDR 0x68


class Gyro {
    
    private:
    bool initialized=false;
    bool calibrated=false;
    float bias_x=0.0, bias_y=0.0, bias_z=0.0;

    enum State {
    STATE_PROMPT,
    STATE_CALIBRATING,
    STATE_STREAMING};

    public:
    State state = STATE_PROMPT;
    bool promptPrinted = false;
    bool initialize();
    bool calibrate();
    void check_sensor_state(State &state, bool &promptPrinted,bool &gyro_ready_for_streaming);
    void getGyroRaw(int16_t &gx, int16_t &gy, int16_t &gz);
    void getGyroCorrected(float &gx, float &gy, float &gz);
};

#endif