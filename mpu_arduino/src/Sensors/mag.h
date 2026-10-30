#ifndef MAG_H
#define MAG_H

#include <stdint.h>

#define QMC_ADDR 0x2C

class Magnetometer {
    
    private:
    bool initialized=false;
    bool calibrated=false;
    float A_inv[3][3]={{1,0,0},{0,1,0},{0,0,1}}; // Inverse of the calibration matrix
    float B[3]={0,0,0};    // Calibration offset

    enum State {
    STATE_PROMPT,
    STATE_CALIBRATING,
    STATE_STREAMING};

    public:
    State state=STATE_PROMPT;
    bool prompPrinted=false;
 
    bool initialize();
    bool calibrate();
    void check_sensor_state(State &state, bool &promptPrinted, bool &mag_ready_for_streaming);
    void getMagRaw(int16_t &mx, int16_t &my, int16_t &mz);
    void getMagCorrected(float &mx, float &my, float &mz);
    
};

#endif