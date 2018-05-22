static double error_integrated = 0.0; 
static double error_previous = 0.0;

void PID_Main (void) {
    
    float delta = 0.01;
    float p_gain = 5;
    float d_gain = 0;
    float i_gain = 0;
    float error_d;
    int control;

    error = abs(altitude - getAltitudeSetPoint());
    error_integrated += error*delta;
    error_d = (error - error_previous)/delta;

    if (error <= 5) {
        pwmSet(1, 30);
    }
    control = error* p_gain + error_integrated*i_gain + error_d*d_gain)
    error_previous = error;

    pwmSet(1, control);


}

void PID_Tail (void) {
    
    float delta = 0.01;
    float p_gain = 3;
    float d_gain = 0;
    float i_gain = 0;
    float error_d;
    int control;

    error = abs(yaw - getYawSetPoint());
    error_integrated += error*delta;
    error_d = (error - error_previous)/delta;

    if (error <= 5) {
        pwmSet(0, 15);
    }
    control = error* p_gain + error_integrated*i_gain + error_d*d_gain)
    error_previous = error;

    pwmSet(0, control);


}