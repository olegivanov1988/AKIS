//---------------------------------------------------------------------------

#ifndef WMkH
#define WMkH
//---------------------------------------------------------------------------
#include "ltr\include\ltr34api.h"
#include "ltr\include\ltr11api.h"
//-------------------------/ CLASS /-----------------------------------------
// ќпределени€ дл€ возвращаемых значений функций
#define RET_OK		0
#define RET_ERROR	1
#define RET_END		2
#define RET_NO_U	3

#define ACQ_BLOCK_SIZE 		1080 // размер собираемых блоков данных
//#define ACQ_BLOCK_SIZE_2 	1024
#define ACQ_BLOCK_QNT 2     // количество блоков дл€ сохранени€ прин€тых данных

#define MK_UTIL2	2
#define MK_UTIL3	3
#define MK_UTIL4	4
#define MK_UTIL5	5

#define UPLUS_MAX	510
#define UPLUS_MIN	460

#define UMINUS_MAX	-510
#define UMINUS_MIN	-460

class WMk
{
friend class TMK_1;
friend class TMK_2;
public:
	WMk();
	~WMk();

protected:
	int __fastcall UtilStop( bool power_off = false );
	void __fastcall Util1Step1()throw(Exception);
	void __fastcall Util2_3Step1(int num_util)throw(Exception);
	void __fastcall Util4_5Step1(int num_util)throw(Exception);
private:
	void __fastcall Init_LTR11_LTR34()throw(Exception);
	void __fastcall StartGen_LTR11_LTR34()throw(Exception);
	int __fastcall IsOpenedLTR34();
	void __fastcall ErrorRelease( int err )throw(Exception);
	static DWORD WINAPI AcquireThread (WMk *System);

private:
	DWORD *ArrayToSend;
	double *VoltArray;
	int NumStep;					// Ќомер шага проверки
	TLTR34 Conf_LTR34;
	TLTR11 Conf_LTR11;
	int RunFlag;
	int AcqBlockReady[ACQ_BLOCK_QNT];
	double AcqBuf[ACQ_BLOCK_QNT][ACQ_BLOCK_SIZE];
	int BlockNumberThread;
	bool ReplaceBuf;
//	bool PerepadUtil1[5];
//	bool PerepadUtil2[5];
//	bool PerepadUtil3[5];
	bool PerepadUtil4[2];
//	bool PerepadUtil5[5];
	HANDLE hnd_acq_thread2;
	double Uplus, Uminus;
};
//---------------------------------------------------------------------------
#endif