#include "buttons4.h"
#include "utils/ustdlib.h"
#include <stdint.h>


 struct setpoints {
    uint8_t altSetPoint; //integer between 0 and 100
    int32_t yawSetPoint; // integer rounded degree value, current degrees from reference +/- change
}setPoints;


/*
 * convert raw yaw counter value to degrees
 * param: yawCount - raw yaw counter data, from global variable in Helicopter.c
 * return: int16_t yaw count converted to degrees
 */
int16_t yawDegreeConvert(int32_t yaw);

/*
 * Use this function to initialize set points for the PID before any TIVA buttons are pressed
 * param: altPercent  - integer from 0 to 100 representing altitude percent
 * param: yaw - raw yaw counter value - to be converted to degrees
 * return: void
 */
void initSetPoints(int32_t yaw, uint8_t altPercent);

/*
 * check if up button is pressed and increment altitude percent set point
 */
void checkAndIncrementAltitude();


/*
 * check if down button is pressed and decrement altitude percent set point
 */
void checkAndDecrementAltitude();

/*
 * check if right button is pressed and increment yaw set point
 */
void checkAndIncrementYaw();

/*
 * check if left button is pressed and decrement yaw set point
 */
void checkAndDecrementYaw();

/*
 * return: most current altitude set point percent
 */
int32_t getAltitudePercentSetPoint();

/*
 * return: most current yaw set point
 */
int8_t getYawDegreesSetPoint();

/*
 * place above main loop, pass yaw variable and altitude percent variable
 *
 * param: yaw - global variable for yaw counter
 * param: altPercent - altitude percentage in integer form
 */
void buttonControllerInit(int32_t yaw, uint8_t altPercent);

void resetPeriphButtons(void);

/*
 * place in the main loop to check the buttons for input commands
 */
void buttonControllerLoop();

