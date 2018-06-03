/*
 * heliPID.c contains code used by the PI controller, the function PIDController(...) is
 * called in the round robin loop in main.c
 */
// @author0 Jed McDermott
// @author1 Thavy Thach
// @author2 Gabriel Lake
// @version 28/05/2018

#include "Helicopter.h"

int32_t error_integrated_main = 0; // accumulated error_int_main
int32_t error_integrated_tail = 0; // accumulated error_int_tail

/**
 * @desc Proportional and Integral control operation for a helicopter.
 * @param uint32_t tmpAltitude current height of helicopter
 * @param int32_t tmpYaw current yaw of helicopter
 * @return N/A
 */
void PIDController(uint32_t tmpAltitude, int32_t tmpYaw){

    int32_t error_main, error_tail, main_control, tail_control;
    float p_gain_main, i_gain_main;
    float p_gain_tail, i_gain_tail;

    // Intergral Increments
    //delta kept at 1, KI used to control the 'time' interval of integration
    float delta = 1;

    // main gains
    p_gain_main = 0.6;
    i_gain_main = 0.01; //implied delta_t in this gain value

    // tail gains
    p_gain_tail = 0.45;
    i_gain_tail = 0.02; // 0.02; //implied delat_t in this gain value;

    // construct error margins
    if (getSW1mode() != LANDED) {
        error_tail = getYawDegreesSetPoint() - yawDegreeConvert(tmpYaw);
        error_main = getAltitudePercentSetPoint() - tmpAltitude;

        error_integrated_tail += error_tail*delta;
        error_integrated_main += error_main*delta;

        tail_control = error_tail* p_gain_tail + error_integrated_tail*i_gain_tail;
        main_control = error_main* p_gain_main + error_integrated_main*i_gain_main;

        setPWM(0,  tail_control);
        setPWM(1,  main_control);
    }
}




