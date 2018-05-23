#include "Helicopter.h"

int main(void){

    initializeHelicopterOperations();


    uint16_t i; // use: circBuffer
   // int32_t isHeightInitialized = 0; // use: init height
   // uint32_t mode = 0; // use: current display mode
   // uint32_t newMode;
    int32_t initMeanVal = 0; // use: initial mean value
    int32_t sum;

    IntMasterEnable(); // Enable interrupts to the processor.
    while (g_ulSampCnt < 20);
    sum = 0;
    for (i = 0; i < BUF_SIZE; i++)
               sum = sum + readCircBuf (&g_inBuffer);
    initMeanVal = sum/BUF_SIZE;

    while (1){
        
        // Background task: calculate the (approximate) mean of the values in the circular buffer and display it, together with the sample number.
        for (i = 0; i < BUF_SIZE; i++)
            sum = sum + readCircBuf (&g_inBuffer);

        // Initializes new height
        /*
        if (isHeightInitialized == BUF_SIZE) {
            initMeanVal = sum/BUF_SIZE;
        }
        isHeightInitialized ++;
        */



        // Does: calculation of altitude & displays screen (OLED).
        //yawDegreeConvert(yaw);
        displayMeanVal ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE, initMeanVal);
        
        // controls the user input when flight mode equals flying.
    //    if ( getSW1mode() == 1 ){
    //        buttonControllerLoop();
    //    }
        //PID Motor control code
       // PIDController(altitude, yawDegreeConvert(yaw));
      //  checkLanded();
      //  startLanding();

        //UART code
      //  UART();

        // Update Display by 8Hz
        updateDisplay(96);
    }
}
