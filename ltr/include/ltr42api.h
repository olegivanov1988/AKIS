
/*#include "ltr\\include\\ltrapidefine.h" 
#include "ltr\\include\\ltr010api.h"   */

#include "ltr\\include\\ltrapi.h"  



#ifdef LTR42API_EXPORTS
#define LTR42API_DllExport(type)   __declspec(dllexport) type APIENTRY  
#else
#define LTR42API_DllExport(type)   __declspec(dllimport) type APIENTRY
#endif

#define LTR42_NO_ERR                                  (0)
#define LTR42_ERR_WRONG_MODULE_DESCR				  (-8001)
#define LTR42_ERR_CANT_OPEN                           (-8002)
#define LTR42_ERR_INVALID_CRATE_SN 			          (-8003)
#define LTR42_ERR_INVALID_SLOT_NUM					  (-8004)
#define LTR42_ERR_CANT_SEND_COMMAND 				  (-8005)
#define LTR42_ERR_CANT_RESET_MODULE				      (-8006)
#define LTR42_ERR_MODULE_NO_RESPONCE				  (-8007)
#define LTR42_ERR_CANT_SEND_DATA					  (-8008)
#define LTR42_ERR_CANT_CONFIG                         (-8009) 
#define LTR42_ERR_CANT_LAUNCH_SEC_MARK				  (-8010)
#define LTR42_ERR_CANT_STOP_SEC_MARK				  (-8011)
#define LTR42_ERR_CANT_LAUNCH_START_MARK			  (-8012)
#define LTR42_ERR_DATA_TRANSMISSON_ERROR			  (-8013)
#define LTR42_ERR_LESS_WORDS_RECEIVED            	  (-8014)
#define LTR42_ERR_PARITY_TO_MODULE              	  (-8015) 
#define LTR42_ERR_PARITY_FROM_MODULE              	  (-8016)
#define LTR42_ERR_WRONG_SECOND_MARK_CONF			  (-8017)
#define LTR42_ERR_WRONG_START_MARK_CONF				  (-8018)
#define LTR42_ERR_CANT_READ_DATA 					  (-8019)
#define LTR42_ERR_CANT_WRITE_EEPROM					  (-8020)
#define LTR42_ERR_CANT_READ_EEPROM					  (-8021) 
#define LTR42_ERR_WRONG_EEPROM_ADDR 				  (-8022)
#define LTR42_ERR_CANT_READ_CONF_REC				  (-8023)
#define LTR42_ERR_WRONG_CONF_REC                      (-8024)

	/* Структура описания модуля */

#pragma pack(4) 

	typedef struct 
	{
		CHAR Name[16];
		CHAR Serial[24];
		CHAR FirmwareVersion[8];// Версия БИОСа
		CHAR FirmwareDate[16];  // Дата создания данной версии БИОСа
	} TINFO_LTR42,*PTINFO_LTR42; 

	typedef struct
	{
		TLTR Channel;
		INT size;   // размер структуры
		BOOLEAN AckEna;

		struct
		{
			INT SecondMark_Mode; // Режим меток. 0 - внутр., 1-внутр.+выход, 2-внешн
			INT StartMark_Mode; // 
		} Marks;  // Структура для работы с временными метками

		TINFO_LTR42 ModuleInfo;

	} TLTR42, *PTLTR42; // Структура описания модуля

#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif
	LTR42API_DllExport(INT) LTR42_Init(PTLTR42 hnd);
	LTR42API_DllExport(INT) LTR42_Open(PTLTR42 hnd, INT net_addr, WORD net_port, CHAR *crate_sn, INT slot_num);
	LTR42API_DllExport(INT) LTR42_Close(PTLTR42 hnd);
	LTR42API_DllExport(INT) LTR42_WritePort(PTLTR42 hnd, WORD OutputData);
	LTR42API_DllExport(INT) LTR42_WriteArray(PTLTR42 hnd, WORD *OutputArray, int ArraySize);  
	LTR42API_DllExport(INT) LTR42_Config(PTLTR42 hnd);
	LTR42API_DllExport(INT) LTR42_StartSecondMark(PTLTR42 hnd);  
	LTR42API_DllExport(INT) LTR42_StopSecondMark(PTLTR42 hnd);  
	LTR42API_DllExport(LPCSTR) LTR42_GetErrorString(INT Error_Code); 
	LTR42API_DllExport(INT) LTR42_MakeStartMark(PTLTR42 hnd);
	LTR42API_DllExport(INT) LTR42_WriteEEPROM(PTLTR42 hnd, INT Address, BYTE val); 
	LTR42API_DllExport(INT) LTR42_ReadEEPROM(PTLTR42 hnd, INT Address, BYTE *val); 
	LTR42API_DllExport(INT) LTR42_IsOpened(PTLTR42 hnd);

#ifdef __cplusplus 
}

#endif
















