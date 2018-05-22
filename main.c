#include "Helicopter.h"

int main(void){
    uint16_t i; // use: circBuffer
    int32_t isInit = 0; // use: init height
    int32_t mode = 0; // use: current display mode
    int32_t initMeanVal = 0; // use: initial mean value

    IntMasterEnable(); // Enable interrupts to the processor.
    while (1){
        // Background task: calculate the (approximate) mean of the values in the
        // circular buffer and display it, together with the sample number.

        doCircularBufferApproximation(0);

        initMeanVal = initializeMeanValue(isInit, initMeanVal); // Initializes new height
        isInit += 1;

        // Calculate, display the rounded mean of the buffer contents, and returns mode.
        mode = displayMeanVal ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE, g_ulSampCnt, initMeanVal, mode);
        updateDisplay(96);
        
    }
}