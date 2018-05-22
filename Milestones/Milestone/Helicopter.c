#include "Helicopter.h"

// The interrupt handler for the for SysTick interrupt.
void SysTickIntHandler(void){
    // Initiate a conversion
    ADCProcessorTrigger(ADC0_BASE, 3); 
    g_ulSampCnt++;
    updateButtons();

    //UART************************************
    static uint8_t tickCount = 0;
    const uint8_t ticksPerSlow = SYSTICK_RATE_HZ / SLOWTICK_RATE_HZ;

    //updateButtons ();     // Poll the buttons
    if (++tickCount >= ticksPerSlow)
    {                       // Signal a slow tick
        tickCount = 0;
        slowTick = true;
    }
    //********************************************
}

// Initialisation functions for the clock (incl. SysTick), ADC, display
void initClock (void){
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);

    setPWMClocks();
    
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

void initDisplay (void){
    OLEDInitialise (); // intialise the Orbit OLED display
}

void calculateRobustMeanValue(uint32_t initMeanVal, uint32_t meanVal){
    altitude = (( ((float)initMeanVal) - ((float) meanVal) ) / RANGE_ALTITUDE) * 100;
    }

/**
 * Function to display the mean ADC value (10-bit value, note) and sample count.
 *
 */
uint32_t displayMeanVal(uint16_t meanVal, uint32_t count, uint16_t initMeanVal, uint32_t mode){
    char string[17];  // 16 characters across the display
    char strins[17];  // 16 characters across the display
    //char line_2[17];  // 16 characters across the display
    //char line_2[17];  // 16 characters across the display

    calculateRobustMeanValue(initMeanVal, meanVal);

    // Up Button Functionality to determine new mode to display.
    if (checkButton(UP) == PUSHED) {
        if (mode == 2) mode = 0;
        else mode += 1;
    }

    switch (mode){
        case 1: // Chaneg Altitude to 
            usnprintf (string, sizeof(string), "Mean ADC = %4d ", meanVal);
            break;
        case 2: // clear oled screen
            OLEDStringDraw ("                ", 0, 0);
            OLEDStringDraw ("                ", 0, 1);
            OLEDStringDraw ("                ", 0, 2);
            OLEDStringDraw ("                ", 0, 3);
            break;
        default:
            usnprintf (string, sizeof(string), "Alt %% = %4d ", altitude);
            break;
    }

    // Displaying Title and conditional string denoting which mode
    if ( mode != 2 ){
        OLEDStringDraw ("Milestone 2", 0, 0); // milestone display
        OLEDStringDraw (string, 0, 1); // meanvalue display
        usnprintf (strins, sizeof(strins), "YAW deg = %5d", yawDegreeConvert(yaw));
        OLEDStringDraw (strins, 0, 2); // yaw display
    }

    return mode;
}

void updateDisplay(int32_t val) {
    SysCtlDelay (SysCtlClockGet() / val);  // Update display at ~8hz
}

void resetHelicopterOperations(void){
    resetPeriphButtons(); // reset peripherals @ buttons4.h and buttonControl.h
    resetPeripheralPWM(); // reset peripherals @ heliPWM.h
}

void initializeHelicopterOperations(void){
    resetHelicopterOperations(); // resets all peripherals used in helicopter

    initButtons(); // button setup @ buttons4.h and buttonControl.h
    initClock (); // clock setup @
    
    initADC (); // Analogue-To-Digital Conversion(s)
    initDisplay (); // OLED DISPLAY
    initYaw(); // yaw

    /** heliPWM.h initializations **/
    initializePWM(0); // init tail
    initializePWM(1); // init main
    setOutputOnline(0,true); // set both PWM output signals online
    setOutputOnline(1,true); // set both PWM output signals online
    SW1setup();
    initCircBuf (&g_inBuffer, BUF_SIZE); // from @ tiva src files
    initialiseUSB_UART();
}






