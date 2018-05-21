#include "Helicopter.h"

void mainProportional(uint32_t altitude){ 
    double error;
    double control;

    pacer_wait () ; 

    error = altitude - getAltitudeSetPoint();
    control = pcontrol_update(error, 5) ; 

    setPWM(1,  (int)control);  
}

void tailProportional(int32_t yaw){
    double error;
    double control;

    pacer_wait () ; 

    error = yaw - getYawSetPoint();
    control = pcontrol_update(error, 3) ; 

    setPWM(1,  (int)control);  
}

double pcontrol_update ( double error , double K_P ){
    return error * K_P ;
}
