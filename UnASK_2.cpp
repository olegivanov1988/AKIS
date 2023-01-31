//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnASK_2.h"
#include "WAsk.h"
#include "UnASK_1.h"
#include "UnVisibleReport.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "iComponent"
#pragma link "iCustomComponent"
#pragma link "iPlotComponent"
#pragma link "iVCLComponent"
#pragma link "iXYPlot"
#pragma link "sLabel"
#pragma link "sBevel"
#pragma link "sButton"
#pragma link "sPanel"
#pragma link "sRadioButton"
#pragma link "iScope"
#pragma link "sListBox"
#pragma link "sDialogs"
#pragma link "acPathDialog"
#pragma resource "*.dfm"
//TForm1 *Form1;
 #include <math.h>
#include <bass.h>
#include <process.h>
#include <stdio.h>
#include <windows.h>
int stepAuto=0; // для автоматической проверки
int ModeAuto=-1; //если автоматическая проверка то 0
const double TASK_2::SecPerDivTable[19] = {0.1, 0.05, 0.02, 0.01, 0.005, 0.002, 0.001,
0.0005, 0.0002, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06, 5E-07, 2E-07, 1E-07};

const double TASK_2::VoltPerDivTable[22] = {50, 20, 10, 5, 2, 1, 0.5, 0.2, 0.1, 0.05, 0.02,
0.01, 0.005, 0.002, 0.001, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06};
HSAMPLE sample;
	HCHANNEL channel;
//---------------------------------------------------------------------------
__fastcall TASK_2::TASK_2(TComponent* Owner, WAsk *wask)
	: TForm(Owner)
{
	wAsk = wask;

	inclCK1 = false;                 // Переменные, которые фиксируют нажатие
	inclCK2 = false;                 // кнопок проверок
	inclCK3 = false;
	power = true;

	sListBox7->ItemIndex = 1;       // выставляем первоначальное значение 50 мс
	sListBox7->Selected[1] = false;
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[1];
	sListBox8->ItemIndex = 3;       // выставляем первоначальное значение 5 В
	sListBox8->Selected[3] = false;
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[3];

	sListBox4->ItemIndex = 1;       // выставляем первоначальное значение 50 мс
	sListBox4->Selected[1] = false;
	iScope2->TimeBase->SecPerDiv = SecPerDivTable[1];
	sListBox5->ItemIndex = 3;       // выставляем первоначальное значение 5 В
	sListBox5->Selected[3] = false;
	iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[3];

	sListBox1->ItemIndex = 10;       // выставляем первоначальное значение 50 мкс
	sListBox1->Selected[10] = false;
	iScope3->TimeBase->SecPerDiv = SecPerDivTable[10];
	sListBox2->ItemIndex = 5;       // выставляем первоначальное значение 1 В
	sListBox2->Selected[5] = false;
	iScope3->Channel[0]->VoltsPerDivision = VoltPerDivTable[5];
	sListBox3->ItemIndex = 1;               // выставляем AC
	sListBox3->Selected[1] = false;
	iScope3->Channel[0]->Coupling = isccAC;

	// Инициализируем массив проверочных переменных
	for(int i=0; i<PARAMETERS_ASK2; i++) {
		wAsk->CheckASK2[i] = 0;
	}

	// Выводим номер блока
	sLabel10->Caption ="№" + wAsk->NumberBlock;
	sLabel10->Left = (1920 - sLabel10->Width) / 2;
	sLabel10->Visible = true;
}
//---------------------------------------------------------------------------
__fastcall TASK_2::~TASK_2()
{
//	delete wAsk;
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
//	if(wAsk->RunFlag) {
//		wAsk->UtilStop(1);	// выключение модулей, если они запущены
//	}

	wAsk->UtilStop(power);	// выключение модулей, если они запущены
	power = true;
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sButton1Click(TObject *Sender)
{
	btnAuto->Enabled=false;
	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	try
	{
		if(inclCK1)
		{
			Util4Stop();

		} else {
			ResetK1();
			//Выставляем размерность графика
			sListBox1->ItemIndex = 2;       // выставляем первоначальное значение 20 мс
			sListBox1->Selected[2] = false;
			iScope3->TimeBase->SecPerDiv = SecPerDivTable[2];
			sListBox2->ItemIndex = 5;       // выставляем первоначальное значение 1 В
			sListBox2->Selected[5] = false;
			iScope3->Channel[0]->VoltsPerDivision = VoltPerDivTable[5];

			iScope3->SamplesPerSecond = 1563;

			wAsk->Util4Step1();
			wAsk->RunFlag = 1;
			scope = 2;                     // Рисуем график на iScope3

			Timer1->Interval = 9000;
			Timer1->Enabled = true;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

			WorkMode = MODE_UTIL4_STEP1;

			sButton1->Caption = "Сигнал СК1 ОТМЕНА";
			inclCK1 = true;

//			sButton1->Enabled = false;
			sButton2->Enabled = false;
			sButton3->Enabled = false;
		}

	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sButton2Click(TObject *Sender)
{
	btnAuto->Enabled=false;
	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	try
	{
		if(inclCK2)
		{
			Util5Stop();

		} else
		{
			ResetK2K3K4();
			// Выставляем размерность графика
			sListBox1->ItemIndex = 2;       // выставляем первоначальное значение 20 мс
			sListBox1->Selected[2] = false;
			iScope3->TimeBase->SecPerDiv = SecPerDivTable[2];
			sListBox2->ItemIndex = 4;       // выставляем первоначальное значение 2 В
			sListBox2->Selected[4] = false;
			iScope3->Channel[0]->VoltsPerDivision = VoltPerDivTable[4];

			iScope3->SamplesPerSecond = 391;

			wAsk->Util5Step1();
			wAsk->RunFlag = 1;
			scope = 3;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

			Timer1->Interval = 5000;                 // Шаг таймера
			Timer1->Enabled = true;
			WorkMode = MODE_UTIL5;

			sButton2->Caption = "Сигнал СК2 ОТМЕНА";
			inclCK2 = true;

			sButton1->Enabled = false;
//			sButton2->Enabled = false;
			sButton3->Enabled = false;
		}
	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sButton3Click(TObject *Sender)
{
	btnAuto->Enabled=false;
	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	try
	{
		if(inclCK3)
		{
			Util6Stop();

		} else
		{
			ResetCK3();
			// Выставляем размерность графика
			sListBox1->ItemIndex = 3;       // выставляем первоначальное значение 10 мс
			sListBox1->Selected[3] = false;
			iScope3->TimeBase->SecPerDiv = SecPerDivTable[3];
			sListBox2->ItemIndex = 5;       // выставляем первоначальное значение 1 В
			sListBox2->Selected[5] = false;
			iScope3->Channel[0]->VoltsPerDivision = VoltPerDivTable[5];

			iScope1->SamplesPerSecond = 4167;
			iScope2->SamplesPerSecond = 4167;
			iScope3->SamplesPerSecond = 4167;

			iScope1->DataBlockClear();
			iScope2->DataBlockClear();
			iScope3->DataBlockClear();

			iScope1->Plot->ClearAllData();
			iScope2->Plot->ClearAllData();
			iScope3->Plot->ClearAllData();

			wAsk->Util6Step1();
			wAsk->RunFlag = 1;
			scope = 1;
			stepUtil6 = 1;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

			Timer1->Interval = 3500;                 // Шаг таймера
			Timer1->Enabled = true;
			WorkMode = MODE_UTIL6;

			sButton3->Caption = "Сигнал СК3 ОТМЕНА";
			inclCK3 = true;

			sButton1->Enabled = false;
			sButton2->Enabled = false;
//			sButton3->Enabled = false;
		}
	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
static char TextK2[] = "Порог 1 - 0\nX6 - U = %.1f В";
static char TextK3[] = "Порог 1 - 0\nX6 - U = %.1f В";
static char TextK4[] = "Порог 1 - 0\nX6 - U = %.1f В";
void __fastcall TASK_2::Timer1Timer(TObject *Sender)
{
	try
	{
		if( WorkMode == MODE_UTIL4_STEP1 )
		{
			// Проверка значения
			if(wAsk->realValueK1 >= REAL_K1_MIN && wAsk->realValueK1 <= REAL_K1_MAX)
			{
				sPanel15->Font->Color = clLime;
				sPanel13->Font->Color = clLime;
				sPanel13->Caption = "НОРМА";
				wAsk->CheckASK2[0] = 1;
			} else {
				sPanel15->Font->Color = clRed;
				sPanel13->Font->Color = clRed;
				sPanel13->Caption = "ОШИБКА";
				wAsk->CheckASK2[0] = 2;
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			}
			if (ModeAuto==0) //если автоматическая проверка, присваиваем номер для след шага, запускаем таймер
			{
			  stepAuto=1;
			  timeZader->Enabled=true;
			  timeZader->Interval=1000;
			}
			CheckEndASK2();
			Util4Stop();
		}
//***************** Util 5 ***************************
		if( WorkMode == MODE_UTIL5 )
		{
			double maxVal = maxValueUtil2;

			switch(wAsk->Util5Step2_4())
			{
				case RET_END:{AnsiString form_string;
							  form_string.printf( TextK2, maxVal );
							  switch(wAsk->NumStep)
							  {
								case 4:{sPanel35->Caption = form_string;
										sPanel35->Font->Color = clLime;
										break;}
                              }
//							  AnsiString form_string;
//							  form_string.printf( TextK2, wAsk->Util5Result[0] );
//							  sPanel21->Caption = form_string;
//							  form_string.printf( TextK3, wAsk->Util5Result[1] );
//							  sPanel9->Caption = form_string;
//							  form_string.printf( TextK4, wAsk->Util5Result[2] );
//							  sPanel11->Caption = form_string;

							  sPanel25->Font->Color = clLime;
							  sPanel25->Caption = "НОРМА";
							  sPanel31->Font->Color = clLime;
							  sPanel31->Caption = "НОРМА";
							  sPanel34->Font->Color = clLime;
							  sPanel34->Caption = "НОРМА";

//							  sPanel21->Font->Color = clLime;
//							  sPanel9->Font->Color = clLime;
//							  sPanel11->Font->Color = clLime;

							  wAsk->CheckASK2[1] = 1;
							  CheckEndASK2();
							  if (ModeAuto==0) //если автоматическая проверка, присваиваем номер для след шага, запускаем таймер
			{
			  stepAuto=2;
			  timeZader->Enabled=true;
			  timeZader->Interval=1000;

			}
							  Util5Stop();
							  break;}

				case RET_OK:{AnsiString form_string;
							 form_string.printf( TextK2, maxVal );
							 switch(wAsk->NumStep)
							 {
								case 2:{sPanel26->Caption = form_string;
										sPanel26->Font->Color = clLime;
										break;}
								case 3:{sPanel32->Caption = form_string;
										sPanel32->Font->Color = clLime;
										break;}
                             }
//							 AnsiString form_string;
//							 form_string.printf( TextK2, wAsk->Util5Result[0] );
//							 sPanel21->Caption = form_string;
//							 form_string.printf( TextK3, wAsk->Util5Result[1] );
//							 sPanel9->Caption = form_string;
//							 form_string.printf( TextK4, wAsk->Util5Result[2] );
//							 sPanel11->Caption = form_string;

							 sPanel25->Font->Color = clLime;
							 sPanel25->Caption = "НОРМА";
							 sPanel31->Font->Color = clLime;
							 sPanel31->Caption = "НОРМА";
							 sPanel34->Font->Color = clLime;
							 sPanel34->Caption = "НОРМА";

//							  sPanel21->Font->Color = clLime;
//							  sPanel9->Font->Color = clLime;
//							  sPanel11->Font->Color = clLime;

							 break;}
				case RET_ERROR:{switch(wAsk->NumStep - 1)
								{
									case 1:{sPanel25->Font->Color = clRed;
											sPanel25->Caption = "ОШИБКА";
											sPanel26->Font->Color = clRed;
												BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
											break;}
									case 2:{sPanel31->Font->Color = clRed;
											sPanel31->Caption = "ОШИБКА";
												BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
											sPanel32->Font->Color = clRed;
											break;}
									case 3:{sPanel34->Font->Color = clRed;
											sPanel34->Caption = "ОШИБКА";
												BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
											sPanel35->Font->Color = clRed;
											break;}
								}
								wAsk->CheckASK2[1] = 2;
								CheckEndASK2();
								if (ModeAuto==0) //если автоматическая проверка, присваиваем номер для след шага, запускаем таймер
			{
			  stepAuto=2;
			  timeZader->Enabled=true;
			  timeZader->Interval=1000;
			}
								Util5Stop();
								break;}
			}


		}
//***************** Util 6 ***************************
		if( WorkMode == MODE_UTIL6 )
		{
//			TerminateThread(hnd_scope_thread2, 0);	// Останавливаем поток первого шага
			wAsk->Util6Step2_6();
			stepUtil6 = 2;

//			HANDLE hnd_scope_thread;
//			DWORD scope_thread_id;
//			DWORD thread_status;
//
//			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

			WorkMode = MODE_UTIL6_STEP2;
			Timer1->Interval = 3500;

			return;
		}

		if( WorkMode == MODE_UTIL6_STEP2 ) {
			Util6Stop();

			if( countPorog_5XP1 >= 3 && (count_sample_5XP1_min < count_sample_5XP1_max) ) {
				sPanel14->Font->Color = clLime;
				sPanel14->Caption = "НОРМА";
				wAsk->CheckASK2[2] = 1;
			} else {
				sPanel14->Font->Color = clRed;
				sPanel14->Caption = "ОШИБКА";
				wAsk->CheckASK2[2] = 2;
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			}

			if( countPorog_7XP1 >= 3 && (count_sample_7XP1_min < count_sample_7XP1_max) ) {
				sPanel16->Font->Color = clLime;
				sPanel16->Caption = "НОРМА";
				wAsk->CheckASK2[3] = 1;
			} else {
				sPanel16->Font->Color = clRed;
				sPanel16->Caption = "ОШИБКА";
				wAsk->CheckASK2[3] = 2;
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			}
			// Для отчёта
			if(countPorog_5XP1 >= 3) {
				wAsk->note_CK3_5XP1_30 = "импульсы есть";
				wAsk->note2_CK3_5XP1_30 = "Норма";
			} else {
				wAsk->note_CK3_5XP1_30 = "импульсов нет";
				wAsk->note2_CK3_5XP1_30 = "Неисправность";
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			}
			if( count_sample_5XP1_min < count_sample_5XP1_max ) {
				wAsk->note_CK3_5XP1_120 = "8.5";
				wAsk->note2_CK3_5XP1_120 = "Норма";
			} else {
				wAsk->note_CK3_5XP1_120 = "импульсы есть";
				wAsk->note2_CK3_5XP1_120 = "Неисправность";
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			}

			if(countPorog_7XP1 >= 3) {
				wAsk->note_CK3_7XP1_120 = "импульсы есть";
				wAsk->note2_CK3_7XP1_120 = "Норма";
			} else {
				wAsk->note_CK3_7XP1_120 = "импульсов нет";
				wAsk->note2_CK3_7XP1_120 = "Неисправность";
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			}
			if( count_sample_7XP1_min < count_sample_7XP1_max ) {
				wAsk->note_CK3_7XP1_30 = "8.5";
				wAsk->note2_CK3_7XP1_30 = "Норма";
			} else {
				wAsk->note_CK3_7XP1_30 = "импульсы есть";
				wAsk->note2_CK3_7XP1_30 = "Неисправность";
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			}
               	if (ModeAuto==0) //если автоматическая проверка, присваиваем номер для след шага, запускаем таймер
			{
			  stepAuto=-1;
			  timeZader->Enabled=true;
			  timeZader->Interval=1000;
			}
			CheckEndASK2();
										  // Проверяем все ли проверки выполнены
		}

	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}


}
//---------------------------------------------------------------------------
void __fastcall TASK_2::Util4Stop()
{
	wAsk->UtilStop();
	inclCK1 = false;
	Timer1->Enabled = false;
	sButton1->Caption = "Сигнал СК1 ЗАПУСК";
	WorkMode = MODE_NULL;
	 if (ModeAuto==0)
	  {
	sButton1->Enabled = false;
	sButton2->Enabled = false;
	sButton3->Enabled = false;
	} else {
//	sButton1->Enabled = true;
	sButton2->Enabled = true;
	sButton3->Enabled = true; }
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::Util5Stop()
{
	wAsk->UtilStop();
	inclCK2 = false;
	Timer1->Enabled = false;
	sButton2->Caption = "Сигнал СК2 ЗАПУСК";
	WorkMode = MODE_NULL;
	if (ModeAuto!=0) {
      sButton1->Enabled = true;
//	sButton2->Enabled = true;
	sButton3->Enabled = true;
	}
	else{
	sButton1->Enabled = false;
	sButton2->Enabled = false;
	sButton3->Enabled = false;
	}


}
//---------------------------------------------------------------------------
void __fastcall TASK_2::Util6Stop()
{
	wAsk->UtilStop();
	inclCK3 = false;
	Timer1->Enabled = false;
	sButton3->Caption = "Сигнал СК3 ЗАПУСК";
	WorkMode = MODE_NULL;
	if (ModeAuto==0)
	  {
	sButton1->Enabled = false;
	sButton2->Enabled = false;
	sButton3->Enabled = false;
	sButton1->Visible=true;
sButton2->Visible=true;
sButton3->Visible=true;
	} else {
	sButton1->Enabled = true;
	sButton2->Enabled = true;
//	sButton3->Enabled = true;
}
}
//---------------------------------------------------------------------------
// Функция потока рисования графика

static char Text4[] = "U = %.2f В";
DWORD WINAPI TASK_2::ScopeThread(TASK_2 *ASK_2)
{
	bool NeedPacket;
	bool NeedPacket2;
	bool NeedPacket3;
	int i;
	int count = 0;
	double maxValue;
	AnsiString form_string;

	ASK_2->countPorog_5XP1 = 0;
	ASK_2->countPorog_7XP1 = 0;

//	ASK_2->Uroven1_5XP1 = true;
//	ASK_2->Uroven1_7XP1 = true;

	ASK_2->count_sample_5XP1_min = 0;
	ASK_2->count_sample_5XP1_max = 0;
	ASK_2->count_sample_7XP1_min = 0;
	ASK_2->count_sample_7XP1_max = 0;

	ASK_2->maxValueUtil2 = 0;                 // Инициализируем переменную

	if(ASK_2->wAsk->BlockNumberThread == 100) // проверка инициализации переменной
		{
			ASK_2->CreateTh();                   // повторный запуск потока через паузу
			ExitThread(0);
			return 0;
		}

	switch(ASK_2->scope)
	{
		case 1:{
				while(ASK_2->wAsk->RunFlag) {

					if(count == ACQ_BLOCK_QNT) count = 0;

					NeedPacket3 = ASK_2->iScope3->NeedPacketsNow;
					NeedPacket = ASK_2->iScope2->NeedPacketsNow;
					NeedPacket2 = ASK_2->iScope1->NeedPacketsNow;
					if(!NeedPacket || !NeedPacket2 || !NeedPacket3) {

						if(!NeedPacket) ASK_2->iScope2->DataBlockClear();
						if(!NeedPacket2) ASK_2->iScope1->DataBlockClear();
						if(!NeedPacket3) ASK_2->iScope3->DataBlockClear();

					}// else {

						if(!ASK_2->iScope2->DataBlockActive && !ASK_2->iScope2->TransferringActve
							&& !ASK_2->iScope1->DataBlockActive && !ASK_2->iScope1->TransferringActve
							&& !ASK_2->iScope3->DataBlockActive && !ASK_2->iScope3->TransferringActve) {

							if(ASK_2->wAsk->AcqBlockReady[count]) {

								ASK_2->iScope3->DataBlockBegin();
								ASK_2->iScope2->DataBlockBegin();
								ASK_2->iScope1->DataBlockBegin();

								int porog = 0;
								bool definPorog = true;
								for( i=0; i < ACQ_BLOCK_SIZE; ) {
									ASK_2->iScope3->AddChannelData(0, ASK_2->wAsk->AcqBuf[count][i++]);

									ASK_2->iScope2->AddChannelData(0, ASK_2->wAsk->AcqBuf[count][i]);
									switch(ASK_2->stepUtil6) {
										case 1:{// Определяем прямоугольный импульс
												if(definPorog == true) {
													if(ASK_2->wAsk->AcqBuf[count][i] < 4 && ASK_2->wAsk->AcqBuf[count][i+3] > 4)
														porog++;
//														if(porog == 1) countSamples++;
													if(porog > 1) {
														ASK_2->countPorog_5XP1++;
														definPorog = false;
													}
												}
												break;}
										case 2:{
												if(ASK_2->wAsk->AcqBuf[count][i] < 4) {
//													ASK_2->Uroven1_5XP1 = false;
													ASK_2->count_sample_5XP1_min++;
												} else ASK_2->count_sample_5XP1_max++;
												break;}

									}
									i++;

									ASK_2->iScope1->AddChannelData(0, ASK_2->wAsk->AcqBuf[count][i]);
									switch(ASK_2->stepUtil6) {
										case 1:{
												if(ASK_2->wAsk->AcqBuf[count][i] < 4) {
//													ASK_2->Uroven1_7XP1 = false;
													ASK_2->count_sample_7XP1_min++;
												} else ASK_2->count_sample_7XP1_max++;
												break;}
										case 2:{// Определяем прямоугольный импульс
												if(definPorog == true) {
													if(ASK_2->wAsk->AcqBuf[count][i] < 4 && ASK_2->wAsk->AcqBuf[count][i+3] > 4)
														porog++;
//														if(porog == 1) countSamples++;
													if(porog > 1) {
														ASK_2->countPorog_7XP1++;
														definPorog = false;
													}
												}
												break;}
									}
									i++;

								}

								ASK_2->iScope3->DataBlockEnd();
								ASK_2->iScope2->DataBlockEnd();
								ASK_2->iScope1->DataBlockEnd();

								ASK_2->wAsk->AcqBlockReady[count] = 0;
								count++;
							}
						}
//					}
				}
				break;}
		case 2:{
				while(ASK_2->wAsk->RunFlag) {

					if(count == ACQ_BLOCK_QNT) count = 0;

					NeedPacket = ASK_2->iScope3->NeedPacketsNow;
					if(!NeedPacket) {
						ASK_2->iScope3->DataBlockClear();
					} else {

						if(!ASK_2->iScope3->DataBlockActive && !ASK_2->iScope3->TransferringActve) {

							if(ASK_2->wAsk->AcqBlockReady[count]) {

								ASK_2->iScope3->DataBlockBegin();
								maxValue = 0;
								for(i=0; i < ACQ_BLOCK_SIZE; i++) {
									ASK_2->iScope3->AddChannelData(0, ASK_2->wAsk->AcqBuf[count][i]);
									if(maxValue < ASK_2->wAsk->AcqBuf[count][i]) maxValue = ASK_2->wAsk->AcqBuf[count][i];
								}
								ASK_2->iScope3->DataBlockEnd();

								//ASK_2->wAsk->AcqBlockReady[ASK_2->wAsk->BlockNumberThread] = 0;
								ASK_2->wAsk->AcqBlockReady[count] = 0;
								count++;
							}
						}
					}
					ASK_2->wAsk->realValueK1 = maxValue / sqrt(2);
					form_string.printf( Text4, ASK_2->wAsk->realValueK1);
//					// Проверка значения
//					if(ASK_2->wAsk->realValueK1 >= REAL_K1_MIN && ASK_2->wAsk->realValueK1 <= REAL_K1_MAX)
//					{
//						ASK_2->sPanel15->Font->Color = clLime;
//						ASK_2->sPanel13->Font->Color = clLime;
//						ASK_2->sPanel13->Caption = "НОРМА";
//						ASK_2->wAsk->CheckASK2[0] = 1;
//					} else {
//						ASK_2->sPanel15->Font->Color = clRed;
//						ASK_2->sPanel13->Font->Color = clRed;
//						ASK_2->sPanel13->Caption = "ОШИБКА";
//						ASK_2->wAsk->CheckASK2[0] = 2;
//					}

					ASK_2->sPanel15->Caption = form_string;
				}
				break;}

		case 3:{
				while(ASK_2->wAsk->RunFlag) {

					if(count == ACQ_BLOCK_QNT) count = 0;

					NeedPacket = ASK_2->iScope3->NeedPacketsNow;
					if(!NeedPacket) {
						ASK_2->iScope3->DataBlockClear();
					} else {

						if(!ASK_2->iScope3->DataBlockActive && !ASK_2->iScope3->TransferringActve) {

							if(ASK_2->wAsk->AcqBlockReady[count]) {

								ASK_2->iScope3->DataBlockBegin();
								for(i=0; i < ACQ_BLOCK_SIZE; i += 4) {
									ASK_2->iScope3->AddChannelData(0, ASK_2->wAsk->AcqBuf[count][i]);
									if(ASK_2->maxValueUtil2 < ASK_2->wAsk->AcqBuf[count][i])
										ASK_2->maxValueUtil2 = ASK_2->wAsk->AcqBuf[count][i];
								}
								ASK_2->iScope3->DataBlockEnd();

								//ASK_2->wAsk->AcqBlockReady[ASK_2->wAsk->BlockNumberThread] = 0;
								ASK_2->wAsk->AcqBlockReady[count] = 0;
								count++;
							}
						}
					}
				}
				break;}
	}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
// Функция повторного запуска графика
void __fastcall TASK_2::CreateTh()
{
	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	Sleep(500);
	hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);
}
//---------------------------------------------------------------------------
// Кнопка завершения проверки и отключения питания
void __fastcall TASK_2::sButton4Click(TObject *Sender)
{
//	wAsk->Util0Step1();
//	wAsk->UtilStop(1);		// Стоп с отключением питания

	this->Close();          // При закрытии отключается питание
}
//---------------------------------------------------------------------------
// Обнуляем всё в точке К1
void __fastcall TASK_2::ResetK1()
{
	sPanel15->Font->Color = clWhite;
	sPanel15->Caption = "U =";
    sPanel13->Font->Color = clWhite;
	sPanel13->Caption = " ";

	iScope3->DataBlockClear();
	iScope3->Plot->ClearAllData();
}
//---------------------------------------------------------------------------
// Обнуляем всё в точке К2, К3 и К4
void __fastcall TASK_2::ResetK2K3K4()
{
	sPanel25->Font->Color = clWhite;
	sPanel25->Caption = "";
	sPanel26->Font->Color = clWhite;
	sPanel26->Caption = "Порог 1 - 0\nX6 - U = ";

	sPanel31->Font->Color = clWhite;
	sPanel31->Caption = "";
	sPanel32->Font->Color = clWhite;
	sPanel32->Caption = "Порог 1 - 0\nX6 - U = ";

	sPanel34->Font->Color = clWhite;
	sPanel34->Caption = "";
	sPanel35->Font->Color = clWhite;
	sPanel35->Caption = "Порог 1 - 0\nX6 - U = ";

	iScope3->DataBlockClear();
	iScope3->Plot->ClearAllData();
}
//---------------------------------------------------------------------------
// Обнуление для кнопки СК3
void __fastcall TASK_2::ResetCK3()
{
	sPanel14->Font->Color = clWhite;
	sPanel14->Caption = "";

	sPanel16->Font->Color = clWhite;
	sPanel16->Caption = "";
}
//---------------------------------------------------------------------------
// Функция, в которой проверяем выполнены ли все утилиты
void __fastcall TASK_2::CheckEndASK2()
{
	for(int i=0; i<PARAMETERS_ASK2; i++) {
		if(wAsk->CheckASK2[i] == 0) return;
	}

	sButton4->Enabled = true;
	sButton5->Enabled = true;
	sButton6->Enabled = true;
	sButton7->Enabled = true;
	sButton8->Enabled = true;

	for(int i=0; i<PARAMETERS_ASK2; i++) {
		if(wAsk->CheckASK2[i] == 2 ) {
			sPanel3->Font->Color = clRed;
			sPanel3->Caption = "Блок неисправен";
				BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"2.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);

//			WorkModeEND = RET_ERROR;
			return;
		}
	}

	sPanel3->Font->Color = clLime;
	sPanel3->Caption = "Параметры в норме";
		BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"1.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
}
//---------------------------------------------------------------------------
// Кнопка Повторить экран 2
void __fastcall TASK_2::sButton5Click(TObject *Sender)
{
	TASK_2 *ASK_2 = new TASK_2(0, wAsk);
	if(!ASK_2) return;
	ASK_2->Show();
	power = false;
    ModeAuto = -1;
	this->Close();
}
//---------------------------------------------------------------------------
// Кнопка Вернуться к экрану 1
void __fastcall TASK_2::sButton6Click(TObject *Sender)
{
	TASK_1 *ASK_1 = new TASK_1(0, wAsk->NumberBlock);
	if(!ASK_1) return;
	ASK_1->Show();

	this->Close();
}
//---------------------------------------------------------------------------
// Печать отчёта
void __fastcall TASK_2::sButton7Click(TObject *Sender)
{
	wAsk->WriteExcel("\\RaportASK.xlsx", true);
}
//---------------------------------------------------------------------------
// Показ отчёта
void __fastcall TASK_2::sButton8Click(TObject *Sender)
{
	wAsk->WriteExcel("\\RaportASK.xlsx", false);

	TVsReport *vsReport = new TVsReport(this, wAsk->NumberBlock);
	if(!vsReport) return;
	vsReport->ShowModal();
	delete vsReport;

	DeleteFile(GetCurrentDir() + "\\~" + wAsk->NumberBlock + ".xlsx");
}
//---------------------------------------------------------------------------

//------------------------/ Обработчики графиков /---------------------------
void __fastcall TASK_2::sListBox1Click(TObject *Sender)
{
	sListBox1->Selected[sListBox1->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox2Click(TObject *Sender)
{
	sListBox2->Selected[sListBox2->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox3Click(TObject *Sender)
{
	sListBox3->Selected[sListBox3->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox4Click(TObject *Sender)
{
	sListBox4->Selected[sListBox4->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox5Click(TObject *Sender)
{
	sListBox5->Selected[sListBox5->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox6Click(TObject *Sender)
{
	sListBox6->Selected[sListBox6->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox7Click(TObject *Sender)
{
	sListBox7->Selected[sListBox7->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox8Click(TObject *Sender)
{
	sListBox8->Selected[sListBox8->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox9Click(TObject *Sender)
{
	sListBox9->Selected[sListBox9->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{
	FocusControl(iScope3); // переводим фокус, чтобы колёсиком мышки нельзя было поменять значение
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox1VScroll(TObject *Sender)
{
	iScope3->TimeBase->SecPerDiv = SecPerDivTable[sListBox1->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox4VScroll(TObject *Sender)
{
	iScope2->TimeBase->SecPerDiv = SecPerDivTable[sListBox4->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox7VScroll(TObject *Sender)
{
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[sListBox7->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox2VScroll(TObject *Sender)
{
	iScope3->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox2->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox5VScroll(TObject *Sender)
{
	iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox5->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox8VScroll(TObject *Sender)
{
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox8->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox3VScroll(TObject *Sender)
{
	if(sListBox3->TopIndex == 0) iScope3->Channel[0]->Coupling = isccDC;
		else iScope3->Channel[0]->Coupling = isccAC;
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox6VScroll(TObject *Sender)
{
	if(sListBox6->TopIndex == 0) iScope2->Channel[0]->Coupling = isccDC;
		else iScope2->Channel[0]->Coupling = isccAC;
}
//---------------------------------------------------------------------------
void __fastcall TASK_2::sListBox9VScroll(TObject *Sender)
{
	if(sListBox9->TopIndex == 0) iScope1->Channel[0]->Coupling = isccDC;
		else iScope1->Channel[0]->Coupling = isccAC;
}
//---------------------------------------------------------------------------



void __fastcall TASK_2::btnAutoClick(TObject *Sender)
{
sButton1->Enabled=false;
sButton1->Visible=true;
sButton2->Visible=false;
sButton3->Visible=false;
btnAuto->Enabled=false;
ModeAuto=0;
	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	try
	{
		if(inclCK1)
		{
			Util4Stop();

		} else {
			ResetK1();
			//Выставляем размерность графика
			sListBox1->ItemIndex = 2;       // выставляем первоначальное значение 20 мс
			sListBox1->Selected[2] = false;
			iScope3->TimeBase->SecPerDiv = SecPerDivTable[2];
			sListBox2->ItemIndex = 5;       // выставляем первоначальное значение 1 В
			sListBox2->Selected[5] = false;
			iScope3->Channel[0]->VoltsPerDivision = VoltPerDivTable[5];

			iScope3->SamplesPerSecond = 1563;

			wAsk->Util4Step1();
			wAsk->RunFlag = 1;
			scope = 2;                     // Рисуем график на iScope3

			Timer1->Interval = 9000;
			Timer1->Enabled = true;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

			WorkMode = MODE_UTIL4_STEP1;

			sButton1->Caption = "Сигнал СК1 ОТМЕНА";
			inclCK1 = true;

//			sButton1->Enabled = false;
			sButton2->Enabled = false;
			sButton3->Enabled = false;
		}

	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
   int  time_step=0;
void __fastcall TASK_2::timeZaderTimer(TObject *Sender)
{
if (time_step>3) {
 timeAuto->Interval = 1000;
 timeAuto->Enabled = true;
 timeZader->Enabled=false;
}
 time_step++;
}
//---------------------------------------------------------------------------

void __fastcall TASK_2::timeAutoTimer(TObject *Sender)
{
   if (stepAuto==1)
   {
   sButton1->Visible=false;
sButton2->Visible=true;
sButton3->Visible=false;
     	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	try
	{
		if(inclCK2)
		{
			Util5Stop();

		} else
		{
			ResetK2K3K4();
			// Выставляем размерность графика
			sListBox1->ItemIndex = 2;       // выставляем первоначальное значение 20 мс
			sListBox1->Selected[2] = false;
			iScope3->TimeBase->SecPerDiv = SecPerDivTable[2];
			sListBox2->ItemIndex = 4;       // выставляем первоначальное значение 2 В
			sListBox2->Selected[4] = false;
			iScope3->Channel[0]->VoltsPerDivision = VoltPerDivTable[4];

			iScope3->SamplesPerSecond = 391;

			wAsk->Util5Step1();
			wAsk->RunFlag = 1;
			scope = 3;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

			Timer1->Interval = 5000;                 // Шаг таймера
			Timer1->Enabled = true;
			WorkMode = MODE_UTIL5;

			sButton2->Caption = "Сигнал СК2 ОТМЕНА";
			inclCK2 = true;

			sButton1->Enabled = false;
//			sButton2->Enabled = false;
			sButton3->Enabled = false;
		}
	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
	timeAuto->Enabled=false;
	}
	if (stepAuto==2)
	{
	sButton1->Visible=false;
sButton2->Visible=false;
sButton3->Visible=true;
	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	try
	{
		if(inclCK3)
		{
			Util6Stop();

		} else
		{
			ResetCK3();
			// Выставляем размерность графика
			sListBox1->ItemIndex = 3;       // выставляем первоначальное значение 10 мс
			sListBox1->Selected[3] = false;
			iScope3->TimeBase->SecPerDiv = SecPerDivTable[3];
			sListBox2->ItemIndex = 5;       // выставляем первоначальное значение 1 В
			sListBox2->Selected[5] = false;
			iScope3->Channel[0]->VoltsPerDivision = VoltPerDivTable[5];

			iScope1->SamplesPerSecond = 4167;
			iScope2->SamplesPerSecond = 4167;
			iScope3->SamplesPerSecond = 4167;

			iScope1->DataBlockClear();
			iScope2->DataBlockClear();
			iScope3->DataBlockClear();

			iScope1->Plot->ClearAllData();
			iScope2->Plot->ClearAllData();
			iScope3->Plot->ClearAllData();

			wAsk->Util6Step1();
			wAsk->RunFlag = 1;
			scope = 1;
			stepUtil6 = 1;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

			Timer1->Interval = 3500;                 // Шаг таймера
			Timer1->Enabled = true;
			WorkMode = MODE_UTIL6;

			sButton3->Caption = "Сигнал СК3 ОТМЕНА";
			inclCK3 = true;

			sButton1->Enabled = false;
			sButton2->Enabled = false;
//			sButton3->Enabled = false;
		}
	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
	timeAuto->Enabled=false;
	}
}
//---------------------------------------------------------------------------

