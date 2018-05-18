

void mainProportional(int32_t altitude, int32_t setpointAlt){ 
    double error;
    double control;

    pacer_wait () ; 

    error = setpointAlt - altitude;
    control = pcontrol_update(error, 1) ;

    setPWM(1,  control);  
}

void tailProportional(int32_t yaw, int32_t setpointYaw){
    double error;
    double control;

    pacer_wait () ; 

    error = setpointYaw - yaw;
    control = pcontrol_update(error, 1) ; // K_P is 1 for now

    setPWM(1,  control);  
}
