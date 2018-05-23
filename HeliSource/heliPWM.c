#include "Helicopter.h"

// "mainRotor" and "tailRotor" are your best friends.

//structs
// PWM configuration
#define PWM_FREQUENCY_MAIN 250
#define PWM_FREQUENCY_TAIL 200
#define PWM_FIXED_DUTY_MAIN 0
#define PWM_FIXED_DUTY_TAIL 0
#define PWM_DIVIDER_CODE SYSCTL_PWMDIV_4
#define PWM_DIVIDER 4

//  PWM Hardware Details M0PWM7 (gen 3)
//  ---Main Rotor PWM: PC5, J4-05
#define PWM_MAIN_BASE        PWM0_BASE
#define PWM_MAIN_GEN         PWM_GEN_3
#define PWM_MAIN_OUTNUM      PWM_OUT_7
#define PWM_MAIN_OUTBIT      PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM  SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE   GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN    GPIO_PIN_5

//  PWM Hardware Details M1PWM5 (gen 2)  PWM hardware address: module 1, generator 2, outputB (PWM5)
//  ---Tail Rotor PWM: PF1, J3-10        GPIO hardware address: Port F, Pin1, J3-10
#define PWM_TAIL_BASE        PWM1_BASE //PWM module 1
#define PWM_TAIL_GEN         PWM_GEN_2
#define PWM_TAIL_OUTNUM      PWM_OUT_5 //PWM5 OutB of generator 2, see p. 10 of TIVA API docs
#define PWM_TAIL_OUTBIT      PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOF //GPIO port F
#define PWM_TAIL_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_TAIL_GPIO_PIN    GPIO_PIN_1

//struct pwmConfig config = {150, 300, 0, 0, 250, 200, SYSCTL_PWMDIV_4, 4};
//
////  PWM Hardware Details M0PWM7 (gen 3)
////  ---Main Rotor PWM: PC5, J4-05
//struct pwmRotor mainRotor = { PWM0_BASE, PWM_GEN_3, PWM_OUT_7, PWM_OUT_7_BIT, SYSCTL_PERIPH_PWM0, SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PC5_M0PWM7, GPIO_PIN_5 };
//
////  PWM Hardware Details M1PWM5 (gen 2)
////  ---Tail Rotor PWM: PF1, J3-10
//struct pwmRotor tailRotor = { PWM1_BASE, PWM_GEN_2, PWM_OUT_5, PWM_OUT_5_BIT, SYSCTL_PERIPH_PWM1, SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PF1_M1PWM5, GPIO_PIN_1 };



void setPWMClocks(void){
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);
}

void setPWM(uint8_t isMainRotor, uint32_t ui32Duty){
	
	if (ui32Duty > 98) ui32Duty = 98;
	else if (ui32Duty < 2) ui32Duty = 2; 
	
	if (isMainRotor == 1){
	    uint32_t ui32Period =
	            SysCtlClockGet() / PWM_DIVIDER / PWM_FREQUENCY_MAIN;

	        PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
	        PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM,
	            ui32Period * ui32Duty / 100);
	    main_duty = ui32Duty;
	} else {
	    // Calculate the PWM period corresponding to the freq.
	       uint32_t ui32Period = SysCtlClockGet() / PWM_DIVIDER / PWM_FREQUENCY_TAIL;

	      // PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);
	       PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM,
	           ui32Period * ui32Duty / 100);
	       tail_duty = ui32Duty;
	}
	// i should save the duty cycles... here at some point.
}

void initializePWM(uint8_t isMainRotor){

	if (isMainRotor == 1){
	    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
	    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

	    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
	    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

	    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
	                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
	    // Set the initial PWM parameters
	    setPWM (isMainRotor, main_duty);

	    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);

	    // Disable the output.  Repeat this call with 'true' to turn O/P on.
	    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);
	} else {
	    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
	    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);

	    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
	    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);

	    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN,
	                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
	    // Set the initial PWM parameters
	    setPWM (isMainRotor, tail_duty);

	    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);

	    // Disable the output.  Repeat this call with 'true' to turn O/P on.
	    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);
	}	
}

void resetPeripheralPWM(){
	 // As a precaution, make sure that the peripherals used are reset
    SysCtlPeripheralReset (PWM_MAIN_PERIPH_GPIO); // Used for PWM output
    SysCtlPeripheralReset (PWM_MAIN_PERIPH_PWM);  // Main Rotor PWM
    SysCtlPeripheralReset (PWM_TAIL_PERIPH_GPIO); // Used for PWM output
    SysCtlPeripheralReset (PWM_TAIL_PERIPH_PWM);  // TAIL rotor PWM
}


void setOutputOnline(int32_t isMainRotor, bool isOn){

	if (isMainRotor == 1){
	    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, isOn);
	} else {
	    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, isOn);
	}

}

	

