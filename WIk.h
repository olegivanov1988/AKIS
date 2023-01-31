//---------------------------------------------------------------------------

#ifndef WIkH
#define WIkH
//---------------------------------------------------------------------------
#include "ltr\include\ltr34api.h"
#include "ltr\include\ltr11api.h"
#include "ltr\include\ltr27api.h"

//-------------------------/ CLASS /-----------------------------------------
// Определения для возвращаемых значений функций
#define RET_OK		0
#define RET_ERROR	1
#define RET_END		2
#define RET_NO_U	3

#define ACQ_BLOCK_SIZE 		4320  // размер собираемых блоков данных
#define ACQ2_BLOCK_SIZE	(2*LTR27_MEZZANINE_NUMBER*64)
#define ACQ_BLOCK_QNT 		1     // количество блоков для сохранения принятых данных

//#define FREQUENCY_K1_MIN	79.5
//#define FREQUENCY_K1_MAX	80.5
//
//#define FREQUENCY_K2_MIN	51.6
//#define FREQUENCY_K2_MAX	52.4

#define TIME_X8_MIN         12.43
#define TIME_X8_MAX         12.57

#define TIME_X10_MIN         19.156
#define TIME_X10_MAX         19.296

#define Util2_K3_MIN		5.7 		// 1.02.17 было 6.1
#define Util2_K3_MAX		5.9         // 1.02.17 было 6.3

#define Util4_SS_MIN		26			// 15.06.17 было 27
#define Util4_ZZZ_MIN		800

#define Util4_SS_MAX		30			// 15.06.17 было 28
#define Util4_ZZZ_MAX		800

#define UTIL4_K4K5_MIN		6.2
#define UTIL4_K4K5_MAX		7.0			// 15.06.17 было 7.1

#define PARAMETERS_IK1  	5  	    // количество контролируемых проверок на экране ИК 1
#define PARAMETERS_IK2  	6  	    // количество контролируемых проверок на экране ИК 1

#define UTIL5_TIME_MIN		0.19
#define UTIL5_TIME_MAX		0.21

#define UTIL6_TIME_MIN		7.3
#define UTIL6_TIME_MAX		8.1

//#define UTIL6_SS_MIN		7
//#define UTIL6_ZZZ_MIN		300
//
//#define UTIL6_SS_MAX		8
//#define UTIL6_ZZZ_MAX		100

#define IK_Ucc	   			7.8		// Напряжение питания модуля
#define IK_Ucc_Low	   		6.5     // Пониженное напряжение питания	// было 6.9 16.06.17

#define CURRENT_MIN			0.12
#define CURRENT_MIDD    	0.68
#define CURRENT_MAX			1.0

#define BLOCK_BU			0
#define BLOCK_IP			1

class ZRaport;

class WIk
{
friend class TIK_1;
friend class TIK_2;
public:
	WIk();
	~WIk();

protected:
	int __fastcall UtilStop( bool power_off = false );
	void __fastcall Util0Step1()throw(Exception);
	void __fastcall Util1Step1()throw(Exception);
	void __fastcall Util2Step0()throw(Exception);
	void __fastcall Util2Step1()throw(Exception);
	int __fastcall Util2Step2_12()throw(Exception);
	void __fastcall Util3Step1()throw(Exception);
	int __fastcall Util3Step2()throw(Exception);
	void __fastcall Util4Step1()throw(Exception);
	int __fastcall Util4Step2()throw(Exception);
	int __fastcall Util4Step3()throw(Exception);
	int __fastcall Util4Step4()throw(Exception);
	void __fastcall Util5Step1( int num_block )throw(Exception);
	int __fastcall Util5Step2()throw(Exception);
	void __fastcall Util6Step1( int num_block )throw(Exception);
	int __fastcall Util6and7Step2()throw(Exception);
	int __fastcall Util6and7Step3()throw(Exception);
//	int __fastcall Util6Step4()throw(Exception);
	void __fastcall Util7Step1()throw(Exception);

	void __fastcall WriteExcel(AnsiString FileName, bool write);
private:
	void __fastcall Init_LTR11_LTR34()throw(Exception);
	void __fastcall Init_LTR27()throw(Exception);
	void __fastcall StartGen_LTR11_LTR34()throw(Exception);
	int __fastcall IsOpenedLTR34();
	void __fastcall ErrorRelease( int err )throw(Exception);
	static DWORD WINAPI AcquireThread (WIk *System);
	static DWORD WINAPI AcquireThread2 (WIk *System);
	static DWORD WINAPI CheckUtil3_7Thread (WIk *System);
	static DWORD WINAPI CheckUtil5Thread (WIk *System);
	static DWORD WINAPI CheckUtil6Step3Thread (WIk *System);

private:
	DWORD *ArrayToSend;
	double *VoltArray;
	int NumStep;					// Номер шага проверки
	TLTR34 Conf_LTR34;
	TLTR11 Conf_LTR11;
    TLTR27 Conf_LTR27;
	int RunFlag,RunFlag2;
	int AcqBlockReady[ACQ_BLOCK_QNT];
	int Acq2BlockReady[ACQ_BLOCK_QNT];
	double AcqBuf[ACQ_BLOCK_QNT][ACQ_BLOCK_SIZE];
	double Acq2Buf[ACQ_BLOCK_QNT][ACQ2_BLOCK_SIZE];
	int BlockNumberThread,BlockNumberThread2;
	bool ReplaceBuf,ReplaceBuf2;
	double Util1frequencyK1;
	double Util1frequencyK2;
	double PressureK3;
	bool Util3K3;
	bool Util4K3;
	double Util4K4;
	double Util4K5;
	bool Util5K6;
	bool Util5K7;
//	bool PerepadUtil1[5];
//	bool PerepadUtil2[5];
//	bool PerepadUtil3[5];
//	bool PerepadUtil4[2];
	bool PerepadUtil7[2];
	HANDLE hnd_acq_thread2;
//	double Uplus, Uminus;
	int CheckIK1[PARAMETERS_IK1];
	int CheckIK2[PARAMETERS_IK2];
	double TimeUtil5;
	AnsiString note_TimeUtil5;
	double TimeUtil6;
	AnsiString note_TimeUtil6;
//	TDateTime timeUtil6K6;
//	TDateTime timeUtil6K8;
	AnsiString NumberBlock;
	int number_block;
	double currentPlus;
	static float TimeMinUtil5[];
	static float TimeMaxUtil5[];
	static float TimeMinUtil6[];
	static float TimeMaxUtil6[];
	int Block;
	AnsiString note_current;
	AnsiString note_PressureK3;
	AnsiString note_timeX8;
	AnsiString note_timeX10;
	int secK4, secK5;
	AnsiString note_Util4X30;
	AnsiString note_Util4X31;
	TDateTime TimeUtil4X6;
	AnsiString note_Util4X6;
	double Util7X34, Util7X32;
	AnsiString note_Util7X34;
	double Util1TimeK1;
	double Util1TimeK2;
	AnsiString time1;
	AnsiString time2;


	ZRaport *zRaport;
};
//---------------------------------------------------------------------------
#endif

/* CheckIK1
0 - первоначальное значение, 1 - норма, 2 - ошибка
[0] - Util 1, частота в К1
[1] - Util 1, частота в К2
[2] - Util 2, напряжение в К3
[3] - Util 3, импульс в К3
[4] - Util 4, время в К3
*/
/* CheckIK2
0 - первоначальное значение, 1 - норма, 2 - ошибка
[0] - Util4, время в Х30
[1] - Util4, время в Х31
[2] - Util5, порог и время между Х27 и Х18
[3] - Util6, порог и время в Х27
[4] - Util7, порог в X34
[5] - Util7, порог в Х32
*/
