/** LIBRARIES | TIVA **/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"


struct pwmConfig {
	uint32_t rateMinHZ;
	uint32_t rateMaxHZ;
	uint32_t mainDutyCycle;
	uint32_t TailDutyCycle;
	uint32_t mainFrequency;
	uint32_t tailFrequency;
	uint32_t dividerCode;
	uint32_t divider;
 };

struct pwmRotor {
	uint32_t base;
	uint32_t gen;
	uint32_t outnum;
	uint32_t outbit;
	uint32_t periphPWM;
	uint32_t periphGPIO;
	uint32_t GPIOBase;
	uint32_t GPIOConfig;
	uint8_t GPIOPin;
};

// configuration for both dreams
struct pwmConfig config = {150, 300, 0, 0, 250, 200, SYSCTL_PWMDIV_4, 4};

//  PWM Hardware Details M0PWM7 (gen 3)
//  ---Main Rotor PWM: PC5, J4-05
struct pwmRotor mainRotor = { PWM0_BASE, PWM_GEN_3, PWM_OUT_7, PWM_OUT_7_BIT, SYSCTL_PERIPH_PWM0, SYS_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PC5_M0PWM7, GPIO_PIN_5 };

//  PWM Hardware Details M1PWM5 (gen 2)  
//  ---Tail Rotor PWM: PF1, J3-10   
struct pwmRotor tailRotor = { PWM1_BASE, PWM_GEN_2, PWM_OUT_5, PWM_OUT_5_BIT, SYSCTL_PERIPH_PWM1, SYS_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PF1_M0PWM5, GPIO_PIN_1 };

void setUpPWMSignals(void); // requires initialization in main()
void setPWMClocks(void);
void setPWM(uint32_t isMainRotor, uint32_t ui32Duty);
void initializePWM(uint8_t isMainMotor);
void resetPeripheralPWM(void);
void setOutputOnline(bool isOn);


// todo: a display of the pwm duty cycle for each motor as a percentage is maintained on the orbit display.

/* DEAD CODE RECOVERY
GHOST(){
	mainConfig = {
		startRateHZ = 250, 
		rateStepHZ = 50, 
		rateMinHZ = 150,
		rateMaxHZ = 300, 
		dutyCycle = 50, 
		frequency = 250, //equals startRateHZ 
		dividerCode = SYSCTL_PWMDIV_4,
		divider = 4
	}
	mallocs mate
	https://www.codingunit.com/c-tutorial-the-functions-malloc-and-free
}	
*/






