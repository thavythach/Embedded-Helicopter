#include "driverlib/uart.h"



#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX


void SysTickIntHandler (void);
void initialiseUSB_UART (void);
void UARTSend (char *string);
void initSysTick (void);


char string[MAX_STR_LEN + 1];
volatile uint8_t slowTick = false;

void
SysTickIntHandler (void)
{
	static uint8_t tickCount = 0;
	const uint8_t ticksPerSlow = SYSTICK_RATE_HZ / SLOWTICK_RATE_HZ;

    updateButtons ();		// Poll the buttons
    if (++tickCount >= ticksPerSlow)
    {						// Signal a slow tick
    	tickCount = 0;
    	slowTick = true;
    }

}


void
initialiseUSB_UART (void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    //
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    //
    // Select the alternate (UART) function for these pins.
    //
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
    		UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
			UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}


void
UARTSend (char *string)
{
    // Loop while there are more characters to send.
    while(*string)
    {
        // Write the next character to the UART Tx FIFO.
        UARTCharPut(UART0_BASE, *string);
        string++;
    }
}

int main(void) {
    initialiseUSB_UART ();
    initSysTick ();

    while(1) {
        if (slowtick) {
            slowTick = false;
            usnprintf (string, "Yaw = %1d", Yaw;
            UARTSend (string);
            usnprintf (string, "Setpoint Yaw = %1d", getYawSetPoint());
            UARTSend (string);
            usnprintf (string, "Alt = %1d", Altitude);
            UARTSend (string);
            usnprintf (string, "Setpoint Alt = %1d", getAltitudeSetPoint());
            UARTSend (string);
            usnprintf (string, "Mode = %s", getSW1Mode());
            UARTSend (string);


        }

    }
}