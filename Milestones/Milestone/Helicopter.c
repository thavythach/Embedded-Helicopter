/**
 * @program Helicopter.c
 * @description An Advanced interrupt driven program which operates with AIN9 to estimate the percentage of altitude range (0.8v)
 * @author0 Thavy Thach
 * @author1 Gabriel Lake
 * @author2 Jed McDermott
 * @last_modified 27/04/2018
 */

/**Libraries**/
#include "Helicopter.h"

/** Global Variables **/
static circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
static uint32_t g_ulSampCnt;    // Counter for the interrupts
static int32_t yaw;
static uint8_t state;
static int32_t altitude; 
static int32_t deg;

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
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);

    setPWMClocks();
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}



void initDisplay (void){
    OLEDInitialise (); // intialise the Orbit OLED display
}

/**
 * Enable the GPIOB peripheral
 */
void initGPIOInterrupts(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // Enable the GPIOB peripheral
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)){} // Wait for the GPIOB module to be ready
    GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler); // register YawIntHandler

    /**Initialize the GPIO pin configuration**/
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, CHANNEL_A_PIN | CHANNEL_B_PIN); // sets pin 0, 1 as in put, SW controlled.
    GPIOIntTypeSet(GPIO_PORTB_BASE, CHANNEL_A_PIN | CHANNEL_B_PIN, GPIO_BOTH_EDGES); // makes pins 0 and 1 rising edge triggered interrupts
    GPIOIntEnable(GPIO_PORTB_BASE, CHANNEL_A_PIN | CHANNEL_B_PIN); // Enable the pin interrupts
}

/**
 * Function to display the mean ADC value (10-bit value, note) and sample count.
 */
uint32_t
displayMeanVal(uint16_t meanVal, uint32_t count, uint16_t initMeanVal, uint32_t mode)
{
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

void YawIntHandler(void){

    // checking state 3
    if (state == 3) {

        // if high // 2 // --
        if (GPIOPinRead(GPIO_PORTB_BASE, CHANNEL_B_PIN)) {
                state = 2;
                yaw--;
        }

        // if low // 4 // ++
        else {
                state = 4;
                yaw++;
        }
    }

    // checking state 1
    else if (state == 1) {

        // if high // 2 // ++
       if (GPIOPinRead(GPIO_PORTB_BASE, CHANNEL_B_PIN)) {
               state = 2;
               yaw++;
       }

       // if low // 4 / --
       else {
               state = 4;
               yaw--;
       }
    }

   // checking state 2
    else if (state == 2) {

       // if high // 3 // ++
      if (GPIOPinRead(GPIO_PORTB_BASE, CHANNEL_B_PIN)) {
              state = 3;
              yaw++;
      }

      // if low // 1 // --
      else {
              state = 1;
              yaw--;
      }

   }

   // checking state 4
    else if (state == 4) {

          // if high // 3 // --
         if (GPIOPinRead(GPIO_PORTB_BASE, CHANNEL_B_PIN)) {

                 state = 3;
                 yaw--;
         }

         // if low // 1 // ++
         else {
                 state = 1;
                 yaw++;
         }
  }

  // else do everything else
  else {
      if(!GPIOPinRead(GPIO_PORTB_BASE, CHANNEL_A_PIN) && !GPIOPinRead(GPIO_PORTB_BASE, CHANNEL_B_PIN)) state = 1;
      if(!GPIOPinRead(GPIO_PORTB_BASE, CHANNEL_A_PIN) && GPIOPinRead(GPIO_PORTB_BASE, CHANNEL_B_PIN)) state = 2;
      if(GPIOPinRead(GPIO_PORTB_BASE, CHANNEL_A_PIN) && GPIOPinRead(GPIO_PORTB_BASE, CHANNEL_B_PIN)) state = 3;
      if(GPIOPinRead(GPIO_PORTB_BASE, CHANNEL_A_PIN) && !GPIOPinRead(GPIO_PORTB_BASE, CHANNEL_B_PIN)) state = 4;
  }


    // clearing interrupts
    GPIOIntClear(GPIO_PORTB_BASE, CHANNEL_B_PIN);
    GPIOIntClear(GPIO_PORTB_BASE, CHANNEL_A_PIN);
}


int main(void){

    // reset peripheral
    resetPeriphButtons();
    resetPeripheralPWM();

    // initializations
    initButtons();
    initClock ();
    initADC ();
    initDisplay ();
    initCircBuf (&g_inBuffer, BUF_SIZE);
    initGPIOInterrupts();
    initializePWM(0); // init tail
    initializePWM(1); // init main
    setOutputOnline(true); // set both PWM signal outputs online

    uint16_t i; // use: circBuffer
    int32_t sum; // use: circBuffer
    int32_t isInit = 0; // use: init height
    int32_t mode = 0; // use: current display mode
    uint32_t newMode; // use: new display mode
    int32_t initMeanVal = 0; // use: initial mean value


    IntMasterEnable(); // Enable interrupts to the processor.
    while (1){
        // Background task: calculate the (approximate) mean of the values in the
        // circular buffer and display it, together with the sample number.

        sum = 0;
        for (i = 0; i < BUF_SIZE; i++)
            sum = sum + readCircBuf (&g_inBuffer);

        // Initializes new height
        if (isInit == 20 || !GPIOPinRead(GPIO_PORTF_BASE, sw1Pin))
            initMeanVal = sum/20;

        // Calculate, display the rounded mean of the buffer contents, and returns mode.
        newMode = displayMeanVal ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE, g_ulSampCnt, initMeanVal, mode);
        mode = newMode;

        SysCtlDelay (SysCtlClockGet() / 96);  // Update display at ~8hz
        isInit += 1;
    }
}

