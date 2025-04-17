#if !defined(__AVR_ATmega328P__)
#include <avr/iom328p.h>
#endif

#include "Uart.h"

#define _BV(bit) (1 << (bit))
#define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
#define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))
#define loop_until_bit_is_set(sfr, bit) do { } while (bit_is_clear(sfr, bit))
#define loop_until_bit_is_clear(sfr, bit) do { } while (bit_is_set(sfr, bit))


void Uart_Init(void) {

    Uart_SetBaudRate(9600);

    /* Enable USART transmitter/receiver */
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);

    /* frame format: 8 data bits, async mode: 1 stop bit */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void Uart_SetBaudRate(unsigned short BuadRate)
{
    //baud rate equation with respect to the clock frequency of uno (16MHz)
    unsigned short UBBR = ( (F_CPU / 16) /  BuadRate ) - 1;
    ////to write the first 8 bits of the 12 bit number ubrr into the 8-bit register UBRR0L
    UBRR0L = (UBBR & 0xFF);
    //to write the last 4 bits of the 12 bit number ubrr into the other 8-bit register UBRR0H
    UBRR0H = (( UBBR >> 8 ) & 0xFF);
}


void Uart_SendChar(unsigned char DataByte)
{
    // Wait until Write buffer is empty (1: empty)
    while ( ! (UCSR0A & ( 1 << UDRE0)) );
    UDR0 = DataByte;
}

unsigned char Uart_ReadData(void) {
    // Wait until data is received
    while ( ! (UCSR0A & ( 1 << RXC0)) );
    return UDR0;
}

//loops over the string to send it one character at a time
void Uart_SendString(const char DataString[]){
    int i;
    for (i=0; DataString[i]; i++)
    {
        Uart_SendChar(DataString[i]);
    }
}