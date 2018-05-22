#include "Helicopter.h"

void mainProportional(uint32_t altitude){ 
    double error;
    double control;

   pacerWait() ;

    error = altitude - getAltitudePercentSetPoint();
    control = pcontrol_update(error, 5) ; 

    setPWM(1,  (int)control);  
}

void tailProportional(int32_t yaw){
    double error;
    double control;

    pacerWait() ;

    error = yaw - getYawDegreesSetPoint();
    control = pcontrol_update(error, 3) ; 

    setPWM(0,  (int)control);
}

double pcontrol_update ( double error , double K_P ){
    return error * K_P ;
}

void PIDController(uint32_t altitude, int32_t yaw){
    mainProportional(altitude);
    tailProportional(altitude);
}

