#include "Helicopter.h"

/**
 * @desc initialize UART peripherals (x2)
 * @param N/A
 * @return N/A
 */
void initialiseUSB_UART (void) {
    // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);

    // Select the alternate (UART) function for these pins.
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}

/**
 * @desc given a string, continuously send a single characters across the serial port
 * @param *string a character array of string values to be used to output to a console on the helicopter remote labs
 * @return N/A
 */
void UARTSend (char *string) {
    // Loop while there are more characters to send.
    while(*string)
    {
        // Write the next character to the UART Tx FIFO.
        UARTCharPut(UART0_BASE, *string);
        string++;
    }
}

/**
 * @desc turns Helicopter mode (int) to corresponding string formatted value (*char)
 * @param int32_t localMode 0 for landed, 1 for flying, 2 for landing
 * @return *char returns corresponding string: 0 = LANDED, 1 = FLYING, 2 = LANDING
 */
char * getSW1modeString(int32_t localMode) {
    switch(localMode) {
        case LANDED :
            return "LANDED";

        case FLYING :
            return "FLYING";

        case LANDING :
            return "LANDING";
        default:
            return "";
    }
}

/**
 * @desc Sending multiple sets of data: yaw, setpoint yaw, altitude, setpoint altitude, current helicopter mode, and a delimitter.
 * @param N/A
 * @return N/A
 */
void UART() {
   if (slowTick) {
       char string[17];

       slowTick = false;

       usnprintf (string, sizeof(string), "\nYaw = %4d\n\r", yawDegreeConvert(yaw));
       UARTSend (string);

       usnprintf (string, sizeof(string), "SP Yaw = %4d\n\r", getYawDegreesSetPoint());
       UARTSend (string);

       usnprintf (string, sizeof(string), "Alt = %3d\n\r", altitude);
       UARTSend (string);

       usnprintf (string, sizeof(string), "SP Alt = %3d\n\r", getAltitudePercentSetPoint());
       UARTSend (string);

       usnprintf (string, sizeof(string), "Mode = %s\n\r", getSW1modeString(getSW1mode()));
       UARTSend (string);

       usnprintf (string, sizeof(string), "------------------------\n\r");
       UARTSend (string);
   }
}
