/** LIBRARIES | TIVA **/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"

/** LIBRARIES | ENCE361 **/
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons4.h"

/** Miscellaneous Prototypes **/
void initDisplay(void); // OLED display
void initClock (void); 
void UpdateDisplay();
void doCircularBufferApproximation(int32_t sum);
int32_t initializeMeanValue(int32_t isInit, int32_t meanVal);
uint32_t displayMeanVal(uint16_t meanVal, uint32_t count, uint16_t initMeanVal, uint32_t mode);
void initializeHelicopterOperations(void);
void resetHelicopterOperations(void);
void calculateRobustMeanValue(uint32_t initMeanVal, uint32_t meanVal);

static circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
static uint32_t g_ulSampCnt;    // Counter for the interrupts
static int32_t yaw; 
static uint8_t state;
static int32_t altitude; 
static int32_t deg;

#define BUF_SIZE 20
#define SAMPLE_RATE_HZ 100 // equation for it... 2*buffsize*fmax (2*10*4)
#define RANGE_ALTITUDE 983.0
#define sw1Pin GPIO_PIN_4 

/** 
 * YAW @heliYAW.c
 * Global Requirements:
 * - state
 * - yaw 
 * - #defines below
 * - tiva files
 * **/
#define CHANNEL_A_PIN GPIO_PIN_0
#define CHANNEL_B_PIN GPIO_PIN_1
#define YAW_PERIPH_GPIO_B SYSCTL_PERIPH_GPIOB
#define YAW_BASE_GPIO_B GPIO_PORTB_BASE
#define YAW_SAMPLE_RATE_HZ 1792 // minimum sampling rate (1792 SAM/s = 112 slots * 4 samples/slot * 4 rev/s)

void YawIntHandler(void);
void initYaw(void);

/**
 * ADC @heliADC.c
 * Global requirements:
 * - none
 * - tiva files
 * **/
void initADC (void);
void ADCIntHandler(void);



/**
 * PWM @heliPWM.c
 * Global Requirements:
 * - none
 * **/

struct pwmConfig {
	uint32_t rateMinHZ; // unused
	uint32_t rateMaxHZ; // unused
	uint32_t mainDutyCycle;
	uint32_t TailDutyCycle;
	uint32_t mainFrequency;
	uint32_t tailFrequency;
	uint32_t dividerCode;
	uint32_t divider;
 };

struct pwmRotor {
	uint32_t base;
	uint32_t gen;
	uint32_t outnum;
	uint32_t outbit;
	uint32_t periphPWM;
	uint32_t periphGPIO;
	uint32_t GPIOBase;
	uint32_t GPIOConfig;
	uint8_t GPIOPin;
};

// configuration for both dreams
struct pwmConfig config = {150, 300, 0, 0, 250, 200, SYSCTL_PWMDIV_4, 4};

//  PWM Hardware Details M0PWM7 (gen 3)
//  ---Main Rotor PWM: PC5, J4-05
struct pwmRotor mainRotor = { PWM0_BASE, PWM_GEN_3, PWM_OUT_7, PWM_OUT_7_BIT, SYSCTL_PERIPH_PWM0, SYS_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PC5_M0PWM7, GPIO_PIN_5 };

//  PWM Hardware Details M1PWM5 (gen 2)  
//  ---Tail Rotor PWM: PF1, J3-10   
struct pwmRotor tailRotor = { PWM1_BASE, PWM_GEN_2, PWM_OUT_5, PWM_OUT_5_BIT, SYSCTL_PERIPH_PWM1, SYS_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PF1_M0PWM5, GPIO_PIN_1 };

// prototypes
void setPWMClocks(void);
void setPWM(uint32_t isMainRotor, uint32_t ui32Duty); 
void initializePWM(uint8_t isMainMotor);
void resetPeripheralPWM(void);
void setOutputOnline(bool isOn);

// todo: a display of the pwm duty cycle for each motor as a percentage is maintained on the orbit display.

/**
 * PID @heliPID.c
 * Global Requirements:
 * - none 
*/

void mainProportional(uint32_t altitude);
void tailProportional(int32_t yaw);
double pcontrol_update ( double error , double K_P );

/**
 * Button Control @ ButtonControl.c
 * **/


#define SW1_PORT  SYSCTL_PERIPH_GPIOA
#define SW1_PORT_BASE  GPIO_PORTA_BASE
#define SW1_PIN  GPIO_PIN_7

 struct setpoints {
    uint8_t altSetPoint; //integer between 0 and 100
    int32_t yawSetPoint; // integer rounded degree value, current degrees from reference +/- change
} setPoints;

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
void SW1IntHandler(void);


void CheckLanding(void);


