/*
 * Helicopter.h includes all the necessary libraries, macros, global variables, and function prototypes
 * used in each source file written to meet specifications, and it plays a vital role in inter-task communication
 * All of this data used throughout the source code may be found
 * here, with the exception of circBufT.h and buttons4.h
 * Each source file must include Helicopter.h
 *
// @author0 Jed McDermott
// @author1 Thavy Thach
// @author2 Gabriel Lake
// @version 28/05/2018
 */


// all #includes**************************************************************************************************
/** LIBRARIES | TIVA **/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/uart.h"
#include <math.h>

/** LIBRARIES | ENCE361 **/
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons4.h"
//**************************************************************************************************************

//Helicopter.c *************************************************************************************************
//  variables and functions defined in Helicopter.c
void initDisplay(void); // OLED display
void initClock (void); 
void updateDisplay(int32_t val);
void displayMeanVal(uint16_t meanVal, uint16_t initMeanVal);
void initializeHelicopterOperations(void);
void resetHelicopterOperations(void);
void calculateRobustMeanValue(uint16_t initMeanVal, uint16_t meanVal);

//global variables defined in Helicopter.c
extern circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
extern volatile uint32_t g_ulSampCnt;    // Counter for the interrupts
extern volatile int32_t yaw;
extern volatile uint8_t state;
extern volatile int16_t altitude;

#define BUF_SIZE 20
#define SAMPLE_RATE_HZ 100
//*****************************************************************************************************************


/**
 * UART @heliUART.c,  variables and functions defined in heliUART.c
 * Global Requirements:
 * **///************************************************************************************************************
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX
#define SYSTICK_RATE_HZ 100
#define SLOWTICK_RATE_HZ 4

volatile uint8_t slowTick;

void initialiseUSB_UART (void);
void UARTSend (char *string);
char * getSW1modeString(int32_t mode);
void UART();
//*******************************************************************************************************************

/** 
 * YAW @heliYAW.c,  variables and functions defined in heliYAW.c
 * Global Requirements:
 * **///*************************************************************************************************************
#define CHANNEL_A_PIN GPIO_PIN_0
#define CHANNEL_B_PIN GPIO_PIN_1
#define YAW_PERIPH_GPIO_B SYSCTL_PERIPH_GPIOB
#define YAW_BASE_GPIO_B GPIO_PORTB_BASE
#define YawReference GPIO_PIN_4

extern volatile int8_t interupt_value;

void yaw_ref(void);
void YawIntHandler(void);
void initYaw(void);
void initYawReference(void);
//*******************************************************************************************************************

/**
 * ADC @heliADC.c,  functions defined in heliADC.c
 * Global requirements:
 * - none
 * - tiva files
 * **///*************************************************************************************************************
void initADC (void);
void ADCIntHandler(void);
//*******************************************************************************************************************


/**
 * PWM @heliPWM.c,  variables and functions defined in heliPWM.c
 * Global Requirements:
 * - none
 * **///*************************************************************************************************************

uint32_t main_duty;
uint32_t tail_duty;

// PWM configuration
#define PWM_FREQUENCY_MAIN 250
#define PWM_FREQUENCY_TAIL 250
#define PWM_DIVIDER_CODE SYSCTL_PWMDIV_4
#define PWM_DIVIDER 4

//  PWM Hardware Details M0PWM7 (gen 3)
//  ---Main Rotor PWM: PC5, J4-05
#define PWM_MAIN_BASE        PWM0_BASE
#define PWM_MAIN_GEN         PWM_GEN_3
#define PWM_MAIN_OUTNUM      PWM_OUT_7
#define PWM_MAIN_OUTBIT      PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM  SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE   GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN    GPIO_PIN_5

//  PWM Hardware Details M1PWM5 (gen 2)  PWM hardware address: module 1, generator 2, outputB (PWM5)
//  ---Tail Rotor PWM: PF1, J3-10        GPIO hardware address: Port F, Pin1, J3-10
#define PWM_TAIL_BASE        PWM1_BASE //PWM module 1
#define PWM_TAIL_GEN         PWM_GEN_2
#define PWM_TAIL_OUTNUM      PWM_OUT_5 //PWM5 OutB of generator 2, see p. 10 of TIVA API docs
#define PWM_TAIL_OUTBIT      PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOF //GPIO port F
#define PWM_TAIL_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_TAIL_GPIO_PIN    GPIO_PIN_1

void setPWMClocks(void);
void setPWM(uint8_t isMainRotor, int32_t ui32Duty);
void initializePWM(void);
void resetPeripheralPWM(void);
void setOutputOnline(int32_t isMainRotor, bool isOn);
//********************************************************************************************************************

/**
 * PID @heliPID.c,  variables and functions defined in heliPID.c
 * Global Requirements:
 * - none 
*///******************************************************************************************************************
void PIDController(uint32_t tmpAltitude, int32_t tmpYaw);
//********************************************************************************************************************

/**
 * Button Control @ ButtonControl.c
 * variables and functions defined in ButtonControl.c
 * **///**************************************************************************************************************
#define SW1_PORT  SYSCTL_PERIPH_GPIOA
#define SW1_PORT_BASE  GPIO_PORTA_BASE
#define SW1_PIN  GPIO_PIN_7
#define LANDED 0
#define FLYING 1
#define LANDING 2

#define RESET_PERIPH SYSCTL_PERIPH_GPIOA
#define RESET_BASE GPIO_PORTA_BASE
#define RESET_PIN GPIO_PIN_6

 struct setpoints {
    uint8_t altSetPoint; //integer between 0 and 100
    int32_t yawSetPoint; // integer rounded degree value, current degrees from reference +/- change
} setPoints;

extern volatile int8_t heliMode;
enum position {down = 0, up = 1};
static int8_t  SW1Position = down;

int32_t yawDegreeConvert(int32_t yaw);
void initSetPoints(int32_t yaw, uint8_t altPercent);
void initResetButton();
void resetSoft();
void checkAndIncrementAltitude();
void checkAndDecrementAltitude();
void checkAndIncrementYaw();
void resetPeriphButtons(void);
void checkAndDecrementYaw();
int32_t getAltitudePercentSetPoint();
int16_t getYawDegreesSetPoint();
void buttonControllerInit(int32_t yaw, int16_t altPercent);
void buttonControllerLoop();
void SW1setup();
uint8_t getSW1mode();
void setSW1mode(int8_t mode);
uint8_t getSW1Position();
void initGPIOAPinChangeInterrupts(void);
void SW1IntHandler(void);
void initializeRef(void);
void checkLanded(void);
void startLanding(void);
//*******************************************************************************************************************
