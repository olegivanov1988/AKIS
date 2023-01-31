#ifndef LTR_LABAPI_H_
#define LTR_LABAPI_H_
/*
����� ��� ���e��� ��� ltrlab
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

	CHAR	Name[16];                // �������� ������ (ASCIIZ-������) 
    CHAR	SerialNumber[16];        // �������� ����� (ASCIIZ-������) 
    CHAR	FirmwareVersion[16];		// ������ ��������, ���� 1.2.3  (ASCIIZ-������) 
//--------------------------------------------------------------
	DWORD	NumChannels;				// ����� ���������� ������� � ������, ��� �������, ��� � ��������		
	TLTR_LAB_CHANNEL_CAP	Channels[LTR_LAB_MAX_CHANNELS];		
//---------------------------------------------------------------
	PVOID	pModuleInternalData;		// ���������� ������ dll ������
//---------------------------------------------------------------
    HANDLE	LogFile;						// ���� ������ � ���������� ����� 
    CHAR	LastErrorStr[255];

    DWORD	TerminateJob;
	DWORD	PauseJob;					// 
    INT		Counter;						// indicate progress
    INT		FatalError;					// fatal error flag 
    INT		CRCError;					// invalid data error counter 
} TLAB_LTR, *PTLAB_LTR;					// ���������� � ������ 
#pragma pack()
/*===============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
/*===============================================================================================*/
DLL_EXPORT(INT) LTRLab_Open  (PTLAB_LTR phLTR); // ��������� ����������
DLL_EXPORT(INT) LTRLab_GetCap(PTLAB_LTR phLTR);	// ���������� ��������� ����������, �������� � ��� ���������� ����������, � ����� ������ ������������ ���������� �������
DLL_EXPORT(INT) LTRLab_Config(PTLAB_LTR phLTR);	// ���������� ���������� � ������ �������� ������������
DLL_EXPORT(INT) LTRLab_Run   (PTLAB_LTR phLTR); // ������ �����- ������ ������
DLL_EXPORT(INT) LTRLab_Pause (PTLAB_LTR phLTR);	// ����� �����-������ ������
DLL_EXPORT(INT) LTRLab_Stop  (PTLAB_LTR phLTR); // ������� �����-������ ������
DLL_EXPORT(INT) LTRLab_Close (PTLAB_LTR phLTR); // ��������� ����������
/*===============================================================================================*/
#ifdef __cplusplus
}
#endif

#endif /*#ifndef LTR_LABAPI_H_*/