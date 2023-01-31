
#ifndef __LTR34API__
#define __LTR34API__

#define _DEBUG_MY


#ifdef LTR34API_EXPORTS
#define LTR34API_DllExport(type)   __declspec(dllexport) type APIENTRY  
#else
#define LTR34API_DllExport(type)   __declspec(dllimport) type APIENTRY
#endif


#include "ltr\\include\\ltrapitypes.h"
#include "ltr\\include\\ltrapi.h"

	// ���������
#define LTR34_ERROR_SEND_DATA				(-3001)
#define LTR34_ERROR_RECV_DATA				(-3002)
#define LTR34_ERROR_RESET_MODULE			(-3003)
#define LTR34_ERROR_NOT_LTR34				(-3004)
#define LTR34_ERROR_CRATE_BUF_OWF			(-3005)
#define LTR34_ERROR_PARITY					(-3006)
#define LTR34_ERROR_OVERFLOW				(-3007)
#define LTR34_ERROR_INDEX					(-3008)
	//
#define LTR34_ERROR							(-3009)
#define LTR34_ERROR_EXCHANGE				(-3010)
#define LTR34_ERROR_FORMAT					(-3011)
#define LTR34_ERROR_PARAMETERS				(-3012)
#define LTR34_ERROR_ANSWER					(-3013)
#define LTR34_ERROR_WRONG_FLASH_CRC			(-3014)
#define LTR34_ERROR_CANT_WRITE_FLASH		(-3015)
#define LTR34_ERROR_CANT_READ_FLASH			(-3016)
#define LTR34_ERROR_CANT_WRITE_SERIAL_NUM	(-3017)
#define LTR34_ERROR_CANT_READ_SERIAL_NUM	(-3018) 
#define LTR34_ERROR_CANT_WRITE_FPGA_VER		(-3019)
#define LTR34_ERROR_CANT_READ_FPGA_VER		(-3020)
#define LTR34_ERROR_CANT_WRITE_CALIBR_VER	(-3021)
#define LTR34_ERROR_CANT_READ_CALIBR_VER	(-3022)
#define LTR34_ERROR_CANT_STOP				(-3023)
#define LTR34_ERROR_SEND_CMD				(-3024)
#define LTR34_ERROR_CANT_WRITE_MODULE_NAME	(-3025)
#define LTR34_ERROR_CANT_WRITE_MAX_CH_QNT	(-3026)
#define LTR34_ERROR_CHANNEL_NOT_OPENED		(-3027)
#define LTR34_ERROR_WRONG_LCH_CONF			(-3028)


	//
#define LTR34_MAX_BUFFER_SIZE		2097151
#define LTR34_EEPROM_SIZE			2048
#define LTR34_USER_EEPROM_SIZE		1024
#define LTR34_DAC_NUMBER_MAX		8

	typedef unsigned char byte;
	typedef unsigned short ushort;
	typedef unsigned int uint;
	typedef enum
	{
		ltr_34_gen_type_sin=0,
		ltr_34_gen_type_pila=1,
		ltr_34_gen_type_mean=2,
		ltr_34_gen_type_max=3
	}_ltr34_gen_type;

	typedef struct
	{
		double angle;
		double Period;
		double Freq;
		double Min;
		double Max;
		_ltr34_gen_type GenType;
	}_ltr34_gen_param_struct;

#ifndef __cplusplus
#define true 1
#define false 0
	typedef byte bool;
#endif
	//
#pragma pack(4)

	typedef struct 
	{
		float FactoryCalibrOffset[2*LTR34_DAC_NUMBER_MAX];
		float FactoryCalibrScale[2*LTR34_DAC_NUMBER_MAX];

	} DAC_CHANNEL_CALIBRATION;


	typedef struct 
	{
		CHAR Name[16];
		CHAR Serial[24];
		CHAR FPGA_Version[8];
		CHAR CalibrVersion[8];  
		BYTE MaxChannelQnt;

	} TINFO_LTR34,*PTINFO_LTR34;

	//**** ������������ ������
	typedef struct 
	{
		int size;    // ������ ��������� 

		TLTR Channel;					  // ��������� ����������� ������ � ������ � �������� � ltrapi.pdf 
		DWORD LChTbl[8];                  // ������� ���������� �������
		//**** ��������� ������           
		byte FrequencyDivisor;            // �������� ������� ������������� 0..60 (31.25..500 ���)
		byte ChannelQnt;             // ����� ������� 0, 1, 2, 3 ������������ (1, 2, 4, 8)
		bool UseClb;
		bool AcknowledgeType;             // ��� ������������� true - �������� ������������� ������� �����, false- �������� ��������� ������ ������ 100 ��
		bool ExternalStart;               // ������� ����� true - ������� �����, false - ����������
		bool RingMode;                    // ����� ������  true - ����� ������, false - ��������� �����
		bool BufferFull;					// ������ - ����� ���������� - ������
		bool BufferEmpty;					// ������ - ����� ���� - ������
		bool DACRunning;					// ������ - �������� �� ���������
		float FrequencyDAC;				// ������ - ������� - �� ������� �������� ��� � ������� ������������
		DAC_CHANNEL_CALIBRATION	DacCalibration;
		TINFO_LTR34 ModuleInfo;
	}TLTR34,*PTLTR34;                      
#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif

	// ������������� ����� ��������� TLTR34
	LTR34API_DllExport (INT) LTR34_Init (TLTR34 *module);

	// ������������ ����� � ������� LTR34.
	// ������ ���������� ����� STOP+RESET � ��������� ������������� ������.
	LTR34API_DllExport (INT) LTR34_Open (TLTR34 *module, DWORD saddr, WORD sport, CHAR *csn, WORD cc);

	// ������ ����� � �������.
	LTR34API_DllExport (INT) LTR34_Close (TLTR34 *module);

	// ����������� �������� ������ ����� � �������.
	LTR34API_DllExport (INT) LTR34_IsOpened (TLTR34 *module);

	/* 
	������� ��� ������ ������ �� ������.
	1) ��� ������ ��� � ������ ECHO - ��� ������, ������������
	�������, ������ ����������� � ������ ������� � ��� ����
	� ������� ������. ����� �������, � ������ ECHO ������
	������ ������ ���������� ������ �� ������.
	2) ��� ������ ��� � ������ PERIOD - ��� ������, ������������
	�������, ������ ����������� � "������������".
	*/
	LTR34API_DllExport (INT) LTR34_Recv (TLTR34 *module, DWORD *data, DWORD *tstamp, DWORD size, DWORD timeout);

	// ������������ ����������� ������
	LTR34API_DllExport (DWORD) LTR34_CreateLChannel(BYTE PhysChannel, bool ScaleFlag); 

	/*
	������� ��� �������� ������ ������
	1) � ltr-������� ����������� ���������� ������� ������
	����������� �� ������ LTR34 �� �������. ������� ���
	������ � ��� � ��������� ������ (RingMode=0)
	������ ����� �������� � ����� �����������, �� ��������
	� ������������ ������ � LTR34. ������, �������
	������������ � ���������� ������� ��������, �.�.
	��� ����������� ���������� ������ � ������
	����� ������ �������� �� ������� ����� ������������� �� ���������
	���������������� ������� ������������� ���, �.�. ��� ������ ��������
	������������� � ���������� ������� ���������� ���������� ������
	����� ������������� ���������� ������� �������� ��������.

	2) ���������� ������� ������ LTR34 �������� �� �������� ����������
	������������ ������� � �������� �������������, ��� � ���������
	������� ����� ��������� � ���������� ������ � ������ (��������,
	�� LTR34 ��� ���� ���������� ����������� ���������� ������, ��
	��� ��� �� ��������� � ������������� �� ������ ��� �� ���������.
	� ���� ������ ������ ��������� ����� ������ �� �������).
	��� ������ ��������� ���������� ������� ������� ������� ������
	������� RESET (��� ���� ����� �������� ���������� �������� � ltr-�������)
	��� ���������� ��������������� � ������ (������� LTR34_Close() � LTR34_Open())
	*/
	LTR34API_DllExport (INT) LTR34_Send (TLTR34 *module, DWORD *data, DWORD size, DWORD timeout);

	LTR34API_DllExport (INT) LTR34_ProcessData(TLTR34 *module, double *src, DWORD *dest, DWORD size, bool volt);

	// ������ ������ � FIFO ����� 
	LTR34API_DllExport (int) LTR34_SendData(TLTR34 *module, double *data, DWORD size, DWORD timeout, bool calibrMainPset, bool calibrExtraVolts);
	// ������ �������� CONFIG
	LTR34API_DllExport (INT) LTR34_Config  (TLTR34 *module);

	// ������ ���.
	LTR34API_DllExport (INT) LTR34_DACStart   (TLTR34 *module);

	// ������� ���.
	LTR34API_DllExport (INT) LTR34_DACStop    (TLTR34 *module);
	LTR34API_DllExport (INT) LTR34_Reset(TLTR34 *module);

	LTR34API_DllExport (INT) LTR34_SetDescription(TLTR34 *module);
	LTR34API_DllExport (INT) LTR34_GetDescription(TLTR34 *module);

	LTR34API_DllExport (INT) LTR34_GetCalibrCoeffs(TLTR34 *module);
	LTR34API_DllExport (INT) LTR34_WriteCalibrCoeffs(TLTR34 *module);

	LTR34API_DllExport (INT) LTR34_ReadFlash(TLTR34 *module, BYTE *data, WORD size, WORD Address);


	// ������� ���������������� ���������
	LTR34API_DllExport (LPCSTR) LTR34_GetErrorString(int error);

	// �������� ����������� ����	
	LTR34API_DllExport (INT) LTR34_TestEEPROM(TLTR34 *module); 

	// ������������������� ������� - ���������� �� ���� ����� � ����, �� �������� :)
	LTR34API_DllExport(INT) LTR34_PrepareGenData(TLTR34 *module, double* GenerateData, int GenLength, _ltr34_gen_param_struct * GenParam);


#ifdef __cplusplus
}
#endif

#endif

