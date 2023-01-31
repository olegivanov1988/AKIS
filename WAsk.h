//---------------------------------------------------------------------------

#ifndef WAskH
#define WAskH
#include "ltr\include\ltr34api.h"
#include "ltr\include\ltr11api.h"
#include "ltr\include\ltr27api.h"
//-------------------------/ CLASS /-----------------------------------------
// Определения для возвращаемых значений функций
#define RET_OK		0
#define RET_ERROR	1
#define RET_END		2
#define RET_NO_U	3
#define ACQ_BLOCK_SIZE 1020 // размер собираемых блоков данных
#define ACQ2_BLOCK_SIZE	(2*LTR27_MEZZANINE_NUMBER*64)
#define ACQ_BLOCK_QNT 	 1     // количество блоков для сохранения принятых данных
#define PARAMETERS_ASK1  7  // количество контролируемых проверок на экране АСК 1
#define PARAMETERS_ASK2  4  // количество контролируемых проверок на экране АСК 2

#define K10_MIN 	3.05
#define K10_MAX 	3.85

#define REAL_K1_MIN 	1.1
#define REAL_K1_MAX 	1.6

#define UPLUS_MAX	9.5
#define UPLUS_MIN	8.4

#define UMINUS_MAX	-9.5
#define UMINUS_MIN	-8.4

#define ASK_Ucc		7.7

#define CURRENT_MIN		1.2
#define CURRENT_MIDD    2.8
#define CURRENT_MAX		3.5

class ZRaport;

class WAsk
{
friend class TASK_1;
friend class TASK_2;
public:
	WAsk();
	~WAsk();

protected:
	void __fastcall Util0Step1()throw(Exception);
	void __fastcall Util1Step1()throw(Exception);
	int __fastcall Util1Step2_8()throw(Exception);
	int __fastcall UtilStop( bool power_off = false );
	void __fastcall Util2Step1()throw(Exception);
	void __fastcall Util3Step1()throw(Exception);
	void __fastcall Util4Step1()throw(Exception);
	void __fastcall Util5Step1()throw(Exception);
	int __fastcall Util5Step2_4()throw(Exception);
	void __fastcall Util6Step1()throw(Exception);
	int __fastcall Util6Step2_6()throw(Exception);
	void __fastcall ErrorRelease( int err )throw(Exception);

	void __fastcall WriteExcel(AnsiString FileName, bool write);

private:
	void __fastcall Init_LTR11_LTR34()throw(Exception);
	void __fastcall Init_LTR27()throw(Exception);
//	void __fastcall Config_LTR11_LTR34()throw(Exception);
	void __fastcall StartGen_LTR11_LTR34()throw(Exception);
//	int __fastcall Init_LTR11_LTR43();
//	int __fastcall Config_LTR11_LTR43();
	int __fastcall IsOpenedLTR34();
	static DWORD WINAPI AcquireThread (WAsk *System);
	static DWORD WINAPI AcquireThread2 (WAsk *System);

	static DWORD WINAPI CheckUtil6Thread (WAsk *System, int step);

private:

	DWORD *ArrayToSend;
	double *VoltArray;
	int NumStep;					// Номер шага проверки
	static double VoltArrayTable[][4];
	static double Util1ResultSet[];
	static double DeltaErrorUtil1[];
	static double AmpUtil5[];
	static int KolSemplKanalUtil6[];
	static float FreqUtil6[];
	static float AmpUtil6[];
	TLTR34 Conf_LTR34;
	TLTR11 Conf_LTR11;
	TLTR27 Conf_LTR27;
	int RunFlag,RunFlag2;
	int AcqBlockReady[ACQ_BLOCK_QNT];
	int Acq2BlockReady[ACQ_BLOCK_QNT];
	double AcqBuf[ACQ_BLOCK_QNT][ACQ_BLOCK_SIZE];
	double Acq2Buf[ACQ_BLOCK_QNT][ACQ2_BLOCK_SIZE];
	int BlockNumberThread,BlockNumberThread2;
	double Util1Result;
	double Util1Porog[8];
	AnsiString noteUtil1Porog[8];
	double Util5Result[3];
	double Util5ResultReport[4][3];
    AnsiString noteUtil5[6];
	bool ReplaceBuf,ReplaceBuf2;
	static double SwitchKomparator[4][3];
	int CheckASK1[PARAMETERS_ASK1];
	int CheckASK2[PARAMETERS_ASK2];
	double maxValueK10;
	double realValueK1;
	double Uplus, Uminus;
	AnsiString noteUplus, noteUminus;
	AnsiString NumberBlock;
	double currentPlus;
	AnsiString note_current;
	AnsiString note_CK3_5XP1_30;
	AnsiString note_CK3_5XP1_120;
	AnsiString note_CK3_7XP1_30;
	AnsiString note_CK3_7XP1_120;
	AnsiString note2_CK3_5XP1_30;
	AnsiString note2_CK3_5XP1_120;
	AnsiString note2_CK3_7XP1_30;
	AnsiString note2_CK3_7XP1_120;

	ZRaport *zRaport;
};
//---------------------------------------------------------------------------
#endif

/* CheckASK1
0 - первоначальное значение, 1 - норма, 2 - ошибка
[0] - визуальное наличие ступенек
[1] - положительное напряжение
[2] - отрицательное напряжение
[3] - проверка таблицы
[4] - напряжение в точке К10
[5] - визуальное наличие ступенек в точке Х9
[6] - проверка тока
*/

/* CheckASK2
0 - первоначальное значение, 1 - норма, 2 - ошибка
[0] - СК1,  реальное значение в К1
[1] - CК2
[2] - CK3 точка 5XP1
[3] - CK3 точка 7XP1
*/
