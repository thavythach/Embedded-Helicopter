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

/** Miscellaneous Prototypes **/
void initDisplay(void); // OLED display
void initClock (void); 
void updateDisplay(int32_t val);
void displayMeanVal(uint16_t meanVal, uint16_t initMeanVal);
void initializeHelicopterOperations(void);
void resetHelicopterOperations(void);
void calculateRobustMeanValue(uint16_t initMeanVal, uint16_t meanVal);

extern circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
extern volatile uint32_t g_ulSampCnt;    // Counter for the interrupts
extern volatile int32_t yaw;
extern volatile uint8_t state;
extern volatile int16_t altitude;
//static int32_t deg;

#define BUF_SIZE 20
#define SAMPLE_RATE_HZ 100 // equation for it... 2*buffsize*fmax (2*10*4)
#define sw1Pin GPIO_PIN_4 




//UART.c***********************************
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX
#define SYSTICK_RATE_HZ 100 //???
#define SLOWTICK_RATE_HZ 4

volatile uint8_t slowTick; //false initially, cant initialize in header? why??

void initialiseUSB_UART (void);
void UARTSend (char *string);
char * getSW1modeString(int32_t mode);
void UART();

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
#define YawReference GPIO_PIN_4

extern volatile int8_t interupt_value;

void yaw_ref(void);
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
uint32_t main_duty;
uint32_t tail_duty;
// configuration for both dreams

// prototypes
void setPWMClocks(void);
void setPWM(uint8_t isMainRotor, int32_t ui32Duty);
void initializePWM(uint8_t isMainMotor);
void resetPeripheralPWM(void);
void setOutputOnline(int32_t isMainRotor, bool isOn);

/**
 * PID @heliPID.c
 * Global Requirements:
 * - none 
*/

void PIDController(uint32_t tmpAltitude, int32_t tmpYaw);
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

extern volatile int8_t heliMode;
#define LANDED 0
#define FLYING 1
#define LANDING 2
enum position {down = 0, up = 1};

static int8_t  SW1Position = down;
void resetSoft();


/*
 * convert raw yaw counter value to degrees
 * param: yawCount - raw yaw counter data, from global variable in Helicopter.c
 * return: int16_t yaw count converted to degrees
 */
int32_t yawDegreeConvert(int32_t yaw);

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
void buttonControllerInit(int32_t yaw, int16_t altPercent);


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

void initializeRef(void);
void checkLanded(void);
void startLanding(void);


