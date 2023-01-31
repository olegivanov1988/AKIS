//-----------------------------------------------------------------------------
// crate controller LTR021
//-----------------------------------------------------------------------------
#ifndef __LTR021api__
#define __LTR021api__
#include "ltr\\include\\ltrapi.h"
#ifdef LTR021API_EXPORTS
  #define LTR021API_DllExport(type)   __declspec(dllexport) type APIENTRY  
#else
  #define LTR021API_DllExport(type)   __declspec(dllimport) type APIENTRY
#endif


#ifdef __cplusplus
extern "C" {
#endif
// коды ошибок
#define LTR021_OK                     (LTR_OK)    /*Выполнено без ошибок.*/
#define LTR021_ERROR_GET_ARRAY        (-200l)     /*Ошибка выполнения команды GET_ARRAY.*/
#define LTR021_ERROR_PUT_ARRAY        (-201l)     /*Ошибка выполнения команды PUT_ARRAY.*/
#define LTR021_ERROR_GET_MODULE_NAME  (-202l)     /*Ошибка выполнения команды GET_MODULE_NAME.*/
#define LTR021_ERROR_GET_MODULE_GESCR (-203l)     /*Ошибка выполнения команды GET_MODULE_DESCRIPTOR.*/
#define LTR021_ERROR_CRATE_TYPE       (-204l)     /*неверный тип крейта.*/
#define LTR021_ERROR_TIMEOUT		  (-205l)	  /*превышение таймаута */
//-----------------------------------------------------------------------------
typedef struct
{
    TDESCRIPTION_MODULE     Module;
    TDESCRIPTION_CPU        Cpu;
    TDESCRIPTION_FPGA       Fpga;
    TDESCRIPTION_INTERFACE  Interface;
} TDESCRIPTION_LTR021;
/*
типы меток LTR021
*/
typedef enum 
{
  LTR021_No_Sync=0,
  LTR021_Rising_Start,
  LTR021_Falling_Start,
  LTR021_Rising_Sync,
  LTR021_Falling_Sync
}_LTR021_Sync_Type;

#pragma pack(4)
//
typedef struct
{
    TLTR ltr;
} TLTR021;
#pragma pack()
//-----------------------------------------------------------------------------
LTR021API_DllExport(INT) LTR021_Init(TLTR021 *module);
LTR021API_DllExport(INT) LTR021_Open(TLTR021 *module, DWORD saddr, WORD sport, CHAR *csn);
LTR021API_DllExport(INT) LTR021_Close(TLTR021 *module);
LTR021API_DllExport(INT) LTR021_GetArray(TLTR021 *module, BYTE *buf, DWORD size, DWORD address);
LTR021API_DllExport(INT) LTR021_PutArray(TLTR021 *module, BYTE *buf, DWORD size, DWORD address);
LTR021API_DllExport(INT) LTR021_GetDescription(TLTR021 *module, TDESCRIPTION_LTR021 *description);
LTR021API_DllExport(INT) LTR021_SetCrateSyncType(TLTR021 *module, DWORD SyncType);
//
LTR021API_DllExport(LPCSTR) LTR021_GetErrorString(INT error);
#ifdef __cplusplus
}
#endif
#endif

