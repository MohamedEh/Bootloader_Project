#ifndef MANAGER_H
#define MANAGER_H

#define eeprom_valid_app_add	((uint8*)0x00)
#define eeprom_app_req_add	((uint8*)0x01)


#define SESSION_CONTROL			(0X10)
#define PROGRAMMING_SESSION	(0x03)
#define DOWNLOAD_REQUEST		(0X34)
#define TRANSFER_DATA				(0X36)
#define TRANSFER_EXIT				(0X37)
#define CHECK_CRC						(0X31)
#define MAX_CODE_SIZE				(0x3800)
#define Page_Bytes					128


typedef enum {
waiting_ProgrammingSession,
waiting_DownloadRequest,
waiting_TransferData,
waiting_TransferExit,
waiting_CheckCRC
}downloadStates ;


void Manager_vMain(void);
void Manager_vRxNotification(uint8* copy_pu8Data_Buffer,uint8 copy_u8Data_Length);
void Manager_vHandleAppReq(void);
void Manager_vPosReq(void);
void Manager_vNegReq(void);
uint8 Manager_vCheckCRC(uint16 Copy_u16StartAdd,uint16 copy_u16EndAdd,uint16 copy_u16ReceivedCRC);
void Boot_Func(uint8 page,uint8*data);

#endif
