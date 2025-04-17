#include <avr/io.h>
#include <util/delay.h>

//Configure Data Direction Register as input or output for a pin while not overwriting the rest of the pins
//0, i & I indicate input, else it's considered as an output
//for input, the bit at the DDRx registers is cleared while for output, the bit is set. 
void Dio_SetPinDirection(unsigned char Port, unsigned char Pin, unsigned char Direction) {
  if (Port == 'B' or Port == 'b') {
    if (Direction == 0 or Direction == 'i' or Direction == 'I') DDRB = DDRB & ~(1 << Pin);
    else DDRB = DDRB | (1 << Pin);
  } else if (Port == 'C' or Port == 'c') {
    if (Direction == 0 or Direction == 'i' or Direction == 'I') DDRC = DDRC & ~(1 << Pin);
    else DDRC = DDRC | (1 << Pin);
  } else if (Port == 'D' or Port == 'd') {
    if (Direction == 0 or Direction == 'i' or Direction == 'I') DDRD = DDRD & ~(1 << Pin);
    else DDRD = DDRD | (1 << Pin);
  }
}

//For output pins, write high or low on the pin using the PORT register
//0, 'l' & 'L' indicate low, else high
//For low, the bit at the PORTx register is cleared, while for high the bit is set.
void Dio_SetPinState(unsigned char Port, unsigned char Pin, unsigned char State) {
  if (Port == 'B' or Port == 'b') {
    if (State == 0 or State == 'l' or State == 'L') PORTB = PORTB & ~(1 << Pin);
    else PORTB = PORTB | (1 << Pin);
  } else if (Port == 'C' or Port == 'c') {
    if (State == 0 or State == 'l' or State == 'L') PORTC = PORTC & ~(1 << Pin);
    else PORTC = PORTC | (1 << Pin);
  } else if (Port == 'D' or Port == 'd') {
    if (State == 0 or State == 'l' or State == 'L') PORTD = PORTD & ~(1 << Pin);
    else PORTD = PORTD | (1 << Pin);
  }
}

//For input pins, get high or low from the pin using PIN register
//the value of the specific bit in the PINx register is returned as the least significant bit by right-shifting the whole 8-bit number by the (Pin)
unsigned char Dio_GetPinState(unsigned char Port, unsigned char Pin) {
  if (Port == 'B' or Port == 'b') return ((PINB & 1 << Pin) >> Pin);
  if (Port == 'C' or Port == 'c') return ((PINC & 1 << Pin) >> Pin);
  if (Port == 'D' or Port == 'd') return ((PIND & 1 << Pin) >> Pin);
}