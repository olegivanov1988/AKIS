//---------------------------------------------------------------------------
// boot loader programming part
//---------------------------------------------------------------------------
#ifndef __ltrbootapi__
#define __ltrbootapi__
#include "ltr\\include\\ltr010api.h"
#ifdef __cplusplus
extern "C" {
#endif
// ���� ������
#define LTRBOOT_OK                     (LTR_OK)     /*��������� ��� ������.*/
#define LTRBOOT_ERROR_GET_ARRAY        (-300l)      /*������ ���������� ������� GET_ARRAY.*/
#define LTRBOOT_ERROR_PUT_ARRAY        (-301l)      /*������ ���������� ������� PUT_ARRAY.*/
#define LTRBOOT_ERROR_CALL_APPL        (-302l)      /*������ ���������� ������� CALL_APPLICATION.*/
#define LTRBOOT_ERROR_GET_DESCRIPTION  (-303l)      /*������ ���������� ������� GET_DESCRIPTION.*/
#define LTRBOOT_ERROR_PUT_DESCRIPTION  (-304l)      /*������ ���������� ������� PUT_DESCRIPTION.*/
//-----------------------------------------------------------------------------
EXPORTCALL(INT) LTRBOOT_Init(TLTR010 *module);
EXPORTCALL(INT) LTRBOOT_Open(TLTR010 *module, DWORD saddr, WORD sport, CHAR *csn);
EXPORTCALL(INT) LTRBOOT_Close(TLTR010 *module);
EXPORTCALL(INT) LTRBOOT_GetArray(TLTR010 *module, BYTE *buf, DWORD size, DWORD address);
EXPORTCALL(INT) LTRBOOT_PutArray(TLTR010 *module, BYTE *buf, DWORD size, DWORD address);
EXPORTCALL(INT) LTRBOOT_GetDescription(TLTR010 *module, TDESCRIPTION_LTR010 *description);
EXPORTCALL(INT) LTRBOOT_SetDescription(TLTR010 *module, TDESCRIPTION_LTR010 *description);
EXPORTCALL(INT) LTRBOOT_CallApplication(TLTR010 *module, DWORD address);
//
EXPORTCALL(LPCSTR) LTRBOOT_GetErrorString(INT error);
#ifdef __cplusplus
}
#endif
#endif

