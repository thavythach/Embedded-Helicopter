#include "buttons4.h"
#include "utils/ustdlib.h"
#include <stdint.h>

#define SW1_PORT  SYSCTL_PERIPH_GPIOA
#define SW1_PORT_BASE  GPIO_PORTA_BASE
#define SW1_PIN  GPIO_PIN_7

 struct setpoints {
    uint8_t altSetPoint; //integer between 0 and 100
    int32_t yawSetPoint; // integer rounded degree value, current degrees from reference +/- change
}setPoints;

static int8_t mode;
static int8_t SW1Position;
enum position {DOWN = 0, UP = 1};



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
 * return: void
 */
void checkAndIncrementYaw();

void resetPeriphButtons(void);

/*
 * check if left button is pressed and decrement yaw set point
 * return: void
 */
void checkAndDecrementYaw();

/*
 * getter for the altitude set point
 * return: most current altitude set point percent
 */
int32_t getAltitudePercentSetPoint();

/*
 * Getter for the yaw set point
 * return: most current yaw set point
 */
int16_t getYawDegreesSetPoint();

/*
 * place above main loop, pass yaw variable and altitude percent variable
 *
 * param: yaw - global variable for yaw counter
 * param: altPercent - altitude percentage in integer form
 */
void buttonControllerInit(int32_t yaw, uint8_t altPercent);


/*
 * place in the main loop to check the buttons for input commands
 */
void buttonControllerLoop();

/*
 * initializer code for SW1
 * return: void
 */
void SW1setup();

/*
 * get the state of the mode switch, SW1. modes: 0 - landed, 1 - flying, 2 - landing
 * helicopter goes to ref yaw position  and motors turn off in made 0, helicopter is enabled for
 * flying in mode 1 and may fly, landing mode (2) is set when the heli is in the process of landing.
 * return: int8_t
 */
uint8_t getSW1mode();

/*
 * use this to set the mode of the switch, based on the previous mode
 * switch position down (0) = mode 0
 * switch position up (1) = mode 1
 * switch position down (0) and heli is not landed = mode 2
 * param: int8_t mode
 * return: void
 */
void setSW1mode(int8_t mode);

/*
 * get the current position of the switch, down(0), up(1)
 * return int8_t (0 or 1)
 */
uint8_t getSW1Position();


/*
 * initialize pin change interrupts on the sw1 port A, pin 7
 * interrupt handler: SW1IntHandler()
 * return: void
 */
void initGPIOAPinChangeInterrupts(void);

/*
 * When sw1 is thrown, set the new mode based on the current state of sw1 and conditions
 * accounted for in setMode()
 */
void SW1IntHandler();



