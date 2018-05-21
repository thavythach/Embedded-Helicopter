#include "Helicopter.h"


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

    initYaw();; 

    /** heliPWM.h initializations **/
    initializePWM(0); // init tail
    initializePWM(1); // init main
    setOutputOnline(true); // set both PWM output signals online
}

int main(void){
    uint16_t i; // use: circBuffer
    int32_t isInit = 0; // use: init height
    int32_t mode = 0; // use: current display mode
    int32_t newMode; // use: new display mode
    int32_t initMeanVal = 0; // use: initial mean value

    IntMasterEnable(); // Enable interrupts to the processor.
    while (1){
        // Background task: calculate the (approximate) mean of the values in the
        // circular buffer and display it, together with the sample number.

        doCircularBufferApproximation(0);

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