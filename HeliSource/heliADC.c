// might need to add tiva source files

#include "helicopter.h"

void initADC (void){
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    
    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);    
    ADCSequenceEnable(ADC0_BASE, 3); // Since sample sequence 3 is now configured, it must be enabled.
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler); // Register the interrupt handler
  
    ADCIntEnable(ADC0_BASE, 3);// Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
}

/**
 * The handler for the ADC conversion complete interrupt.
 * Writes to the circular buffer.
 **/
void ADCIntHandler(void){
    uint32_t ulValue;

    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue); // Get the single sample from ADC0.  ADC_BASE is defined in
    
    writeCircBuf (&g_inBuffer, ulValue); // Place it in the circular buffer (advancing write index)
    
    ADCIntClear(ADC0_BASE, 3); // Clean up, clearing the interrupt
}
