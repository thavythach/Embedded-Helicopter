#include "Helicopter.h"

int main(void){

    initializeHelicopterOperations();

    uint16_t i; // use: circBuffer
    int32_t isHeightInitialized = 0; // use: init height
    int32_t mode = 0; // use: current display mode
    int32_t initMeanVal = 0; // use: initial mean value

    IntMasterEnable(); // Enable interrupts to the processor.
    while (1){
        
        // Background task: calculate the (approximate) mean of the values in the circular buffer and display it, together with the sample number.
        int32_t sum = 0;
        for (i = 0; i < BUF_SIZE; i++)
            sum = sum + readCircBuf (&g_inBuffer);

        // Initializes new height
        if (isHeightInitialized == 20 || !GPIOPinRead(GPIO_PORTF_BASE, sw1Pin))
            initMeanVal = sum/20;
        isHeightInitialized ++;

        // Calculate, display the rounded mean of the buffer contents, and returns mode.
        mode = displayMeanVal ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE, g_ulSampCnt, initMeanVal, mode);

        
        // Update Display by 8Hz
        updateDisplay(96);
    }
}