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
#include "ButtonControl.h"



int16_t yawDegreeConvert(int32_t yaw) {
    return (int)(yaw * 0.8035714285714286);
}


void initSetPoints(int32_t yaw, uint8_t altPercent) {
    setPoints.altSetPoint = altPercent;
    setPoints.yawSetPoint = yawDegreeConvert(yaw);
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

int8_t getYawDegreesSetPoint() {
    return setPoints.yawSetPoint;
}

void resetPeriphButtons(void){
    SysCtlPeripheralReset (UP_BUT_PERIPH);        // UP button GPIO
    SysCtlPeripheralReset (DOWN_BUT_PERIPH);      // DOWN button GPIO
    SysCtlPeripheralReset (RIGHT_BUT_PERIPH);      // RIGHT button GPIO
    SysCtlPeripheralReset (LEFT_BUT_PERIPH);      // LEFT button GPIO
}

void buttonControllerInit(int32_t yaw, uint8_t altPercent) {
    initSetPoints(yaw, altPercent);
}

void buttonControllerLoop() {
    checkAndIncrementAltitude();
    checkAndDecrementAltitude();
    checkAndIncrementYaw();
    checkAndDecrementYaw();
}






