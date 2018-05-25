#include "Helicopter.h"

static double error_integrated = 0.0;
static double error_previous = 0.0;

void mainProportional(uint32_t tmpAltitude){
    double error;
    double control;

    double control_1;
    float delta = 0.01;
    float p_gain = 7;
    float i_gain = 0;
    float d_gain = 0;
    float error_d;
    //pacerWait() ;
    if (getSW1mode() != 0) {
        error = abs(getAltitudePercentSetPoint() - tmpAltitude);
        error_integrated += error*delta;
        error_d = (error - error_previous)/delta;
        control = error* p_gain + error_integrated*i_gain + error_d*d_gain;
        error_previous = error;


        if (tmpAltitude > getAltitudePercentSetPoint()) {
            control = 2;
        }
        setPWM(1,  (int)control);
    }
}

void tailProportional(int32_t tmpYaw){
    double error;
    double control;
    float delta = 0.01; //Intergral/derivative increments
    float p_gain = 7;
    float i_gain = 0;
    float d_gain = 0;
    float error_d;
    //pacerWait() ;

    if (getSW1mode() != 0) { //Need to make the posistion relative to the yaw degrees. Nedd to add derivative and intergal control to advoid this.
        error = abs(getYawDegreesSetPoint() - yawDegreeConvert(tmpYaw));
        error_integrated += error*delta;
        error_d = (error - error_previous)/delta;
        control = error* p_gain + error_integrated*i_gain + error_d*d_gain;
        error_previous = error;
        setPWM(0,  (int)control);
    }
}

void PIDController(uint32_t tmpAltitude, int32_t tmpYaw){
    mainProportional(tmpAltitude);
    tailProportional(tmpYaw);
}



