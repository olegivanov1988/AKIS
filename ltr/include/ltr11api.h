#ifndef LTR11API_H_
#define LTR11API_H_

#include <windows.h>
#include "ltr\\include\\ltrapi.h"

#ifdef __cplusplus
extern "C" {                                 // only need to export C interface if
                                             // used by C++ source code
#endif

/*================================================================================================*/
#define LTR11_CLOCK                  (15000) /* тактовая частота модуля в кГц */
#define LTR11_MAX_CHANNEL            (32)    /* Максимальное число физических каналов */
#define LTR11_MAX_LCHANNEL           (128)   /* Максимальное число логических каналов */
#define LTR11_ADC_RANGEQNT           (4)     /* количество входных диапазонов АЦП */

/* Коды ошибок, возвращаемые функциями библиотеки */
#define LTR11_ERR_INVALID_DESCR        (-1000) /* указатель на описатель модуля равен NULL */
#define LTR11_ERR_INVALID_ADCMODE      (-1001) /* недопустимый режим запуска АЦП */
#define LTR11_ERR_INVALID_ADCLCHQNT    (-1002) /* недопустимое количество логических каналов */
#define LTR11_ERR_INVALID_ADCRATE      (-1003) /* недопустимое значение частоты дискретизации АЦП
                                                * модуля
                                                */
#define LTR11_ERR_INVALID_ADCSTROBE    (-1004) /* недопустимый источник тактовой частоты для АЦП */
#define LTR11_ERR_GETFRAME             (-1005) /* ошибка получения кадра данных с АЦП */
#define LTR11_ERR_GETCFG               (-1006) /* ошибка чтения конфигурации */
#define LTR11_ERR_CFGDATA              (-1007) /* ошибка при получении конфигурации модуля */
#define LTR11_ERR_CFGSIGNATURE         (-1008) /* неверное значение первого байта конфигурационной
                                                * записи модуля
                                                */
#define LTR11_ERR_CFGCRC               (-1009) /* неверная контрольная сумма конфигурационной
                                                * записи
                                                */
#define LTR11_ERR_INVALID_ARRPOINTER   (-1010) /* указатель на массив равен NULL */
#define LTR11_ERR_ADCDATA_CHNUM        (-1011) /* неверный номер канала в массиве данных от АЦП */
#define LTR11_ERR_INVALID_CRATESN      (-1012) /* указатель на строку с серийным номером крейта
                                                * равен NULL
                                                */
#define LTR11_ERR_INVALID_SLOTNUM      (-1013) /* недопустимый номер слота в крейте */
#define LTR11_ERR_NOACK                (-1014) /* нет подтверждения от модуля */
#define LTR11_ERR_MODULEID             (-1015) /* попытка открытия модуля, отличного от LTR11 */
#define LTR11_ERR_INVALIDACK           (-1016) /* неверное подтверждение от модуля */
#define LTR11_ERR_ADCDATA_SLOTNUM      (-1017) /* неверный номер слота в данных от АЦП */
#define LTR11_ERR_ADCDATA_CNT          (-1018) /* неверный счетчик пакетов в данных от АЦП */
#define LTR11_ERR_INVALID_STARTADCMODE (-1019) /* неверный режим старта сбора данных */

/* Режимы запуска АЦП */
#define LTR11_ADCMODE_ACQ            (0x00)  /* сбор данных */
#define LTR11_ADCMODE_TEST_U1P       (0x04)  /* подача тестового напряжения +U1 */
#define LTR11_ADCMODE_TEST_U1N       (0x05)  /* подача тестового напряжения -U1 */
#define LTR11_ADCMODE_TEST_U2N       (0x06)  /* подача тестового напряжения -U2 */
#define LTR11_ADCMODE_TEST_U2P       (0x07)  /* подача тестового напряжения +U2 */

/* Режим начала сбора данных модулем */
#define LTR11_STARTADCMODE_INT       (0)     /* внутренний старт (по команде хоста) */
#define LTR11_STARTADCMODE_EXTRISE   (1)     /* по фронту внешнего сигнала; */
#define LTR11_STARTADCMODE_EXTFALL   (2)     /* по спаду внешнего сигнала. */

/* Источник тактирования АЦП */
#define LTR11_INPMODE_EXTRISE        (0)     /* запуск преобразования по фронту внешнего сигнала */
#define LTR11_INPMODE_EXTFALL        (1)     /* запуск преобразования по спаду внешнего сигнала */
#define LTR11_INPMODE_INT            (2)     /* внутренний запуск АЦП */

/* Входные дипазоны каналов */
#define LTR11_CHGANE_10000MV         (0)     /* +-10 В (10000 мВ) */
#define LTR11_CHGANE_2500MV          (1)     /* +-2.5 В (2500 мВ) */
#define LTR11_CHGANE_625MV           (2)     /* +-0.625 В (625 мВ) */
#define LTR11_CHGANE_156MV           (3)     /* +-0.156 В (156 мВ) */

/* Режимы работы каналов */
#define LTR11_CHMODE_16CH            (0)     /* 16-канальный */
#define LTR11_CHMODE_32CH            (1)     /* 32-канальный */


#ifdef LTR11API_EXPORTS
  #define LTR11API_DllExport(type) __declspec(dllexport) type APIENTRY
#else
  #define LTR11API_DllExport(type) __declspec(dllimport) type APIENTRY
#endif
/*================================================================================================*/


/*================================================================================================*/
#pragma pack (4)
typedef struct
    {
    CHAR Name[16];                          /* название модуля (строка) */
    CHAR Serial[24];                        /* серийный номер модуля (строка) */

    WORD Ver;                               /* версия ПО модуля (младший байт - минорная,
                                             * старший - мажорная
                                             */
    CHAR Date[14];                          /* дата создания ПО (строка) */
    struct
        {
        double Offset;                      /* смещение нуля */
        double Gain;                        /* масштабный коэффициент */
        } CbrCoef[LTR11_ADC_RANGEQNT];      /* калибровочные коэффициенты для каждого диапазона */
    } TINFO_LTR11, *PTINFO_LTR11;           /* информация о модуле */

typedef struct
    {
    INT size;                               /* размер структуры в байтах */
    TLTR Channel;                           /* описатель канала связи с модулем */

    INT StartADCMode;                       /* режим начала сбора данных:
                                             * LTR11_STARTADCMODE_INT     - внутренний старт (по
                                             *                              команде хоста);
                                             * LTR11_STARTADCMODE_EXTRISE - по фронту внешнего
                                             *                              сигнала;
                                             * LTR11_STARTADCMODE_EXTFALL - по спаду внешнего
                                             *                              сигнала.
                                             */
    INT InpMode;                            /* режим ввода данных с АЦП
                                             *  LTR11_INPMODE_INT     - внутренний запуск АЦП
                                             *                          (частота задается AdcRate)
                                             *  LTR11_INPMODE_EXTRISE - по фронту внешнего сигнала
                                             *  LTR11_INPMODE_EXTFALL - по спаду внешнего сигнала
                                             */
    INT LChQnt;                             /* число активных логических каналов (размер кадра) */
    BYTE LChTbl[LTR11_MAX_LCHANNEL];        /* управляющая таблица с активными логическими каналами
                                             * структура одного байта таблицы: MsbGGMMCCCCLsb
                                             *   GG   - входной диапазон:
                                             *          0 - +-10 В;
                                             *          1 - +-2.5 В;
                                             *          2 - +-0.625 В;
                                             *          3 - +-0.156В;
                                             *   MM   - режим:
                                             *          0 - 16-канальный, каналы 1-16;
                                             *          1 - измерение собственного напряжения
                                             *              смещения нуля;
                                             *          2 - 32-канальный, каналы 1-16;
                                             *          3 - 32-канальный, каналы 17-32;
                                             *   CCCC - номер физического канала:
                                             *          0 - канал 1 (17);
                                             *          . . .
                                             *          15 - канал 16 (32).
                                             */
    INT ADCMode;                            /* режим сбора данных или тип тестового режима */
    struct
        {
        INT divider;                        /* делитель тактовой частоты модуля, значения:
                                             * 2..65535
                                             */
        INT prescaler;                      /* пределитель тактовой частоты модуля:
                                             * 1, 8, 64, 256, 1024
                                             */
        } ADCRate;                          /* параметры для задания частоты дискретизации АЦП
                                             * частота рассчитывается по формуле:
                                             * F = LTR11_CLOCK/(prescaler*(divider+1)
                                             * ВНИМАНИЕ!!! Частота 400 кГц является особым случаем:
                                             * для ее установки пределитель и делитель должны иметь
                                             * следующие значения:
                                             *   prescaler = 1
                                             *   divider   = 36
                                             */
    double ChRate;                          /* частота одного канала в кГц (период кадра) при
                                             * внутреннем запуске АЦП
                                             */
    TINFO_LTR11 ModuleInfo;                 /* информация о модуле LTR11 */
    } TLTR11, *PTLTR11;                     /* описатель состояния модуля LTR11 и указатель на
                                             * него
                                             */
#pragma pack ()
/*================================================================================================*/

/*================================================================================================*/
LTR11API_DllExport(INT) LTR11_Close(PTLTR11 hnd);
LTR11API_DllExport(INT) LTR11_GetConfig(PTLTR11 hnd);
LTR11API_DllExport(LPCSTR) LTR11_GetErrorString(INT err);
LTR11API_DllExport(INT) LTR11_GetFrame(PTLTR11 hnd, DWORD *buf);
LTR11API_DllExport(INT) LTR11_Init(PTLTR11 hnd);
LTR11API_DllExport(INT) LTR11_Open(PTLTR11 hnd, DWORD net_addr, WORD net_port, CHAR *crate_sn,
    INT slot_num);
LTR11API_DllExport(INT) LTR11_ProcessData(PTLTR11 hnd, DWORD *src, double *dest, INT *size,
    BOOL calibr, BOOL volt);
LTR11API_DllExport(INT) LTR11_SetADC(PTLTR11 hnd);
LTR11API_DllExport(INT) LTR11_Start(PTLTR11 hnd);
LTR11API_DllExport(INT) LTR11_Stop(PTLTR11 hnd);
/*================================================================================================*/

#ifdef __cplusplus
}                                          // only need to export C interface if
                                           // used by C++ source code
#endif

#endif                      /* #ifndef LTR11API_H_ */
