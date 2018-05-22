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

//initialize static variables *************************************************************

//Functions *******************************************************************************
int16_t yawDegreeConvert(int32_t tempYaw) {
    return ((int) tempYaw * 0.8035714285714286);
}


void initSetPoints(int32_t tempYaw, uint8_t altPercent) {
    setPoints.altSetPoint = altPercent;
    setPoints.yawSetPoint = yawDegreeConvert(tempYaw);
}


void checkAndIncrementAltitude() {
    if (checkButton(UP) == PUSHED) {
        if (setPoints.altSetPoint <= 90) {
            setPoints.altSetPoint += 10;
        }
    }
}

void checkAndDecrementAltitude() {
    if (checkButton(DOWN) == PUSHED) {
        if (setPoints.altSetPoint >= 10) {
            setPoints.altSetPoint -= 10;
        }
    }
}

void checkAndIncrementYaw() {
    if (checkButton(RIGHT) == PUSHED) {
       setPoints.yawSetPoint += 15;
    }
}

void checkAndDecrementYaw() {
    if (checkButton(LEFT) == PUSHED) {
         setPoints.yawSetPoint -= 15;
      }
}

int32_t getAltitudePercentSetPoint() {
    return setPoints.altSetPoint;
}

int16_t getYawDegreesSetPoint() {
    return setPoints.yawSetPoint;
}

void buttonControllerInit(int32_t tempYaw, uint8_t altPercent) {
    initSetPoints(tempYaw, altPercent);
}

void buttonControllerLoop() {
    checkAndIncrementAltitude();
    checkAndDecrementAltitude();
    checkAndIncrementYaw();
    checkAndDecrementYaw();
}

void SW1setup() {
    SysCtlPeripheralEnable (SW1_PORT);
       GPIOPinTypeGPIOInput (SW1_PORT_BASE, UP_BUT_PIN);
       GPIOPadConfigSet (SW1_PORT_BASE, SW1_PIN, GPIO_STRENGTH_2MA,
          GPIO_PIN_TYPE_STD_WPD); //pull up or pull down??

}

uint8_t getSW1Position() {
    uint8_t SW1State = GPIOPinRead(SW1_PORT_BASE, SW1_PIN);
    return SW1State;
}

void setSW1mode(int8_t newMode) {
    mode = newMode;
}

uint8_t getSW1mode() {
    return mode;
}

void resetPeriphButtons(void){
    SysCtlPeripheralReset (UP_BUT_PERIPH);        // UP button GPIO
    SysCtlPeripheralReset (DOWN_BUT_PERIPH);      // DOWN button GPIO
    SysCtlPeripheralReset (RIGHT_BUT_PERIPH);      // RIGHT button GPIO
    SysCtlPeripheralReset (LEFT_BUT_PERIPH);      // LEFT button GPIO
}

void initGPIOAPinChangeInterrupts(void){


    // Enable the GPIOA peripheral
    SysCtlPeripheralEnable(SW1_PORT);

    /**Wait for the GPIOA module to be ready**/
    while(!SysCtlPeripheralReady(SW1_PORT)){
    }

    GPIOIntRegister(SW1_PORT, SW1IntHandler);

    /**Initialize the GPIO pin configuration**/

    // sets pin 0, 1 as in put, SW controlled.
    GPIOPinTypeGPIOInput(SW1_PORT, SW1_PIN);


    // makes sw1 pin rising and falling edge triggered interrupt
    GPIOIntTypeSet(SW1_PORT, SW1_PIN, GPIO_BOTH_EDGES);

    // Enable the sw1 pin interrupt
    GPIOIntEnable(SW1_PORT, SW1_PIN);
}

void SW1IntHandler() {
    if (getSW1mode() != 2) {
        if ((getSW1mode() == 1) && !getSW1Position()) {
            setSW1mode(2);
        }
        if (getSW1Position()) {
            setSW1mode(1);
        }
    }
  
   //add code to landing routine part of controller that can set the mode from 2 to 0
    //or when the ref yaw is matched and current mode is zero and altitude is zero, set mode to zero(landed)
}

void CheckLanding(void){
    if ((getSW1mode() == 2) && altitude <= 2) { // && ((ref-)) {
        setSW1mode(0);
    }
}




