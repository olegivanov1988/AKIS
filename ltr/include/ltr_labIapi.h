#ifndef LTR_LABIAPI_H_
#define LTR_LABIAPI_H_
/*
����� ��� ������������ ��� ltrlab
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
	HANDLE	hDevice;					// ��������� �� ��� ����������
//--------------------------------------------------------------
	CHAR	Name[16];					// �������� ������ (ASCIIZ-������)     
    CHAR	Version[16];				// ������ ��������, ���� 1.2.3  (ASCIIZ-������) 
//--------------------------------------------------------------
	DWORD	NumChannels;				// ����� ���������� ������� � ������, ��� �������, ��� � ��������		
	TLTR_LAB_CHANNEL_CAP	Channels[LTR_LAB_I_MAX_CHANNELS];		
//---------------------------------------------------------------
	PVOID	pModuleInternalData;		// ���������� ������ dll ������
//---------------------------------------------------------------
    HANDLE	LogFile;					// ���� ������ � ����������
    CHAR	LastErrorStr[255];			//

    DWORD	TerminateJob;
	DWORD	PauseJob;					//     
}TLAB_I_LTR, *PTLAB_I_LTR;

#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif

/*===============================================================================================*/
DLL_EXPORT(INT) LTRLab_Open  (PTLAB_I_LTR phLTR); // ��������� ����������
DLL_EXPORT(INT) LTRLab_GetCap(PTLAB_I_LTR phLTR);	// ���������� ��������� ����������, �������� � ��� ���������� ����������, � ����� ������ ������������ ���������� �������
DLL_EXPORT(INT) LTRLab_Config(PTLAB_I_LTR phLTR);	// ���������� ���������� � ������ �������� ������������

DLL_EXPORT(INT) LTRLab_Show (PTLAB_I_LTR phLTR); // ��������
DLL_EXPORT(INT) LTRLab_Hide (PTLAB_I_LTR phLTR); // ������

DLL_EXPORT(INT) LTRLab_ShowPropertiesDialog (PTLAB_I_LTR phLTR); // �������� ���� �������

DLL_EXPORT(INT) LTRLab_Run(PTLAB_I_LTR phLTR); // ���������� � ������
DLL_EXPORT(INT) LTRLab_Stop(PTLAB_I_LTR phLTR); // ���������� � ������

DLL_EXPORT(INT) LTRLab_Close (PTLAB_I_LTR phLTR); // ��������� ����������


/*===============================================================================================*/

#ifdef __cplusplus
}
#endif

#endif //LTR_LABIAPI_H_