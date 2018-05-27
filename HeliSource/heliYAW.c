#include "Helicopter.h"

volatile int8_t interupt_value = 1;
int32_t yi = 0; //global counter

//INTERUPT HANDLER
void yaw_ref(void){
    yi++;
    if (yi == 1) {
        yaw = 0;
    }
    interupt_value = yawDegreeConvert(yaw);
    GPIOIntClear(GPIO_PORTC_BASE, YawReference);
}


/**
 * Enable the GPIOB peripheral
 */
void initYaw(void){
    // Enable the GPIOB peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    /**Wait for the GPIOB module to be ready**/
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)){
    }

    /**Wait for the GPIOC module to be ready**/
    //while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC)){
    //}

    GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler);
    //GPIOIntRegister(GPIO_PORTC_BASE, yaw_ref);

    /**Initialize the GPIO pin configuration**/

    // sets pin 0, 1 as in put, SW controlled.
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, CHANNEL_A_PIN | CHANNEL_B_PIN);
    //GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, YawReference);


    // makes pins 0 and 1 rising edge triggered interrupts
    GPIOIntTypeSet(GPIO_PORTB_BASE, CHANNEL_A_PIN | CHANNEL_B_PIN, GPIO_BOTH_EDGES);
    //GPIOIntTypeSet(GPIO_PORTC_BASE, YawReference, GPIO_BOTH_EDGES);

    // Enable the pin interrupts
    GPIOIntEnable(GPIO_PORTB_BASE, CHANNEL_A_PIN | CHANNEL_B_PIN);
    //GPIOIntEnable(GPIO_PORTC_BASE, YawReference);


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
