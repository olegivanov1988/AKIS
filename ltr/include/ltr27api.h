#ifndef __LTR27API__
#define __LTR27API__

#ifdef LTR27APIWIN_EXPORTS
  #define LTR27API_DllExport(type)   __declspec(dllexport)  type APIENTRY 
#else
  #define LTR27API_DllExport(type)   __declspec(dllimport) type APIENTRY 
#endif

#include "ltr\\include\\ltrapitypes.h"
#include "ltr\\include\\ltrapi.h"
#ifdef __cplusplus
extern "C" {
#endif
// ���������
#define LTR27_ERROR_SEND_DATA				(-3000)
#define LTR27_ERROR_RECV_DATA				(-3001)
#define LTR27_ERROR_RESET_MODULE			(-3002)
#define LTR27_ERROR_NOT_LTR27				(-3003)
#define LTR27_ERROR_PARITY				(-3004)
#define LTR27_ERROR_OVERFLOW				(-3005)
#define LTR27_ERROR_INDEX				(-3006)
//
#define LTR27_ERROR					(-3007)
#define LTR27_ERROR_FORMAT				(-3008)
#define LTR27_ERROR_CRC					(-3010)
#define LTR27_ERROR_EXCHANGE_RECIEVE            	(-3011)
#define LTR27_ERROR_EXCHANGE_SEND	            	(-3012)
#define LTR27_ERROR_EXCHANGE_SIZE	            	(-3013)
#define LTR27_ERROR_EXCHANGE_PARAM	            	(-3014)
#define LTR27_ERROR_EXCHANGE_TIME	            	(-3015)
#define LTR27_ERROR_EXCHANGE_ECHO	            	(-3016)
//
#define LTR27_MEZZANINE_NUMBER     			8
//
#define LTR27_DATA_CORRECTION      			(1<<0)
#define LTR27_DATA_VALUE           			(1<<1)
// ������� ��� �������
#define LTR27_MODULE_DESCRIPTION         		(1<<0)
#define LTR27_MEZZANINE1_DESCRIPTION     		(1<<1)
#define LTR27_MEZZANINE2_DESCRIPTION     		(1<<2)
#define LTR27_MEZZANINE3_DESCRIPTION     		(1<<3)
#define LTR27_MEZZANINE4_DESCRIPTION     		(1<<4)
#define LTR27_MEZZANINE5_DESCRIPTION     		(1<<5)
#define LTR27_MEZZANINE6_DESCRIPTION     		(1<<6)
#define LTR27_MEZZANINE7_DESCRIPTION     		(1<<7)
#define LTR27_MEZZANINE8_DESCRIPTION     		(1<<8)
#define LTR27_ALL_MEZZANINE_DESCRIPTION \
(LTR27_MEZZANINE1_DESCRIPTION|LTR27_MEZZANINE2_DESCRIPTION|LTR27_MEZZANINE3_DESCRIPTION|LTR27_MEZZANINE4_DESCRIPTION|\
 LTR27_MEZZANINE5_DESCRIPTION|LTR27_MEZZANINE6_DESCRIPTION|LTR27_MEZZANINE7_DESCRIPTION|LTR27_MEZZANINE8_DESCRIPTION)
#define LTR27_ALL_DESCRIPTION (LTR27_MODULE_DESCRIPTION|LTR27_ALL_MEZZANINE_DESCRIPTION)
//
#pragma pack(4)
//**** ������������ ������
// �������� ������
typedef struct                      //
{                                   //
  TDESCRIPTION_MODULE    Module;    //
  TDESCRIPTION_CPU       Cpu;       //
  TDESCRIPTION_MEZZANINE Mezzanine[LTR27_MEZZANINE_NUMBER];
} TINFO_LTR27;               //

typedef struct 
{                    
  //**** ��������� ����������       //
  INT size;
  TLTR Channel;                         //
  BYTE subchannel;                  //
  //**** ��������� ������           //
  BYTE   FrequencyDivisor;	 	    // �������� ������� ������������� 0..255 (1000..4 ��)
  struct TMezzanine 
  {               
      CHAR Name[16];                // �������� ���������
      CHAR Unit[16];                // ���������� ���������� ���.��������
      double ConvCoeff[2];          // ������� � �������� ��� ��������� ���� � ���.��������
      double CalibrCoeff[4];        // ������������� ������������
  } Mezzanine[LTR27_MEZZANINE_NUMBER];// ������������� ��������

  TINFO_LTR27 ModuleInfo;
} TLTR27;                           //

#pragma pack()
// �������� �������
LTR27API_DllExport(INT) LTR27_Init       (TLTR27 *module);
LTR27API_DllExport(INT) LTR27_Open       (TLTR27 *module, DWORD saddr, WORD sport, CHAR *csn, WORD cc);
LTR27API_DllExport(INT) LTR27_Close      (TLTR27 *module);
LTR27API_DllExport(INT) LTR27_IsOpened   (TLTR27 *module);
LTR27API_DllExport(INT) LTR27_Echo       (TLTR27 *module);
LTR27API_DllExport(INT) LTR27_GetConfig  (TLTR27 *module);
LTR27API_DllExport(INT) LTR27_SetConfig  (TLTR27 *module);
LTR27API_DllExport(INT) LTR27_ADCStart   (TLTR27 *module);
LTR27API_DllExport(INT) LTR27_ADCStop    (TLTR27 *module);
LTR27API_DllExport(INT) LTR27_Recv       (TLTR27 *module, DWORD *data, DWORD *tstamp, DWORD size, DWORD timeout);
LTR27API_DllExport(INT) LTR27_ProcessData(TLTR27 *module, DWORD *src_data, double *dst_data, DWORD *size, BOOL calibr, BOOL value); 
LTR27API_DllExport(INT) LTR27_GetDescription(TLTR27 *module, WORD flags);
LTR27API_DllExport(INT) LTR27_WriteMezzanineDescr(TLTR27 *module, BYTE mn);
// ������� ���������������� ���������
LTR27API_DllExport(LPCSTR) LTR27_GetErrorString(INT error);
LTR27API_DllExport(INT) LTR27_ClearBuffer(TLTR27 *module, BOOL wait_response);
#ifdef __cplusplus
}
#endif
#endif



