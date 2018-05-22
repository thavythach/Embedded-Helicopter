#include "Helicopter.h"
/*
 * generate a ~10ms delay for calculations to take place
 */
void pacerWait() {

    IntMasterDisable();
    SysCtlDelay(SysCtlClockGet() / 100 / 3 );
    IntMasterEnable();
}
