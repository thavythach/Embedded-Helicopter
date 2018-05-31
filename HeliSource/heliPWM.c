#include "Helicopter.h"

// PWM configuration
#define PWM_FREQUENCY_MAIN 250
#define PWM_FREQUENCY_TAIL 250
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

/**
 * @desc Configures PWM clocks
 * @param N/A
 * @return N/A
 */
void setPWMClocks(void){
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);
}

/**
 * @desc setPWM allows either the tail or the main rotor to be set at a specific duty cycle (limited between 2% and 98%)
 * @param uint8_t isMainRotor 1 if mainRotor, otherwise tailRotor is used
 * @param int32_t duty cycle that is kept within 2% and 98% to control the rotors.
 * @return N/A
 */
void setPWM(uint8_t isMainRotor, int32_t ui32Duty){
    uint32_t ui32Period;
	
    // main duty configuration

	if (isMainRotor == 1){
	    if (ui32Duty > 60) ui32Duty = 60;
        else if (ui32Duty < 11) ui32Duty = 11;

	    ui32Period = SysCtlClockGet() / PWM_DIVIDER / PWM_FREQUENCY_MAIN;

        PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
        PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM,
            ui32Period * ui32Duty / 100);

	    main_duty = ui32Duty;
	}

	// tail duty configuration

	else {
	   if (ui32Duty > 80) ui32Duty = 80;
       else if (ui32Duty < 2) ui32Duty = 2;

       ui32Period = SysCtlClockGet() / PWM_DIVIDER / PWM_FREQUENCY_TAIL;

       PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);
       PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM,
           ui32Period * ui32Duty / 100);
       tail_duty = ui32Duty;
	}
}

/**
 * @desc initializes all PWM peripherals and initial duty cycles of main and tail
 * @param N/A
 * @return N/A
 */
void initializePWM(void){

        /*****************MAIN*******************/
        // init main
        SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
        SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);
        GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
        GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);
        PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                        PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

        // MAIN PWM:
        main_duty = 8;
        setPWM (1, main_duty);

        PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);
        setOutputOnline(1,false); // main SIGNALS off

	    /*****************TAIL*******************/
	    // init tail
	    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
	    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);
	    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
	    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);
	    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN,
	                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

	    // PWM:
        tail_duty = 2;
	    setPWM (0, tail_duty);

	    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);
	    setOutputOnline(0,false); // tail SIGNALS off

}

/**
 * @desc resets both main/tail rotor and main/tail PWM output peripherals
 * @param N/A
 * @return N/A
 */
void resetPeripheralPWM(void){
	 // As a precaution, make sure that the peripherals used are reset
    SysCtlPeripheralReset (PWM_MAIN_PERIPH_GPIO); // Used for PWM output
    SysCtlPeripheralReset (PWM_MAIN_PERIPH_PWM);  // Main Rotor PWM
    SysCtlPeripheralReset (PWM_TAIL_PERIPH_GPIO); // Used for PWM output
    SysCtlPeripheralReset (PWM_TAIL_PERIPH_PWM);  // TAIL rotor PWM
}

/**
 * @desc sets either the main or tail Rotor PWM output signals offline or online
 * @param int32_t isMainRotor 1 - mainRotor, otherwise tailRotor is used
 * @param bool isOn true to turn on PWM output signals, otherwise false for offline.
 */
void setOutputOnline(int32_t isMainRotor, bool isOn){
	if (isMainRotor == 1)PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, isOn);
	else PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, isOn);
}

	

