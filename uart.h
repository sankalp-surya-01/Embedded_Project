//Sets baud rate, enables Tx and Rx, selects asynchronous mode, and sets frame format to 8-bits
void Uart_Init(void);

//calculates the respective frequency and writes the 12-bit number
void Uart_SetBaudRate(unsigned short BuadRate);

//waits untill the buffer is empty to send the byte (8-bits)
void Uart_SendChar(unsigned char DataByte);

//waits untill the data is recieved and returns the received byte
unsigned char Uart_ReadData();

//loops over the string to send it one character at a time
void Uart_SendString(unsigned char DataString[], unsigned char Size);