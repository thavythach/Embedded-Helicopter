T/**
 * @desc Helicopter Rig Control Project for ENCE361 Embedded Systems I
 * @author0 Jed McDermott
 * @author1 Thavy Thach
 * @author2 Gabriel Lake
 * @version 28/05/2018
 */

#include "Helicopter.h"

int main(void){

    initializeHelicopterOperations(); // all initializations
    g_ulSampCnt = 0; // sample count

    uint16_t i; // use: circBuffer
    static int32_t sum = 0; // use: circBuffer
    int32_t isHeightInitialized = 0; // use: initialization of Height
    int32_t initMeanVal = 0; // use: initial mean value
    int8_t once = 1;

    IntMasterEnable(); // Enable interrupts to the processor.

    while (1){
        // soft reset button polling code
       // resetSoft();
        // Background task: calculate the (approximate) mean of the values in the circular buffer and display it, together with the sample number.
        sum = 0;
        if (g_ulSampCnt > BUF_SIZE) { //If we have more than BUF_SIZE many samples...

            for (i = 0; i < BUF_SIZE; i++) { //Get a sum of the buffer
                sum = sum + readCircBuf (&g_inBuffer);
            }

            // Initializes new height and yaw if landed.
            if (isHeightInitialized == 0 || getSW1mode() == LANDED) { //If we haven't initialised the height,
                yaw = 0;
                setPoints.yawSetPoint = 0;
                initMeanVal = sum/BUF_SIZE; //Calculate the initial mean value from the sum
                isHeightInitialized = 1;
                once = 1;
            }
        }a

        // Does: calculation of altitude & displays screen (OLED).
        displayMeanVal ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE, initMeanVal);
        
        // controls the user input when flight mode equals flying.
        if ( getSW1mode() == FLYING){
            if (once) {
                initializeRef();
                once = 0;
            }

            buttonControllerLoop();
        }

        //PID Motor control code
        PIDController(altitude, yaw);
        //check for landing condition and land the heli
        if (getSW1mode() == LANDING){
            checkLanded();
        }

        //UART code
        UART();

        // Update Display by 8Hz
        updateDisplay(96);
    }
}
