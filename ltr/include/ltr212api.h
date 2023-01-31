#ifndef __LTR212API__H
#define __LTR212API__H
#include "ltr\\include\\ltrapi.h"  

//#define LTR212API_EXPORTS      

#ifdef LTR212API_EXPORTS
  #define LTR212API_DllExport(type)   __declspec(dllexport) type APIENTRY  
#else
  #define LTR212API_DllExport(type)   __declspec(dllimport) type APIENTRY
#endif

// ���� ������. �������� ��. �-� LRT212_GetErrorString()

#define LTR212_NO_ERR				                       (0)     
#define LTR212_ERR_INVALID_DESCR                           (-2001)
#define LTR212_ERR_INVALID_CRATE_SN                        (-2002)
#define LTR212_ERR_INVALID_SLOT_NUM                        (-2003) 
#define LTR212_ERR_CANT_INIT                               (-2004) 
#define LTR212_ERR_CANT_OPEN_CHANNEL                       (-2005)
#define LTR212_ERR_CANT_CLOSE                              (-2006) 
#define LTR212_ERR_CANT_LOAD_BIOS                          (-2007) 
#define LTR212_ERR_CANT_SEND_COMMAND                       (-2008) 
#define LTR212_ERR_CANT_READ_EEPROM	                       (-2009)
#define LTR212_ERR_CANT_WRITE_EEPROM                       (-2010)
#define LTR212_ERR_CANT_LOAD_IIR	                       (-2011)
#define LTR212_ERR_CANT_LOAD_FIR	                       (-2012) 
#define LTR212_ERR_CANT_RESET_CODECS                       (-2013)
#define LTR212_ERR_CANT_SELECT_CODEC                       (-2014)    
#define LTR212_ERR_CANT_WRITE_REG                          (-2015) 
#define LTR212_ERR_CANT_READ_REG	                       (-2016)
#define LTR212_ERR_WRONG_ADC_SETTINGS                      (-2017)  
#define LTR212_ERR_WRONG_VCH_SETTINGS                      (-2018) 
#define LTR212_ERR_CANT_SET_ADC		                       (-2019)
#define LTR212_ERR_CANT_CALIBRATE	                       (-2020)
#define LTR212_ERR_CANT_START_ADC	                       (-2021)
#define LTR212_ERR_INVALID_ACQ_MODE                        (-2022) 
#define LTR212_ERR_CANT_GET_DATA	                       (-2023)
#define LTR212_ERR_CANT_MANAGE_FILTERS                     (-2024)
#define LTR212_ERR_CANT_STOP		                       (-2025)
#define LTR212_ERR_CANT_GET_FRAME                          (-2026)
#define LTR212_ERR_INV_ADC_DATA		                       (-2026)
#define LTR212_ERR_TEST_NOT_PASSED	                       (-2027)
#define LTR212_ERR_CANT_WRITE_SERIAL_NUM                   (-2028)
#define LTR212_ERR_CANT_RESET_MODULE                       (-2029) 
#define LTR212_ERR_MODULE_NO_RESPONCE                      (-2030) 
#define LTR212_ERR_WRONG_FLASH_CRC	                       (-2031)
#define LTR212_ERR_CANT_USE_FABRIC_AND_USER_CALIBR_SYM	   (-2032)  
#define LTR212_ERR_CANT_START_INTERFACE_TEST			   (-2033)
#define LTR212_ERR_WRONG_BIOS_FILE			               (-2034) 
#define LTR212_ERR_CANT_USE_CALIBR_MODE			           (-2035)  
#define LTR212_ERR_PARITY_ERROR                            (-2036)  
#define LTR212_ERR_CANT_LOAD_CLB_COEFFS                    (-2037)
#define LTR212_ERR_CANT_LOAD_FABRIC_CLB_COEFFS             (-2038) 
#define LTR212_ERR_CANT_GET_VER							   (-2039)
#define LTR212_ERR_CANT_GET_DATE						   (-2040)
#define LTR212_ERR_WRONG_SN						           (-2041)  
#define LTR212_ERR_CANT_EVAL_DAC						   (-2042)    
#define LTR212_ERR_ERROR_OVERFLOW						   (-2043)
#define LTR212_ERR_SOME_CHENNEL_CANT_CLB				   {-2044}


#define MAX_212_CH 8

 /******** ����������� ��������� �������� ������ *************/

#pragma pack(4) 

typedef struct 
{
	CHAR Name[16];
	CHAR Serial[24];
	CHAR BiosVersion[8];// ������ �����
    CHAR BiosDate[16];  // ���� �������� ������ ������ �����
} TINFO_LTR212,*PTINFO_LTR212; 


typedef struct
 {
  INT size;
  TLTR Channel;
  INT AcqMode; // ����� ����� ������
  INT UseClb;  // ���� ������������� ������������� �����-���
  INT UseFabricClb;// ���� ������������� ��������� ������������� �����-���
  INT LChQnt;	 // ���-�� ������������ ����������� �������
  INT LChTbl[8];  //������� ����������� �������
  INT REF;		 // ���� �������� �������� ����������
  INT AC;		 // ���� ���������������� �������� ����������
  double Fs;     // ������� ������������� ���
  
  
  struct
   {
    INT IIR;         // ���� ������������� ���-�������
    INT FIR;         // ���� ������������� ���-�������
    INT Decimation;  // �������� ������������ ��������� ��� ���-�������
    INT TAP;		 // ������� ���-������� 
    CHAR IIR_Name[512+1]; // ������ ���� � ����� � �����-�� ������������ ���-������� 
    CHAR FIR_Name[512+1]; // ������ ���� � ����� � �����-�� ������������ ���-�������
   } filter;   // ���������, �������� ������ � �����. �������� � �� �����-��.
  
   TINFO_LTR212 ModuleInfo;
    
   WORD CRC_PM; // ��� ���������� �����������
   WORD CRC_Flash_Eval; // ��� ���������� �����������
   WORD CRC_Flash_Read;   // ��� ���������� �����������
   
 } TLTR212, *PTLTR212; // ��������� �������� ������ 
 
typedef struct{
	DWORD Offset  [MAX_212_CH];
	DWORD Scale   [MAX_212_CH];
	BYTE DAC_Value[MAX_212_CH];
}TLTR212_Usr_Clb;
 
#pragma pack()

#define MAXTAPS 255		// ������������ �������� ������� ���-�������
#define MINTAPS 3		// ����������� �������� ������� ���-������� 


#define ACQ_MODE_MEDIUM_PRECISION    (0)
#define ACQ_MODE_HIGH_PRECISION      (1)
#define ACQ_MODE_8CH_HIGH_PRECISION  (2) 

//
#define TIMEOUT_CMD_SEND								    /*(2000)*/ (2000)
#define TIMEOUT_CMD_RECIEVE								    /*(4000)*/ (2000)



#pragma pack(4)
typedef struct {
  double fs;
  BYTE    decimation;
  BYTE    taps;
  SHORT    koeff[MAXTAPS];
} ltr212filter; // ���������, ������������� ��� �������� �������
#pragma pack()
         
                                  
// ������� api-����������. �������� ��.� ����������
#ifdef __cplusplus
extern "C" { 
 #endif     

// ��������� ������������
LTR212API_DllExport (INT) LTR212_Init(PTLTR212 hnd);
LTR212API_DllExport(INT) LTR212_IsOpened(PTLTR212 hnd);     
LTR212API_DllExport (INT) LTR212_Open(PTLTR212 hnd, DWORD net_addr, WORD net_port, CHAR *crate_sn, INT slot_num, CHAR *biosname);
LTR212API_DllExport (INT) LTR212_Close(PTLTR212 hnd);
LTR212API_DllExport (INT) LTR212_CreateLChannel(INT PhysChannel, INT Scale);
LTR212API_DllExport (INT) LTR212_SetADC(PTLTR212 hnd);
LTR212API_DllExport (INT) LTR212_Start(PTLTR212 hnd); 
LTR212API_DllExport (INT) LTR212_Stop(PTLTR212 hnd); 
LTR212API_DllExport (INT) LTR212_Recv(PTLTR212 hnd, DWORD *data, DWORD *tmark, DWORD size, DWORD timeout);                 
LTR212API_DllExport (INT) LTR212_ProcessData(PTLTR212 hnd, DWORD *src, double *dest, DWORD *size, BOOL volt);   
LTR212API_DllExport (LPCSTR) LTR212_GetErrorString(INT Error_Code); 
LTR212API_DllExport (INT) LTR212_Calibrate(PTLTR212 hnd, BYTE *LChannel_Mask, INT mode, INT reset); 
LTR212API_DllExport (INT) LTR212_CalcFS(PTLTR212 hnd, double *fsBase, double *fs);
LTR212API_DllExport (INT) LTR212_TestEEPROM(PTLTR212 hnd);  

// ��������������� �������
LTR212API_DllExport (INT) LTR212_ProcessDataTest(PTLTR212 hnd, DWORD *src, double *dest, DWORD *size, BOOL volt,DWORD *bad_num);  
LTR212API_DllExport (INT) LTR212_ReadFilter(CHAR *fname, ltr212filter *filter); 
LTR212API_DllExport (INT) LTR212_WriteSerialNumber(PTLTR212 hnd, CHAR *sn, WORD Code); 
LTR212API_DllExport (INT) LTR212_TestInterfaceStart(PTLTR212 hnd, INT PackDelay); 
LTR212API_DllExport (DWORD) LTR212_CalcTimeOut(PTLTR212 hnd, DWORD n);        

//�������� �������, ������ � ������ ���������������� ������������� (� ������� ������)
//���� ������ �� ������������� ���� DOUBLE
LTR212API_DllExport (INT) LTR212_ReadUSR_Clb (PTLTR212 hnd, TLTR212_Usr_Clb *CALLIBR);
LTR212API_DllExport (INT) LTR212_WriteUSR_Clb(PTLTR212 hnd, TLTR212_Usr_Clb *CALLIBR);


#ifdef __cplusplus 
 } 
#endif
                                          
#endif

