

#include "Helicopter.h"

/********************************************************************************************************************
 * Hellicopter.c contains a majority of peripheral initialization code, background miscellaneous tasks including
 * calculating the altitude from the averaged samples and updating the TIVA display, and also SysTick foreground tasks
 * such as sampling ADC data and pacing the UART
// @author0 Jed McDermott
// @author1 Thavy Thach
// @author2 Gabriel Lake
// @version 28/05/2018
 *///*****************************************************************************************************************

//Global Variables*********************************************************************************************
circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
volatile uint32_t g_ulSampCnt;    // Counter for the interrupts
volatile int32_t yaw;           //raw yaw counter updated by the yaw signal state machine interrupt handler
volatile uint8_t state;
volatile int16_t altitude;      //global altitude variable expressed as a percent of altitude range
//*************************************************************************************************************


//functions****************************************************************************************************


/*
 * The interrupt handler for the for SysTick interrupt.
 */
void SysTickIntHandler(void){
    //ADC****************************************
    // Initiate a conversion
    ADCProcessorTrigger(ADC0_BASE, 3); 
    g_ulSampCnt++;
    updateButtons();

    //UART************************************
    static uint8_t tickCount = 0;
    const uint8_t ticksPerSlow = SYSTICK_RATE_HZ / SLOWTICK_RATE_HZ;

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

    SysTickEnable();
    SysTickIntEnable();
}

void initDisplay (void){
    OLEDInitialise (); // intialise the Orbit OLED display
}

/*
 * Calculate the altidue based on the digital altitude range, initial altitude ADC value
 * and the current ADC value
 * @param: uint16_t initMeanval - inital ADC value for zero altitude
 * @param: uint16_t meanVal - current ADC value converted from analog altitude input
 * @return: void
 */
void calculateRobustMeanValue(uint16_t initMeanVal, uint16_t meanVal){
    float range_altitude = 983.0;
    altitude = (( ((float)initMeanVal) - ((float) meanVal) ) / range_altitude) * 100;
}

/**
 * Function to display the required data on the Orbit Boosterpack OLED display
 * @param: uint16_t initMeanval - inital ADC value for zero altitude
 * @param: uint16_t meanVal - current ADC value converted from analog altitude input
 */
void displayMeanVal(uint16_t meanVal, uint16_t initMeanVal){
    char line[17];  // 16 characters across the display

    calculateRobustMeanValue(initMeanVal, meanVal);

    // Displaying Title and conditional string denoting which mode
        usnprintf (line, sizeof(line), "Alt %% = %3d ", altitude);
        OLEDStringDraw (line, 0, 0); // meanvalue display
        usnprintf (line, sizeof(line), "YAW = %5d", yawDegreeConvert(yaw));
        OLEDStringDraw (line, 0, 1); // yaw display
        usnprintf (line, sizeof(line), "M Duty: %3d", main_duty);
        OLEDStringDraw (line, 0, 2); // main motor duty cycle in %
        usnprintf (line, sizeof(line), "T Duty: %3d", tail_duty);
        OLEDStringDraw (line, 0, 3); // tail motor duty cycle in %


}

/*
 * delay function for the round robin loop
 */
void updateDisplay(int32_t val) {
    SysCtlDelay (SysCtlClockGet() / val);  // Update display at ~8hz
}

/*
 * call to reset PWM and button peripherals
 */
void resetHelicopterOperations(void){
    resetPeriphButtons(); // reset peripherals @ buttons4.h and buttonControl.h
    resetPeripheralPWM(); // reset peripherals @ heliPWM.h
}

/*
 * call in main.c to initialize everything
 */
void initializeHelicopterOperations(void){
    resetHelicopterOperations(); // resets all peripherals used in helicopter
    initClock (); // clock setup @
    initButtons(); // button setup @ buttons4.h and buttonControl.h

    initADC (); // Analogue-To-Digital Conversion(s)

    /** heliPWM.h initializations **/
    initializePWM(); // init tail and main
    initResetButton();
    SW1setup();
    initYaw(); // yaw
    initYawReference();
    initCircBuf (&g_inBuffer, BUF_SIZE); // from @ tiva src files

    buttonControllerInit(yawDegreeConvert(yaw), altitude);
    initialiseUSB_UART();
    initDisplay (); // OLED DISPLAY
}






