

#include "ltr\\include\\ltrapi.h" 

#ifdef LTR41API_EXPORTS
  #define LTR41API_DllExport(type)   __declspec(dllexport) type APIENTRY  
#else
  #define LTR41API_DllExport(type)   __declspec(dllimport) type APIENTRY
#endif


// Коды ошибок  Начинать с 7000!!! Для LTR42 - с 8000!
#define LTR41_NO_ERR                                  (0)
#define LTR41_ERR_WRONG_MODULE_DESCR				  (-7001)
#define LTR41_ERR_CANT_OPEN                           (-7002)
#define LTR41_ERR_INVALID_CRATE_SN 			          (-7003)
#define LTR41_ERR_INVALID_SLOT_NUM					  (-7004)
#define LTR41_ERR_CANT_SEND_COMMAND 				  (-7005)
#define LTR41_ERR_CANT_RESET_MODULE				      (-7006)
#define LTR41_ERR_MODULE_NO_RESPONCE				  (-7007)
#define LTR41_ERR_CANT_CONFIG                         (-7008) 
#define LTR41_ERR_CANT_LAUNCH_SEC_MARK				  (-7009)
#define LTR41_ERR_CANT_STOP_SEC_MARK				  (-7010)
#define LTR41_ERR_CANT_LAUNCH_START_MARK			  (-7011)
#define LTR41_ERR_LESS_WORDS_RECEIVED            	  (-7012)
#define LTR41_ERR_PARITY_TO_MODULE              	  (-7013) 
#define LTR41_ERR_PARITY_FROM_MODULE              	  (-7014)
#define LTR41_ERR_WRONG_SECOND_MARK_CONF			  (-7015)
#define LTR41_ERR_WRONG_START_MARK_CONF				  (-7016)
#define LTR41_ERR_CANT_READ_DATA 					  (-7017)
#define LTR41_ERR_CANT_WRITE_EEPROM					  (-7018)
#define LTR41_ERR_CANT_READ_EEPROM					  (-7019) 
#define LTR41_ERR_WRONG_EEPROM_ADDR 				  (-7020)
#define LTR41_ERR_CANT_READ_CONF_REC				  (-7021)
#define LTR41_ERR_WRONG_CONF_REC                      (-7022)
#define LTR41_ERR_CANT_START_STREAM_READ              (-7023)   
#define LTR41_ERR_CANT_STOP_STREAM_READ               (-7024) 
#define LTR41_ERR_WRONG_IO_DATA						  (-7025)
#define LTR41_ERR_WRONG_STREAM_READ_FREQ_SETTINGS	  (-7026)
#define LTR41_ERR_ERROR_OVERFLOW					  (-7027)

 // Таймауты приема и передачи команд
#define TIMEOUT_CMD_SEND							   (4000)
#define TIMEOUT_CMD_RECIEVE							   (6000)
 


#ifdef __cplusplus
extern "C" {
 #endif

/* Структура описания модуля */

#pragma pack(4) 

 typedef struct 
{
	CHAR Name[16];
	CHAR Serial[24];
	CHAR FirmwareVersion[8];// Версия БИОСа
    CHAR FirmwareDate[16];  // Дата создания данной версии БИОСа
	

} TINFO_LTR41,*PTINFO_LTR41; 

typedef struct
  {
  
   INT size;   // размер структуры   
   TLTR Channel;

   double StreamReadRate;
   
   struct
    {
    
    INT SecondMark_Mode; // Режим меток. 0 - внутр., 1-внутр.+выход, 2-внешн
    INT StartMark_Mode; // 
    
    } Marks;  // Структура для работы с временными метками
    
    TINFO_LTR41 ModuleInfo;

       
  } TLTR41, *PTLTR41; // Структура описания модуля

#pragma pack()

LTR41API_DllExport (INT) LTR41_Init(PTLTR41 hnd);
LTR41API_DllExport (INT) LTR41_Open(PTLTR41 hnd, INT net_addr, WORD net_port, CHAR *crate_sn, INT slot_num);
LTR41API_DllExport (INT) LTR41_IsOpened(PTLTR41 hnd);       
LTR41API_DllExport (INT) LTR41_Close(PTLTR41 hnd);
LTR41API_DllExport (INT) LTR41_ReadPort(PTLTR41 hnd, WORD *InputData);
LTR41API_DllExport (INT) LTR41_StartStreamRead(PTLTR41 hnd); 
LTR41API_DllExport (INT) LTR41_StopStreamRead(PTLTR41 hnd); 
LTR41API_DllExport (INT) LTR41_Recv(PTLTR41 hnd, DWORD *data, DWORD *tmark, DWORD size, DWORD timeout); 
LTR41API_DllExport (INT) LTR41_ProcessData(PTLTR41 hnd, DWORD *src, WORD *dest, DWORD *size);   
LTR41API_DllExport (INT) LTR41_Config(PTLTR41 hnd);
LTR41API_DllExport (INT) LTR41_StartSecondMark(PTLTR41 hnd);  
LTR41API_DllExport (INT) LTR41_StopSecondMark(PTLTR41 hnd);  
LTR41API_DllExport (LPCSTR) LTR41_GetErrorString(INT Error_Code); 
LTR41API_DllExport (INT) LTR41_MakeStartMark(PTLTR41 hnd);
LTR41API_DllExport (INT) LTR41_WriteEEPROM(PTLTR41 hnd, INT Address, BYTE val); 
LTR41API_DllExport (INT) LTR41_ReadEEPROM(PTLTR41 hnd, INT Address, BYTE *val); 

 #ifdef __cplusplus 
 }
 
#endif
















