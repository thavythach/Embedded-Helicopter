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
#include "buttonsControl.h"
#include "heliPWM.h"
#include "heliADC.h"

/** CONSTANTS | DEFINITIONS **/
#define BUF_SIZE 20
#define SAMPLE_RATE_HZ 100 // equation for it... 2*buffsize*fmax (2*10*4)
#define sw1Pin GPIO_PIN_4
#define UP_BUT_PERIPH  SYSCTL_PERIPH_GPIOE
#define UP_BUT_PORT_BASE  GPIO_PORTE_BASE
#define UP_BUT_PIN  GPIO_PIN_0
#define CHANNEL_A_PIN GPIO_PIN_0
#define CHANNEL_B_PIN GPIO_PIN_1
#define YAW_SAMPLE_RATE_HZ 1792 // minimum sampling rate (1792 SAM/s = 112 slots * 4 samples/slot * 4 rev/s)

/** PROTOTYPES | INITIALIZATIONS **/
void initGPIOInterrupts(void);
void initButtonConfiguration(void);

/** PROTOTYPES | INTERRUPT HANDLERS **/
void YawIntHandler(void);



