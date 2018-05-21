#include "heliPWM.h"

// "mainRotor" and "tailRotor" are your best friends.

void setPWMClocks(void){
    SysCtlPWMClockSet(config.dividerCode); 	// Set the PWM clock rate (using the prescaler)
}

void setPWM(uint8_t isMainRotor, uint32_t ui32Duty){
	
	if (ui32Duty > 98) ui32Duty = 98;
	else if (ui32Duty < 2) ui32Duty = 2; 
	
	// Calculate the PWM period corresponding to the freq.
	uint32_t ui32freq, ui32base, ui32gen, ui32outnum; 
	
	if (isMainRotor == 1){
		ui32freq = config.mainFrequency;
		ui32base = mainRotor.base;
		ui32gen = mainRotor.gen;
		ui32outnum = mainRotor.outnum;
		config.mainDutyCycle = ui32Duty;
	} else {
		ui32freq = config.tailFrequency;
		ui32base = tailRotor.base;
		ui32gen = tailRotor.gen;
		ui32outnum = tailRotor.outnum;
		config.tailDutyCycle = ui32Duty;
	}
	
	uint32_t ui32Period = SysCtlClockGet() / config.divider / ui32freq;
    //PWMGenPeriodSet(ui32base, ui32gen, ui32Period); // not used b/c we are not setting the frequency
    PWMPulseWidthSet(ui32base, ui32outnum,
        ui32Period * ui32Duty / 100);

}

void initializePWM(uint8_t isMainMotor){
	
	uint32_t ui32periphPWM, ui32periphGPIO, ui32GPIOConfig, ui32base, ui32gen, ui32outbit, ui32Duty, ui32freq; 
	uint8_t ui32GPIOPin;
	
	if (isMainRotor == 1){
		ui32periphPWM = mainRotor.periphPWM;
		ui32periphGPIO = mainRotor.periphGPIO;
		ui32GPIOConfig = mainRotor.GPIOConfig;
		ui32base = mainRotor.base;
		ui32gen = mainRotor.gen;
		ui32outbit = mainRotor.outbit;
		ui32Duty = config.mainDutyCycle;
		ui32freq = config.mainFrequency;
		ui32GPIOPin = mainRotor.GPIOPin;
	} else {
		ui32periphPWM = tailRotor.periphPWM;
		ui32periphGPIO = tailRotor.periphGPIO;
		ui32GPIOConfig = tailRotor.GPIOConfig;
		ui32base = tailRotor.base;
		ui32gen = tailRotor.gen;
		ui32outbit = tailRotor.outbit;
		ui32Duty = config.tailDutyCycle;
		ui32freq = config.tailFrequency;
		ui32GPIOPin = tailRotor.GPIOPin;
	}	
	
	SysCtlPeripheralEnable(ui32periphPWM);
    SysCtlPeripheralEnable(ui32periphGPIO);
	
    GPIOPinConfigure(ui32periphGPIO);
	
    GPIOPinTypePWM(ui32base, ui32GPIOPin);
	
    PWMGenConfigure(ui32base, ui32gen,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);	
					
    setPWM(isMainMotor, ui32Duty); // Set the initial PWM parameters
    PWMGenEnable(ui32base, ui32gen);
    PWMOutputState(ui32base, ui32outbit, false); // Disable the output.  Repeat this call with 'true' to turn O/P on.
}

void resetPeripheralPWM(){

	 // As a precaution, make sure that the peripherals used are reset
    SysCtlPeripheralReset (mainRotor.periphGPIO); // Used for PWM output
    SysCtlPeripheralReset (mainRotor.periphPWM);  // Main Rotor PWM
    SysCtlPeripheralReset (tailRotor.periphGPIO); // Used for PWM output
    SysCtlPeripheralReset (tailRotor.periphPWM);  // TAIL rotor PWM
}


void setOutputOnline(bool isOn){
	isOn = true; // for right now
	PWMOutputState(mainRotor.base, mainRotor.outbit, isOn); // true
    PWMOutputState(tailRotor.base, tailRotor.outbit, isOn); // true
}

	

