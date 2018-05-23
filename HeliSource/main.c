#include "Helicopter.h"

int main(void){

    initializeHelicopterOperations();
    g_ulSampCnt = 0;

    uint16_t i; // use: circBuffer
   static int32_t sum = 0;
    int32_t isHeightInitialized = 0; // use: init height
   // uint32_t mode = 0; // use: current display mode
   // uint32_t newMode;
    int32_t initMeanVal = 0; // use: initial mean value


    IntMasterEnable(); // Enable interrupts to the processor.
//    while (g_ulSampCnt < BUF_SIZE);



    while (1){
        
        // Background task: calculate the (approximate) mean of the values in the circular buffer and display it, together with the sample number.
        sum = 0;
        if (g_ulSampCnt > BUF_SIZE) { //If we have more than BUF_SIZE many samples...

            for (i = 0; i < BUF_SIZE; i++) { //Get a sum of the buffer
                sum = sum + readCircBuf (&g_inBuffer);
            }

            // Initializes new height
            if (isHeightInitialized == 0) { //If we haven't initialised the height,
                initMeanVal = sum/BUF_SIZE; //Calculate the initial mean value from the sum
                isHeightInitialized = 1;
            }

        }




        // Does: calculation of altitude & displays screen (OLED).
        //yawDegreeConvert(yaw);
        displayMeanVal ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE, initMeanVal);
        
        // controls the user input when flight mode equals flying.

        if ( getSW1mode() == 1 ){
            buttonControllerLoop();
        }

        //PID Motor control code
        PIDController(altitude, yaw);
        checkLanded();
       // startLanding();


        //UART code
      //  UART();

        // Update Display by 8Hz
        updateDisplay(96);
    }
}
