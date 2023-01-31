   
/*#include "ltr\\include\\ltrapidefine.h" 
#include "ltr\\include\\ltr010api.h"  */


#include "ltr\\include\\ltrapi.h"  
    

#define RISING_EDGE              (0)
#define FALLING_EDGE             (1)


#ifdef LTR51API_EXPORTS
  #define LTR51API_DllExport(type)   __declspec(dllexport) type APIENTRY  
#else
  #define LTR51API_DllExport(type)   __declspec(dllimport) type APIENTRY
#endif

// Коды ошибок
#define LTR51_NO_ERR                                  (0)
#define LTR51_ERR_WRONG_MODULE_DESCR				  (-5001)
#define LTR51_ERR_CANT_OPEN                           (-5002)
#define LTR51_ERR_CANT_LOAD_ALTERA                    (-5003)   
#define LTR51_ERR_INVALID_CRATE_SN 			          (-5004)
#define LTR51_ERR_INVALID_SLOT_NUM					  (-5005)
#define LTR51_ERR_CANT_SEND_COMMAND 				  (-5006)
#define LTR51_ERR_CANT_RESET_MODULE				      (-5007)
#define LTR51_ERR_MODULE_NO_RESPONCE				  (-5008)
#define LTR51_ERR_CANT_OPEN_MODULE					  (-5009)
#define LTR51_ERR_PARITY_TO_MODULE              	  (-5010) 
#define LTR51_ERR_PARITY_FROM_MODULE              	  (-5011) 
#define LTR51_ERR_ALTERA_TEST_FAILED				  (-5012)
#define LTR51_ERR_CANT_START_DATA_AQC				  (-5013)
#define LTR51_ERR_CANT_STOP_DATA_AQC				  (-5014) 
#define LTR51_ERR_CANT_SET_FS                         (-5015) 
#define LTR51_ERR_CANT_SET_BASE                       (-5016)
#define LTR51_ERR_CANT_SET_EDGE_MODE                  (-5017)
#define LTR51_ERR_CANT_SET_THRESHOLD                  (-5018)
#define LTR51_WRONG_DATA							  (-5019)
#define LTR51_ERR_WRONG_HIGH_THRESOLD_SETTINGS		  (-5020)
#define LTR51_ERR_WRONG_LOW_THRESOLD_SETTINGS		  (-5021)
#define LTR51_ERR_WRONG_FPGA_FILE					  (-5022)
#define LTR51_ERR_CANT_READ_ID_REC 				      (-5023)
#define LTR51_ERR_WRONG_ID_REC					      (-5024)
#define LTR51_ERR_WRONG_FS_SETTINGS	         		  (-5025)
#define LTR51_ERR_WRONG_BASE_SETTINGS	        	  (-5026)  
#define LTR51_ERR_CANT_WRITE_EEPROM					  (-5027)
#define LTR51_ERR_CANT_READ_EEPROM					  (-5028) 
#define LTR51_ERR_WRONG_EEPROM_ADDR 				  (-5029)
#define LTR51_ERR_WRONG_THRESHOLD_VALUES 		      (-5030)  
#define LTR51_ERR_ERROR_OVERFLOW	                  (-5031)
#define LTR51_ERR_MODULE_WRONG_ACQ_TIME_SETTINGS	  (-5032)
#define LTR51_ERR_NOT_ENOUGH_POINTS					  (-5033)
#define LTR51_ERR_WRONG_SRC_SIZE					  (-5034)


#ifdef __cplusplus
extern "C" {
 #endif

#define TIMEOUT_CMD_SEND							   (4000)
#define TIMEOUT_CMD_RECIEVE							   (6000)


/* Структура описания модуля */

#pragma pack(4) 

typedef struct 
{
	CHAR Name[16];
	CHAR Serial[24];
	CHAR FirmwareVersion[8];// Версия прошивки AVR
    CHAR FirmwareDate[16];  // Дата создания данной версии прошивки AVR  
    CHAR FPGA_Version[8]; // Версия прошивки ПЛИС
	

} TINFO_LTR51,*PTINFO_LTR51; 

typedef struct
  {
   										   
   INT size;               // размер структуры
   
   TLTR Channel;  
   WORD ChannelsEna;       // Маска доступных каналов (показывает, какие субмодули подкл.)
   
   INT  SetUserPars;	   // Указывает, задаются ли Fs и Base пользователем
   
   INT LChQnt;             // Количество логических каналов    
   DWORD LChTbl[16];       // Таблица логических каналов
   
   double Fs;                // Частота выборки сэмплов
   WORD Base;                // Делитель частоты измерения
   double F_Base;			 // Частота измерений F_Base=Fs/Base
   
   
   int AcqTime;            // Время сбора в миллисекундах       
   int TbaseQnt;		   // Количество периодов измерений, необходимое для обеспечения указанного интревала измерения
   
   TINFO_LTR51 ModuleInfo;
   
    
  } TLTR51, *PTLTR51; // Структура описания модуля

#pragma pack()


LTR51API_DllExport (INT) LTR51_Init(PTLTR51 hnd);
LTR51API_DllExport (INT) LTR51_Open(PTLTR51 hnd, DWORD net_addr, WORD net_port, CHAR *crate_sn, INT slot_num, char *ttf_name);
LTR51API_DllExport (INT) LTR51_IsOpened(PTLTR51 hnd);
LTR51API_DllExport (INT) LTR51_Close(PTLTR51 hnd);
LTR51API_DllExport (LPCSTR) LTR51_GetErrorString(INT Error_Code); 
LTR51API_DllExport (INT) LTR51_WriteEEPROM(PTLTR51 hnd, INT Address, BYTE val); 
LTR51API_DllExport (INT) LTR51_ReadEEPROM(PTLTR51 hnd, INT Address, BYTE *val);
LTR51API_DllExport (DWORD) LTR51_CreateLChannel(INT PhysChannel, double *HighThreshold, double *LowThreshold, INT ThresholdRange, int EdgeMode); 
LTR51API_DllExport (INT) LTR51_Config(PTLTR51 hnd);
LTR51API_DllExport (INT) LTR51_Start(PTLTR51 hnd);
LTR51API_DllExport (INT) LTR51_Stop(PTLTR51 hnd);
LTR51API_DllExport (INT) LTR51_Recv(PTLTR51 hnd, DWORD *data, DWORD *tmark, DWORD size, DWORD timeout);      
LTR51API_DllExport (INT) LTR51_ProcessData(PTLTR51 hnd, DWORD *src, DWORD *dest, double *Frequency, DWORD *size);   
LTR51API_DllExport (INT) LTR51_GetThresholdVals(PTLTR51 hnd, INT LChNumber, double *HighThreshold, double *LowThreshold, INT ThresholdRange);
LTR51API_DllExport (DWORD) LTR51_CalcTimeOut(PTLTR51 hnd, INT n);
INT LTR51_EvaluateFrequencies(PTLTR51 hnd);


#ifdef __cplusplus 
 }
#endif  

















