#ifndef LTR_LABIAPI_H_
#define LTR_LABIAPI_H_
/*
хидер для инструментов под ltrlab
*/ 

#include "ltrlabapitypes.h"

/*===============================================================================================*/
#ifdef LTR_LABIAPI_EXPORTS
 #define DLL_EXPORT(type) __declspec(dllexport) type APIENTRY  
#else
 #define DLL_EXPORT(type) __declspec(dllimport) type APIENTRY
#endif

#define LTR_LAB_I_MAX_CHANNELS 256

#pragma pack(4)

typedef struct
{
	DWORD	size;
//--------------------------------------------------------------
	HANDLE	hDevice;					// указатель на сам инструмент
//--------------------------------------------------------------
	CHAR	Name[16];					// название модуля (ASCIIZ-строка)     
    CHAR	Version[16];				// версия прошивки, типа 1.2.3  (ASCIIZ-строка) 
//--------------------------------------------------------------
	DWORD	NumChannels;				// общее количество каналов в модуле, как входных, так и выходных		
	TLTR_LAB_CHANNEL_CAP	Channels[LTR_LAB_I_MAX_CHANNELS];		
//---------------------------------------------------------------
	PVOID	pModuleInternalData;		// внутренние данные dll модуля
//---------------------------------------------------------------
    HANDLE	LogFile;					// файл отчета о выполнении
    CHAR	LastErrorStr[255];			//

    DWORD	TerminateJob;
	DWORD	PauseJob;					//     
}TLAB_I_LTR, *PTLAB_I_LTR;

#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif

/*===============================================================================================*/
DLL_EXPORT(INT) LTRLab_Open  (PTLAB_I_LTR phLTR); // открываем устройство
DLL_EXPORT(INT) LTRLab_GetCap(PTLAB_I_LTR phLTR);	// возвращает параметры устройства, возможно и при неоткрытом устройстве, в таком случае возвращается количество каналов
DLL_EXPORT(INT) LTRLab_Config(PTLAB_I_LTR phLTR);	// конфигурит устройство и правит заданную конфигурацию

DLL_EXPORT(INT) LTRLab_Show (PTLAB_I_LTR phLTR); // показать
DLL_EXPORT(INT) LTRLab_Hide (PTLAB_I_LTR phLTR); // скрыть

DLL_EXPORT(INT) LTRLab_ShowPropertiesDialog (PTLAB_I_LTR phLTR); // показать окно свойств

DLL_EXPORT(INT) LTRLab_Run(PTLAB_I_LTR phLTR); // Подготовка к старту
DLL_EXPORT(INT) LTRLab_Stop(PTLAB_I_LTR phLTR); // Подготовка к старту

DLL_EXPORT(INT) LTRLab_Close (PTLAB_I_LTR phLTR); // закрываем устройство


/*===============================================================================================*/

#ifdef __cplusplus
}
#endif

#endif //LTR_LABIAPI_H_