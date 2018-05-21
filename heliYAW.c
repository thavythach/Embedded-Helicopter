#include "Helicopter.h"

/**
 * Enable the GPIOB peripheral
 */
void initYaw(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // Enable the GPIOB peripheral
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)){} // Wait for the GPIOB module to be ready
    GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler); // register YawIntHandler

    /**Initialize the GPIO pin configuration**/
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, CHANNEL_A_PIN | CHANNEL_B_PIN); // sets pin 0, 1 as in put, SW controlled.
    GPIOIntTypeSet(GPIO_PORTB_BASE, CHANNEL_A_PIN | CHANNEL_B_PIN, GPIO_BOTH_EDGES); // makes pins 0 and 1 rising edge triggered interrupts
    GPIOIntEnable(GPIO_PORTB_BASE, CHANNEL_A_PIN | CHANNEL_B_PIN); // Enable the pin interrupts
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