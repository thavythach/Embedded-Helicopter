#include "Helicopter.h"


void initialiseUSB_UART (void) {
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

void UARTSend (char *string) {
    // Loop while there are more characters to send.
    while(*string)
    {
        // Write the next character to the UART Tx FIFO.
        UARTCharPut(UART0_BASE, *string);
        string++;
    }
}

char * getSW1modeString(int32_t localMode) {
    switch(localMode) {
        case 0 :
            return "landed";

        case 1 :
            return "flying";

        case 2 :
            return "landing";
        default:
            return "";
    }
}
void UART() {
       if (slowTick) {
           char string[17];

           slowTick = false;

           usnprintf (string, sizeof(string), "Yaw = %4d\n\r", yawDegreeConvert(yaw));
           UARTSend (string);

           usnprintf (string, sizeof(string), "SP Yaw = %4d\n\r", getYawDegreesSetPoint());
           UARTSend (string);

           usnprintf (string, sizeof(string), "Alt = %3d\n\r", altitude);
           UARTSend (string);

           usnprintf (string, sizeof(string), "SP Alt = %3d\n\r", getAltitudePercentSetPoint());
           UARTSend (string);

           usnprintf (string, sizeof(string), "Mode = %s\n\r", getSW1modeString(getSW1mode()));
           UARTSend (string);
       }
}
