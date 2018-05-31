/*
 * Author: created by helicopter group A03
 * Function: provide methods for use in the main file 'Helicopter.c'
 * that meet the following criteria
 *
 * These methods are intended to interface button pushes with the C implemented PID controller
 *
 * 1. increase the craft altitude by 10% when the UP button is pressed
 * 2. decrease the craft altitude by 10% when the DOWN button is pressed
 * 3. rotate the craft 15 degrees CW when the RIGHT button is pressed
 * 4. rotate the craft 15 degrees CCW when the LEFT button is pressed
 * 5. Do not allow button operation that will overshoot altitude range.
 * 6. Provide methods that may pass set point data to the PID controller,
 * controlling the motors based on button presses.
 *
 * Method index
 *
 *
 */
#include "Helicopter.h"
volatile int8_t heliMode = 0;
int8_t switchFlipIndex = 0;
int8_t tempAlt = 0;

//initialize static variables *************************************************************

//Functions *******************************************************************************

/*
 * convert raw yaw counter value to degrees
 * param: yawCount - raw yaw counter data, from global variable in Helicopter.c
 * return: int16_t yaw count converted to degrees
 */
int32_t yawDegreeConvert(int32_t tempYaw) {
    return (int)(((float)tempYaw) * 0.8035714285714286);
}

/*
 * Use this function to initialize set points for the PID before any TIVA buttons are pressed
 * param: altPercent  - integer from 0 to 100 representing altitude percent
 * param: yaw - raw yaw counter value - to be converted to degrees
 * return: void
 */
void initSetPoints(int32_t tempYaw, uint8_t altPercent) {
    setPoints.altSetPoint = altPercent;
    setPoints.yawSetPoint = yawDegreeConvert(tempYaw);
}

void initResetButton() {
    SysCtlPeripheralEnable(RESET_PERIPH);
    GPIOPinTypeGPIOInput(RESET_BASE, RESET_PIN);
    GPIOPadConfigSet (RESET_BASE, RESET_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); // PULL UP
}

void resetSoft(){
    uint8_t resetState = GPIOPinRead(RESET_BASE, RESET_PIN);
    if (resetState){
        SysCtlReset();
    }
}

/*
 * check if up button is pressed and increment altitude percent set point
 */
void checkAndIncrementAltitude() {
    if (checkButton(UP) == PUSHED) {
        if (setPoints.altSetPoint <= 90) {
            setPoints.altSetPoint += 10;
        }
    }
}

/*
 * check if down button is pressed and decrement altitude percent set point
 */
void checkAndDecrementAltitude() {
    if (checkButton(DOWN) == PUSHED) {
        if (setPoints.altSetPoint >= 10) {
            setPoints.altSetPoint -= 10;
        }
    }
}

/*
 * check if right button is pressed and increment yaw set point
 * return: void
 */
void checkAndIncrementYaw() {
    if (checkButton(RIGHT) == PUSHED) {
        setPoints.yawSetPoint += 15;
    }
}

/*
 * check if left button is pressed and decrement yaw set point
 * return: void
 */
void checkAndDecrementYaw() {
    if (checkButton(LEFT) == PUSHED) {
         setPoints.yawSetPoint -= 15;
      }
}

/*
 * getter for the altitude set point
 * return: most current altitude set point percent
 */
int32_t getAltitudePercentSetPoint() {
    return setPoints.altSetPoint;
}

/*
 * Getter for the yaw set point
 * return: most current yaw set point
 */
int16_t getYawDegreesSetPoint() {
    return setPoints.yawSetPoint;
}

/*
 * place above main loop, pass yaw variable and altitude percent variable
 *
 * param: yaw - global variable for yaw counter
 * param: altPercent - altitude percentage in integer form
 */
void buttonControllerInit(int32_t tempYaw, int16_t altPercent) {
    initSetPoints(tempYaw, altPercent);
    initGPIOAPinChangeInterrupts();
}

/*
 * place in the main loop to check the buttons for input commands
 */
void buttonControllerLoop() {
    checkAndIncrementAltitude();
    checkAndDecrementAltitude();
    checkAndIncrementYaw();
    checkAndDecrementYaw();
}

/*
 * initializer code for SW1
 * return: void
 */
void SW1setup() {
    SysCtlPeripheralEnable (SW1_PORT);
   GPIOPinTypeGPIOInput (SW1_PORT_BASE, UP_BUT_PIN);
   GPIOPadConfigSet (SW1_PORT_BASE, SW1_PIN, GPIO_STRENGTH_2MA,
      GPIO_PIN_TYPE_STD_WPD); //pull down?

}

/*
 * get the current position of the switch, down(0), up(1)
 * return int8_t (0 or 1)
 */
uint8_t getSW1Position() {
    uint8_t SW1State = GPIOPinRead(SW1_PORT_BASE, SW1_PIN);
    return SW1State;
}

/*
 * use this to set the mode of the switch, based on the previous mode
 * switch position down (0) = mode 0
 * switch position up (1) = mode 1
 * switch position down (0) and heli is not landed = mode 2
 * param: int8_t mode
 * return: void
 */
void setSW1mode(int8_t newMode) {
    heliMode = newMode;
}

/*
 * get the state of the mode switch, SW1. modes: 0 - landed, 1 - flying, 2 - landing
 * helicopter goes to ref yaw position  and motors turn off in made 0, helicopter is enabled for
 * flying in mode 1 and may fly, landing mode (2) is set when the heli is in the process of landing.
 * return: int8_t
 */
uint8_t getSW1mode() {
    return heliMode;
}

/**
 * resets up/down/left/right button peripherals
 */
void resetPeriphButtons(void){
    SysCtlPeripheralReset (UP_BUT_PERIPH);        // UP button GPIO
    SysCtlPeripheralReset (DOWN_BUT_PERIPH);      // DOWN button GPIO
    SysCtlPeripheralReset (RIGHT_BUT_PERIPH);      // RIGHT button GPIO
    SysCtlPeripheralReset (LEFT_BUT_PERIPH);      // LEFT button GPIO
}

/*
 * initialize pin change interrupts on the sw1 port A, pin 7
 * interrupt handler: SW1IntHandler()
 * return: void
 */
void initGPIOAPinChangeInterrupts(void){


    // Enable the GPIOA peripheral
    SysCtlPeripheralEnable(SW1_PORT);

    /**Wait for the GPIOA module to be ready**/
    while(!SysCtlPeripheralReady(SW1_PORT)){
    }

    GPIOIntRegister(SW1_PORT_BASE, SW1IntHandler);

    /**Initialize the GPIO pin configuration**/

    // sets pin 0, 1 as in put, SW controlled.
    GPIOPinTypeGPIOInput(SW1_PORT_BASE, SW1_PIN);


    // makes sw1 pin rising and falling edge triggered interrupt
    GPIOIntTypeSet(SW1_PORT_BASE, SW1_PIN, GPIO_BOTH_EDGES);

    // Enable the sw1 pin interrupt
    GPIOIntEnable(SW1_PORT_BASE, SW1_PIN);


}

/*
 * When sw1 is thrown, set the new mode based on the current state of sw1 and conditions
 * accounted for in setMode()
 */
void SW1IntHandler() {
    if (getSW1mode() != LANDING) {
        if ((getSW1mode() == FLYING) && !getSW1Position()) {
            setSW1mode(LANDING);
        }
        if (getSW1Position()) {
            setSW1mode(FLYING);
            setOutputOnline(0,true);
            setOutputOnline(1,true);
        }
    }
    GPIOIntClear(SW1_PORT_BASE, SW1_PIN);
  
   //add code to landing routine part of controller that can set the mode from 2 to 0
    //or when the ref yaw is matched and current mode is zero and altitude is zero, set mode to zero(landed)
}


/**
 * @desc Constantly checking if Helicopter is in LANDED mode.
 * If it is in FLYING mode, then switched to LANDING, then the condition will reset alt setpoints
 * and set a new yaw setpoint and switch to LANDED mode (disabling PWM output signals).
 * @param N/A
 * @return N/A
 */
void checkLanded(void){
    startLanding();
    if ((getSW1mode() == LANDING) && altitude <= 2) { // && ((ref-)) {
       // setPWM(0, 0);
       // setPWM(1, 0);
        setOutputOnline(0,false); // set both PWM output signals online
        setOutputOnline(1,false); // set both PWM output signals online
        setSW1mode(LANDED);
    }
}

/*
 * @desc if LANDING mode is set, then Helicopter resets alt setpoints and sets a new yaw setpoint.
 * @todo FIX this function
 */
void startLanding(void){
    if (getSW1mode() == LANDING){
        setPoints.yawSetPoint = yawDegreeConvert(yaw);
        setPoints.altSetPoint = altitude - 5;
        while (altitude > 2) {
            PIDController(altitude, yaw);
            UART();
            if (altitude < (setPoints.altSetPoint + 1)) {
                setPoints.altSetPoint -= 5;
            }
        }
       // setSW1mode(LANDED);
    }
}






