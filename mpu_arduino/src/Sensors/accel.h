#ifndef ACCEL_H
#define ACCEL_H

#include <stdint.h>
#define MPU_ADDR 0x68

class Accelerometer {
    
    private:
    bool initialized=false;
    bool calibrated=false;
    float bias_x=0.0, bias_y=0.0, bias_z=0.0;
    float scale_x=1.0, scale_y=1.0, scale_z=1.0;

    enum State {
    STATE_PROMPT,
    STATE_CALIBRATING,
    STATE_STREAMING};

    public:
    State state = STATE_PROMPT;
    bool promptPrinted = false;
    bool initialize();
    bool calibrate();
    void check_sensor_state(State &state, bool &promptPrinted,bool &accel_ready_for_streaming);
    void getAccelRaw(int16_t &ax, int16_t &ay, int16_t &az);
    void getAccelCorrected(float &ax, float &ay, float &az);
};

#endif