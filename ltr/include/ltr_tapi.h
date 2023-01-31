#ifndef LTR_TAPI_H_
#define LTR_TAPI_H_


/*===============================================================================================*/
#ifndef LTR_TESTAPI_DLL 
 #define DLL_EXPORT(type) type
#else
 #define DLL_EXPORT(type) type __declspec(dllexport)
#endif
/*===============================================================================================*/

/*===============================================================================================*/
#pragma pack(4)

typedef struct
{
    LPVOID ltr_handle;

    DWORD  NetworkAddr;
    WORD   NetworkPort;
    CHAR   CrateSerial[16];
    INT    SlotNumber;

    CHAR   Name[16];                   /* �������� ������ (ASCIIZ-������) */
    CHAR   SerialNumber[16];           /* �������� ����� (ASCIIZ-������) */
    CHAR   FirmwareVersion[16];        /* ������ ����������� �� (ASCIIZ-������ �������� ���������:
                                        * "1.3.5")
                                        */

    double inDataRateMax;              /* ������������ �������� �������� ������ ������ � ��������
                                        * ������ � �������
                                        */
    double inDataRateMin;              /* ����������� �������� �������� ������ ������ � ��������
                                        * ������ � �������
                                        */
    double outDataRateMax;             /* ������������ �������� ��������� ������ ������ � ��������
                                        * ������ � �������
                                        */
    double outDataRateMin;             /* ����������� �������� �������� ������ ������ � ��������
                                        * ������ � �������
                                        */

    double inDataRate;                 /* �������� �������� ������ � �������� ������ � ������� */
    DWORD  inDataStepSize;             /* ������ ������� ������ �� ���� ��� ������ */
    double outDataRate;                /* �������� ��������� ������ � �������� ������ � ������� */
    DWORD  outDataStepSize;            /* ������ �������� ������ �� ���� ��� ������ */

    HANDLE LogFile;                    /* ���� ������ � ���������� ����� */
    CHAR   LastErrorStr[255];

    INT    TerminateJob;
    INT    Counter;                    /* indicate progress */
    INT    FatalError;                 /* fatal error flag */
    INT    CRCError;                   /* invalid data error counter */

} TLTR_HANDLE, *PTLTR_HANDLE;          /* ���������� � ������ */
#pragma pack()
/*===============================================================================================*/


/*===============================================================================================*/
#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

DLL_EXPORT(INT) LTRTest_Close (PTLTR_HANDLE phLTR);
DLL_EXPORT(INT) LTRTest_Open  (PTLTR_HANDLE phLTR);
DLL_EXPORT(INT) LTRTest_Run   (PTLTR_HANDLE phLTR);

#ifdef __cplusplus
}
#endif
/*===============================================================================================*/

#endif /*#ifndef LTR_TAPI_H_*/