#include "STD_TYPES.h"
//#include "Err_Type.h"
#include <avr/boot.h>
#include <avr/eeprom.h>
#include "BIT_MATH.h"
#include "GIE_interface.h"
#include "BLD_UART.h"
#include "App.h"
#include "Manager.h"
#include "BLD_UART.h"
//#include "DIO_interface.h"
//#include "PORT_interface.h"
//#include <util/delay.h>
//#define DDRC	*((volatile uint8*)0x34)
//#define PORTC	*((volatile uint8*)0x35)

#define Page_Bytes	128
#define No_of_Pages	2
void (*JmpApp)(void)=(void (*)(void))0x00;

//intitialize & locate two bytes in eeprom
uint8 eeData[] EEMEM = {0x00,0x00};



void main(void)
{
	uint8 AppValid,AppReq;
	//read value of the two bytes from eeprom
	AppValid = eeprom_read_byte(eeprom_valid_app_add);
	AppReq = eeprom_read_byte(eeprom_app_req_add);


	if((AppValid!=1)|(AppReq==1))
	{
		UART_vInit();
#if USE_INT==1
		//Move IVT to start of boot loader section
		APP_vMoveIvt(BLD_SEC);
#endif
		if(AppReq==1)
		{
			Manager_vHandleAppReq();
			//clear flag of app req
			eeprom_update_byte(eeprom_app_req_add,0);
		}
		while(1)
		{
#if USE_INT==0
			UART_vRxHandler();
#endif
			Manager_vMain();
		}
	}
	else
	{
		//valid app jump to its address
		JmpApp();
	}
}


extern void APP_vMoveIvt(uint8 copy_u8section)
{
	//Move IVT to start of app section
	if(copy_u8section==APP_SEC)
	{
		//set change enable bit
		GICR=(1<<GICR_IVCE);
		//clr selection bit for move to app section
		GICR &= ~(1<<GICR_IVSEL);
	}
	else
	{
		//set change enable bit
		GICR=(1<<GICR_IVCE);
		//set selection bit for move to boot flash section
		GICR |= (1<<GICR_IVSEL);
	}
}
