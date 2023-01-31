//-----------------------------------------------------------------------------
// common part
//-----------------------------------------------------------------------------
#ifndef __ltrapi__
#define __ltrapi__
#include <windows.h>
#include "ltr\\include\\ltrapidefine.h"
#include "ltr\\include\\ltrapitypes.h"

#ifdef LTRAPIWIN_EXPORTS
  #define LTRAPIWIN_DllExport(type)   __declspec(dllexport) type APIENTRY 
#else
  #define LTRAPIWIN_DllExport(type)   __declspec(dllimport) type APIENTRY 
#endif


// ���� ������
#define LTR_OK                     ( 0l)     /*��������� ��� ������.*/
#define LTR_ERROR_UNKNOWN          (-1l)     /*����������� ������.*/
#define LTR_ERROR_PARAMETERS       (-2l)     /*������ ������� ����������.*/
#define LTR_ERROR_PARAMETRS        LTR_ERROR_PARAMETERS
#define LTR_ERROR_MEMORY_ALLOC     (-3l)     /*������ ������������� ��������� ������.*/
#define LTR_ERROR_OPEN_CHANNEL     (-4l)     /*������ �������� ������ ������ � ��������.*/
#define LTR_ERROR_OPEN_SOCKET      (-5l)     /*������ �������� ������.*/
#define LTR_ERROR_CHANNEL_CLOSED   (-6l)     /*������. ����� ������ � �������� �� ������.*/
#define LTR_ERROR_SEND             (-7l)     /*������ ����������� ������.*/
#define LTR_ERROR_RECV             (-8l)     /*������ ������ ������.*/
#define LTR_ERROR_EXECUTE          (-9l)     /*������ ������ � �����-������������.*/
#define LTR_WARNING_MODULE_IN_USE  (-10l)    /*����� ������ � �������� ������ � ������� ���������
                                                � � ����� - �� ���*/
#define LTR_ERROR_NOT_CTRL_CHANNEL (-11l)    /* ����� ������ ��� ���� �������� ������ ���� CC_CONTROL */

#define LTR_DEFAULT_SEND_RECV_TIMEOUT   10000UL

// �������� ��� ���������� ������� ����������, ���������� ��� ����������������� ����������������
enum en_LTR_UserIoCfg
    {
    LTR_USERIO_DIGIN1   = 1,    // ����� �������� ������ � ���������� � DIGIN1
    LTR_USERIO_DIGIN2   = 2,    // ����� �������� ������ � ���������� � DIGIN2
    LTR_USERIO_DIGOUT   = 0,    // ����� �������� ������� (����������� ��. en_LTR_DigOutCfg)
    LTR_USERIO_DEFAULT  = LTR_USERIO_DIGOUT
    };

// �������� ��� ���������� �������� DIGOUTx
enum en_LTR_DigOutCfg
    {
    LTR_DIGOUT_CONST0   = 0x00, // ���������� ������� ����������� "0"
    LTR_DIGOUT_CONST1   = 0x01, // ���������� ������� ���������� "1"
    LTR_DIGOUT_USERIO0  = 0x02, // ����� ��������� � ����� userio0 (PF1 � ���. 0, PF1 � ���. 1)
    LTR_DIGOUT_USERIO1  = 0x03, // ����� ��������� � ����� userio1 (PG13)
    LTR_DIGOUT_DIGIN1   = 0x04, // ����� ��������� �� ����� DIGIN1
    LTR_DIGOUT_DIGIN2   = 0x05, // ����� ��������� �� ����� DIGIN2
    LTR_DIGOUT_START    = 0x06, // �� ����� �������� ����� "�����"
    LTR_DIGOUT_SECOND   = 0x07, // �� ����� �������� ����� "�������"
    LTR_DIGOUT_DEFAULT  = LTR_DIGOUT_CONST0
    };

// �������� ��� ���������� ������� "�����" � "�������"
enum en_LTR_MarkMode
    {
    LTR_MARK_OFF                = 0x00, // ����� ���������
    LTR_MARK_EXT_DIGIN1_RISE    = 0x01, // ����� �� ������ DIGIN1
    LTR_MARK_EXT_DIGIN1_FALL    = 0x02, // ����� �� ����� DIGIN1
    LTR_MARK_EXT_DIGIN2_RISE    = 0x03, // ����� �� ������ DIGIN2
    LTR_MARK_EXT_DIGIN2_FALL    = 0x04, // ����� �� ����� DIGIN2
    LTR_MARK_INTERNAL           = 0x05  // ���������� ��������� �����
    };

//-----------------------------------------------------------------------------
#pragma pack(4)

typedef struct 
    {
    DWORD saddr;                       // ������� ����� �������
    WORD  sport;                       // ������� ���� �������
    CHAR  csn[16];                     // �������� ����� ������
    WORD  cc;                          // ����� ������ ������
    DWORD flags;                       // ����� ��������� ������
    DWORD tmark;                       // ��������� �������� ����� �������
    //                                 //
    LPVOID internal;                   // ��������� �� �����
    }
    TLTR;

// ��������� ������������ LTR-EU
typedef struct
    {
    // ��������� ����� ����������
    // [0] PF1 � ���. 0, PF1 � ���. 1+
    // [1] PG13
    // [2] PF3, ������ ���. 1+, ������ ����
    // [3] ������
    WORD        userio[4];          // ���� �� �������� LTR_USERIO_...

    // ��������� ������� DIGOUTx
    WORD        digout[2];          // ������������ ������� (LTR_DIGOUT_...)
    WORD        digout_en;          // ���������� ������� DIGOUT1, DIGOUT2
    }
    TLTR_CONFIG;

#pragma pack()
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

// -- ������� ��� ����������� ����������
LTRAPIWIN_DllExport(INT) LTR__GenericCtlFunc
    (
    TLTR* ltr,                          // ���������� LTR
    LPCVOID request_buf,                // ����� � ��������
    DWORD request_size,                 // ����� ������� (� ������)
    LPVOID reply_buf,                   // ����� ��� ������ (��� NULL)
    DWORD reply_size,                   // ����� ������ (� ������)
    INT ack_error_code,                 // ����� ������, ���� ack �� GOOD (0 = �� ������������ ack)
    DWORD timeout                       // �������, ��
    );

// -- ������� ������ ����������

// ������������� ����������� ��������� TLTR
LTRAPIWIN_DllExport(INT)    LTR_Init(TLTR *ltr);
// �������� ���������� � �������
LTRAPIWIN_DllExport(INT)    LTR_Open(TLTR *ltr);
// �������� ���������� � �������
LTRAPIWIN_DllExport(INT)    LTR_Close(TLTR *ltr);
// ��������, ����������� �� ���������� � �������
LTRAPIWIN_DllExport(INT)    LTR_IsOpened(TLTR *ltr);
// ��������� ���������� ��������� �� ������ �� �� ����
LTRAPIWIN_DllExport(LPCSTR) LTR_GetErrorString(INT error);

// -- ������� ������� ������

// ����� ������ �� ������
LTRAPIWIN_DllExport(INT)    LTR_Recv(TLTR *ltr, DWORD *data, DWORD *tmark, DWORD size, DWORD timeout);
// ������� ������ � �����
LTRAPIWIN_DllExport(INT)    LTR_Send(TLTR *ltr, DWORD *data, DWORD size, DWORD timeout);

// -- �������, ���������� � ����������� ������� �����-����������� (������ ���� ������ CC_CONTROL)

// ��������� ������ �������, ������������ � �������
LTRAPIWIN_DllExport(INT)    LTR_GetCrates(TLTR *ltr, BYTE *csn);
// ��������� ������ ������� � ������
LTRAPIWIN_DllExport(INT)    LTR_GetCrateModules(TLTR *ltr, WORD *mid);
// ���������� ����������� �������� ltrserver.exe
LTRAPIWIN_DllExport(INT)    LTR_SetServerProcessPriority(TLTR *ltr, DWORD Priority);
// ������ �������� ������ (������, ��� ����������)
LTRAPIWIN_DllExport(INT)    LTR_GetCrateInfo(TLTR *ltr, TCRATE_INFO * CrateInfo);
// ��������� ����� ������ �� ������ (���� ��� ���������� CC_RAW_DATA_FLAG)
LTRAPIWIN_DllExport(INT)    LTR_GetCrateRawData(TLTR *ltr, DWORD * data, DWORD * tmark, DWORD size, DWORD timeout);
// ������� ������������ �������� ������ � ������� ������
LTRAPIWIN_DllExport(INT)    LTR_Config(TLTR *ltr, const TLTR_CONFIG *conf);
// ��������� ���������� ��������� ��������� �����
LTRAPIWIN_DllExport(INT)    LTR_StartSecondMark(TLTR *ltr, enum en_LTR_MarkMode mode);
// ���������� ���������� ��������� ��������� �����
LTRAPIWIN_DllExport(INT)    LTR_StopSecondMark(TLTR *ltr);
// ������ ����� ����� (����������)
LTRAPIWIN_DllExport(INT)    LTR_MakeStartMark(TLTR *ltr, enum en_LTR_MarkMode mode);
// ������ ���������� �������� ltrserver.exe
LTRAPIWIN_DllExport(INT)    LTR_GetServerProcessPriority(TLTR *ltr, DWORD* Priority);

// -- ������� ���������� �������� (�������� �� ������������ ������,
//    � �.�. ��� ������ ����� ������ �������� ����� CSN_SERVER_CONTROL

// ������ ������ ������ ������� (DWORD, 0x01020304 = 1.2.3.4)
LTRAPIWIN_DllExport(INT)    LTR_GetServerVersion(TLTR *ltr, DWORD *version);

// ��������� ������ ��������� ������� IP-������� (������������ � �� ������������)
// ������������� ����������� ���� ������� "���������� IP-��������".
LTRAPIWIN_DllExport(INT)    LTR_GetListOfIPCrates(TLTR *ltr,
    DWORD max_entries, DWORD ip_net, DWORD ip_mask,
    DWORD *entries_found, DWORD *entries_returned,
    TIPCRATE_ENTRY *info_array);

// ���������� IP-������ ������ � ������� (� ����������� � ����� ������������).
// ���� ����� ��� ����������, ���������� ������.
LTRAPIWIN_DllExport(INT)    LTR_AddIPCrate(TLTR *ltr, DWORD ip_addr, DWORD flags, BOOL permanent);

// �������� IP-������ ������ �� ������� (� �� ����� ������������, ���� �� �����������).
// ���� ������ ���, ���� ����� �����, ���������� ������.
LTRAPIWIN_DllExport(INT)    LTR_DeleteIPCrate(TLTR *ltr, DWORD ip_addr, BOOL permanent);

// ������������ ���������� � IP-������� (����� ������ ���� � �������)
// ���� ������ ���, ���������� ������. ���� ����� ��� ��������� ��� ���� ������� �����������,
// ���������� LTR_OK.
// ���� ��������� LTR_OK, ��� ������, ��� ���������� ������� ���������� ����������.
// ��������� ����� ��������� ����� �� LTR_GetCrates() ��� LTR_GetListOfIPCrates()
LTRAPIWIN_DllExport(INT)    LTR_ConnectIPCrate(TLTR *ltr, DWORD ip_addr);

// ���������� ���������� � IP-������� (����� ������ ���� � �������)
// ���� ������ ���, ���������� ������. ���� ����� ��� ��������, ���������� LTR_OK.
// ���� ��������� LTR_OK, ��� ������, ��� ���������� ������� ��������� ����������.
// ��������� ����� ��������� ����� �� LTR_GetCrates() ��� LTR_GetListOfIPCrates()
LTRAPIWIN_DllExport(INT)    LTR_DisconnectIPCrate(TLTR *ltr, DWORD ip_addr);

// ������������ ���������� �� ����� IP-��������, �������� ���� "���������������"
// ���� ��������� LTR_OK, ��� ������, ��� ���������� ������� ���������� ����������.
// ��������� ����� ��������� ����� �� LTR_GetCrates() ��� LTR_GetListOfIPCrates()
LTRAPIWIN_DllExport(INT)    LTR_ConnectAllAutoIPCrates(TLTR *ltr);

// ���������� ���������� �� ����� IP-��������
// ���� ��������� LTR_OK, ��� ������, ��� ���������� ������� ��������� ����������.
// ��������� ����� ��������� ����� �� LTR_GetCrates() ��� LTR_GetListOfIPCrates()
LTRAPIWIN_DllExport(INT)    LTR_DisconnectAllIPCrates(TLTR *ltr);

// ��������� ������ ��� IP-������
LTRAPIWIN_DllExport(INT)    LTR_SetIPCrateFlags(TLTR *ltr, DWORD ip_addr, DWORD new_flags, BOOL permanent);

// ������ ������ ������ IP-������� � ��������� ����
LTRAPIWIN_DllExport(INT)    LTR_GetIPCrateDiscoveryMode(TLTR *ltr, BOOL *enabled, BOOL *autoconnect);

// ��������� ������ ������ IP-������� � ��������� ����
LTRAPIWIN_DllExport(INT)    LTR_SetIPCrateDiscoveryMode(TLTR *ltr, BOOL enabled, BOOL autoconnect, BOOL permanent);

// ������ ������ ������������
LTRAPIWIN_DllExport(INT)    LTR_GetLogLevel(TLTR *ltr, INT *level);

// ��������� ������ ������������
LTRAPIWIN_DllExport(INT)    LTR_SetLogLevel(TLTR *ltr, INT level, BOOL permanent);

// ������� ��������� ltrserver (� �������� ���� ����������).
// ����� ��������� ���������� ������� ����� � �������� ��������.
LTRAPIWIN_DllExport(INT)    LTR_ServerRestart(TLTR *ltr);

// ������� ��������� ltrserver (� �������� ���� ����������)
// ����� ��������� ���������� ������� ����� � �������� ��������.
LTRAPIWIN_DllExport(INT)    LTR_ServerShutdown(TLTR *ltr);

// ��������� �������� �� ��������� ��� send/recv
// ������������ � LTR_Send � LTR_Recv, ���� ����� ������� 0,
// � ����� �� ���� �������� ������������ ������.
LTRAPIWIN_DllExport(INT)    LTR_SetTimeout(TLTR *ltr, DWORD ms);

#ifdef __cplusplus
}
#endif
#endif


