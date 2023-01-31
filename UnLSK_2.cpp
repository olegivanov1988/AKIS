//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnLSK_2.h"
#include "WLsk.h"
#include "UnLSK_1.h"
#include "UnVisibleReport.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "iScope"
#pragma link "sBevel"
#pragma link "sButton"
#pragma link "sLabel"
#pragma link "sListBox"
#pragma link "sPanel"
#pragma link "sRadioButton"
#pragma resource "*.dfm"
#include "bass.h"
#include <process.h>
#include <stdio.h>
#include <windows.h>
//TLSK_2 *LSK_2;
const double TLSK_2::SecPerDivTable[19] = {0.1, 0.05, 0.02, 0.01, 0.005, 0.002, 0.001,
0.0005, 0.0002, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06, 5E-07, 2E-07, 1E-07};
  int autoProv;
const double TLSK_2::VoltPerDivTable[22] = {50, 20, 10, 5, 2, 1, 0.5, 0.2, 0.1, 0.05, 0.02,
0.01, 0.005, 0.002, 0.001, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06};
	HSAMPLE sample;
	HCHANNEL channel;
//---------------------------------------------------------------------------
__fastcall TLSK_2::TLSK_2(TComponent* Owner, WLsk *wlsk)
	: TForm(Owner)
{
	wLsk = wlsk;
	if (wLsk->autoProv) {
	   autoProv=0;
	   sButton1->Enabled=false;
	   sButton2->Enabled=false;
	   sButton3->Enabled=false;
	   sButton4->Enabled=false;
	   sButton12->Enabled=false;
	   timeAuto->Interval=100;
	   timeAuto->Enabled=true;
	}  else autoProv=-1;
	power = true;
	inclLS1 = false;
	inclLS2 = false;
	inclLS3 = false;
	inclLS4 = false;
	inclLS5 = false;

	sListBox2->ItemIndex = 3;       // выставляем первоначальное значение 5 В
	sListBox2->Selected[3] = false;
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[3];
//	sListBox1->ItemIndex = 0;       // выставляем первоначальное значение 10 мс
//	sListBox1->Selected[0] = false;
//	iScope1->TimeBase->SecPerDiv = SecPerDivTable[0];

	sLabel23->Caption ="№" + wLsk->NumberBlock;
	sLabel23->Left = (1920 - sLabel23->Width) / 2;
	sLabel23->Visible = true;

	// Инициализируем массив проверочных переменных CheckLSK2_1
	for(int i=0; i<5; i++) {
		for(int j=0; j<PARAMETERS_LSK2_1; j++)
			wLsk->CheckLSK2_1[i][j] = 0;
	}
	// Инициализируем массив проверочных переменных CheckLSK2_2
	for(int i=0; i<4; i++) {
		for(int j=0; j<PARAMETERS_LSK2_2; j++)
			wLsk->CheckLSK2_2[i][j] = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
//	if(wLsk->RunFlag) {
//		wLsk->UtilStop(1);	// выключение модулей, если они запущены
//	}
	wLsk->UtilStop(power);	// выключение модулей, если они запущены
	power = true;
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sButton1Click(TObject *Sender)
{
	if(inclLS1) {
		Util1Stop();
	} else {
		ResetAllUtill();
		wLsk->RunFlag = 1;

		wLsk->Util1Step1();
		WorkMode = MODE_UTIL1_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 500;
		PaintScopeUtil1();				// Рисуем график
		ExposeVh(0, 0, Vh, Vh);         // Выставляем напряжения

		sButton2->Enabled = false;
		sButton3->Enabled = false;
		sButton4->Enabled = false;
		sButton12->Enabled = false;

		sButton1->Caption = "Сигнал ЛС1\nОТМЕНА";
		inclLS1 = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sButton2Click(TObject *Sender)
{
	if(inclLS2) {
		Util2Stop();
	} else {
		ResetAllUtill();
		wLsk->RunFlag = 1;

		wLsk->Util2Step1();
		WorkMode = MODE_UTIL2_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 500;
		PaintScopeUtil2();				// Рисуем график
		ExposeVh(0, 0, Vh, Vh);         // Выставляем напряжения

		sButton1->Enabled = false;
		sButton3->Enabled = false;
		sButton4->Enabled = false;
		sButton12->Enabled = false;

		sButton2->Caption = "Сигнал ЛС2\nОТМЕНА";
		inclLS2 = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sButton3Click(TObject *Sender)
{
	if(inclLS3) {
		Util3Stop();
	} else {
		ResetAllUtill();
		wLsk->RunFlag = 1;

		wLsk->Util3Step1();
		WorkMode = MODE_UTIL3_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 500;
		PaintScopeUtil3();				 // Рисуем график
		ExposeVh(Vh, 0, Vh, Vh);         // Выставляем напряжения

		sButton1->Enabled = false;
		sButton2->Enabled = false;
		sButton4->Enabled = false;
		sButton12->Enabled = false;

		sButton3->Caption = "Сигнал ЛС3\nОТМЕНА";
		inclLS3 = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sButton4Click(TObject *Sender)
{
	if(inclLS4) {
		Util4Stop();
	} else {
		ResetAllUtill();
		wLsk->RunFlag = 1;

		wLsk->Util4_5Step1(UTIL_4);
		WorkMode = MODE_UTIL4_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 500;
		PaintScopeUtil4();               // Рисуем график
		ExposeVh(Vh, Vh, 0, Vh);         // Выставляем напряжения

		sButton1->Enabled = false;
		sButton2->Enabled = false;
		sButton3->Enabled = false;
		sButton12->Enabled = false;

		sButton4->Caption = "Сигнал ЛС4\nОТМЕНА";
		inclLS4 = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sButton12Click(TObject *Sender)
{
	if(inclLS5) {
		Util5Stop();
	} else {
		ResetAllUtill();
		wLsk->RunFlag = 1;

		wLsk->Util4_5Step1(UTIL_5);
		WorkMode = MODE_UTIL5_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 500;
		PaintScopeUtil5();             	 // Рисуем график
		ExposeVh(Vh, 0, Vh, Vh);         // Выставляем напряжения

		sButton1->Enabled = false;
		sButton2->Enabled = false;
		sButton3->Enabled = false;
		sButton4->Enabled = false;

		sButton12->Caption = "Сигнал ЛС5\nОТМЕНА";
		inclLS5 = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TLSK_2::Timer1Timer(TObject *Sender)
{
	try
	{
		if(WorkMode == MODE_UTIL1_STEP1) {
			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wLsk->CheckUtil1Thread, wLsk, 0, &scope_thread_id);
			hnd_scope_thread2 = hnd_scope_thread;

			wLsk->Util1Step2();
			WorkMode = MODE_UTIL1_STEP2;
			Timer1->Interval = 1300;           // Продолжительность второго шага
			return;
		}

		if(WorkMode == MODE_UTIL1_STEP2) {
//			wLsk->CheckUtil1();
			DisplayResultUtil1_5Step2(wLsk->PerepadUtil1, 1);

			TerminateThread(hnd_scope_thread2, 0);
//			CloseHandle(hnd_scope_thread2);

			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wLsk->CheckUtil1Step3Thread, wLsk, 0, &scope_thread_id);

//			wLsk->Util1Step3();
			wLsk->Util1_2_3Step3();
			WorkMode = MODE_UTIL1_STEP3;
			Timer1->Interval = 300;           // Продолжительность третьего шага
			return;
		}

		if(WorkMode == MODE_UTIL1_STEP3) {
			DisplayResultUtil1_4Step3(wLsk->maxK1_K3, K1_K3_MIN, K1_K3_MAX, 0); // Выводим результат на экран
			Util1Stop();
		}
//***************** Util 2 ***************************
		if(WorkMode == MODE_UTIL2_STEP1) {
//			wLsk->AcqBlockReady[0] = 0;
//			wLsk->AcqBlockReady[1] = 0;

//			wLsk->ReplaceBuf = true;

			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wLsk->CheckUtil2Thread, wLsk, 0, &scope_thread_id);
			hnd_scope_thread2 = hnd_scope_thread;

			wLsk->Util2Step2();
			WorkMode = MODE_UTIL2_STEP2;
			Timer1->Interval = 4000;           // Продолжительность второго шага
			return;
		}

		if(WorkMode == MODE_UTIL2_STEP2) {

			DisplayResultUtil1_5Step2(wLsk->PerepadUtil2, 2);

			TerminateThread(hnd_scope_thread2, 0);
			wLsk->AcqBlockReady[0] = 0;
			wLsk->AcqBlockReady[1] = 0;

			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wLsk->CheckUtil2Step3Thread, wLsk, 0, &scope_thread_id);

			wLsk->Util1_2_3Step3();
			WorkMode = MODE_UTIL2_STEP3;
			Timer1->Interval = 300;           // Продолжительность третьего шага

			return;
		}

		if(WorkMode == MODE_UTIL2_STEP3) {
			DisplayResultUtil1_4Step3(wLsk->maxK1_K3_Util2, K1_K3_MIN_UTIL2, K1_K3_MAX_UTIL2, 1);
			Util2Stop();
		}
//***************** Util 3 ***************************
		if(WorkMode == MODE_UTIL3_STEP1) {
//			wLsk->AcqBlockReady[0] = 0;
//			wLsk->AcqBlockReady[1] = 0;

			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wLsk->CheckUtil3Thread, wLsk, 0, &scope_thread_id);
			hnd_scope_thread2 = hnd_scope_thread;

			wLsk->Util3Step2();
			WorkMode = MODE_UTIL3_STEP2;
			Timer1->Interval = 2000;           // Продолжительность второго шага
			return;
		}

		if(WorkMode == MODE_UTIL3_STEP2) {
			DisplayResultUtil1_5Step2(wLsk->PerepadUtil3, 3);
			TerminateThread(hnd_scope_thread2, 0);           // Принудительно останавливаем поток wLsk->CheckUtil3Thread

			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wLsk->CheckUtil3Step3Thread, wLsk, 0, &scope_thread_id);

			wLsk->Util1_2_3Step3();
			WorkMode = MODE_UTIL3_STEP3;
			Timer1->Interval = 300;           // Продолжительность третьего шага

			return;
		}

		if(WorkMode == MODE_UTIL3_STEP3) {
			DisplayResultUtil1_4Step3(wLsk->maxK1_K3_Util3, K1_K3_MIN_UTIL3, K1_K3_MAX_UTIL3, 2);
			Util3Stop();
		}
//***************** Util 4 ***************************
		if(WorkMode == MODE_UTIL4_STEP1) {
			wLsk->AcqBlockReady[0] = 0;
			wLsk->AcqBlockReady[1] = 0;

			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wLsk->CheckUtil4Thread, wLsk, 0, &scope_thread_id);
			hnd_scope_thread2 = hnd_scope_thread;

			wLsk->Util4_5Step2(UTIL_4);
			WorkMode = MODE_UTIL4_STEP2;
			Timer1->Interval = 2000;           // Продолжительность второго шага
			return;
		}

		if(WorkMode == MODE_UTIL4_STEP2) {
//			wLsk->CheckUtil1();

			TerminateThread(hnd_scope_thread2, 0);
			CloseHandle(hnd_scope_thread2);
			DisplayResultUtil1_5Step2(wLsk->PerepadUtil4, 4);

			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wLsk->CheckUtil4Step3Thread, wLsk, 0, &scope_thread_id);

			WorkMode = MODE_UTIL4_STEP3;
			Timer1->Interval = 200;           // Продолжительность третьего шага
			return;
		}

		if(WorkMode == MODE_UTIL4_STEP3) {
//			wLsk->CheckUtil2();
//			DisplayResultUtil1();

			DisplayResultUtil1_4Step3(wLsk->maxK1_K3_Util4, K1_K3_MIN_UTIL4, K1_K3_MAX_UTIL4, 3);
			Util4Stop();
		}
//***************** Util 5 ***************************
		if(WorkMode == MODE_UTIL5_STEP1) {
			wLsk->AcqBlockReady[0] = 0;
			wLsk->AcqBlockReady[1] = 0;

			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wLsk->CheckUtil5Thread, wLsk, 0, &scope_thread_id);

			wLsk->Util4_5Step2(UTIL_5);
			WorkMode = MODE_UTIL5_STEP2;
			Timer1->Interval = 2000;           // Продолжительность второго шага
			return;
		}

		if(WorkMode == MODE_UTIL5_STEP2) {
//			wLsk->CheckUtil2();
//			DisplayResultUtil1();

			DisplayResultUtil1_5Step2(wLsk->PerepadUtil5, 5);
			Util5Stop();
			Sleep(500);                                      // Выдерживаем паузу
		}
//********************************************
	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::Util1Stop()
{
	wLsk->UtilStop();

	inclLS1 = false;
	Timer1->Enabled = false;
	sButton1->Caption = "Сигнал ЛС1\nЗАПУСК";
	WorkMode = MODE_NULL;
	  if (wLsk->autoProv) {
			sButton2->Enabled = false;
			sButton3->Enabled = false;
			sButton4->Enabled = false;
			sButton12->Enabled = false;
			autoProv=1;
			timeZader->Interval=15;
			timeZader->Enabled=true;
	  }
			else
			  {
				sButton2->Enabled = true;
				sButton3->Enabled = true;
				sButton4->Enabled = true;
				sButton12->Enabled = true;
			   }
	CheckEndLSK2();

}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::Util2Stop()
{
	wLsk->UtilStop();

	inclLS2 = false;
	Timer1->Enabled = false;
	sButton2->Caption = "Сигнал ЛС2\nЗАПУСК";
	WorkMode = MODE_NULL;
	 if (wLsk->autoProv)
	 {
			sButton1->Enabled = false;
			sButton3->Enabled = false;
			sButton4->Enabled = false;
			sButton12->Enabled = false;
			autoProv=2;
			timeZader->Interval=15;
			timeZader->Enabled=true;
	  }
			else
			{
				sButton1->Enabled = true;
				sButton3->Enabled = true;
				sButton4->Enabled = true;
				sButton12->Enabled = true;
			 }
	CheckEndLSK2();
//	autoProv=2;
//	timeZader->Interval=15;
//	timeZader->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::Util3Stop()
{
	wLsk->UtilStop();

	inclLS3 = false;
	Timer1->Enabled = false;
	sButton3->Caption = "Сигнал ЛС3\nЗАПУСК";
	WorkMode = MODE_NULL;

	if (wLsk->autoProv)
	 {
			sButton1->Enabled = false;
			sButton2->Enabled = false;
			sButton4->Enabled = false;
			sButton12->Enabled = false;
			autoProv=3;
			timeZader->Interval=15;
			timeZader->Enabled=true;
	  }
			else
		   {
				sButton1->Enabled = true;
				sButton2->Enabled = true;
				sButton4->Enabled = true;
				sButton12->Enabled = true;
			 }
	CheckEndLSK2();
//	autoProv=3;
//	timeZader->Interval=15;
//	timeZader->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::Util4Stop()
{
	wLsk->UtilStop();

	inclLS4 = false;
	Timer1->Enabled = false;
	sButton4->Caption = "Сигнал ЛС4\nЗАПУСК";
	WorkMode = MODE_NULL;
    if (wLsk->autoProv)
	 {
			sButton1->Enabled = false;
			sButton3->Enabled = false;
			sButton2->Enabled = false;
			sButton12->Enabled = false;
			autoProv=4;
			timeZader->Interval=15;
			timeZader->Enabled=true;
	  }
			else
			{
				sButton1->Enabled = true;
				sButton2->Enabled = true;
				sButton3->Enabled = true;
				sButton12->Enabled = true;
			}
	CheckEndLSK2();
//	autoProv=4;
//	timeZader->Interval=15;
//	timeZader->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::Util5Stop()
{
	wLsk->UtilStop();

	inclLS5 = false;
	Timer1->Enabled = false;
	sButton12->Caption = "Сигнал ЛС5\nЗАПУСК";
	WorkMode = MODE_NULL;
    if (wLsk->autoProv)
	 {
			sButton1->Enabled = false;
			sButton3->Enabled = false;
			sButton4->Enabled = false;
			sButton2->Enabled = false ;
				sButton1->Visible=true;
				sButton2->Visible = true;
				sButton3->Visible = true;
				sButton4->Visible = true;
				sButton12->Visible = true;

	  }
			else
		   {
			    sButton1->Enabled = true;
				sButton2->Enabled = true;
				sButton3->Enabled = true;
				sButton4->Enabled = true;
			}
	CheckEndLSK2(); //проверка окончена. вывод результатов
//	autoProv=1;
//	timeZader->Interval=15;
//	timeZader->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::PaintScopeUtil1()
{
	int i;

	iScope1->DataBlockBegin();

	iScope1->AddChannelData(0, 0);
	iScope1->AddChannelData(1, 0);
	iScope1->AddChannelData(2, 0);

	for(i=0; i < 100; i++) {
		iScope1->AddChannelData(0, 8.5);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 0);
	}

	for(i=0; i < 8; i++) {
		iScope1->AddChannelData(0, 0);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 0);
	}

	for(i=0; i < 125; i++) {
		iScope1->AddChannelData(0, 0);
		iScope1->AddChannelData(1, 0);
		iScope1->AddChannelData(2, 0);
	}

	for(i=0; i < 117; i++) {
		iScope1->AddChannelData(0, 0);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 0);
	}

	for(i=0; i < 150; i++) {
		iScope1->AddChannelData(0, 8.5);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 0);
	}

	iScope1->DataBlockEnd();
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::PaintScopeUtil2()
{
	int i;

	iScope1->DataBlockBegin();

	iScope1->AddChannelData(0, 0);
	iScope1->AddChannelData(1, 0);
	iScope1->AddChannelData(2, 0);

	for(i=0; i < 100; i++) {
		iScope1->AddChannelData(0, 8.5);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 0);
	}

	for(i=0; i < 250; i++) {
		iScope1->AddChannelData(0, 0);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 0);
	}

	for(i=0; i < 150; i++) {
		iScope1->AddChannelData(0, 8.5);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 0);
	}

	iScope1->DataBlockEnd();
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::PaintScopeUtil3()
{
	int i,j;

	iScope1->DataBlockBegin();

	iScope1->AddChannelData(0, 0);
	iScope1->AddChannelData(1, 0);
	iScope1->AddChannelData(2, 0);

	for(i=0; i < 100; i++) {
		iScope1->AddChannelData(0, 8.5);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 8.5);
	}

	for(i=0; i < 125; i++) {
		iScope1->AddChannelData(0, 0);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 8.5);
	}

	for(i=0; i < 8; i++) {
		iScope1->AddChannelData(0, 0);
		iScope1->AddChannelData(1, 0);
		iScope1->AddChannelData(2, 8.5);
	}

	for(i=0; i < 84; i++) {
		iScope1->AddChannelData(0, 0);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 8.5);
	}

	for(i=0; i < 183; i++) {
		iScope1->AddChannelData(0, 8.5);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 8.5);
	}

	iScope1->DataBlockEnd();
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::PaintScopeUtil4()
{
	int i;

	iScope1->DataBlockBegin();

	iScope1->AddChannelData(0, 0);
	iScope1->AddChannelData(1, 0);
	iScope1->AddChannelData(2, 0);

	for(i=0; i < 125; i++) {
		iScope1->AddChannelData(0, 8.5);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 8.5);
	}

	for(i=0; i < 125; i++) {
		iScope1->AddChannelData(0, 0);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 8.5);
	}

//	for(i=0; i < 125; i++) {
//		iScope1->AddChannelData(0, 0);
//		iScope1->AddChannelData(1, 0);
//		iScope1->AddChannelData(2, 8.5);
//	}
		for(i=0; i < 62; i++) {
		iScope1->AddChannelData(0, 0);
		iScope1->AddChannelData(1, 0);
		iScope1->AddChannelData(2, 8.5);
	}
		for(i=0; i < 63; i++) {
		iScope1->AddChannelData(0, 0);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 8.5);
	}
	for(i=0; i < 125; i++) {
		iScope1->AddChannelData(0, 8.5);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 8.5);
	}

	iScope1->DataBlockEnd();
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::PaintScopeUtil5()
{
	int i;

	iScope1->DataBlockBegin();

	iScope1->AddChannelData(0, 0);			// Минимальное значение
	iScope1->AddChannelData(1, 0);
	iScope1->AddChannelData(2, 0);

	for(i=0; i < 125; i++) {
		iScope1->AddChannelData(0, 8.5);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 8.5);
	}

	for(i=0; i < 125; i++) {
		iScope1->AddChannelData(0, 0);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 8.5);
	}

	for(i=0; i < 125; i++) {
		iScope1->AddChannelData(0, 0);
		iScope1->AddChannelData(1, 0);
		iScope1->AddChannelData(2, 8.5);
	}

	for(i=0; i < 125; i++) {
		iScope1->AddChannelData(0, 0);
		iScope1->AddChannelData(1, 8.5);
		iScope1->AddChannelData(2, 8.5);
	}

	iScope1->DataBlockEnd();
}
//---------------------------------------------------------------------------
//void __fastcall TLSK_2::MassiveScope()
//{
//	int i = 0;
//
//	for(i=0; i<2; i++) {
//		a[i++] = 8;
//		a[i] = 7;
//	}
//
//	for(i=2; i < 500; i++) {
//		a[i] = 6;
////		a[i] = 0;
//	}
//
////	for(i=250; i < 500; i++) {
////		a[i] = 3.5;
//////		a[i] = 1;
////	}
//	int t=0;
//}//---------------------------------------------------------------------------
//void __fastcall TLSK_2::DisplayResultUtil1Step2()
//{
//	if(wLsk->PerepadUtil1[0] == false) {
//		sPanel16->Font->Color = clRed;
//		sPanel16->Caption = "Нет перекл. ОШИБКА";
//	} else {
//		sPanel16->Font->Color = clLime;
//		sPanel16->Caption = "Порог 1 - 0 НОРМА";
//	}
//
//	if(wLsk->PerepadUtil1[1] == true) {
//		sPanel18->Font->Color = clRed;
//		sPanel18->Caption = "Нет перекл. ОШИБКА";
//	} else {
//		sPanel18->Font->Color = clLime;
//		sPanel18->Caption = "Порог 1 - 0 НОРМА";
//	}
//
//	if(wLsk->PerepadUtil1[2] == true) {
//		sPanel23->Font->Color = clRed;
//		sPanel23->Caption = "Нет перекл. ОШИБКА";
//	} else {
//		sPanel23->Font->Color = clLime;
//		sPanel23->Caption = "Порог 1 - 0 НОРМА";
//    }
//
//	if(wLsk->PerepadUtil1[3] == true) {
//		sPanel34->Font->Color = clRed;
//		sPanel34->Caption = "Нет перекл. ОШИБКА";
//	} else {
//		sPanel34->Font->Color = clLime;
//		sPanel34->Caption = "Порог 1 - 0 НОРМА";
//	}
//
//	if(wLsk->PerepadUtil1[4] == true) {
//		sPanel29->Font->Color = clRed;
//		sPanel29->Caption = "Нет перекл. ОШИБКА";
//	} else {
//		sPanel29->Font->Color = clLime;
//		sPanel29->Caption = "Порог 1 - 0 НОРМА";
//	}
//}//---------------------------------------------------------------------------void __fastcall TLSK_2::DisplayResultUtil1_5Step2(bool *PerepadUtil, int numberUtil){
	switch(numberUtil) {
		case 1:{if(PerepadUtil[0] == false) {
					sPanel16->Font->Color = clRed;
					sPanel16->Caption = "Нет перекл. ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][0] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel16->Font->Color = clLime;
					sPanel16->Caption = "Порог 0 - 1 НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][0] = 1;
				}

				if(PerepadUtil[1] == true) {
					sPanel18->Font->Color = clRed;
					sPanel18->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][1] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel18->Font->Color = clLime;
					sPanel18->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][1] = 1;
				}

				if(PerepadUtil[2] == true) {
					sPanel23->Font->Color = clRed;
					sPanel23->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][2] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel23->Font->Color = clLime;
					sPanel23->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][2] = 1;
				}

				if(PerepadUtil[3] == true) {
					sPanel34->Font->Color = clRed;
					sPanel34->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][3] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel34->Font->Color = clLime;
					sPanel34->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][3] = 1;
				}

				if(PerepadUtil[4] == true) {
					sPanel29->Font->Color = clRed;
					sPanel29->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][4] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel29->Font->Color = clLime;
					sPanel29->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][4] = 1;
				}
				break;}
		case 2:{if(PerepadUtil[0] == true) {
					sPanel16->Font->Color = clRed;
					sPanel16->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][0] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel16->Font->Color = clLime;
					sPanel16->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][0] = 1;
				}

				if(PerepadUtil[1] == false) {
					sPanel18->Font->Color = clRed;
					sPanel18->Caption = "Нет перекл. ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][1] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel18->Font->Color = clLime;
					sPanel18->Caption = "Порог 1 - 0 НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][1] = 1;
				}

				if(PerepadUtil[2] == true) {
					sPanel23->Font->Color = clRed;
					sPanel23->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][2] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel23->Font->Color = clLime;
					sPanel23->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][2] = 1;
				}

				if(PerepadUtil[3] == true) {
					sPanel34->Font->Color = clRed;
					sPanel34->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][3] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel34->Font->Color = clLime;
					sPanel34->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][3] = 1;
				}

				if(PerepadUtil[4] == true) {
					sPanel29->Font->Color = clRed;
					sPanel29->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][4] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel29->Font->Color = clLime;
					sPanel29->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][4] = 1;
				}
				break;}
		case 3:{if(PerepadUtil[0] == true) {
					sPanel16->Font->Color = clRed;
					sPanel16->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][0] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel16->Font->Color = clLime;
					sPanel16->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][0] = 1;
				}

				if(PerepadUtil[1] == true) {
					sPanel18->Font->Color = clRed;
					sPanel18->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][1] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel18->Font->Color = clLime;
					sPanel18->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][1] = 1;
				}

				if(PerepadUtil[2] == false) {
					sPanel23->Font->Color = clRed;
					sPanel23->Caption = "Нет перекл. ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][2] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel23->Font->Color = clLime;
					sPanel23->Caption = "Порог 0 - 1 НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][2] = 1;
				}

				if(PerepadUtil[3] == true) {
					sPanel34->Font->Color = clRed;
					sPanel34->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][3] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel34->Font->Color = clLime;
					sPanel34->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][3] = 1;
				}

				if(PerepadUtil[4] == true) {
					sPanel29->Font->Color = clRed;
					sPanel29->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][4] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel29->Font->Color = clLime;
					sPanel29->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][4] = 1;
				}
				break;}
		case 4:{if(PerepadUtil[0] == true) {
					sPanel16->Font->Color = clRed;
					sPanel16->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][0] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel16->Font->Color = clLime;
					sPanel16->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][0] = 1;
				}

				if(PerepadUtil[1] == true) {
					sPanel18->Font->Color = clRed;
					sPanel18->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][1] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel18->Font->Color = clLime;
					sPanel18->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][1] = 1;
				}

				if(PerepadUtil[2] == true) {
					sPanel23->Font->Color = clRed;
					sPanel23->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][2] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel23->Font->Color = clLime;
					sPanel23->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][2] = 1;
				}

				if(PerepadUtil[3] == false) {
					sPanel34->Font->Color = clRed;
					sPanel34->Caption = "Нет перекл. ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][3] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel34->Font->Color = clLime;
					sPanel34->Caption = "Порог 0 - 1 НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][3] = 1;
				}

				if(PerepadUtil[4] == true) {
					sPanel29->Font->Color = clRed;
					sPanel29->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][4] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel29->Font->Color = clLime;
					sPanel29->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][4] = 1;
				}
				break;}
		case 5:{if(PerepadUtil[0] == true) {
					sPanel16->Font->Color = clRed;
					sPanel16->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][0] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel16->Font->Color = clLime;
					sPanel16->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][0] = 1;
				}

				if(PerepadUtil[1] == true) {
					sPanel18->Font->Color = clRed;
					sPanel18->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][1] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel18->Font->Color = clLime;
					sPanel18->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][1] = 1;
				}

				if(PerepadUtil[2] == true) {
					sPanel23->Font->Color = clRed;
					sPanel23->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][2] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel23->Font->Color = clLime;
					sPanel23->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][2] = 1;
				}

				if(PerepadUtil[3] == true) {
					sPanel34->Font->Color = clRed;
					sPanel34->Caption = "ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][3] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel34->Font->Color = clLime;
					sPanel34->Caption = "НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][3] = 1;
				}

				if(PerepadUtil[4] == false) {
					sPanel29->Font->Color = clRed;
					sPanel29->Caption = "Нет перекл. ОШИБКА";
					wLsk->CheckLSK2_1[numberUtil - 1][4] = 2;
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else {
					sPanel29->Font->Color = clLime;
					sPanel29->Caption = "Порог 0 - 1 НОРМА";
					wLsk->CheckLSK2_1[numberUtil - 1][4] = 1;
				}
				break;}
	}
}//---------------------------------------------------------------------------
// double *valueK1_K3 - значение из потока
// double Min - минимальное значение для сравнения
// double Max - максимальное значение для сравнения
// int numberUtil - индекс для проверочной переменной (номер утилиты - 1)
static char Text1[] = "U = %.1f В";
void __fastcall TLSK_2::DisplayResultUtil1_4Step3(double *valueK1_K3, double Min, double Max, int numberUtil)
{
	AnsiString form_string;

	if(valueK1_K3[0] >= Min && valueK1_K3[0] <= Max) {
		form_string.printf( Text1, valueK1_K3[0] );
		sPanel22->Caption = form_string;
		sPanel22->Font->Color = clLime;
		sPanel21->Font->Color = clLime;
		sPanel21->Caption = "НОРМА";
		wLsk->CheckLSK2_2[numberUtil][0] = 1;
	} else {
		form_string.printf( Text1, valueK1_K3[0] );
		sPanel22->Caption = form_string;
		sPanel22->Font->Color = clRed;
		sPanel21->Font->Color = clRed;
		sPanel21->Caption = "ОШИБКА";
		wLsk->CheckLSK2_2[numberUtil][0] = 2;
			BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
	}

	if(valueK1_K3[1] >= Min && valueK1_K3[1] <= Max) {
		form_string.printf( Text1, valueK1_K3[1] );
		sPanel5->Caption = form_string;
		sPanel5->Font->Color = clLime;
		sPanel4->Font->Color = clLime;
		sPanel4->Caption = "НОРМА";
		wLsk->CheckLSK2_2[numberUtil][1] = 1;
	} else {
		form_string.printf( Text1, valueK1_K3[1] );
		sPanel5->Caption = form_string;
		sPanel5->Font->Color = clRed;
		sPanel4->Font->Color = clRed;
		sPanel4->Caption = "ОШИБКА";
		wLsk->CheckLSK2_2[numberUtil][1] = 2;
			BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
	}

	if(valueK1_K3[2] >= Min && valueK1_K3[2] <= Max) {
		form_string.printf( Text1, valueK1_K3[2] );
		sPanel7->Caption = form_string;
		sPanel7->Font->Color = clLime;
		sPanel6->Font->Color = clLime;
		sPanel6->Caption = "НОРМА";
		wLsk->CheckLSK2_2[numberUtil][2] = 1;
	} else {
		form_string.printf( Text1, valueK1_K3[2] );
		sPanel7->Caption = form_string;
		sPanel7->Font->Color = clRed;
		sPanel6->Font->Color = clRed;
		sPanel6->Caption = "ОШИБКА";
		wLsk->CheckLSK2_2[numberUtil][2] = 2;
			BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
	}
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::ResetAllUtill()
{
	sPanel16->Font->Color = clWhite;
	sPanel16->Caption = "Порог 0 - 1\n";

	sPanel18->Font->Color = clWhite;
	sPanel18->Caption = "Порог 0 - 1\n";

	sPanel23->Font->Color = clWhite;
	sPanel23->Caption = "Порог 0 - 1\n";

	sPanel34->Font->Color = clWhite;
	sPanel34->Caption = "Порог 0 - 1\n";

	sPanel29->Font->Color = clWhite;
	sPanel29->Caption = "Порог 0 - 1\n";

	iScope1->Plot->ClearAllData();

	sPanel22->Font->Color = clWhite;
	sPanel22->Caption = "U =";
	sPanel21->Caption = "";

	sPanel5->Font->Color = clWhite;
	sPanel5->Caption = "U =";
	sPanel4->Caption = "";

	sPanel7->Font->Color = clWhite;
	sPanel7->Caption = "U =";
	sPanel6->Caption = "";
}
//---------------------------------------------------------------------------
static char Text2[] = "Uвх = %.1f В";
void __fastcall TLSK_2::ExposeVh(double Vh3, double Vh4, double Vh5, double VhUl)
{
	AnsiString form_string;

	form_string.printf( Text2, Vh3 );
	sLabel21->Caption = form_string;

	form_string.printf( Text2, Vh4 );
	sLabel16->Caption = form_string;

	form_string.printf( Text2, Vh5 );
	sLabel17->Caption = form_string;

	form_string.printf( Text2, VhUl );
	sLabel12->Caption = form_string;
}
int provMess = -1;
//---------------------------------------------------------------------------
void __fastcall TLSK_2::CheckEndLSK2()
{
	for(int i=0; i<5; i++) {
		for(int j=0; j<PARAMETERS_LSK2_1; j++)
			if(wLsk->CheckLSK2_1[i][j] == 0) return;
	}

	for(int i=0; i<3; i++) {
		for(int j=0; j<PARAMETERS_LSK2_2; j++)
			if(wLsk->CheckLSK2_2[i][j] == 0) return;
	}

	sButton13->Enabled = true;
	sButton14->Enabled = true;

	sButton5->Enabled = true;
	sButton6->Enabled = true;
	sButton7->Enabled = true;

	for(int i=0; i<5; i++) {
		for(int j=0; j<PARAMETERS_LSK2_1; j++) {
			if(wLsk->CheckLSK2_1[i][j] == 2) {
			sPanel3->Font->Color = clRed;
			wLsk->checkBlock = false;			// Один из порогов не прошел
			provMess++;
//			if (provMess==0)
//			 {
//			  BASS_Init(-1,44100,0,0,NULL);
//			  sample = BASS_SampleLoad(false,"2.wav",0,0,1,BASS_SAMPLE_MONO) ;
//			  channel =  BASS_SampleGetChannel(sample,false);
//			  BASS_ChannelPlay(channel,false);
//			 }
			sPanel3->Caption = "Блок неисправен";
			return;
			}
		}
	}

	for(int i=0; i<3; i++) {
		for(int j=0; j<PARAMETERS_LSK2_2; j++) {
			if(wLsk->CheckLSK2_2[i][j] == 2) {
			wLsk->checkBlock = false;			// В блоке что-то неисправно
			sPanel3->Font->Color = clRed;
				provMess++;
			if (provMess==0)
			 {
			  BASS_Init(-1,44100,0,0,NULL);
			  sample = BASS_SampleLoad(false,"2.wav",0,0,1,BASS_SAMPLE_MONO) ;
			  channel =  BASS_SampleGetChannel(sample,false);
			  BASS_ChannelPlay(channel,false);
			 }
			sPanel3->Caption = "Блок неисправен";
			return;
			}
		}
	}
    	BASS_Init(-1,44100,0,0,NULL);
		sample = BASS_SampleLoad(false,"1.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
	sPanel3->Font->Color = clLime;
	sPanel3->Caption = "Параметры в норме";

	wLsk->checkBlock = true;				// Пороги исправны
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sButton5Click(TObject *Sender)
{
//	wLsk->UtilStop(1);		// Стоп с отключением питания

	this->Close();          // При закрытии отключается питание
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sButton6Click(TObject *Sender)
{
	TLSK_2 *LSK_2 = new TLSK_2(0, wLsk);
	if(!LSK_2) return;
	LSK_2->Show();
	power = false;

	this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sButton7Click(TObject *Sender)
{
	TLSK_1 *LSK_1 = new TLSK_1(0, wLsk->NumberBlock);
	if(!LSK_1) return;
	LSK_1->Show();

	this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sButton13Click(TObject *Sender)
{
	wLsk->WriteExcel("\\RaportLSK.xlsx", true);
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sButton14Click(TObject *Sender)
{
	wLsk->WriteExcel("\\RaportLSK.xlsx", false);

	TVsReport *vsReport = new TVsReport(this, wLsk->NumberBlock);
	if(!vsReport) return;
	vsReport->ShowModal();
	delete vsReport;

	DeleteFile(GetCurrentDir() + "\\~" + wLsk->NumberBlock + ".xlsx");
}
//---------------------------------------------------------------------------

//------------------------/ Обработчики графиков /---------------------------
void __fastcall TLSK_2::sListBox1Click(TObject *Sender)
{
	sListBox1->Selected[sListBox1->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sListBox2Click(TObject *Sender)
{
	sListBox2->Selected[sListBox2->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------

void __fastcall TLSK_2::sListBox3Click(TObject *Sender)
{
	sListBox3->Selected[sListBox3->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sListBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{
	FocusControl(iScope1); // переводим фокус, чтобы колёсиком мышки нельзя было поменять значение
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sListBox1VScroll(TObject *Sender)
{
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[sListBox1->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sListBox2VScroll(TObject *Sender)
{
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox2->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TLSK_2::sListBox3VScroll(TObject *Sender)
{
	if(sListBox3->TopIndex == 0) iScope1->Channel[0]->Coupling = COPLING_AC;
		else iScope1->Channel[0]->Coupling = COPLING_DC;
}
//---------------------------------------------------------------------------
//void __fastcall TLSK_2::EnableButton(bool enable)
//{
//	sButton1->Enabled = enable;
//	sButton2->Enabled = enable;
//	sButton3->Enabled = enable;
//	sButton4->Enabled = enable;
//	sButton12->Enabled = enable;
//}










void __fastcall TLSK_2::timeAutoTimer(TObject *Sender)
{
   if (autoProv==0)    //ЛС1
		{
         	ResetAllUtill();
			wLsk->RunFlag = 1;

			wLsk->Util1Step1();
			WorkMode = MODE_UTIL1_STEP1;
			Timer1->Enabled = true;
			Timer1->Interval = 500;
			PaintScopeUtil1();				// Рисуем график
			ExposeVh(0, 0, Vh, Vh);         // Выставляем напряжения
			sButton1->Visible=true;
			sButton2->Visible = false;
			sButton3->Visible = false;
			sButton4->Visible = false;
			sButton12->Visible = false;
			sButton2->Enabled = false;
			sButton3->Enabled = false;
			sButton4->Enabled = false;
			sButton12->Enabled = false;

			sButton1->Caption = "Сигнал ЛС1\nОТМЕНА";
			inclLS1 = true;
			timeAuto->Enabled=false;
		 }
		else
			if (autoProv==1)  //ЛС2
				{
				sButton1->Visible=false;
				sButton2->Visible = true;
				sButton3->Visible = false;
				sButton4->Visible = false;
				sButton12->Visible = false;
				    ResetAllUtill();
					wLsk->RunFlag = 1;

					wLsk->Util2Step1();
					WorkMode = MODE_UTIL2_STEP1;
					Timer1->Enabled = true;
					Timer1->Interval = 500;
					PaintScopeUtil2();				// Рисуем график
					ExposeVh(0, 0, Vh, Vh);         // Выставляем напряжения

					sButton1->Enabled = false;
					sButton3->Enabled = false;
					sButton4->Enabled = false;
					sButton12->Enabled = false;

					sButton2->Caption = "Сигнал ЛС2\nОТМЕНА";
					inclLS2 = true;
					timeAuto->Enabled=false;
				}
				else
						if (autoProv==2)     //ЛС3
							{
							sButton1->Visible=false;
							sButton2->Visible = false;
							sButton3->Visible = true;
							sButton4->Visible = false;
							sButton12->Visible = false;
                                ResetAllUtill();
								wLsk->RunFlag = 1;

								wLsk->Util3Step1();
								WorkMode = MODE_UTIL3_STEP1;
								Timer1->Enabled = true;
								Timer1->Interval = 500;
								PaintScopeUtil3();				 // Рисуем график
								ExposeVh(Vh, 0, Vh, Vh);         // Выставляем напряжения

								sButton1->Enabled = false;
								sButton2->Enabled = false;
								sButton4->Enabled = false;
								sButton12->Enabled = false;

								sButton3->Caption = "Сигнал ЛС3\nОТМЕНА";
								inclLS3 = true;
								timeAuto->Enabled=false;
							}
							else
									if (autoProv==3)  //ЛС4
										{
											sButton1->Visible=false;
											sButton2->Visible = false;
											sButton3->Visible = false;
											sButton4->Visible = true;
											sButton12->Visible = false;
                                            ResetAllUtill();
											wLsk->RunFlag = 1;

											wLsk->Util4_5Step1(UTIL_4);
											WorkMode = MODE_UTIL4_STEP1;
											Timer1->Enabled = true;
											Timer1->Interval = 500;
											PaintScopeUtil4();               // Рисуем график
											ExposeVh(Vh, Vh, 0, Vh);         // Выставляем напряжения

											sButton1->Enabled = false;
											sButton2->Enabled = false;
											sButton3->Enabled = false;
											sButton12->Enabled = false;

											sButton4->Caption = "Сигнал ЛС4\nОТМЕНА";
											inclLS4 = true;
											timeAuto->Enabled=false;
										}
										else
											if (autoProv==4)   //ЛС5
												{
													sButton1->Visible=false;
													sButton2->Visible = false;
													sButton3->Visible = false;
													sButton4->Visible = false;
													sButton12->Visible = true;
													ResetAllUtill();
													wLsk->RunFlag = 1;

													wLsk->Util4_5Step1(UTIL_5);
													WorkMode = MODE_UTIL5_STEP1;
													Timer1->Enabled = true;
													Timer1->Interval = 500;
													PaintScopeUtil5();             	 // Рисуем график
													ExposeVh(Vh, 0, Vh, Vh);         // Выставляем напряжения

													sButton1->Enabled = false;
													sButton2->Enabled = false;
													sButton3->Enabled = false;
													sButton4->Enabled = false;

													sButton12->Caption = "Сигнал ЛС5\nОТМЕНА";
													inclLS5 = true;
													timeAuto->Enabled=false;
												}

}
//---------------------------------------------------------------------------
 int   time_step=0;
void __fastcall TLSK_2::timeZaderTimer(TObject *Sender)
{
	if (time_step>25)
	{
		timeAuto->Interval = 10;
		timeAuto->Enabled = true;
		timeZader->Enabled=false;
	}
 time_step++;
}
//---------------------------------------------------------------------------

