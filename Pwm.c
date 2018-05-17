#include "Pwm.h"

// "mainRotor" and "tailRotor" are your best friends.

void setPWMClocks(void){
	// Set the PWM clock rate (using the prescaler)
    SysCtlPWMClockSet(config->dividerCode);
}

void setPWM(uint8_t isMainRotor, uint32_t ui32Duty){
	
	// Calculate the PWM period corresponding to the freq.
	uint32_t ui32freq, ui32base, ui32gen, ui32outnum; 
	
	if (isMainRotor == 1){
		ui32freq = config->mainFrequency;
		ui32base = mainMotor->base;
		ui32gen = mainMotor->gen;
		ui32outnum = mainMotor->outnum;
		config->mainDutyCycle = ui32Duty;
	} else {
		ui32freq = config->tailFrequency;
		ui32base = tailMotor->base;
		ui32gen = tailMotor->gen;
		ui32outnum = tailMotor->outnum;
		config->tailDutyCycle = ui32Duty;
	}
	
	uint32_t ui32Period = SysCtlClockGet() / config->divide / ui32freq;
    PWMGenPeriodSet(ui32base, ui32gen, ui32Period);
    PWMPulseWidthSet(ui32base, ui32outnum,
        ui32Period * ui32Duty / 100);
	

}

void initializePWM(uint8_t isMainMotor){
	
	uint32_t ui32periphPWM, ui32periphGPIO, ui32GPIOConfig, ui32base, ui32gen, ui32outbit, ui32Duty, ui32freq; 
	uint8_t ui32GPIOPin;
	
	if (isMainRotor == 1){
		ui32periphPWM = mainMotor->periphPWM;
		ui32periphGPIO = mainMotor->periphGPIO;
		ui32GPIOConfig = mainMotor->GPIOConfig;
		ui32base = mainMotor->base;
		ui32gen = mainMotor->gen;
		ui32outbit = mainMotor->outbit;
		ui32Duty = config->mainDutyCycle;
		ui32freq = config->mainFrequency;
		ui32GPIOPin = mainRotor->GPIOPin;
	} else {
		ui32periphPWM = tailMotor->periphPWM;
		ui32periphGPIO = tailMotor->periphGPIO;
		ui32GPIOConfig = tailMotor->GPIOConfig;
		ui32base = tailMotor->base;
		ui32gen = tailMotor->gen;
		ui32outbit = tailMotor->outbit;
		ui32Duty = config->tailDutyCycle;
		ui32freq = config->tailFrequency;
		ui32GPIOPin = tailRotor->GPIOPin;
	}	
	
	SysCtlPeripheralEnable(ui32periphPWM);
    SysCtlPeripheralEnable(ui32periphGPIO);
	
    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
	
    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);
	
    PWMGenConfigure(ui32base, ui32gen,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);	
					
    setPWMTAIL (ui32freq, ui32Duty); // Set the initial PWM parameters
    PWMGenEnable(ui32base, ui32gen);
    PWMOutputState(ui32base, ui32outbit, false); // Disable the output.  Repeat this call with 'true' to turn O/P on.
}

	

