#ifndef LTR_LABAPI_H_
#define LTR_LABAPI_H_
/*
хидер для модeлей под ltrlab
*/
#include "ltr\\include\\ltrlabapitypes.h"

/*===============================================================================================*/
#ifdef LTR_LABAPI_EXPORTS
 #define DLL_EXPORT(type) __declspec(dllexport) type APIENTRY  
#else
 #define DLL_EXPORT(type) __declspec(dllimport) type APIENTRY
#endif

#pragma pack(4)

typedef struct
{
	DWORD	size;
//---------------------------------------------------------
    LPVOID ltr_handle;
//---------------------------------------------------------
    DWORD	NetworkAddr;
    WORD	NetworkPort;
    CHAR	CrateSerial[16];
    INT		SlotNumber;

	CHAR	Name[16];                // название модуля (ASCIIZ-строка) 
    CHAR	SerialNumber[16];        // серийный номер (ASCIIZ-строка) 
    CHAR	FirmwareVersion[16];		// версия прошивки, типа 1.2.3  (ASCIIZ-строка) 
//--------------------------------------------------------------
	DWORD	NumChannels;				// общее количество каналов в модуле, как входных, так и выходных		
	TLTR_LAB_CHANNEL_CAP	Channels[LTR_LAB_MAX_CHANNELS];		
//---------------------------------------------------------------
	PVOID	pModuleInternalData;		// внутренние данные dll модуля
//---------------------------------------------------------------
    HANDLE	LogFile;						// файл отчета о выполнении теста 
    CHAR	LastErrorStr[255];

    DWORD	TerminateJob;
	DWORD	PauseJob;					// 
    INT		Counter;						// indicate progress
    INT		FatalError;					// fatal error flag 
    INT		CRCError;					// invalid data error counter 
} TLAB_LTR, *PTLAB_LTR;					// информация о модуле 
#pragma pack()
/*===============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
/*===============================================================================================*/
DLL_EXPORT(INT) LTRLab_Open  (PTLAB_LTR phLTR); // открываем устройство
DLL_EXPORT(INT) LTRLab_GetCap(PTLAB_LTR phLTR);	// возвращает параметры устройства, возможно и при неоткрытом устройстве, в таком случае возвращается количество каналов
DLL_EXPORT(INT) LTRLab_Config(PTLAB_LTR phLTR);	// конфигурит устройство и правит заданную конфигурацию
DLL_EXPORT(INT) LTRLab_Run   (PTLAB_LTR phLTR); // запуск сбора- выдачи данных
DLL_EXPORT(INT) LTRLab_Pause (PTLAB_LTR phLTR);	// Пауза сбора-выдачи данных
DLL_EXPORT(INT) LTRLab_Stop  (PTLAB_LTR phLTR); // останов сбора-выдачи данных
DLL_EXPORT(INT) LTRLab_Close (PTLAB_LTR phLTR); // закрываем устройство
/*===============================================================================================*/
#ifdef __cplusplus
}
#endif

#endif /*#ifndef LTR_LABAPI_H_*/