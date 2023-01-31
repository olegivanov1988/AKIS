//---------------------------------------------------------------------------

#ifndef WSystemH
#define WSystemH
#include "ltr\include\ltr34api.h"
#include "ltr\include\ltr11api.h"
#include "ltr\include\ltr43api.h"
//-------------------------/ CLASS /-----------------------------------------
// ќпределени€ дл€ возвращаемых значений функций
#define RET_OK		0
#define RET_ERROR	1
#define RET_END		2
#define RET_NO_U	3
#define ACQ_BLOCK_SIZE 1024 // размер собираемых блоков данных
#define ACQ_BLOCK_QNT 2     // количество блоков дл€ сохранени€ прин€тых данных
class WSystem
{
friend class TASK_1;
friend class TASK_2;
public:
	WSystem();
	~WSystem();

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
private:
	void __fastcall Init_LTR11_LTR34()throw(Exception);
//	void __fastcall Config_LTR11_LTR34()throw(Exception);
	void __fastcall StartGen_LTR11_LTR34()throw(Exception);
//	int __fastcall Init_LTR11_LTR43();
//	int __fastcall Config_LTR11_LTR43();
	int __fastcall IsOpenedLTR34();

private:

	DWORD *ArrayToSend;
	double *VoltArray;
	int NumStep;					// Ќомер шага проверки
	static double VoltArrayTable[][4];
	static double Util1ResultSet[];
	static double DeltaErrorUtil1[];
	static double AmpUtil5[];
	static int KolSemplKanalUtil6[];
	static float FreqUtil6[];
	TLTR34 Conf_LTR34;
	TLTR11 Conf_LTR11;
	TLTR43 Conf_LTR43;
	int RunFlag;
	int AcqBlockReady[ACQ_BLOCK_QNT];
	double AcqBuf[ACQ_BLOCK_QNT][ACQ_BLOCK_SIZE];
	int BlockNumberThread;
	float Util1Result;
	double Util5Result[3];
	bool ReplaceBuf;
	static DWORD WINAPI AcquireThread (WSystem *System);
	static double SwitchKomparator[4][3];

};
//---------------------------------------------------------------------------
#endif
