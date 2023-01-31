#include "ltr\\include\\ltrapitypes.h"
#include "ltr\\include\\ltrapi.h"

#ifndef __LTR22API__
#define __LTR22API__

#ifdef LTR22API_EXPORTS
	#define LTR22API_DllExport(type)	__declspec(dllexport) type APIENTRY  
	#define LTR22API_Export				__declspec(dllexport)
#else
	#define LTR22API_DllExport(type)	__declspec(dllimport) type APIENTRY
	#define LTR22API_Export				__declspec(dllimport) 
#endif

#define LTR22_MODULE_CODE 0x1616 // 
#define LTR22_ADC_NUMBERS 4
#define LTR22_ADC_CHANNELS LTR22_ADC_NUMBERS
#define LTR22_RANGE_NUMBER 6
#define LTR22_RANGE_OVERFLOW 7

#define LTR22_MAX_DISC_FREQ_NUMBER 25	// количество выставляемых частот lbcrhtnbpfwbb

typedef BYTE byte;

#ifndef __cplusplus
	#define true 1
	#define false 0
	typedef byte bool;
#endif

#pragma pack(4)

typedef struct 
{	
	TDESCRIPTION_MODULE Description;	// описание модуля
	TDESCRIPTION_CPU CPU;				// описание AVR

} TINFO_LTR22, *PTINFO_LTR22;


typedef struct 
{
	float FactoryCalibOffset[LTR22_RANGE_NUMBER];
	float FactoryCalibScale[LTR22_RANGE_NUMBER];

	float UserCalibOffset[LTR22_RANGE_NUMBER];
	float UserCalibScale[LTR22_RANGE_NUMBER];

} ADC_CHANNEL_CALIBRATION;


//**** конфигурация модуля
typedef struct 
{                    
  //**** служебная информация       //
  INT size;							// размер структуры TLTR22 1036 байт
  TLTR Channel;                     // служебная структура для работы с крейтом через сервер

  // настройки модуля
  byte Fdiv_rg;						// дивайзер частоты клоков 1..15
  bool Adc384;						// дополнительный дивайзер частоты сэмплов true =3 false =2
  bool AC_DC_State;					// состояние true =AC+DC false=AC 
  bool MeasureADCZero;				// измерение Zero true - включено false - выключено
  bool DataReadingProcessed;		// состояние считывания АЦП true-АЦП считывается false - нет
  byte	ADCChannelRange[LTR22_ADC_NUMBERS];// предел имзерений АЦП по каналам 0 - 1В 1 - 0.3В 2 - 0.1В 3 - 0.03В 4 - 10В 5 - 3В    

  bool ChannelEnabled[LTR22_ADC_NUMBERS];		// Состояние каналов, включен - true выключен - false

  int FreqDiscretizationIndex;			// частота дискретизации, выставленная сейчас 0..24 - в зависимости от частоты
										// из массива LTR22_DISK_FREQ_ARRAY
   
  byte SyncType;		// Тип синхронизации 0 - внутренний старт по сигналу Go 
						//1 - фазировка модуля
						//2 - внешний старт
						//3 - резервировано  
  bool SyncMaster;		// true - модуль генерит сигнал, false - модуль принимает синхросигнал

  TINFO_LTR22 ModuleInfo;
  ADC_CHANNEL_CALIBRATION ADCCalibration[LTR22_ADC_NUMBERS][LTR22_MAX_DISC_FREQ_NUMBER];

} TLTR22,*PTLTR22;  

#pragma pack()

// возможные варианты ошибок
// константы
#define LTR22_ERROR_SEND_DATA					(-6000)
#define LTR22_ERROR_RECV_DATA					(-6001)
#define LTR22_ERROR_NOT_LTR22					(-6002)
#define LTR22_ERROR_OVERFLOW					(-6003)
#define LTR22_ERROR_CANNOT_DO_WHILE_ADC_RUNNING			(-6004)
#define LTR22_ERROR_MODULE_INTERFACE				(-6005)
#define LTR22_ERROR_INVALID_FREQ_DIV				(-6006)
#define LTR22_ERROR_INVALID_TEST_HARD_INTERFACE			(-6007)
#define LTR22_ERROR_INVALID_DATA_RANGE_FOR_THIS_CHANNEL		(-6008)
#define LTR22_ERROR_INVALID_DATA_COUNTER			(-6009)
#define LTR22_ERROR_PRERARE_TO_WRITE				(-6010)
#define LTR22_ERROR_WRITE_AVR_MEMORY				(-6011)
#define LTR22_ERROR_READ_AVR_MEMORY				(-6012)
#define LTR22_ERROR_PARAMETERS					(-6013)


/*
-----------------------------------------------------------------------------------------
 декларирование функций 
*/
#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

LTR22API_Export extern const int LTR22_DISK_FREQ_ARRAY[LTR22_MAX_DISC_FREQ_NUMBER];


LTR22API_DllExport(INT) LTR22_Init(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_Close(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_Open(TLTR22 *module, DWORD saddr, WORD sport, CHAR *csn, WORD cc);
LTR22API_DllExport(INT) LTR22_IsOpened(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_GetConfig(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_SetConfig(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_ClearBuffer(TLTR22 *module, bool wait_response);
LTR22API_DllExport(INT) LTR22_StartADC(TLTR22 *module, bool WaitSync);
LTR22API_DllExport(INT) LTR22_StopADC(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_SetSyncPriority(TLTR22 *module, bool SyncMaster);
LTR22API_DllExport(INT) LTR22_SyncPhaze(TLTR22 *module, DWORD timeout);
LTR22API_DllExport(INT) LTR22_SwitchMeasureADCZero(TLTR22 *module, bool SetMeasure);
LTR22API_DllExport(INT) LTR22_SetFreq(TLTR22 *module, bool adc384, byte Freq_dv);
LTR22API_DllExport(INT) LTR22_SwitchACDCState(TLTR22 *module, bool ACDCState);
LTR22API_DllExport(INT) LTR22_SetADCRange(TLTR22 *module, byte ADCChannel, byte ADCChannelRange);
LTR22API_DllExport(INT) LTR22_SetADCChannel(TLTR22 *module, byte ADCChannel, bool EnableADC);
LTR22API_DllExport(INT) LTR22_GetCalibrovka(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_Recv(TLTR22 *module, DWORD *data, DWORD *tstamp, DWORD size, DWORD timeout);
LTR22API_DllExport(INT) LTR22_GetModuleDescription(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_ProcessData(TLTR22 *module, DWORD *src_data, double *dst_data, 
										 DWORD size, bool calibrMainPset, bool calibrExtraVolts, byte * OverflowFlags);
LTR22API_DllExport(INT) LTR22_ProcessDataTest(TLTR22 *module, DWORD *src_data, double *dst_data, 
										 DWORD size, bool calibrMainPset, bool calibrExtraVolts, byte * OverflowFlags,
										 LPCWSTR FilePath);
LTR22API_DllExport(INT) LTR22_ReadAVREEPROM(TLTR22 *module, byte *Data, DWORD BeginAddress, DWORD size);
LTR22API_DllExport(INT) LTR22_WriteAVREEPROM(TLTR22 *module, byte *Data, DWORD BeginAddress, DWORD size);
	
/*
тестовые функции
*/
LTR22API_DllExport(INT) LTR22_TestHardwareInterface(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_GetADCData(TLTR22 *module, double * Data, DWORD Size, DWORD time, 
										bool calibrMainPset, bool calibrExtraVolts);
LTR22API_DllExport(INT) LTR22_ReopenModule(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_ReadAVRBroaching(TLTR22 *module, byte *Data, DWORD size, DWORD BeginPage, DWORD PageNumbers);


LTR22API_DllExport(INT) LTR22_WriteBroaching(TLTR22 *module, byte *Data, DWORD size,
											   bool WriteCalibrovkaAndDescription, bool ProgrammAVR);

LTR22API_DllExport(LPCSTR) LTR22_GetErrorString(int ErrorCode);
#ifdef __cplusplus
}
#endif


#endif
