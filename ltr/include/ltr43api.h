
/*#include "ltr\\include\\ltrapidefine.h" 
#include "ltr\\include\\ltr010api.h"   */

#include "ltr\\include\\ltrapi.h"

#ifdef LTR43API_EXPORTS
#define LTR43API_DllExport(type)   __declspec(dllexport) type APIENTRY  
#else
#define LTR43API_DllExport(type)   __declspec(dllimport) type APIENTRY
#endif


// ���� ������
#define LTR43_NO_ERR                                  (0)
#define LTR43_ERR_WRONG_MODULE_DESCR				  (-4001)
#define LTR43_ERR_CANT_OPEN                           (-4002)
#define LTR43_ERR_INVALID_CRATE_SN 			          (-4003)
#define LTR43_ERR_INVALID_SLOT_NUM					  (-4004)
#define LTR43_ERR_CANT_SEND_COMMAND 				  (-4005)
#define LTR43_ERR_CANT_RESET_MODULE				      (-4006)
#define LTR43_ERR_MODULE_NO_RESPONCE				  (-4007)
#define LTR43_ERR_CANT_SEND_DATA					  (-4008)
#define LTR43_ERR_CANT_CONFIG                         (-4009) 
#define LTR43_ERR_CANT_RS485_CONFIG                   (-4010)
#define LTR43_ERR_CANT_LAUNCH_SEC_MARK				  (-4011)
#define LTR43_ERR_CANT_STOP_SEC_MARK				  (-4012)
#define LTR43_ERR_CANT_LAUNCH_START_MARK			  (-4013)
#define LTR43_ERR_CANT_STOP_RS485RCV				  (-4014)
#define LTR43_ERR_RS485_CANT_SEND_BYTE			      (-4015)  
#define LTR43_ERR_RS485_FRAME_ERR_RCV			      (-4016) 
#define LTR43_ERR_RS485_PARITY_ERR_RCV			      (-4017)
#define LTR43_ERR_WRONG_IO_GROUPS_CONF			      (-4018)   
#define LTR43_ERR_RS485_WRONG_BAUDRATE			      (-4019)
#define LTR43_ERR_RS485_WRONG_FRAME_SIZE			  (-4020)
#define LTR43_ERR_RS485_WRONG_PARITY_CONF			  (-4021)
#define LTR43_ERR_RS485_WRONG_STOPBIT_CONF			  (-4022)
#define LTR43_ERR_DATA_TRANSMISSON_ERROR			  (-4023)
#define LTR43_ERR_RS485_CONFIRM_TIMEOUT       		  (-4024) 
#define LTR43_ERR_RS485_SEND_TIMEOUT       			  (-4025)
#define LTR43_ERR_LESS_WORDS_RECEIVED            	  (-4026)
#define LTR43_ERR_PARITY_TO_MODULE              	  (-4027) 
#define LTR43_ERR_PARITY_FROM_MODULE              	  (-4028)
#define LTR43_ERR_WRONG_IO_LINES_CONF              	  (-4029)   
#define LTR43_ERR_WRONG_SECOND_MARK_CONF			  (-4030)
#define LTR43_ERR_WRONG_START_MARK_CONF				  (-4031)
#define LTR43_ERR_CANT_READ_DATA 					  (-4032)
#define LTR43_ERR_RS485_CANT_SEND_PACK				  (-4033)
#define LTR43_ERR_RS485_CANT_CONFIGURE                (-4034)
#define LTR43_ERR_CANT_WRITE_EEPROM					  (-4035)
#define LTR43_ERR_CANT_READ_EEPROM					  (-4036) 
#define LTR43_ERR_WRONG_EEPROM_ADDR 				  (-4037)
#define LTR43_ERR_RS485_WRONG_PACK_SIZE 			  (-4038)  
#define LTR43_ERR_RS485_WRONG_OUT_TIMEOUT 			  (-4039) 
#define LTR43_ERR_RS485_WRONG_IN_TIMEOUT 			  (-4040)
#define LTR43_ERR_CANT_READ_CONF_REC				  (-4041)
#define LTR43_ERR_WRONG_CONF_REC                      (-4042)
#define LTR43_ERR_RS485_CANT_STOP_TST_RCV             (-4043) 
#define LTR43_ERR_CANT_START_STREAM_READ              (-4044)   
#define LTR43_ERR_CANT_STOP_STREAM_READ               (-4045) 
#define LTR43_ERR_WRONG_IO_DATA						  (-4046)
#define LTR43_ERR_WRONG_STREAM_READ_FREQ_SETTINGS	  (-4047)
#define LTR43_ERR_ERROR_OVERFLOW					  (-4048)



/* ��������� �������� ������ */

#pragma pack(4) 

typedef struct 
{
	CHAR Name[16];
	CHAR Serial[24];
	CHAR FirmwareVersion[8];// ������ �����
	CHAR FirmwareDate[16];  // ���� �������� ������ ������ �����


} TINFO_LTR43,*PTINFO_LTR43; 

typedef struct
{

	INT size;   // ������ ���������    
	TLTR Channel;

	double StreamReadRate;

	struct
	{

		INT Port1;	   // ����������� ����� �����/������ ������ 1
		INT Port2;	   // ����������� ����� �����/������ ������ 2 
		INT Port3;    // ����������� ����� �����/������ ������ 3 
		INT Port4;	   // ����������� ����� �����/������ ������ 4 

	} IO_Ports;

	struct
	{

		INT FrameSize;	  // ���-�� ��� � �����
		INT Baud;		  // �������� ������ � �����
		INT StopBit;	  // ���-�� ����-���
		INT Parity;		  // ��������� ���� ��������
		INT SendTimeoutMultiplier; // ��������� �������� ��������
		INT ReceiveTimeoutMultiplier; // ��������� �������� ������ �������������

	} RS485; // ��������� ��� ������������ RS485

	struct
	{

		INT SecondMark_Mode; // ����� �����. 0 - �����., 1-�����.+�����, 2-�����
		INT StartMark_Mode; // 

	} Marks;  // ��������� ��� ������ � ���������� �������

	TINFO_LTR43 ModuleInfo;  

} TLTR43, *PTLTR43; // ��������� �������� ������

#pragma pack()


#ifdef __cplusplus
extern "C" {
#endif

	LTR43API_DllExport (INT) LTR43_Init(TLTR43 *hnd);
	LTR43API_DllExport(INT) LTR43_IsOpened(PTLTR43 hnd);
	LTR43API_DllExport (INT) LTR43_Open(TLTR43 *hnd, DWORD net_addr, WORD net_port, CHAR *crate_sn, INT slot_num);
	LTR43API_DllExport (INT) LTR43_Close(TLTR43 *hnd);
	LTR43API_DllExport (INT) LTR43_WritePort(TLTR43 *hnd, DWORD OutputData);
	LTR43API_DllExport (INT) LTR43_WriteArray(TLTR43 *hnd, DWORD *OutputArray, BYTE ArraySize);  
	LTR43API_DllExport (INT) LTR43_ReadPort(TLTR43 *hnd, DWORD *InputData);
	LTR43API_DllExport (INT) LTR43_StartStreamRead(TLTR43 *hnd); 
	LTR43API_DllExport (INT) LTR43_StopStreamRead(TLTR43 *hnd); 
	LTR43API_DllExport (INT) LTR43_Recv(TLTR43 *hnd, DWORD *data, DWORD *tmark, DWORD size, DWORD timeout); 
	LTR43API_DllExport (INT) LTR43_ProcessData(TLTR43 *hnd, DWORD *src, DWORD *dest, DWORD *size);   
	LTR43API_DllExport (INT) LTR43_Config(TLTR43 *hnd);
	LTR43API_DllExport (INT) LTR43_StartSecondMark(TLTR43 *hnd);  
	LTR43API_DllExport (INT) LTR43_StopSecondMark(TLTR43 *hnd);  
	LTR43API_DllExport (LPCSTR) LTR43_GetErrorString(INT Error_Code); 
	LTR43API_DllExport (INT) LTR43_MakeStartMark(TLTR43 *hnd);
	LTR43API_DllExport (INT) LTR43_RS485_Exchange(TLTR43 *hnd, SHORT *PackToSend, SHORT *ReceivedPack, INT OutPackSize, INT InPackSize);            
	LTR43API_DllExport (INT) LTR43_WriteEEPROM(TLTR43 *hnd, INT Address, BYTE val); 
	LTR43API_DllExport (INT) LTR43_ReadEEPROM(TLTR43 *hnd, INT Address, BYTE *val); 
	LTR43API_DllExport (INT)  LTR43_RS485_TestReceiveByte(TLTR43 *hnd, INT OutBytesQnt,INT InBytesQnt);        
	LTR43API_DllExport (INT)  LTR43_RS485_TestStopReceive(TLTR43 *hnd);  

#ifdef __cplusplus 
}

#endif
















