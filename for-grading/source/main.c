/**
 * @desc Helicopter Rig Control Project for ENCE361 Embedded Systems I
 * @author0 Jed McDermott
 * @author1 Thavy Thach
 * @author2 Gabriel Lake
 * @version 28/05/2018
 */
//*****************************************************************************************
//main.c contains the round robin loop. Code that is packaged into comprehensive functions
//in other source files is called here to perform background tasks.
//*****************************************************************************************

#include "Helicopter.h"

int main(void){

    initializeHelicopterOperations(); // all initializations

    //define globals, and declare/define instance variables
    g_ulSampCnt = 0; // sample count
    uint16_t i; // use: circBuffer
    static int32_t sum = 0; // use: circBuffer
    int32_t isHeightInitialized = 0; // use: initialization of Height
    int32_t initMeanVal = 0; // use: initial ADC mean value
    int8_t once = 1; //one time flag

    // Enable interrupts to the processor.
    IntMasterEnable();

    while (1){
        // Task: soft reset button polling code
        resetSoft();

        //Task:
        /********************************************************************************************************
         * once the buffer is full, average the ADC samples in the circular buffer, then calculate altitude
         */
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

        }
        //calculation of altitude & displays screen (OLED).
        displayMeanVal ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE, initMeanVal);
        //***********************************************************************************************************
        
        /***********************************************************************************************************
         * Task: polls the user input when flight mode equals flying and orients the helicopter to initial position
         * also updates set points based on user input
         */
        if ( getSW1mode() == FLYING){
            if (once) {
                initializeRef();
                once = 0;
            }

            buttonControllerLoop(); //poll buttons for user input, update set points
        }
        //********************************************************************************************************

        //Task: provide a controller for stable flight, sets the PWM signals to the motors based on set points
        PIDController(altitude, yaw);

        //check for landing condition and land the heli
        if (getSW1mode() == LANDING){
            checkLanded();
        }

        //Task: send data back to the user via UART module
        UART();

        // delay the main and control display update frequency, 24Hz
        updateDisplay(32);
    }
}
