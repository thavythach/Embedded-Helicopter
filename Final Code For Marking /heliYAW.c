#include "Helicopter.h"
#include <math.h>
volatile int8_t interupt_value = 1;
volatile static int32_t foundRefYaw = 0; //global flag
int32_t val = 0;
int8_t lock = 1;


/**
 * @desc interrupt handling finding the yaw reference value when looking at the helicopter.
 * @param N/A
 * @return N/A
 */
void yaw_ref(void){
    foundRefYaw += 1;
    if (foundRefYaw == 1) {
        yaw = 0;
        setPoints.yawSetPoint = interupt_value;
    }

   // OLEDStringDraw ("refyaw now", 0, 3); // tail motor duty cycle in %
    GPIOIntClear(GPIO_PORTC_BASE, YawReference);
}

/**
 * @desc helps initialize they initial yaw reference as helicopter looks at the camera on remote labs
 * @param N/A
 * @return N/A
 */
void initializeRef(void) {

   int32_t yawDegrees = yawDegreeConvert(yaw);
   //setPoints.yawSetPoint = yawDegrees - 5;
   while ((foundRefYaw < 1) && (abs(yawDegrees) < 360)) {
       yawDegrees = yawDegreeConvert(yaw);
    //   resetSoft();
       if (abs(yawDegrees) < abs(setPoints.yawSetPoint + 2)) {
           setPoints.yawSetPoint -= 5;
       }
  /*
   while (foundRefYaw == 0) {
       setPWM(0, 15);
   }
   */
       PIDController(altitude, yaw);
       UART();

   }

}


/**
 * @desc initializations the Yaw Peripherals (GPIOB) with BOTH_EDGES
 * @param N/A
 * @return N/A
 */
void initYaw(void){
    // Enable the GPIOB peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    /**Wait for the GPIOB module to be ready**/
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)){
    }

    GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler);

    /**Initialize the GPIO pin configuration**/

    // sets pin 0, 1 as in put, SW controlled.
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, CHANNEL_A_PIN | CHANNEL_B_PIN);


    // makes pins 0 and 1 both edge triggered interrupts
    GPIOIntTypeSet(GPIO_PORTB_BASE, CHANNEL_A_PIN | CHANNEL_B_PIN, GPIO_BOTH_EDGES);

    // Enable the pin interrupts
    GPIOIntEnable(GPIO_PORTB_BASE, CHANNEL_A_PIN | CHANNEL_B_PIN);
}

/**
 * @desc initializations the Yaw Reference Peripherals (GPIOC) with FALLING_EDGE
 * @param N/A
 * @return N/A
 */
void initYawReference(void){
    // Enable the GPIOB peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    /**Wait for the GPIOC module to be ready**/
   while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC)){}

   GPIOIntRegister(GPIO_PORTC_BASE, yaw_ref);

   // sets pin 4 as in put, SW controlled.
   GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, YawReference);

   // makes pins pin 4 falling edge triggered interrupts
   GPIOIntTypeSet(GPIO_PORTC_BASE, YawReference, GPIO_RISING_EDGE);

   // Enable the pin interrupts
   GPIOIntEnable(GPIO_PORTC_BASE, YawReference);
}


/**
 * @desc Quadrature Decoding State Machine to control yaw value.
 * @param N/A
 * @return N/A
 */
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
