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

    CHAR   Name[16];                   /* название модуля (ASCIIZ-строка) */
    CHAR   SerialNumber[16];           /* серийный номер (ASCIIZ-строка) */
    CHAR   FirmwareVersion[16];        /* версия встроенного ПО (ASCIIZ-строка подобная следующей:
                                        * "1.3.5")
                                        */

    double inDataRateMax;              /* максимальная скорость входного потока данных в единицах
                                        * данных в секунду
                                        */
    double inDataRateMin;              /* минимальная скорость входного потока данных в единицах
                                        * данных в секунду
                                        */
    double outDataRateMax;             /* максимальная скорость выходного потока данных в единицах
                                        * данных в секунду
                                        */
    double outDataRateMin;             /* минимальная скорость входного потока данных в единицах
                                        * данных в секунду
                                        */

    double inDataRate;                 /* скорость входного потока в единицах данных в секунду */
    DWORD  inDataStepSize;             /* размер входных данных за один шаг работы */
    double outDataRate;                /* скорость выходного потока в единицах данных в секунду */
    DWORD  outDataStepSize;            /* размер выходных данных за один шаг работы */

    HANDLE LogFile;                    /* файл отчета о выполнении теста */
    CHAR   LastErrorStr[255];

    INT    TerminateJob;
    INT    Counter;                    /* indicate progress */
    INT    FatalError;                 /* fatal error flag */
    INT    CRCError;                   /* invalid data error counter */

} TLTR_HANDLE, *PTLTR_HANDLE;          /* информация о модуле */
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