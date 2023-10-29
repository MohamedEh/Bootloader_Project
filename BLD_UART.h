#ifndef BLD_UART_H
#define BLD_UART_H

#define USE_INT		0

#define UBRRH			*((volatile uint8*)0x40)
#define UBRRL			*((volatile uint8*)0x29)
#define UCSRA			*((volatile uint8*)0x2B)
#define UCSRB			*((volatile uint8*)0x2A)
#define UCSRC			*((volatile uint8*)0x40)
#define UDR				*((volatile uint8*)0x2C)

#define IDLE	0

#define RUNNING	1

void UART_vInit(void);
void UART_vSendChar(uint8 copy_u8Data);
uint8 UART_u8ReceiveChar(void);
void UART_vRxHandler(void);

#endif
