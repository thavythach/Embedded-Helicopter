#include "Helicopter.h"

int32_t error_integrated_main = 0.0;
int32_t error_integrated_tail = 0.0;

void PIDController(uint32_t tmpAltitude, int32_t tmpYaw){

    int32_t error_main;
    int32_t error_tail;
    int32_t control_tail;
    int32_t main_control;

    float p_gain_main = 0.6;
    float i_gain_main = 0;
    float delta = 1; //Intergral/derivative increments
    float p_gain_tail = 0.3;
    float i_gain_tail = 0.02;

    if (getSW1mode() != 0) {
        error_tail = getYawDegreesSetPoint() - yawDegreeConvert(tmpYaw); // p_tail
        error_main = getAltitudePercentSetPoint() - tmpAltitude; // p_main

        error_integrated_tail += error_tail*delta; // integral_tail
        error_integrated_main += error_main*delta; // integral_main

        control_tail = error_tail* p_gain_tail + error_integrated_tail*i_gain_tail; // tail_duty
        main_control = error_main* p_gain_main + error_integrated_main*i_gain_main;

        setPWM(0,  control_tail);
        setPWM(1,  main_control);
    }
}




