#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "BLD_UART.h"
#include "GIE_interface.h"
#include <util/delay.h>

uint8 u8Data_Buffer[255];
uint8 u8Data_Length;

#define DDRC	*((volatile uint8*)0x34)
#define PORTC	*((volatile uint8*)0x35)

void UART_vInit(void)
{
	  // Set BaudRate -> 9600/16MhZ
	  UBRRL = 103;
	  UBRRH = 0;
	  // Set Frame Format -> 8 data, 1 stop, No Parity
	  UCSRC = 0x86;
	  // Enable RX and TX and Rx interrupt
	  UCSRB = 0x18;
#if USE_INT==1
	  UCSRB |=(1<<7);
	  //Enable Global INT
	  GIE_voidEnable();
#endif
}

void UART_vSendChar(uint8 copy_u8Data)
{
	//wait till Transmit register is empty
	while(GET_BIT(UCSRA,5)==0);
	UDR=copy_u8Data;
}
uint8 UART_u8ReceiveChar(void)
{
	uint8 copy_u8Data;
	//wait till the receive flag is raised
	while(GET_BIT(UCSRA,7)==0);
	copy_u8Data=UDR;
	return copy_u8Data;
}
#if USE_INT == 1
void __vector_13(void) __attribute__((signal))
void __vector_13(void)
{
		static uint8 Local_u8State = IDLE;
		static uint8 Local_u8Index = 0;

			if(Local_u8State==IDLE)
			{
				u8Data_Length = UDR;
				Local_u8State = RUNNING;
			}
			else
			{
				u8Data_Buffer[Local_u8Index]=UDR;
				Local_u8Index++;
				if(Local_u8Index==u8Data_Length)
				{
					Local_u8Index=0;
					Local_u8State=IDLE;
					Manager_vRxNotification(u8Data_Buffer,u8Data_Length);
				}
			}


}
#else
void UART_vRxHandler(void)
{
	static uint8 Local_u8State = IDLE;
	static uint8 Local_u8Index = 0;
	DDRC =0xFF;

	if(GET_BIT(UCSRA,7)==1)
	{
		PORTC = 2;
		if(Local_u8State==IDLE)
		{
			u8Data_Length = UDR;
			Local_u8State = RUNNING;
		}
		else
		{
			u8Data_Buffer[Local_u8Index]=UDR;
			Local_u8Index++;
			if(Local_u8Index==u8Data_Length)
			{
				Local_u8Index=0;
				Local_u8State=IDLE;
				Manager_vRxNotification(u8Data_Buffer,u8Data_Length);
			}
		}
		//_delay_ms(1000);

	}
}
#endif
