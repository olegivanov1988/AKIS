//-----------------------------------------------------------------------------
// crate controller ltr010
//-----------------------------------------------------------------------------
#ifndef __ltr010api__
#define __ltr010api__
#include "ltr\\include\\ltrapi.h"
#ifdef __cplusplus
extern "C" {
#endif
// ���� ������
#define LTR010_OK                     (LTR_OK)    /*��������� ��� ������.*/
#define LTR010_ERROR_GET_ARRAY        (-100l)     /*������ ���������� ������� GET_ARRAY.*/
#define LTR010_ERROR_PUT_ARRAY        (-101l)     /*������ ���������� ������� PUT_ARRAY.*/
#define LTR010_ERROR_GET_MODULE_NAME  (-102l)     /*������ ���������� ������� GET_MODULE_NAME.*/
#define LTR010_ERROR_GET_MODULE_DESCR (-103l)     /*������ ���������� ������� GET_MODULE_DESCRIPTOR.*/
#define LTR010_ERROR_INIT_FPGA        (-104l)     /*������ ���������� ������� INIT_FPGA.*/
#define LTR010_ERROR_RESET_FPGA       (-105l)     /*������ ���������� ������� RESET_FPGA.*/
#define LTR010_ERROR_INIT_DMAC        (-106l)     /*������ ���������� ������� INIT_DMAC.*/
#define LTR_ERROR_LOAD_FPGA           (-107l)     /*������ ���������� ������� LOAD_FPGA.*/
#define LTR010_ERROR_OPEN_FILE        (-108l)     /*������ �������� �����.*/
#define LTR010_ERROR_GET_INFO_FPGA    (-109l)     /*������ ���������� ������� GET_INFO_FPGA.*/
//-----------------------------------------------------------------------------
#pragma pack(4)
typedef struct
    {
    TDESCRIPTION_MODULE     Module;
    TDESCRIPTION_CPU        Cpu;
    TDESCRIPTION_FPGA       Fpga;
    TDESCRIPTION_INTERFACE  Interface;
    }
    TDESCRIPTION_LTR010;
//
typedef struct
    {
    TLTR ltr;
    }
    TLTR010;
#pragma pack()
//-----------------------------------------------------------------------------
EXPORTCALL(INT) LTR010_Init(TLTR010 *module);
EXPORTCALL(INT) LTR010_Open(TLTR010 *module, DWORD saddr, WORD sport, CHAR *csn);
EXPORTCALL(INT) LTR010_Close(TLTR010 *module);
EXPORTCALL(INT) LTR010_GetArray(TLTR010 *module, BYTE *buf, DWORD size, DWORD address);
EXPORTCALL(INT) LTR010_PutArray(TLTR010 *module, BYTE *buf, DWORD size, DWORD address);
EXPORTCALL(INT) LTR010_GetDescription(TLTR010 *module, TDESCRIPTION_LTR010 *description);
EXPORTCALL(INT) LTR010_LoadFPGA(TLTR010 *module, CHAR *fname, BYTE rdma, BYTE wdma);
//
EXPORTCALL(LPCSTR) LTR010_GetErrorString(INT error);
#ifdef __cplusplus
}
#endif
#endif

