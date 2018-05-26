#include "Helicopter.h"

static double error_integrated = 0.0;

void mainProportional(uint32_t tmpAltitude){
    double error;
    double control;

    float delta = 1;
    float p_gain = 3;
    float i_gain = 0;


    if (getSW1mode() != 0) {
        error = abs(getAltitudePercentSetPoint() - tmpAltitude);
        error_integrated += error*delta;
        control = error* p_gain + error_integrated*i_gain;


        if (tmpAltitude > getAltitudePercentSetPoint()) {
            control = 2;
        }
        setPWM(1,  (int)control);
    }
}

void tailProportional(int32_t tmpYaw){
    double error;
    double control;
    float delta = 1; //Intergral/derivative increments
    float p_gain = 1;
    float i_gain = 0;

    if (getSW1mode() != 0) { //Need to make the posistion relative to the yaw degrees. Nedd to add derivative and intergal control to advoid this.
        error = getYawDegreesSetPoint() - yawDegreeConvert(tmpYaw);
        if (error >= 0) {
            error_integrated += error*delta;
            control = error* p_gain + error_integrated*i_gain;
            if (tmpYaw > getYawDegreesSetPoint()) {
                control = 0;
            }

            setPWM(0,  (int)control);
        }
        else {
            error_integrated += abs(error)*delta;
            control = abs(error)* p_gain + error_integrated*i_gain;
            if (tmpYaw < getYawDegreesSetPoint()) {
                control = 0;
            }
            setPWM(1,  (int)control);
        }

    }


}

void PIDController(uint32_t tmpAltitude, int32_t tmpYaw){
    mainProportional(tmpAltitude);
    tailProportional(tmpYaw);
}



