//---------------------------------------------------------------------------

#ifndef WLskH
#define WLskH
#include "ltr\include\ltr34api.h"
#include "ltr\include\ltr11api.h"
#include "ltr\include\ltr27api.h"

//-------------------------/ CLASS /-----------------------------------------
// Определения для возвращаемых значений функций
#define RET_OK		0
#define RET_ERROR	1
#define RET_END		2
#define RET_NO_U	3

#define ACQ_BLOCK_SIZE 		1080 	// размер собираемых блоков данных
#define ACQ2_BLOCK_SIZE		(2*LTR27_MEZZANINE_NUMBER*64)
#define ACQ_BLOCK_QNT 		2     	// количество блоков для сохранения принятых данных
#define ACQ_BLOCK_QNT2      1
#define PARAMETERS_LSK1  	3  		// количество контролируемых проверок на экране ЛСК 1
#define PARAMETERS_LSK2_1  	5  		// количество контролируемых проверок на экране ЛСК 2
#define PARAMETERS_LSK2_2  	3  		// количество контролируемых проверок на экране ЛСК 2

#define K1_K3_MIN 	6.7
#define K1_K3_MAX 	7.1

#define K1_K3_MIN_UTIL2 	6.7
#define K1_K3_MAX_UTIL2 	7.1

#define K1_K3_MIN_UTIL3 	0.0
#define K1_K3_MAX_UTIL3 	0.5

#define K1_K3_MIN_UTIL4 	0.0
#define K1_K3_MAX_UTIL4 	0.5

//#define FREQUENCY_K9_MIN 	123.5
//#define FREQUENCY_K9_MAX 	126.5

#define TIME_K9_MIN 		7.9
#define TIME_K9_MAX 		8.1

#define X8_MIN		6.7				// было 6.5 15.06.17
#define X8_MAX		7.3             // было 7.0 15.06.17

#define Vh			8.5

#define UTIL_4		4
#define UTIL_5		5

#define LSK_Ucc		7.7
#define LSK_Uh		9.0
#define LSK_InpUl	7.0

#define CURRENT_MIN		0.04
#define CURRENT_MIDD    0.12  //Изменить на 0,12 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define CURRENT_MAX		0.16

class ZRaport;

class WLsk
{
friend class TLSK_1;
friend class TLSK_2;
public:
	WLsk();
	~WLsk();

protected:
	void __fastcall Util0Step1()throw(Exception);
	void __fastcall Util1Step1()throw(Exception);
	int __fastcall Util1Step2()throw(Exception);
	int __fastcall Util1_2_3Step3()throw(Exception);
	int __fastcall UtilStop( bool power_off = false );
//	void __fastcall CheckUtil1();
//	void __fastcall CheckUtil2();
	void __fastcall Util2Step1()throw(Exception);
	int __fastcall Util2Step2()throw(Exception);
	void __fastcall Util3Step1()throw(Exception);
	int __fastcall Util3Step2()throw(Exception);
	void __fastcall Util4_5Step1(int num_util)throw(Exception);
	int __fastcall Util4_5Step2(int num_util)throw(Exception);
	void __fastcall WriteExcel(AnsiString FileName, bool write);
	void __fastcall WriteExcel_LSK_1(AnsiString FileName, bool write);
private:
	void __fastcall Init_LTR11_LTR34()throw(Exception);
	void __fastcall Init_LTR27()throw(Exception);
	void __fastcall StartGen_LTR11_LTR34()throw(Exception);
	int __fastcall IsOpenedLTR34();
	void __fastcall ErrorRelease( int err )throw(Exception);
	static DWORD WINAPI AcquireThread (WLsk *System);
	static DWORD WINAPI AcquireThread2 (WLsk *System);
	static DWORD WINAPI CheckUtil1Thread (WLsk *System);
	static DWORD WINAPI CheckUtil1Step3Thread (WLsk *System);
	static DWORD WINAPI CheckUtil2Thread (WLsk *System);
	static DWORD WINAPI CheckUtil2Step3Thread (WLsk *System);
	static DWORD WINAPI CheckUtil3Thread (WLsk *System);
	static DWORD WINAPI CheckUtil3Step3Thread (WLsk *System);
	static DWORD WINAPI CheckUtil4Thread (WLsk *System);
	static DWORD WINAPI CheckUtil4Step3Thread (WLsk *System);
	static DWORD WINAPI CheckUtil5Thread (WLsk *System);

private:

	DWORD *ArrayToSend;
	double *VoltArray;
	int NumStep;					// Номер шага проверки
//	static double VoltArrayTable[][4];
//	static double Util1ResultSet[];
//	static double DeltaErrorUtil1[];
//	static double AmpUtil5[];
//	static int KolSemplKanalUtil6[];
//	static float FreqUtil6[];
	TLTR34 Conf_LTR34;
	TLTR11 Conf_LTR11;
	TLTR27 Conf_LTR27;
	int RunFlag;
	int RunFlag2;
	int AcqBlockReady[ACQ_BLOCK_QNT];
	int Acq2BlockReady[ACQ_BLOCK_QNT];
	double AcqBuf[ACQ_BLOCK_QNT][ACQ_BLOCK_SIZE];
	double Acq2Buf[ACQ_BLOCK_QNT2][ACQ2_BLOCK_SIZE];
	int BlockNumberThread,BlockNumberThread2;
//	float Util1Result;
//	double Util5Result[3];
	bool ReplaceBuf,ReplaceBuf2;
//	bool checkUtil1[5];
//	bool checkUtil2[5];
    bool PerepadUtil1[5];
	bool PerepadUtil2[5];
	bool PerepadUtil3[5];
	bool PerepadUtil4[5];
	bool PerepadUtil5[5];
	double maxK1_K3[3];
	double maxK1_K3_Util2[3];
	double maxK1_K3_Util3[3];
	double maxK1_K3_Util4[3];
	double frequencyK9;
	double maxValueX8 ;
	int CheckLSK1[PARAMETERS_LSK1];
	int CheckLSK2_1[5][PARAMETERS_LSK2_1];
	int CheckLSK2_2[4][PARAMETERS_LSK2_2];
//	int AcqBlockSize;
	AnsiString NumberBlock;
	double currentPlus;
	AnsiString note_current;
	AnsiString note_frequencyK9;
     bool autoProv;
	HANDLE hnd_acq_thread2;

	ZRaport *zRaport;

	bool checkBlock;	// Проверка блока на пороги

};
//---------------------------------------------------------------------------
#endif

/* CheckLSK1
0 - первоначальное значение, 1 - норма, 2 - ошибка
[0] - проверка частоты в К9
[1] - проверка тока
[2] - проверка в точке X8(К10)
*/

/* CheckLSK2_1 [номер сигнала ЛС] [параметр]
0 - первоначальное значение, 1 - норма, 2 - ошибка
[0] -
[1] -
[2] -
[3] -
[4] -
*/

/* CheckLSK2_2 [номер сигнала ЛС] [параметр]
0 - первоначальное значение, 1 - норма, 2 - ошибка
[0] - напряжение в К1
[1] - напряжение в К2
[2] - напряжение в К3
*/

/* checkBlock
true - исправен, false - неисправен
*/
