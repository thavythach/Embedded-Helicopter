//*****************************************************************************
//
// AltHeliEstimation.c - An Advanced interrupt driven program which operates with AIN9 to estimate the percentage of altitude range (0.8V).
//
// Author:  Thavy Thach, Gabriel Lake, Jed McDermott
// Last modified:	19.3.2018
// Adapted from: Adcdemo1.c


//*****************************************************************************
// Libraries
//*****************************************************************************
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
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"

//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE 10
#define SAMPLE_RATE_HZ 100
#define sw1Pin GPIO_PIN_4
#define UP_BUT_PERIPH  SYSCTL_PERIPH_GPIOE
#define UP_BUT_PORT_BASE  GPIO_PORTE_BASE
#define UP_BUT_PIN  GPIO_PIN_0

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t g_inBuffer;		// Buffer of size BUF_SIZE integers (sample values)
static uint32_t g_ulSampCnt;	// Counter for the interrupts

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Initiate a conversion
    //
    ADCProcessorTrigger(ADC0_BASE, 3); 
    g_ulSampCnt++;
}

//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void
ADCIntHandler(void)
{
	uint32_t ulValue;
	
	//
	// Get the single sample from ADC0.  ADC_BASE is defined in
	// inc/hw_memmap.h
	ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
	//
	// Place it in the circular buffer (advancing write index)
	writeCircBuf (&g_inBuffer, ulValue);
	//
	// Clean up, clearing the interrupt
	ADCIntClear(ADC0_BASE, 3);                          
}

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();

}

void 
initADC (void)
{
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    
    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);    
                             

    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);
  
    //
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);
  
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

void
initDisplay (void)
{
    OLEDInitialise (); // intialise the Orbit OLED display
}

//*****************************************************************************
//
// Function to display the mean ADC value (10-bit value, note) and sample count.
//
//*****************************************************************************
uint32_t
displayMeanVal(uint16_t meanVal, uint32_t count, uint16_t initMeanVal, uint32_t mode)
{
	char string[17];  // 16 characters across the display
	
	float rangeAltitude = 983.0;

    uint32_t robustMeanVal = (( ((float)initMeanVal) - ((float) meanVal) ) / rangeAltitude) * 100;

    // edge case: past 0.8V
    if (meanVal < (initMeanVal-rangeAltitude))
        robustMeanVal = 100;

    // Up Button Functionality to determine new mode to display.
    if (GPIOPinRead (UP_BUT_PORT_BASE, UP_BUT_PIN)) {
        if (mode == 2) mode = 0;
        else mode += 1;
    }

    //
    uint32_t i=0;
    switch (mode){
        case 0:
            usnprintf (string, sizeof(string), "Alt %% = %4d ", robustMeanVal);
            break;
        case 1:
            usnprintf (string, sizeof(string), "Mean ADC = %4d ", meanVal);
            break;
        case 2:
            for (i=0; i<4; i++)
                OLEDStringDraw ("                ", 0, i);
            break;
        default:
            usnprintf (string, sizeof(string), "Alt %% = %4d ", robustMeanVal);
            break;
    }

    // Displaying Title and conditional string denoting which mode
    if ( mode != 2 ){
        OLEDStringDraw ("Milestone 1", 0, 0);
        OLEDStringDraw (string, 0, 1);
    }

    return mode;
}


int
main(void)

{
	uint16_t i;
	int32_t sum;
	
	initClock ();
	initADC ();
	initDisplay ();
	initCircBuf (&g_inBuffer, BUF_SIZE);

	// left button initial setup with hardware
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPadConfigSet(GPIO_PORTF_BASE, sw1Pin, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
	GPIODirModeSet(GPIO_PORTF_BASE, sw1Pin, GPIO_DIR_MODE_IN);

	// up button initial setup changes between three states (Alt %, ADC Mean Value, & Blank)
	SysCtlPeripheralEnable (UP_BUT_PERIPH);
	GPIOPinTypeGPIOInput (UP_BUT_PORT_BASE, UP_BUT_PIN);
    GPIOPadConfigSet (UP_BUT_PORT_BASE, UP_BUT_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
	int32_t initMeanVal = 0;

	int32_t isInit = 0;
	int32_t mode = 0;
	uint32_t newMode;

    //
    // Enable interrupts to the processor.
    IntMasterEnable();

	while (1)
	{
		//
		// Background task: calculate the (approximate) mean of the values in the
		// circular buffer and display it, together with the sample number.

	    sum = 0;
		for (i = 0; i < BUF_SIZE; i++){
			sum = sum + readCircBuf (&g_inBuffer);
		}

		// Initializes new height
		if (isInit == 10 || !GPIOPinRead(GPIO_PORTF_BASE, sw1Pin)) {
		    initMeanVal = sum/10;
		}

		// Calculate, display the rounded mean of the buffer contents, and returns mode.
		newMode = displayMeanVal ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE, g_ulSampCnt, initMeanVal, mode);
		mode = newMode;

		SysCtlDelay (SysCtlClockGet() / 24);  // Update display at ~ 2 Hz make it update faster change 6
		isInit += 1;
	}
}

