#ifndef __ADC__
#define __ADC__

void Adc_Init(void);                                           //ADC Enble, prescalar, and reference voltage

unsigned short Adc_ReadChannel(unsigned char channel);         //selects the channel and returns the result of the A/D conversion

#endif