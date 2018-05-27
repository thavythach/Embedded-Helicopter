#include "Helicopter.h"

int32_t error_integrated_main = 0.0;
int32_t error_integrated_tail = 0.0;
int32_t main_control = 0;
void mainProportional(uint32_t tmpAltitude){
    int32_t error;

    float delta = 1;
    float p_gain = 3;
    float i_gain = 0.01;


    if (getSW1mode() != 0) {
        error = getAltitudePercentSetPoint() - tmpAltitude;
        error_integrated_main += error*delta;
        main_control = error* p_gain + error_integrated_main*i_gain;
        setPWM(1,  (int)main_control);
    }
}

void tailProportional(int32_t tmpYaw){
    int32_t error;
    int32_t control;
    float delta = 1; //Intergral/derivative increments
    float p_gain = 1.5;
    float i_gain = 0;

    if (getSW1mode() != 0) { //Need to make the posistion relative to the yaw degrees. Nedd to add derivative and intergal control to advoid this.
        error = getYawDegreesSetPoint() - yawDegreeConvert(tmpYaw);
        if (error >= 0) {
            error_integrated_tail += error*delta;
            control = error* p_gain + error_integrated_tail*i_gain;
            setPWM(0,  (int)control);
        }
        else {
            error_integrated_tail += abs(error)*delta;
            control = abs(error)* p_gain + error_integrated_tail*i_gain;
            if (main_control >= control) {
                setPWM(1,  (int)main_control);
            }
            else {
                setPWM(1,  (int)control);

            }

        }

    }


}

void PIDController(uint32_t tmpAltitude, int32_t tmpYaw){
    mainProportional(tmpAltitude);
    tailProportional(tmpYaw);
}



