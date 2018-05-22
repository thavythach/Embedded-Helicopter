#include "Helicopter.h"

// The interrupt handler for the for SysTick interrupt.
void SysTickIntHandler(void){
    // Initiate a conversion
    ADCProcessorTrigger(ADC0_BASE, 3); 
    g_ulSampCnt++;
    updateButtons();
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

/**
 * Function to display the mean ADC value (10-bit value, note) and sample count.
 */
uint32_t displayMeanVal(uint16_t meanVal, uint32_t count, uint16_t initMeanVal, uint32_t mode){
    char string[17];  // 16 characters across the display
    char strins[17];  // 16 characters across the display
    char strinss[17];  // 16 characters across the display

    float rangeAltitude = 983.0;

    int32_t robustMeanVal = (( ((float)initMeanVal) - ((float) meanVal) ) / rangeAltitude) * 100;
    altitude = robustMeanVal;

    // Up Button Functionality to determine new mode to display.
    if (checkButton(UP) == PUSHED) {
        if (mode == 2) mode = 0;
        else mode += 1;
    }

    uint32_t i=0;
    switch (mode){
        case 0:
            usnprintf (string, sizeof(string), "Alt %% = %4d ", robustMeanVal);
            break;
        case 1:
            usnprintf (string, sizeof(string), "Mean ADC = %4d ", meanVal);
            break;
        case 2:
            OLEDStringDraw ("                ", 0, 0);
            OLEDStringDraw ("                ", 0, 1);
            OLEDStringDraw ("                ", 0, 2);
            OLEDStringDraw ("                ", 0, 3);
            break;
        default:
           usnprintf (string, sizeof(string), "Alt %% = %4d ", robustMeanVal);
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

void doCircularBufferApproximation(int32_t sum){
    uint16_t i = 0;
    for (i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf (&g_inBuffer);
}

int32_t initializeMeanValue(int32_t isInit, int32_t meanVal){
    if (isInit == 20 || !GPIOPinRead(GPIO_PORTF_BASE, sw1Pin))
        meanVal = sum/20;
    return mealVal;
}

void updateDisplay(int32_t value){
    SysCtlDelay (SysCtlClockGet() / value);  // Update display at ~8hz
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

    initCircBuf (&g_inBuffer, BUF_SIZE); // from @ tiva src files

    initYaw(); // yaw

    /** heliPWM.h initializations **/
    initializePWM(0); // init tail
    initializePWM(1); // init main
    setOutputOnline(true); // set both PWM output signals online
}






