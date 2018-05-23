#include "Helicopter.h"

void mainProportional(uint32_t tmpAltitude){
    double error;
    double control;

    //pacerWait() ;
    if (getSW1mode() != 0) {
        error = abs(getAltitudePercentSetPoint() - tmpAltitude);

        control = pcontrol_update(error, 1) ;
        if (tmpAltitude > getAltitudePercentSetPoint()) {
            control = 2;
        }
        setPWM(1,  (int)control);
    }
}

void tailProportional(int32_t tmpYaw){
    double error;
    double control;

    //pacerWait() ;

    if (getSW1mode() != 0) {
        error = abs(getYawDegreesSetPoint() - yawDegreeConvert(tmpYaw));
        control = pcontrol_update(error, 1) ;

        setPWM(0,  (int)control);
    }
}

double pcontrol_update ( double error , double K_P ){
    return error * K_P ;
}

void PIDController(uint32_t tmpAltitude, int32_t tmpYaw){
    mainProportional(tmpAltitude);
    tailProportional(tmpYaw);
}



