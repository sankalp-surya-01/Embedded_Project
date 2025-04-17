//dio.h is a c library to configure, read and write digital pins on AVR-based MCU

//Configure DDR register as input or output for a pin while not overwriting the rest of the pins
void Dio_SetPinDirection(unsigned char Port, unsigned char Pin, unsigned char Direction);

//For output pins, write high or low on the pin using the PORT register
void Dio_SetPinState(unsigned char Port, unsigned char Pin, unsigned char State);

//For input pins, get high or low from the pin using PIN register
unsigned char Dio_GetPinState(unsigned char Port, unsigned char Pin);