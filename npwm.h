struct pwmConfig {
	int rateMinHZ;
	int rateMaxHZ;
	int mainDutyCycle;
	int tailDutyCycle;
	int mainFrequency;
	int tailFrequency;
	int dividerCode;
	int divider;
 };

struct pwmRotor {
	int base;
	int gen;
	int outnum;
	int outbit;
	int periphPWM;
	int periphGPIO;
	int GPIOBase;
	int GPIOConfig;
	int GPIOPin;
};

// configuration for both dreams
struct pwmConfig config = {150, 300, 0, 0, 250, 200, 4, 4};

//  PWM Hardware Details M0PWM7 (gen 3)
//  ---Main Rotor PWM: PC5, J4-05
struct pwmRotor mainRotor = { 0, 1, 0, 1, 0, 1, 0, 1, 0};

//  PWM Hardware Details M1PWM5 (gen 2)  
//  ---Tail Rotor PWM: PF1, J3-10   
struct pwmRotor tailRotor = { 0, 1, 0, 1, 0, 1, 0, 1, 0};

void setUpPWMSignals(void); // requires initialization in main()
void setPWM(int isMainRotor, int ui32Duty);
void initializePWM(int isMainMotor);

// TODO
void resetPeripheralPWM();
void setOutputOnline(int isMainMotor);

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






