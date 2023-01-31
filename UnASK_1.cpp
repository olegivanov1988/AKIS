//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "UnASK_1.h"
#include "WAsk.h"
  #include "UnVisibleReport.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sButton"
#pragma link "sGroupBox"
#pragma link "sBitBtn"
#pragma link "sLabel"
#pragma link "sPanel"
#pragma link "sComboBoxes"
#pragma link "iComponent"
#pragma link "iCustomComponent"
#pragma link "iPlotComponent"
#pragma link "iVCLComponent"
#pragma link "iXYPlot"
#pragma link "Chart"
#pragma link "TeEngine"
#pragma link "TeeProcs"
#pragma link "sBevel"
#pragma link "iLed"
#pragma link "iLedRound"
#pragma link "iScope"
#pragma link "sRadioButton"
#pragma link "sListBox"
#pragma link "sUpDown"
#pragma link "sSpeedButton"
#pragma link "sEdit"
#pragma resource "*.dfm"
//TASK_1 *ASK_1;

#include <bass.h>
#include <process.h>
#include <stdio.h>
#include <windows.h>
//static DWORD WINAPI ScopeThread(LPVOID param);
const double TASK_1::SecPerDivTable[19] = {0.1, 0.05, 0.02, 0.01, 0.005, 0.002, 0.001,
0.0005, 0.0002, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06, 5E-07, 2E-07, 1E-07};

const double TASK_1::VoltPerDivTable[22] = {50, 20, 10, 5, 2, 1, 0.5, 0.2, 0.1, 0.05, 0.02,
0.01, 0.005, 0.002, 0.001, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06};
HSAMPLE sample;
	HCHANNEL channel;
//---------------------------------------------------------------------------
__fastcall TASK_1::TASK_1(TComponent* Owner, AnsiString number_block)
	: TForm(Owner)
{
	wAsk = new WAsk;
	incl = false;                   // Переменные, которые фиксируют нажатие
	inclK10 = false;                // кнопок проверок
	inclK9 = false;
	inclK8K7 = false;
	power = true;

	sListBox2->ItemIndex = 5;       // выставляем первоначальное значение 1 В
	sListBox2->Selected[5] = false;
	iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[5];
	sListBox1->ItemIndex = 3;       // выставляем первоначальное значение 10 мс
	sListBox1->Selected[3] = false;
	iScope2->TimeBase->SecPerDiv = SecPerDivTable[3];

	sListBox4->ItemIndex = 3;       // выставляем первоначальное значение 10 мс
	sListBox4->Selected[3] = false;
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[3];
	sListBox5->ItemIndex = 5;       // выставляем первоначальное значение 1 В
	sListBox5->Selected[5] = false;
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[5];

	wAsk->NumberBlock = number_block;

	// Проверяем повторно мы на этом экране или нет
	if(wAsk->NumberBlock != "-1") {
		sBitBtn1->Enabled = true;
		sButton7->Visible = false;
		sEdit1->Visible = false;
		sLabel8->Visible = false;

		sLabel10->Caption ="№" + wAsk->NumberBlock;
		sLabel10->Left = (1920 - sLabel10->Width) / 2;
		sLabel10->Visible = true;

	} else {
		sButton7->Visible = true;
		sEdit1->Visible = true;
		sLabel8->Visible = true;
	}

}
//---------------------------------------------------------------------------
void __fastcall TASK_1::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
//	if(wAsk->RunFlag) {
//		wAsk->UtilStop(1);	// выключение модулей, если они запущены
//	}

	wAsk->UtilStop(power);	// выключение модулей, если они запущены
	power = true;
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sBitBtn1Click(TObject *Sender)
{
//	if(wAsk->IsOpenedLTR34() == 0) return; // на случай быстрого повторного нажатия кнопки

	if(inclK8K7)
	{
		sBitBtn1->Down = true;
//		wAsk->Util0Step1();
		Util4Stop();                    // Там отключаем питание

		// Очистка графиков и значений перед выключением питания
		ResetK7K8();
		ResetK10();
		ResetLighteTable();
		iScope1->Plot->ClearAllData();
		iScope2->Plot->ClearAllData();

		sButton3->Enabled = false;
		sButton4->Enabled = false;
		sButton5->Enabled = false;

		sButton1->Enabled = false;
		sButton11->Enabled = false;
		sButton10->Enabled = false;
		sButton2->Enabled = false;
		sPanel3->Font->Color = clBlack;
		sPanel3->Caption = "Проверка блока";
		sLabel3->Caption = "Перейти к экрану 2?";
	} else
	{
		try
		{
			sBitBtn1->Enabled = false;

			wAsk->ReplaceBuf = true;
			wAsk->Util0Step1();

			Timer1->Interval = 2000;            // Время ожидания результата напряжений
			Timer1->Enabled = true;
			inclK8K7 = true;
			WorkMode = MODE_UTIL4;
			sBitBtn1->Down = true;

			// Инициализируем массив проверочных переменных
			for(int i=0; i<PARAMETERS_ASK1; i++) {
				wAsk->CheckASK1[i] = 0;
			}

			wAsk->ReplaceBuf2 = true;
		}
		catch( Exception &e )
		{
			MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
		}
	}
}
//---------------------------------------------------------------------------
static char Text1[] = "Uупр = %.0f В";
static char Text2[] = "Uупр = 0 B";
static char Text3[] = "Uопор = %.3f В";
static char Text4[] = "U = %.2f В";
static char Text5[] = "Uвт+ = %.1f В";
static char Text6[] = "Uвт- = %.1f В";
void __fastcall TASK_1::sButton3Click(TObject *Sender)
{
	if(incl)
	{
		Util1Stop();

//		sButton4->Enabled = true;
//		sButton5->Enabled = true;
//		sBitBtn1->Enabled = true;
	} else
	{
		try
		{
			ResetLighteTable();
			wAsk->Util1Step1();
			AnsiString form_string;
			sButton3->Caption = "В Ы К Л Ю Ч И Т Ь";
			form_string.printf( Text1, wAsk->VoltArrayTable[0][0] );
			sLabUpr1->Caption = form_string;
			form_string.printf( Text1, wAsk->VoltArrayTable[0][1] );
			sLabUpr2->Caption = form_string;
			form_string.printf( Text1, wAsk->VoltArrayTable[0][2] );
			sLabUpr3->Caption = form_string;
			Timer1->Interval = 150;                                    // Быстрый запуск
			Timer1->Enabled = true;
			incl = true;
			WorkMode = MODE_UTIL1;
//			LighteTable(wAsk->NumStep);

			sButton4->Enabled = false;
			sButton5->Enabled = false;
			sBitBtn1->Enabled = false;
		}
		catch( Exception &e )
		{
			MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
		}
	}
//	k_pressure = 0; // обнуляем номер строки таблицы напряжений
}
//---------------------------------------------------------------------------

void __fastcall TASK_1::sButton5Click(TObject *Sender)
{
	sLabUpr1->Caption = "F = 62,5 Гц";
	sLabUpr2->Caption = "F = 31,25 Гц";
	sLabUpr3->Caption = "F=15,625 Гц";

	if(inclK9)
	{
		Util2Stop();

//		sButton3->Enabled = true;
//		sButton4->Enabled = true;
//		sBitBtn1->Enabled = true;
	} else {
		try
		{
			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			wAsk->RunFlag = 1;
			scope = 1;
			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

			wAsk->Util2Step1();
			Timer1->Interval = 5500;
			Timer1->Enabled = true;
			inclK9 = true;
			WorkMode = MODE_UTIL2;
			sButton5->Caption = "ОТМЕНА";

			sButton3->Enabled = false;
			sButton4->Enabled = false;
			sBitBtn1->Enabled = false;

			sButton6->Enabled = true;           // Кнопка отсутствия ступенек
		}
		catch( Exception &e )
		{
			MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sButton4Click(TObject *Sender)
{
	try
	{
		if(inclK10)
			{
				Util3Stop();

//				sButton3->Enabled = true;
//				sButton5->Enabled = true;
//				sBitBtn1->Enabled = true;
			} else
			{
				HANDLE hnd_scope_thread;
				DWORD scope_thread_id;
				DWORD thread_status;

				ResetK10();                         // Обнуляем всё что связано с точкой К10
				wAsk->RunFlag = 1;
				scope = 2;

				wAsk->Util3Step1();
//				Sleep(1000);

				// Запускаем поток для графика
				hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

				Timer1->Interval = 5500;
				Timer1->Enabled = true;
				WorkMode = MODE_UTIL3;
				sButton4->Caption = "ОТМЕНА";
				inclK10 = true;

				sButton3->Enabled = false;
				sButton5->Enabled = false;
				sBitBtn1->Enabled = false;

				sButton8->Enabled = true;           // Кнопка отсутствия ступенек
			}
	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
static char Text7[] = "I+ = %.1f мА";
void __fastcall TASK_1::Timer1Timer(TObject *Sender)
{
	Timer1->Interval = 2000;

	if( WorkMode == MODE_UTIL1 )
	{
		try
		{
			switch(wAsk->Util1Step2_8())
			 {
				 case RET_OK:{AnsiString form_string;
							  form_string.printf( Text1, wAsk->VoltArrayTable[wAsk->NumStep - 1][0] );
							  sLabUpr1->Caption = form_string;
							  form_string.printf( Text1, wAsk->VoltArrayTable[wAsk->NumStep - 1][1] );
							  sLabUpr2->Caption = form_string;
							  form_string.printf( Text1, wAsk->VoltArrayTable[wAsk->NumStep - 1][2] );
							  sLabUpr3->Caption = form_string;
							  form_string.printf( Text3, wAsk->Util1Result );
							  sPanel9->Font->Color = clLime;
							  sPanel9->Caption = form_string;
							  LighteTable(wAsk->NumStep - 1);
							  wAsk->noteUtil1Porog[wAsk->NumStep - 1] = "Норма";
							  break;}

				 case RET_END:{AnsiString form_string;
							   form_string.printf( Text3, wAsk->Util1Result );
							   sPanel9->Caption = form_string;
							   wAsk->CheckASK1[3] = 1;
							   wAsk->noteUtil1Porog[7] = "Норма";
							   Util1Stop();
							   LighteTable(8);
//							   wAsk->AcqBlockReady[0] = 0;                     // После завершения блоки должны быть готовы к
//							   wAsk->AcqBlockReady[1] = 0;                     // приёму данных с АЦП, в случае повторной проверки
							   break;}

				 case RET_ERROR:{AnsiString form_string;
								 form_string.printf( Text3, wAsk->Util1Result );
								 sPanel9->Caption = form_string;
								 wAsk->CheckASK1[3] = 2;
								 wAsk->noteUtil1Porog[wAsk->NumStep - 1] = "Неисправность";
								 BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
								 Util1Stop();
								 LighteTable(wAsk->NumStep - 1);      // Засветка зелёным перед жёлтой строкой
								 ErrorLighteTable(wAsk->NumStep, clYellow);
								 break;}

				 case RET_NO_U:{AnsiString form_string;
								form_string.printf( Text3, wAsk->Util1Result );
								sPanel9->Caption = form_string;
								wAsk->CheckASK1[3] = 2;
								wAsk->noteUtil1Porog[wAsk->NumStep - 1] = "Неисправность";
								BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
								Util1Stop();
								LighteTable(wAsk->NumStep - 1);      // Засветка зелёным перед красной строкой
								ErrorLighteTable(wAsk->NumStep, clRed);
								break;}
				 default:{Util1Stop();}
			 }
		}
		catch( Exception &e )
		{
			MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
		}
	}

	if( WorkMode == MODE_UTIL2 )
	{
		if(wAsk->CheckASK1[0] == 0) wAsk->CheckASK1[0] = 1;
		Util2Stop();
		Timer1->Enabled = false;
		WorkMode = MODE_NULL;
	}
//***************** Util 3 ***************************
	if( WorkMode == MODE_UTIL3 )
	{
//		wAsk->Util1Stop();
		CheckK10();
		Util3Stop();
		Timer1->Enabled = false;
		WorkMode = MODE_NULL;
	}
//***************** Util 4 ***************************
	if( WorkMode == MODE_UTIL4 )
	{
		bool power1, power2, power3;
		AnsiString form_string1, form_string2;

		// Проверка тока
		wAsk->currentPlus = wAsk->Acq2Buf[wAsk->BlockNumberThread2][32];     // Берём 32 значение для тока
//		double currentPlus = 0;
//		for( int i = 0; i < ACQ2_BLOCK_SIZE; ) {
//			if(wAsk->Acq2Buf[wAsk->BlockNumberThread2][i] > currentPlus)
//				currentPlus = wAsk->Acq2Buf[wAsk->BlockNumberThread2][i];
//			i += 16;
//		}
		form_string1.printf( Text7, wAsk->currentPlus );
		sPanel15->Caption = form_string1;
		if(wAsk->currentPlus >= CURRENT_MIN	&& wAsk->currentPlus <= CURRENT_MIDD) {
			sPanel15->Font->Color = clLime;
			sPanel14->Font->Color = clLime;
			sPanel14->Caption = "НОРМА";
			wAsk->note_current = "Норма";
			wAsk->CheckASK1[6] = 1;
			power3 = true;
		} else {
			if(wAsk->currentPlus > CURRENT_MIDD && wAsk->currentPlus < CURRENT_MAX) {
				sPanel15->Font->Color = clYellow;
				sPanel14->Font->Color = clYellow;
				sPanel14->Caption = "ОТКЛОНЕНИЕ";
				wAsk->note_current = "Неисправность";
				wAsk->CheckASK1[6] = 2;
			} else {
				sPanel15->Font->Color = clRed;
				sPanel14->Font->Color = clRed;
				sPanel14->Caption = "ОШИБКА";
				wAsk->note_current = "Неисправность";
				wAsk->CheckASK1[6] = 2;
			}
			BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			power3 = false;
		}

		wAsk->Uplus = wAsk->AcqBuf[wAsk->BlockNumberThread][512];
		form_string1.printf( Text5, wAsk->Uplus );
		sPanel17->Caption = form_string1;

		wAsk->Uminus = wAsk->AcqBuf[wAsk->BlockNumberThread][513];
		form_string2.printf( Text6, wAsk->Uminus );
		sPanel12->Caption = form_string2;

		// Положительное напряжение
		if( SimpleRoundTo(wAsk->Uplus, -1) < UPLUS_MIN || SimpleRoundTo(wAsk->Uplus, -1)> UPLUS_MAX) {
			sPanel17->Font->Color = clRed;
			sPanel16->Font->Color = clRed;
			sPanel16->Caption = "ОШИБКА";
			wAsk->noteUplus = "Неисправность";
			wAsk->CheckASK1[1] = 2;
			BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			power1 = false;
		} else {
			sPanel17->Font->Color = clLime;
			sPanel16->Font->Color = clLime;
			sPanel16->Caption = "НОРМА";
			wAsk->noteUplus = "Норма";
			power1 = true;
			wAsk->CheckASK1[1] = 1;
		}

		// Отрицательное напряжение
		if( wAsk->Uminus >= UMINUS_MIN || wAsk->Uminus <= UMINUS_MAX) {
			sPanel12->Font->Color = clRed;
			sPanel11->Font->Color = clRed;
			sPanel11->Caption = "ОШИБКА";
			wAsk->noteUminus = "Неисправность";
			wAsk->CheckASK1[2] = 2;
			BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			power2 = false;
		} else {
			sPanel12->Font->Color = clLime;
			sPanel11->Font->Color = clLime;
			sPanel11->Caption = "НОРМА";
            wAsk->noteUminus = "Норма";
			power2 = true;
			wAsk->CheckASK1[2] = 1;
		}

		if(power1 && power2 && power3) {
			sButton3->Enabled = true;
			sButton4->Enabled = true;
			sButton5->Enabled = true;
			sBitBtn1->Font->Color = clLime;
			sBitBtn1->Caption = "ОТКЛЮЧИТЬ ПИТАНИЕ";
			wAsk->UtilStop();
		} else Util4Stop();

		wAsk->ReplaceBuf = false;
		Timer1->Enabled = false;
		WorkMode = MODE_NULL;
		sBitBtn1->Down = false;
		sBitBtn1->Enabled = true;
	}

}
//---------------------------------------------------------------------------
void __fastcall TASK_1::Util1Stop()
{
	wAsk->UtilStop();
	Timer1->Enabled = false;
	sButton3->Caption = "В К Л Ю Ч И Т Ь";
	sLabUpr1->Caption = Text2;
	sLabUpr2->Caption = Text2;
	sLabUpr3->Caption = Text2;
	WorkMode = MODE_NULL;
	incl = false;

	sButton4->Enabled = true;
	sButton5->Enabled = true;
	sBitBtn1->Enabled = true;

	TransitionASK2();                 // Проверяем можно ли переходить на второй экран
}
//---------------------------------------------------------------------------
void __fastcall  TASK_1::Util2Stop()
{
	wAsk->UtilStop();
	Timer1->Enabled = false;
	sButton5->Caption = "ФОРМИРОВАТЕЛЬ";
	WorkMode = MODE_NULL;
	inclK9 = false;

	sButton3->Enabled = true;
	sButton4->Enabled = true;
	sBitBtn1->Enabled = true;

	sButton6->Enabled = false;
	TransitionASK2();                // Проверяем можно ли переходить на второй экран
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::Util3Stop()
{
	wAsk->UtilStop();
	Timer1->Enabled = false;
	sButton4->Caption = "ЗАПУСК";
	WorkMode = MODE_NULL;
	inclK10 = false;

	sButton3->Enabled = true;
	sButton5->Enabled = true;
	sBitBtn1->Enabled = true;

	sButton8->Enabled = false;           // Кнопка отсутствия ступенек

	// Если на кнопку не нажали, то норма
	if(wAsk->CheckASK1[5] == 0) wAsk->CheckASK1[5] = 1;
	TransitionASK2();                    // Проверяем можно ли переходить на второй экран
}
//---------------------------------------------------------------------------
void __fastcall  TASK_1::Util4Stop()
{
	wAsk->UtilStop(1);

	sBitBtn1->Font->Color = clRed;
	sBitBtn1->Caption = "ЗАПУСК ПИТАНИЯ";
	inclK8K7 = false;
	sBitBtn1->Down = false;
}
//---------------------------------------------------------------------------
// Обнуляем всё в точках К7 и К8
void __fastcall TASK_1::ResetK7K8()
{
	sPanel17->Font->Color = clWhite;
	sPanel17->Caption = "Uвт+ =";
	sPanel16->Font->Color = clWhite;
	sPanel16->Caption = "";

	sPanel12->Font->Color = clWhite;
	sPanel12->Caption = "Uвт- =";
	sPanel11->Font->Color = clWhite;
	sPanel11->Caption = "";

	sPanel15->Font->Color = clWhite;
	sPanel15->Caption = "I+ =";
	sPanel14->Font->Color = clWhite;
	sPanel14->Caption = "";
}
//---------------------------------------------------------------------------
// Подсвечиваем таблицу, Step - номер шага (строка)
void __fastcall TASK_1::LighteTable(int Step)
{
	switch(Step) {
		case 0:{Table_1_0->SkinData->CustomColor = true;
				Table_1_1->SkinData->CustomColor = true;
				Table_1_2->SkinData->CustomColor = true;
				Table_1_3->SkinData->CustomColor = true;
				Table_1_0->Color = (TColor) 0x0083E2A7;
				Table_1_1->Color = (TColor) 0x0083E2A7;
				Table_1_2->Color = (TColor) 0x0083E2A7;
				Table_1_3->Color = (TColor) 0x0083E2A7;
				break;}
		case 1:{Table_1_0->Color = clLime;
				Table_1_1->Color = clLime;
				Table_1_2->Color = clLime;
				Table_1_3->Color = clLime;
				Table_2_0->SkinData->CustomColor = true;
				Table_2_1->SkinData->CustomColor = true;
				Table_2_2->SkinData->CustomColor = true;
				Table_2_3->SkinData->CustomColor = true;
				Table_2_0->Color = (TColor) 0x0083E2A7;
				Table_2_1->Color = (TColor) 0x0083E2A7;
				Table_2_2->Color = (TColor) 0x0083E2A7;
				Table_2_3->Color = (TColor) 0x0083E2A7;
				break;}
		case 2:{Table_2_0->Color = clLime;
				Table_2_1->Color = clLime;
				Table_2_2->Color = clLime;
				Table_2_3->Color = clLime;
				Table_3_0->SkinData->CustomColor = true;
				Table_3_1->SkinData->CustomColor = true;
				Table_3_2->SkinData->CustomColor = true;
				Table_3_3->SkinData->CustomColor = true;
				Table_3_0->Color = (TColor) 0x0083E2A7;
				Table_3_1->Color = (TColor) 0x0083E2A7;
				Table_3_2->Color = (TColor) 0x0083E2A7;
				Table_3_3->Color = (TColor) 0x0083E2A7;
				break;}
		case 3:{Table_3_0->Color = clLime;
				Table_3_1->Color = clLime;
				Table_3_2->Color = clLime;
				Table_3_3->Color = clLime;
				Table_4_0->SkinData->CustomColor = true;
				Table_4_1->SkinData->CustomColor = true;
				Table_4_2->SkinData->CustomColor = true;
				Table_4_3->SkinData->CustomColor = true;
				Table_4_0->Color = (TColor) 0x0083E2A7;
				Table_4_1->Color = (TColor) 0x0083E2A7;
				Table_4_2->Color = (TColor) 0x0083E2A7;
				Table_4_3->Color = (TColor) 0x0083E2A7;
				break;}
		case 4:{Table_4_0->Color = clLime;
				Table_4_1->Color = clLime;
				Table_4_2->Color = clLime;
				Table_4_3->Color = clLime;
				Table_5_0->SkinData->CustomColor = true;
				Table_5_1->SkinData->CustomColor = true;
				Table_5_2->SkinData->CustomColor = true;
				Table_5_3->SkinData->CustomColor = true;
				Table_5_0->Color = (TColor) 0x0083E2A7;
				Table_5_1->Color = (TColor) 0x0083E2A7;
				Table_5_2->Color = (TColor) 0x0083E2A7;
				Table_5_3->Color = (TColor) 0x0083E2A7;
				break;}
		case 5:{Table_5_0->Color = clLime;
				Table_5_1->Color = clLime;
				Table_5_2->Color = clLime;
				Table_5_3->Color = clLime;
				Table_6_0->SkinData->CustomColor = true;
				Table_6_1->SkinData->CustomColor = true;
				Table_6_2->SkinData->CustomColor = true;
				Table_6_3->SkinData->CustomColor = true;
				Table_6_0->Color = (TColor) 0x0083E2A7;
				Table_6_1->Color = (TColor) 0x0083E2A7;
				Table_6_2->Color = (TColor) 0x0083E2A7;
				Table_6_3->Color = (TColor) 0x0083E2A7;
				break;}
		case 6:{Table_6_0->Color = clLime;
				Table_6_1->Color = clLime;
				Table_6_2->Color = clLime;
				Table_6_3->Color = clLime;
				Table_7_0->SkinData->CustomColor = true;
				Table_7_1->SkinData->CustomColor = true;
				Table_7_2->SkinData->CustomColor = true;
				Table_7_3->SkinData->CustomColor = true;
				Table_7_0->Color = (TColor) 0x0083E2A7;
				Table_7_1->Color = (TColor) 0x0083E2A7;
				Table_7_2->Color = (TColor) 0x0083E2A7;
				Table_7_3->Color = (TColor) 0x0083E2A7;
				break;}
		case 7:{Table_7_0->Color = clLime;
				Table_7_1->Color = clLime;
				Table_7_2->Color = clLime;
				Table_7_3->Color = clLime;
				Table_8_0->SkinData->CustomColor = true;
				Table_8_1->SkinData->CustomColor = true;
				Table_8_2->SkinData->CustomColor = true;
				Table_8_3->SkinData->CustomColor = true;
				Table_8_0->Color = (TColor) 0x0083E2A7;
				Table_8_1->Color = (TColor) 0x0083E2A7;
				Table_8_2->Color = (TColor) 0x0083E2A7;
				Table_8_3->Color = (TColor) 0x0083E2A7;
				break;}
		case 8:{Table_8_0->Color = clLime;
				Table_8_1->Color = clLime;
				Table_8_2->Color = clLime;
				Table_8_3->Color = clLime;
				break;}
	}
    sPanel5->Font->Color = clLime;
	sPanel5->Caption = "НОРМА";
}
//---------------------------------------------------------------------------
// Функция рисования графика
DWORD WINAPI TASK_1::ScopeThread(TASK_1 *ASK_1)
{
	int i;
	int count = 0;
	bool NeedPacket;
//	double maxValue = 0;
	AnsiString form_string;

	ASK_1->wAsk->maxValueK10 = 0;

	if(ASK_1->wAsk->BlockNumberThread == 100) // проверка инициализации переменной
		{
			ASK_1->CreateTh();                   // повторный запуск потока через паузу
			ExitThread(0);
			return 0;
		}

	switch(ASK_1->scope)
	{
		case 1:{
			while(ASK_1->wAsk->RunFlag) {

				if(count == ACQ_BLOCK_QNT) count = 0;

				NeedPacket = ASK_1->iScope1->NeedPacketsNow;
				if(!NeedPacket) {
					ASK_1->iScope1->DataBlockClear();
				} else
				{

					if(!ASK_1->iScope1->DataBlockActive && !ASK_1->iScope1->TransferringActve) {

						if(ASK_1->wAsk->AcqBlockReady[count]) {

							ASK_1->iScope1->DataBlockBegin();
							for(i=0; i < ACQ_BLOCK_SIZE; i++) {
								ASK_1->iScope1->AddChannelData(0, ASK_1->wAsk->AcqBuf[count][i]);
							}
							ASK_1->iScope1->DataBlockEnd();

//							ASK_1->wAsk->AcqBlockReady[ASK_1->wAsk->BlockNumberThread] = 0;
							ASK_1->wAsk->AcqBlockReady[count] = 0;
							count++;
						}
					}
				}
			}
			break;}
		case 2:{
			while(ASK_1->wAsk->RunFlag) {

				if(count == ACQ_BLOCK_QNT) count = 0;

				NeedPacket = ASK_1->iScope2->NeedPacketsNow;
				if(!NeedPacket) {
					ASK_1->iScope2->DataBlockClear();
				} else
				{

					if(!ASK_1->iScope2->DataBlockActive && !ASK_1->iScope2->TransferringActve) {

						if(ASK_1->wAsk->AcqBlockReady[count]) {

						ASK_1->iScope2->DataBlockBegin();
						for(i=0; i < ACQ_BLOCK_SIZE; i++) {
							ASK_1->iScope2->AddChannelData(0, ASK_1->wAsk->AcqBuf[count][i]);
							if(ASK_1->wAsk->maxValueK10 < ASK_1->wAsk->AcqBuf[count][i]) ASK_1->wAsk->maxValueK10 = ASK_1->wAsk->AcqBuf[count][i];
						}
						ASK_1->iScope2->DataBlockEnd();

						//ASK_1->wAsk->AcqBlockReady[ASK_1->wAsk->BlockNumberThread] = 0;
						ASK_1->wAsk->AcqBlockReady[count] = 0;
						count++;
						}

					}
				}
				form_string.printf( Text4, ASK_1->wAsk->maxValueK10);
				ASK_1->sPanel22->Caption = form_string;
//				maxValue = 3;                                                 //для имитации
//				if(maxValue > K10_MAX || (maxValue < K10_MIN && maxValue > 0.01) ) {
//					ASK_1->sPanel22->Font->Color = clYellow;
//					ASK_1->sPanel21->Font->Color = clYellow;
//					ASK_1->sPanel21->Caption = "ОТКЛОНЕНИЕ";
//				} else { if(maxValue < 0.1)
//							{
//								ASK_1->sPanel22->Font->Color = clRed;
//								ASK_1->sPanel21->Font->Color = clRed;
//								ASK_1->sPanel21->Caption = "ОШИБКА";
//							} else
//							{
//								ASK_1->sPanel22->Font->Color = clLime;
//								ASK_1->sPanel21->Font->Color = clLime;
//								ASK_1->sPanel21->Caption = "НОРМА";
//							}
//				}

			}
			break;}
	}
	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
// Функция повторного запуска графика
void __fastcall TASK_1::CreateTh()
{
	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	Sleep(500);
	hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);
}
//---------------------------------------------------------------------------
// Проверяем значение в точке К10
void __fastcall TASK_1::CheckK10()
{
	if(wAsk->maxValueK10 > K10_MAX || (wAsk->maxValueK10 < K10_MIN && wAsk->maxValueK10 > 0.01) ) {
		sPanel22->Font->Color = clYellow;
		sPanel21->Font->Color = clYellow;
		sPanel21->Caption = "ОТКЛОНЕНИЕ";
		BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
	} else { if(wAsk->maxValueK10 < 0.1)
				{
					sPanel22->Font->Color = clRed;
					sPanel21->Font->Color = clRed;
					sPanel21->Caption = "ОШИБКА";
					wAsk->CheckASK1[4] = 2;
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				} else
				{
					sPanel22->Font->Color = clLime;
					sPanel21->Font->Color = clLime;
					sPanel21->Caption = "НОРМА";
					wAsk->CheckASK1[4] = 1;
				}
	}
}
//---------------------------------------------------------------------------
// Подсвечиваем таблицу в случае ошибки Step - номер шага, cl - цвет (красный или жёлтый)
void __fastcall TASK_1::ErrorLighteTable(int Step, TColor cl)
{
	switch(Step - 1) {
		case 0:{Table_1_0->SkinData->CustomColor = true;
				Table_1_1->SkinData->CustomColor = true;
				Table_1_2->SkinData->CustomColor = true;
				Table_1_3->SkinData->CustomColor = true;
				Table_1_0->Color = cl;
				Table_1_1->Color = cl;
				Table_1_2->Color = cl;
				Table_1_3->Color = cl;
				break;}
		case 1:{Table_2_0->SkinData->CustomColor = true;
				Table_2_1->SkinData->CustomColor = true;
				Table_2_2->SkinData->CustomColor = true;
				Table_2_3->SkinData->CustomColor = true;
				Table_2_0->Color = cl;
				Table_2_1->Color = cl;
				Table_2_2->Color = cl;
				Table_2_3->Color = cl;
				break;}
		case 2:{Table_3_0->SkinData->CustomColor = true;
				Table_3_1->SkinData->CustomColor = true;
				Table_3_2->SkinData->CustomColor = true;
				Table_3_3->SkinData->CustomColor = true;
				Table_3_0->Color = cl;
				Table_3_1->Color = cl;
				Table_3_2->Color = cl;
				Table_3_3->Color = cl;
				break;}
		case 3:{Table_4_0->SkinData->CustomColor = true;
				Table_4_1->SkinData->CustomColor = true;
				Table_4_2->SkinData->CustomColor = true;
				Table_4_3->SkinData->CustomColor = true;
				Table_4_0->Color = cl;
				Table_4_1->Color = cl;
				Table_4_2->Color = cl;
				Table_4_3->Color = cl;
				break;}
		case 4:{Table_5_0->SkinData->CustomColor = true;
				Table_5_1->SkinData->CustomColor = true;
				Table_5_2->SkinData->CustomColor = true;
				Table_5_3->SkinData->CustomColor = true;
				Table_5_0->Color = cl;
				Table_5_1->Color = cl;
				Table_5_2->Color = cl;
				Table_5_3->Color = cl;
				break;}
		case 5:{Table_6_0->SkinData->CustomColor = true;
				Table_6_1->SkinData->CustomColor = true;
				Table_6_2->SkinData->CustomColor = true;
				Table_6_3->SkinData->CustomColor = true;
				Table_6_0->Color = cl;
				Table_6_1->Color = cl;
				Table_6_2->Color = cl;
				Table_6_3->Color = cl;
				break;}
		case 6:{Table_7_0->SkinData->CustomColor = true;
				Table_7_1->SkinData->CustomColor = true;
				Table_7_2->SkinData->CustomColor = true;
				Table_7_3->SkinData->CustomColor = true;
				Table_7_0->Color = cl;
				Table_7_1->Color = cl;
				Table_7_2->Color = cl;
				Table_7_3->Color = cl;
				break;}
		case 7:{Table_8_0->SkinData->CustomColor = true;
				Table_8_1->SkinData->CustomColor = true;
				Table_8_2->SkinData->CustomColor = true;
				Table_8_3->SkinData->CustomColor = true;
				Table_8_0->Color = cl;
				Table_8_1->Color = cl;
				Table_8_2->Color = cl;
				Table_8_3->Color = cl;
				break;}
	}

	sPanel5->Font->Color = cl;
	sPanel9->Font->Color = cl;

	if(cl == clYellow) {
		sPanel5->Caption = "ОТКЛОНЕНИЕ";
		BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
	} else {
		sPanel5->Caption = "ОШИБКА";
		BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
    }
}
//---------------------------------------------------------------------------
// Приводим таблицу в первоначальное состояние
void __fastcall TASK_1::ResetLighteTable()
{
	Table_1_0->Color = clBtnFace;
	Table_1_0->SkinData->CustomColor = false;
	Table_1_1->Color = clBtnFace;
	Table_1_1->SkinData->CustomColor = false;
	Table_1_2->Color = clBtnFace;
	Table_1_2->SkinData->CustomColor = false;
	Table_1_3->Color = clBtnFace;
	Table_1_3->SkinData->CustomColor = false;

	Table_2_0->Color = clBtnFace;
	Table_2_0->SkinData->CustomColor = false;
	Table_2_1->Color = clBtnFace;
	Table_2_1->SkinData->CustomColor = false;
	Table_2_2->Color = clBtnFace;
	Table_2_2->SkinData->CustomColor = false;
	Table_2_3->Color = clBtnFace;
	Table_2_3->SkinData->CustomColor = false;

	Table_3_0->Color = clBtnFace;
	Table_3_0->SkinData->CustomColor = false;
	Table_3_1->Color = clBtnFace;
	Table_3_1->SkinData->CustomColor = false;
	Table_3_2->Color = clBtnFace;
	Table_3_2->SkinData->CustomColor = false;
	Table_3_3->Color = clBtnFace;
	Table_3_3->SkinData->CustomColor = false;

	Table_4_0->Color = clBtnFace;
	Table_4_0->SkinData->CustomColor = false;
	Table_4_1->Color = clBtnFace;
	Table_4_1->SkinData->CustomColor = false;
	Table_4_2->Color = clBtnFace;
	Table_4_2->SkinData->CustomColor = false;
	Table_4_3->Color = clBtnFace;
	Table_4_3->SkinData->CustomColor = false;

	Table_5_0->Color = clBtnFace;
	Table_5_0->SkinData->CustomColor = false;
	Table_5_1->Color = clBtnFace;
	Table_5_1->SkinData->CustomColor = false;
	Table_5_2->Color = clBtnFace;
	Table_5_2->SkinData->CustomColor = false;
	Table_5_3->Color = clBtnFace;
	Table_5_3->SkinData->CustomColor = false;

	Table_6_0->Color = clBtnFace;
	Table_6_0->SkinData->CustomColor = false;
	Table_6_1->Color = clBtnFace;
	Table_6_1->SkinData->CustomColor = false;
	Table_6_2->Color = clBtnFace;
	Table_6_2->SkinData->CustomColor = false;
	Table_6_3->Color = clBtnFace;
	Table_6_3->SkinData->CustomColor = false;

	Table_7_0->Color = clBtnFace;
	Table_7_0->SkinData->CustomColor = false;
	Table_7_1->Color = clBtnFace;
	Table_7_1->SkinData->CustomColor = false;
	Table_7_2->Color = clBtnFace;
	Table_7_2->SkinData->CustomColor = false;
	Table_7_3->Color = clBtnFace;
	Table_7_3->SkinData->CustomColor = false;

	Table_8_0->Color = clBtnFace;
	Table_8_0->SkinData->CustomColor = false;
	Table_8_1->Color = clBtnFace;
	Table_8_1->SkinData->CustomColor = false;
	Table_8_2->Color = clBtnFace;
	Table_8_2->SkinData->CustomColor = false;
	Table_8_3->Color = clBtnFace;
	Table_8_3->SkinData->CustomColor = false;

	sPanel5->Font->Color = clWhite;
	sPanel5->Caption = "";
	sPanel9->Font->Color = clWhite;
	sPanel9->Caption = "Uопор =";
}
//---------------------------------------------------------------------------
// Обнуляем всё в точке К10
void __fastcall TASK_1::ResetK10()
{
	sPanel22->Font->Color = clWhite;
	sPanel22->Caption = "U =";
	sPanel21->Font->Color = clWhite;
	sPanel21->Caption = "";
}
//---------------------------------------------------------------------------
// Переходим на второй экран или снова первый
void __fastcall TASK_1::sButton1Click(TObject *Sender)
{
	switch(WorkModeEND) {
		case RET_OK:{	TASK_2 *ASK_2 = new TASK_2(0, wAsk);
						if(!ASK_2) return;
						ASK_2->Show();
						power = false;
						break;}

		case RET_ERROR:{TASK_1 *ASK_1 = new TASK_1(0, wAsk->NumberBlock);
						if(!ASK_1) return;
						ASK_1->Show();
						break;}
	}

	this->Close();
}
//---------------------------------------------------------------------------
// Кнопка отсутствия графика в правой части экрана
void __fastcall TASK_1::sButton6Click(TObject *Sender)
{
	wAsk->CheckASK1[0] = 2;
	sButton6->Enabled = false;
}
//---------------------------------------------------------------------------
// Кнопка отсутствия графика в точке Х9
void __fastcall TASK_1::sButton8Click(TObject *Sender)
{
	wAsk->CheckASK1[5] = 2;
	sButton8->Enabled = false;
}
//---------------------------------------------------------------------------
// Функция проверки готовности перехода на следующий экран
void __fastcall TASK_1::TransitionASK2()
{
	for(int i=0; i<PARAMETERS_ASK1; i++) {
		if(wAsk->CheckASK1[i] == 0) return;
	}

	sButton1->Enabled = true;
	sButton11->Enabled = true;
	sButton10->Enabled = true;
	sButton2->Enabled = true;

	for(int i=0; i<PARAMETERS_ASK1; i++) {
		if(wAsk->CheckASK1[i] == 2 ) {
			sPanel3->Font->Color = clRed;
			sPanel3->Caption = "Блок неисправен";
			sLabel3->Caption = "Повторить проверку?";
			WorkModeEND = RET_ERROR;
			BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"2.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			return;
        }
	}

	WorkModeEND = RET_OK;
	sPanel3->Font->Color = clLime;
	sPanel3->Caption = "Параметры в норме";
	sLabel3->Caption = "Перейти к экрану \"2\"?";
	BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"1.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
}
//---------------------------------------------------------------------------
// Закрываем текущее окно (Нет)
void __fastcall TASK_1::sButton2Click(TObject *Sender)
{
//	wAsk->Util0Step1();
//	wAsk->UtilStop(1);		// Стоп с отключением питания

	this->Close();
}
//---------------------------------------------------------------------------
// Кнопка ввода номера блока
void __fastcall TASK_1::sButton7Click(TObject *Sender)
{
	if(sEdit1->Text != "") {
		sBitBtn1->Enabled = true;
		sButton7->Visible = false;
		sEdit1->Visible = false;
		sLabel8->Visible = false;

		wAsk->NumberBlock = sEdit1->Text;

		sLabel10->Caption ="№" + sEdit1->Text;
		sLabel10->Left = (1920 - sLabel10->Width) / 2;
		sLabel10->Visible = true;
	} else {
    	ShowMessage("Введите номер блока!");
	}
}
//---------------------------------------------------------------------------

//------------------------/ Обработчики графиков /---------------------------
void __fastcall TASK_1::sListBox1Click(TObject *Sender)
{
	sListBox1->Selected[sListBox1->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sListBox2Click(TObject *Sender)
{
	sListBox2->Selected[sListBox2->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sListBox3Click(TObject *Sender)
{
	sListBox3->Selected[sListBox3->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sListBox4Click(TObject *Sender)
{
	sListBox4->Selected[sListBox4->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sListBox5Click(TObject *Sender)
{
	sListBox5->Selected[sListBox5->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sListBox6Click(TObject *Sender)
{
	sListBox6->Selected[sListBox6->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sListBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{
	FocusControl(iScope2); // переводим фокус, чтобы колёсиком мышки нельзя было поменять значение
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sListBox1VScroll(TObject *Sender)
{
	iScope2->TimeBase->SecPerDiv = SecPerDivTable[sListBox1->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sListBox2VScroll(TObject *Sender)
{
	iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox2->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sListBox3VScroll(TObject *Sender)
{
	if(sListBox3->TopIndex == 0) iScope2->Channel[0]->Coupling = COPLING_AC;
		else iScope2->Channel[0]->Coupling = COPLING_DC;
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sListBox4VScroll(TObject *Sender)
{
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[sListBox4->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sListBox5VScroll(TObject *Sender)
{
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox5->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TASK_1::sListBox6VScroll(TObject *Sender)
{
	if(sListBox6->TopIndex == 0) iScope1->Channel[0]->Coupling = COPLING_AC;
		else iScope1->Channel[0]->Coupling = COPLING_DC;
}
//---------------------------------------------------------------------------
// Кнопка быстрого перехода на второе окно
void __fastcall TASK_1::sButton9Click(TObject *Sender)
{
	TASK_2 *ASK_2 = new TASK_2(0, wAsk);
	if(!ASK_2) return;
	ASK_2->Show();
	power = false;

	this->Close();
}
//---------------------------------------------------------------------------

void __fastcall TASK_1::sButton10Click(TObject *Sender)
{
	wAsk->WriteExcel("\\RaportASK.xlsx", true);
}
//---------------------------------------------------------------------------

void __fastcall TASK_1::sButton11Click(TObject *Sender)
{
	wAsk->WriteExcel("\\RaportASK.xlsx", false);

	TVsReport *vsReport = new TVsReport(this, wAsk->NumberBlock);
	if(!vsReport) return;
	vsReport->ShowModal();
	delete vsReport;

	DeleteFile(GetCurrentDir() + "\\~" + wAsk->NumberBlock + ".xlsx");
}
//---------------------------------------------------------------------------

