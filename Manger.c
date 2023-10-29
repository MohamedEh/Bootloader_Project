#include "STD_TYPES.h"
//#include "Err_Type.h"
#include <avr/boot.h>
#include "util/crc16.h"
#include "avr/pgmspace.h"
#include "avr/delay.h"
#include "BIT_MATH.h"
#include "GIE_interface.h"
#include "BLD_UART.h"
#include "Manager.h"



void (*ptr_func)(void)=(void (*)(void))0x0;

volatile uint8 IsReqReceived = 0,u8Data_Length,u8SID,u8DownloadState;
volatile uint8*pu8Data_Buffer=NULL;





void Boot_Func(uint8 page,uint8*data)
{
	GIE_voidDisable();
	uint16 address=page*Page_Bytes,word=0;
	boot_page_erase_safe(address);
	for(uint8 i=0;i<Page_Bytes;i+=2)
	{
		word=*data++;
		word |= (*data++)<<8;
		boot_page_fill_safe(address+i,word);
	}
	boot_page_write_safe(address);
	boot_rww_enable_safe();
}


void Manager_vPosReq(void)
{
	UART_vSendChar(u8SID+0x40);
}
void Manager_vNegReq(void)
{
	UART_vSendChar(0x7F);
}

void Manager_vMain(void)
{
	static uint16 Local_u16CodeSize,Local_u16ReceivedLen,Local_u16ReceivedCRC;
	static uint8 Local_u8ValidReq,Local_u8PageNo=0,Local_u8IsCRCValid;


	if(IsReqReceived)
	{

		switch(u8SID)
		{
		case	SESSION_CONTROL:

			if((pu8Data_Buffer[0]==PROGRAMMING_SESSION)&&(u8Data_Length==2)&&(u8DownloadState==waiting_ProgrammingSession))
			{

				Manager_vPosReq();
				u8DownloadState=waiting_DownloadRequest;
			}
			else
			{
				Manager_vNegReq();
				u8DownloadState=waiting_ProgrammingSession;

			}
			break;

		case	DOWNLOAD_REQUEST:
			if((u8Data_Length==3)&&(u8DownloadState==waiting_DownloadRequest))
			{
				Local_u16CodeSize = pu8Data_Buffer[0]<<8  | pu8Data_Buffer[1];
				if(Local_u16CodeSize<MAX_CODE_SIZE)
				{
					Manager_vPosReq();
					u8DownloadState=waiting_TransferData;
					Local_u8ValidReq=1;
				}


			}
			if(Local_u8ValidReq==0)
			{
				Manager_vNegReq();
				u8DownloadState=waiting_ProgrammingSession;

			}
			break;

		case	TRANSFER_DATA:
			if((u8Data_Length==Page_Bytes+1)&&(u8DownloadState==waiting_TransferData))
			{
				Boot_Func(Local_u8PageNo,pu8Data_Buffer);
				Manager_vPosReq();
				Local_u8PageNo++;
				Local_u16ReceivedLen +=Page_Bytes;
				if(Local_u16ReceivedLen==Local_u16CodeSize)
				{
					u8DownloadState=waiting_TransferExit;
				}
				else
				{
					//do nothing
				}
			}
			else
			{
				Manager_vNegReq();
				u8DownloadState=waiting_ProgrammingSession;

			}
			break;

		case	TRANSFER_EXIT:
			if((u8Data_Length==1)&&(u8DownloadState==waiting_TransferExit))
			{
				Manager_vPosReq();
				u8DownloadState=waiting_CheckCRC;

			}
			else
			{
				Manager_vNegReq();
				u8DownloadState=waiting_ProgrammingSession;

			}
			break;

		case	CHECK_CRC:
			if((u8Data_Length==3)&&(u8DownloadState==waiting_CheckCRC))
			{
				Local_u16ReceivedCRC = pu8Data_Buffer[0]<<8|pu8Data_Buffer[1];
				Local_u8IsCRCValid = Manager_vCheckCRC(0,Local_u16CodeSize,Local_u16ReceivedCRC);
				if(Local_u8IsCRCValid)
				{
					Manager_vPosReq();
					eeprom_update_byte(eeprom_valid_app_add,1);
#if USE_INT ==1
		APP_vMoveIvt(APP_SEC);
#endif
		_delay_ms(100);
		ptr_func();

				}
				else
				{
					Manager_vNegReq();


				}



			}
			else
			{
				Manager_vNegReq();


			}
			u8DownloadState=waiting_ProgrammingSession;

			break;

		default:

			Manager_vNegReq();
			u8DownloadState=waiting_ProgrammingSession;


		}
		IsReqReceived = 0;
	}
	else
	{
		//do nothing
	}
}

void Manager_vRxNotification(uint8* copy_pu8Data_Buffer,uint8 copy_u8Data_Length)
{

	IsReqReceived=1;
	u8SID=copy_pu8Data_Buffer[0];
	pu8Data_Buffer=&copy_pu8Data_Buffer[1];
	u8Data_Length=copy_u8Data_Length;

}

void Manager_vHandleAppReq(void)
{
	u8SID=0x10;
	Manager_vPosReq();
	u8DownloadState=waiting_DownloadRequest;
}






uint8 Manager_vCheckCRC(uint16 Copy_u16StartAdd,uint16 copy_u16EndAdd,uint16 copy_u16ReceivedCRC)
{
	uint8 Local_u8Return=1;
	uint16 Local_u16Add;
	uint8 Local_u8Byte;
	uint16 Local_u16CRC16 = 0xFFFF;
	for(Local_u16Add=Copy_u16StartAdd;Local_u16Add<copy_u16EndAdd;Local_u16Add++)
	{
		Local_u8Byte = pgm_read_byte(Local_u16Add);
		Local_u16CRC16 = _crc16_update(Local_u16CRC16,Local_u8Byte);
	}
	if(Local_u16CRC16!=copy_u16ReceivedCRC)
	{
		Local_u8Return= 0;
	}
	return Local_u8Return;
}
