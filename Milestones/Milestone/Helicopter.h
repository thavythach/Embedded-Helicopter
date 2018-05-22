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
#include "buttonsControl.h"

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
void tailProportional(int32_t yaw){
double pcontrol_update ( double error , double K_P );

