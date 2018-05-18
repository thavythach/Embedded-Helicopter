

void mainProportional(uint32_t altitude){ 
    double error;
    double control;

    pacer_wait () ; 

    error = altitude - getAltitudeSetPoint();
    control = pcontrol_update(error, 1) ;

    setPWM(1,  control);  
}

void tailProportional(int32_t yaw){
    double error;
    double control;

    pacer_wait () ; 

    error = yaw - getYawSetPoint();
    control = pcontrol_update(error, 1) ; // K_P is 1 for now

    setPWM(1,  control);  
}
