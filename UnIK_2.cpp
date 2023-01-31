//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnIK_2.h"
#include "WIk.h"
#include "UnIK_1.h"
#include "UnVisibleReport.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "iScope"
#pragma link "sButton"
#pragma link "sLabel"
#pragma link "sListBox"
#pragma link "sPanel"
#pragma link "sRadioButton"
#pragma link "sBitBtn"
#pragma link "iComponent"
#pragma link "iCustomComponent"
#pragma link "iSevenSegmentDisplay"
#pragma link "iSevenSegmentInteger"
#pragma link "iVCLComponent"
#pragma resource "*.dfm"

#include <bass.h>
#include <process.h>
#include <stdio.h>
#include <windows.h>
//TIK_2 *IK_2;
const double TIK_2::SecPerDivTable[19] = {0.1, 0.05, 0.02, 0.01, 0.005, 0.002, 0.001,
0.0005, 0.0002, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06, 5E-07, 2E-07, 1E-07};
int ModeAuto=-1;
const double TIK_2::VoltPerDivTable[22] = {50, 20, 10, 5, 2, 1, 0.5, 0.2, 0.1, 0.05, 0.02,
0.01, 0.005, 0.002, 0.001, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06};
bool provT=false;
bool provT1=false;
HSAMPLE sample;
	HCHANNEL channel;
//---------------------------------------------------------------------------
__fastcall TIK_2::TIK_2(TComponent* Owner, WIk *wik)
	: TForm(Owner)
{
	wIk = wik;

	inclPower = false;                // Переменная, которая фиксирует нажатие кнопки питания
	stepUprImp2 = 0;
	stepUprImp3 = 0;
	 provT=false;
	 provT1=false;
//	power = true;

	sListBox4->ItemIndex = 0;       		// выставляем первоначальное значение 0,1 с
	sListBox4->Selected[0] = false;
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[0];
	sListBox5->ItemIndex = 4;       		// выставляем первоначальное значение 2 В
	sListBox5->Selected[4] = false;
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[4];

	sListBox3->ItemIndex = 0;       		// выставляем первоначальное значение 0,1 с
	sListBox3->Selected[0] = false;
	iScope2->TimeBase->SecPerDiv = SecPerDivTable[0];
	sListBox1->ItemIndex = 4;       		// выставляем первоначальное значение 2 В
	sListBox1->Selected[4] = false;
	iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[4];

	// Выводим номер блока
	switch(wIk->Block) {
		case BLOCK_BU:{sLabel4->Caption = "X18";
					   sLabel16->Caption ="Блок БУ №" + wIk->NumberBlock;
					   break;}
		case BLOCK_IP:{sLabel4->Caption = "X29";
					   sLabel16->Caption ="Блок ИП №" + wIk->NumberBlock;
					   break;}
	}
	sLabel16->Left = (1920 - sLabel16->Width) / 2;
	sLabel16->Visible = true;

	// Инициализируем массив проверочных переменных
	for(int i=0; i<PARAMETERS_IK2; i++) {
		wIk->CheckIK2[i] = 0;
	}
//		stepUprImp2 = 1;
}
//---------------------------------------------------------------------------
__fastcall TIK_2::~TIK_2()
{
//	delete wIk;
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
//	if(wIk->RunFlag) {
//		wIk->UtilStop(1);	// выключение модулей, если они запущены
//	}

	wIk->UtilStop(1);	// выключение модулей, если они запущены
//	power = true;
}
//---------------------------------------------------------------------------
// Кнопка питания
void __fastcall TIK_2::sBitBtn1Click(TObject *Sender)
{
	if(inclPower)
	{
		sBitBtn1->Down = true;
		Util4Stop();                    // Там отключаем питание
		ResetUtil5();
		ResetUtil6();
		ResetUtil7();
	} else {
		inclPower = true;
		sBitBtn1->Down = true;
		sBitBtn1->Enabled = false;

		WorkMode = MODE_UTIL4_STEP4;
		wIk->Util4Step4();
		Timer1->Enabled = true;
		Timer1->Interval = 1000;                 // Шаг 1 с
		step = 1;

//		sButton3->Enabled = false;
		iSevenSegmentInteger1->Visible = true;

		checkK4 = false;                         // Переменные для проверки
		checkK5 = false;                         //
	}
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sButton2Click(TObject *Sender)
{
  sButton1->Enabled = false;
  sButton2->Enabled = false;

  switch(stepUprImp2) {
	case 0:{
//		if(wIk->IsOpenedLTR34() == 0) return; 			// Если запущена другая проверка

		ResetUtil5();
		wIk->RunFlag = 1;

		wIk->Util5Step1(wIk->Block);
		WorkMode = MODE_UTIL5_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 1500;

//		sButton2->Enabled = false;
		stepUprImp2 = 1;
		break;}
	case 1:{
		ResetUtil6();
		wIk->RunFlag = 1;

		wIk->Util6Step1(wIk->Block);
		WorkMode = MODE_UTIL6_STEP3;
		Timer1->Enabled = true;
		Timer1->Interval = 1500;                       // Длительность первого шага

//		sButton2->Enabled = false;
		stepUprImp2 = 2;
		break;}
	case 2:{
		ResetUtil7();
		wIk->RunFlag = 1;

//		HANDLE hnd_scope_thread;
//		DWORD scope_thread_id;
//		DWORD thread_status;
//
//		hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

		wIk->Util7Step1();
		WorkMode = MODE_UTIL7_STEP4;
		Timer1->Enabled = true;
		Timer1->Interval = 1000;                       // Длительность первого шага

//		sButton2->Enabled = false;
		stepUprImp2 = 0;
		break;}
  }
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sButton1Click(TObject *Sender)
{
  sButton1->Enabled = false;
  sButton2->Enabled = false;

  switch(stepUprImp3) {
	case 0:{
//		ResetUtil4K3();
		wIk->RunFlag = 1;

		wIk->Util6Step1(wIk->Block);
		WorkMode = MODE_UTIL6_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 1000;                       // Длительность первого шага

//		sButton1->Enabled = false;
		stepUprImp3 = 1;
		break;}
	case 1:{
//		ResetUtil4K3();
		wIk->RunFlag = 1;

		wIk->Util7Step1();
		WorkMode = MODE_UTIL7_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 1000;                       // Длительность первого шага

//		sButton1->Enabled = false;
		stepUprImp3 = 2;
		break;}
	case 2:{
//		ResetUtil4K3();
		wIk->RunFlag = 1;

		wIk->Util6Step1(wIk->Block);
		WorkMode = MODE_UTIL6_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 1000;                       // Длительность первого шага

//		sButton1->Enabled = false;
		stepUprImp3 = 0;
		break;}
  }
}
//---------------------------------------------------------------------------
static char Text1[] = "U = %.1f В";
static char Text2[] = "t = %.2f c";
static char Text3[] = "t = %.2f c";

void __fastcall TIK_2::Timer1Timer(TObject *Sender)
{
	AnsiString form_string;

	try
	{
//***************** Util 4 ***************************
		if(WorkMode == MODE_UTIL4_STEP4) {
			bool thread = true;
			int count = 0;

			wIk->Util4K4 = 0;                        // Обнуляем переменные
			wIk->Util4K5 = 0;

			while(thread) {
				if(count == ACQ_BLOCK_QNT) break;

				if(wIk->AcqBlockReady[count]) {

					for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
						if( wIk->AcqBuf[count][i] > wIk->Util4K4 ) wIk->Util4K4 = wIk->AcqBuf[count][i];
						i++;
						if( wIk->AcqBuf[count][i] > wIk->Util4K5 ) wIk->Util4K5 = wIk->AcqBuf[count][i];
						i++;
					}
				}
				wIk->AcqBlockReady[count] = 0;
				count++;
			}

			form_string.printf( Text1, wIk->Util4K4 );
			sPanel26->Caption = form_string;
			form_string.printf( Text1, wIk->Util4K5 );
			sPanel17->Caption = form_string;
//			sLabel14->Caption = IntToStr(step);
			iSevenSegmentInteger1->Value = step;

			// Проверка в точке X30(К4)
			if(wIk->Util4K4 >= UTIL4_K4K5_MIN && wIk->Util4K4 <= UTIL4_K4K5_MAX) {

				if( step >= 604 && step <= 624 && checkK4 == false) {
					checkK4 == true;

					if (!provT) {
							 wIk->time1 =  IntToStr(step);
							 provT=true;
					}
					wIk->secK4 = step;
					sPanel25->Caption = "T = " + wIk->time1;   //IntToStr(step);

					sPanel26->Font->Color = clLime;
					sPanel25->Font->Color = clLime;
				}
			} else {
				if(step > 624) {
					sPanel26->Font->Color = clRed;
					sPanel25->Font->Color = clRed;
				}
            }

			// Проверка в точке X31(К5)
			if(wIk->Util4K5 >= UTIL4_K4K5_MIN && wIk->Util4K5 <= UTIL4_K4K5_MAX) {

				if( step >= 620 && step <= 640 && checkK5 == false) {
					checkK5 == true;
					if (!provT1) {
							 wIk->time2 =  IntToStr(step);
							 provT1=true;
					}
					wIk->secK5 = step;
					sPanel16->Caption = "T = " + wIk->time2;

					sPanel17->Font->Color = clLime;
					sPanel16->Font->Color = clLime;
				}
			} else {
				if(step > 640) {
					sPanel17->Font->Color = clRed;
					sPanel16->Font->Color = clRed;
				}
            }

			step++;

			if(step > 660) {
				// Окончательная проверка в точке X30(К4)
				if(wIk->Util4K4 >= UTIL4_K4K5_MIN && wIk->Util4K4 <= UTIL4_K4K5_MAX
				&& wIk->secK4 >= 604 && wIk->secK4 <= 624) {
					sPanel24->Font->Color = clLime;
					sPanel24->Caption = "НОРМА";
					wIk->note_Util4X30 = "Норма";
					wIk->CheckIK2[0] = 1;
				} else {
					sPanel26->Font->Color = clRed;
					sPanel25->Font->Color = clRed;
					sPanel24->Font->Color = clRed;
					sPanel24->Caption = "ОШИБКА";
					wIk->note_Util4X30 = "Неисправность";
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
					wIk->CheckIK2[0] = 2;
				}

				// Окончательная проверка в точке X31(К5)
				if(wIk->Util4K5 >= UTIL4_K4K5_MIN && wIk->Util4K5 <= UTIL4_K4K5_MAX
				&& wIk->secK5 >= 620 && wIk->secK5 <= 640) {
					sPanel15->Font->Color = clLime;
					sPanel15->Caption = "НОРМА";
					wIk->note_Util4X31 = "Норма";
					wIk->CheckIK2[1] = 1;
				} else {
					sPanel17->Font->Color = clRed;
					sPanel16->Font->Color = clRed;
					sPanel15->Font->Color = clRed;
					sPanel15->Caption = "ОШИБКА";
					wIk->note_Util4X31 = "Неисправность";
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
					wIk->CheckIK2[1] = 2;
				}

				Timer1->Enabled = false;
				wIk->UtilStop();
				WorkMode = MODE_NULL;
				sBitBtn1->Font->Color = clLime;
				sBitBtn1->Caption = "ОТКЛЮЧИТЬ ПИТАНИЕ";
				btnAuto->Enabled=true;
				sBitBtn1->Down = false;
				sBitBtn1->Enabled = true;

				sButton2->Enabled = true;
				CheckEndIK2();
			}
		}
//***************** Util 5 ***************************
		if(WorkMode == MODE_UTIL5_STEP1) {
			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;
			 timeAuto->Enabled=false;
			// Создаём ожидающий активации поток
			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wIk->CheckUtil5Thread, wIk, 0, &scope_thread_id);

			wIk->Util5Step2();
//			ResumeThread(hnd_scope_thread);	//активировать поток

			WorkMode = MODE_UTIL5_STEP2;
			if(wIk->Block == BLOCK_BU) {
				Timer1->Interval = 1000;
			} else {
				Timer1->Interval = wIk->TimeMaxUtil5[wIk->number_block - 1]*2000;
			}

			return;
		}

		if(WorkMode == MODE_UTIL5_STEP2) {
			wIk->TimeUtil5 = SimpleRoundTo(wIk->TimeUtil5, -2);
			form_string.printf( Text2, wIk->TimeUtil5 );
			sPanel10->Caption = form_string;

			// В зависимости от блока выбираем минимальное и максимальное значение для сравнения
			double min, max;
			if(wIk->Block == BLOCK_BU) {
				min = UTIL5_TIME_MIN;
				max = UTIL5_TIME_MAX;
			} else {
				min = wIk->TimeMinUtil5[wIk->number_block - 1];
				max = wIk->TimeMaxUtil5[wIk->number_block - 1];
			}

			if(wIk->TimeUtil5 >= min && wIk->TimeUtil5 <= max) {
				sPanel10->Font->Color = clLime;
				sPanel30->Font->Color = clLime;
				sPanel30->Caption = "НОРМА";

				sPanel9->Font->Color = clLime;
				sPanel9->Caption = "Порог 0 - 1\nНОРМА";
				wIk->CheckIK2[2] = 1;
				wIk->note_TimeUtil5 = "Норма";
			} else {
				sPanel10->Font->Color = clRed;
				sPanel30->Font->Color = clRed;
				sPanel30->Caption = "ОШИБКА";
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);

				sPanel9->Font->Color = clRed;
				sPanel9->Caption = "Нет перекл. ОШИБКА";
				wIk->CheckIK2[2] = 2;
				wIk->note_TimeUtil5 = "Неисправность";
            }

			wIk->UtilStop();   CheckEndIK2();
				if(ModeAuto==0)
			{
			stepUprImp3=0;
			timeZader->Enabled=true;
			timeZader->Interval=100;
			sButton1->Enabled = false;
			sButton2->Enabled = false;
			}   else
		 {	sButton1->Enabled = true;
			sButton2->Enabled = true; }
			Timer1->Enabled = false;
			WorkMode = MODE_NULL;

//				stepUprImp2 = 1;
//		timeAutoTimer->Enabled = true;
//		timeAutoTimer->Interval = 1500;

		}
//***************** Util 6 ***************************
		if(WorkMode == MODE_UTIL6_STEP1) {
		   timeAuto->Enabled=false;
			wIk->Util6and7Step2();

			Timer1->Interval = 800;           // Подаём импульсы
			WorkMode = MODE_UTIL6_STEP2;
			return;
		}

		if(WorkMode == MODE_UTIL6_STEP2) {
//			HANDLE hnd_scope_thread;
//			DWORD scope_thread_id;
//			DWORD thread_status;
//
//			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wIk->CheckUtil6Step2_3Thread, wIk, 0, &scope_thread_id);
//			hnd_scope_thread2 = hnd_scope_thread;
////			wIk->Util6Step3and6();
//
//			Timer1->Interval = 10000;           // Третий шаг 10с
//			WorkMode = MODE_UTIL6_STEP3;
//			return;

			wIk->UtilStop();
			 if (ModeAuto==0)
			  {
				stepUprImp2=0;
				timeZader->Enabled=true;
				timeZader->Interval=100;
				sButton1->Enabled = false;
			sButton2->Enabled = false;
			 } else
		  {	sButton1->Enabled = true;
			sButton2->Enabled = true; }
			Timer1->Enabled = false;
			WorkMode = MODE_NULL;

		}

	if(WorkMode == MODE_UTIL6_STEP3) {
	timeAuto->Enabled=false;
//			TDateTime t;
//
//			t = wIk->timeUtil6K8 - wIk->timeUtil6K6;
//			sPanel23->Caption = "t = " + t.FormatString("ss.zzz");
//
//			if( t >= EncodeTime(0, 0, UTIL6_SS_MIN, UTIL6_ZZZ_MIN) && t <= EncodeTime(0, 0, UTIL6_SS_MAX, UTIL6_ZZZ_MAX) ) {
//				sPanel23->Font->Color = clLime;
//			} else {
//				sPanel23->Font->Color = clRed;
//			}
//
//			TerminateThread(hnd_scope_thread2, 0);	// Принудительное завершение потока считывания порогов К6 и К8
////			wIk->Util6Step4();
//
//			Timer1->Interval = 1000;           		// Четвёртый шаг 1с
//			WorkMode = MODE_UTIL6_STEP4;
//			return;

			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wIk->CheckUtil6Step3Thread, wIk, 0, &scope_thread_id);

			wIk->Util6and7Step3();
			if(wIk->Block == BLOCK_BU) {
				Timer1->Interval = 10000;           // Определяем интервал 7,7 с
				}
				else {
					Timer1->Interval = wIk->TimeMaxUtil6[wIk->number_block - 1]*2000;
					}

			WorkMode = MODE_UTIL6_STEP4;
			return;
		}

		if(WorkMode == MODE_UTIL6_STEP4) {
//			TDateTime t;
//
//			t = wIk->timeUtil6K8 - wIk->timeUtil6K6;
//			sPanel23->Caption = "t = " + t.FormatString("ss.z");

			// Переработанный вывод времени
			form_string.printf( Text3, wIk->TimeUtil6 );
			sPanel33->Caption = form_string;

			// В зависимости от блока выбираем минимальное и максимальное значение для сравнения
			double min, max;
			if(wIk->Block == BLOCK_BU) {
				min = UTIL6_TIME_MIN;
				max = UTIL6_TIME_MAX;
			} else {
				min = wIk->TimeMinUtil6[wIk->number_block - 1];
				max = wIk->TimeMaxUtil6[wIk->number_block - 1];
			}
//			else { //Блок ИП (номер блока варьируется от 1 до 6 от 8 до 18 от 20 до 24 )
//				   if (wIk->number_block<=6)
//				   {                           //если номер блока до 7
//					min = wIk->TimeMinUtil6[wIk->number_block - 1];
//					max = wIk->TimeMaxUtil6[wIk->number_block - 1];
//				   }
//				   else
//						 if (wIk->number_block>=8 && wIk->number_block<=18)
//							 {        //если номер блока от 8(вкл) до 19(не вкл)
//								min = wIk->TimeMinUtil6[wIk->number_block -2];
//								max = wIk->TimeMaxUtil6[wIk->number_block -2];
//							 }
//							else
//							 if (wIk->number_block>=20 && wIk->number_block<=24)
//								 {   //если номер блока от 20 до 24
//									min = wIk->TimeMinUtil6[wIk->number_block -3];
//									max = wIk->TimeMaxUtil6[wIk->number_block -3];
//								 }
//			}

//			if( t >= EncodeTime(0, 0, UTIL6_SS_MIN, UTIL6_ZZZ_MIN) && t <= EncodeTime(0, 0, UTIL6_SS_MAX, UTIL6_ZZZ_MAX) ) {
			if( wIk->TimeUtil6 >= min && wIk->TimeUtil6 <= max ) {
				sPanel33->Font->Color = clLime;

				sPanel14->Font->Color = clLime;
				sPanel14->Caption = "Порог 0 - 1\nНОРМА";
				wIk->CheckIK2[3] = 1;
				wIk->note_TimeUtil6 = "Норма";
			} else {
				sPanel33->Font->Color = clRed;

				sPanel14->Font->Color = clRed;
				sPanel14->Caption = "Нет перекл.\nОШИБКА";
				wIk->CheckIK2[3] = 2;
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				wIk->note_TimeUtil6 = "Неисправность";
			}

			wIk->UtilStop(); CheckEndIK2();
			 if (ModeAuto==0)
			 {
			   stepUprImp3=1;
			   timeZader->Enabled=true;
			   timeZader->Interval=100;
			   sButton2->Enabled=false;
			   sButton1->Enabled = false;
			 }  else
		  {	sButton1->Enabled = true;
			sButton2->Enabled = true; }
			Timer1->Enabled = false;
			WorkMode = MODE_NULL;

//				stepUprImp2 = 2;
//		timeAutoTimer->Enabled = true;
//		timeAutoTimer->Interval = 1500;

        }
//***************** Util 7 ***************************
		if(WorkMode == MODE_UTIL7_STEP1) {
//			HANDLE hnd_scope_thread;
//			DWORD scope_thread_id;
//			DWORD thread_status;
//
//			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);
//
////			wIk->Util6Step2and5();
//
//			Timer1->Interval = 3500;           		// Пятый шаг 1с
//			WorkMode = MODE_UTIL6_STEP5;
//			return;
			 timeAuto->Enabled=false;
			wIk->Util6and7Step2();

			Timer1->Interval = 800;           // Подаём импульсы
			WorkMode = MODE_UTIL7_STEP2;
			return;
		}

		if(WorkMode == MODE_UTIL7_STEP2) {
			wIk->UtilStop();
			Timer1->Enabled = false;
			WorkMode = MODE_NULL;
			  if (ModeAuto==0)
			 {
			   stepUprImp2=1;
			   timeZader->Enabled=true;
			   timeZader->Interval=100;
			   sButton2->Enabled=false;
			   sButton1->Enabled = false;
			 }   else
		   {	sButton1->Enabled = true;
			sButton2->Enabled = true; }
		}

		if(WorkMode == MODE_UTIL7_STEP4) {
		timeAuto->Enabled=false;
			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

			wIk->Util6and7Step3();
			Timer1->Interval = 10000;          // Смотрим импульсы на графиках
			WorkMode = MODE_UTIL7_STEP5;
			return;
		}

		if(WorkMode == MODE_UTIL7_STEP5) {
			wIk->UtilStop();
			// Проверка в точке Х34
			if(wIk->PerepadUtil7[0]) {
				sPanel19->Font->Color = clLime;
				sPanel19->Caption = "НОРМА";
				wIk->note_Util7X34 = "Норма";
				wIk->CheckIK2[4] = 1;
			} else {
				sPanel19->Font->Color = clRed;
				sPanel19->Caption = "ОШИБКА";
				wIk->note_Util7X34 = "Неисправность";
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				wIk->CheckIK2[4] = 2;
			}
			// Проверка в точке Х32
			if(wIk->PerepadUtil7[1]) {
				sPanel31->Font->Color = clLime;
				sPanel31->Caption = "НОРМА";
				wIk->CheckIK2[5] = 1;
			} else {
				sPanel31->Font->Color = clRed;
				sPanel31->Caption = "ОШИБКА";
				wIk->CheckIK2[5] = 2;
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			}   CheckEndIK2();
              if (ModeAuto!=0)
			 {
			sButton1->Enabled = true;
			sButton2->Enabled = true;
			}
			else
			{	sButton1->Enabled = false;
			sButton2->Enabled = false; }
			Timer1->Enabled = false;
			WorkMode = MODE_NULL;


			  //	stepUprImp2 = 10;
	 //	timeAutoTimer->Enabled = false;
	   //	timeAutoTimer->Interval = 1500;
        }

	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::Util4Stop()
{
	wIk->UtilStop(1);

	sBitBtn1->Font->Color = clRed;
	sBitBtn1->Caption = "ЗАПУСК ПИТАНИЯ";
	inclPower = false;
	sBitBtn1->Down = false;
}
//---------------------------------------------------------------------------
// Потоковая функция рисования графика
DWORD WINAPI TIK_2::ScopeThread(TIK_2 *IK_2)
{
	int i;
	int count = 0;
	bool NeedPacket1, NeedPacket2;
	AnsiString form_string;
	bool flag1 = false;
	bool flag2 = true;                             // Переменная позволяет рисовать в Х34
	int count_samples = 0;


	IK_2->wIk->Util7X34 = 0;
	IK_2->wIk->Util7X32 = 0;

	for(i=0; i<2; i++) {
		IK_2->wIk->PerepadUtil7[i] = false;
    }

	if(IK_2->wIk->BlockNumberThread == 100) // проверка инициализации переменной
		{
			IK_2->CreateTh();                   // повторный запуск потока через паузу
			ExitThread(0);
			return 0;
		}

	while(IK_2->wIk->RunFlag) {

		if(count == ACQ_BLOCK_QNT) count = 0;

		NeedPacket1 = IK_2->iScope1->NeedPacketsNow;
		NeedPacket2 = IK_2->iScope2->NeedPacketsNow;

//		if(!NeedPacket1 || !NeedPacket2) {
//			if(!NeedPacket1 && flag1 == false) {
//				IK_2->iScope1->DataBlockClear();
//				flag2 == false;
//			}
//			if(!NeedPacket2) IK_2->iScope2->DataBlockClear();
//		}// else {

			if(!IK_2->iScope2->DataBlockActive && !IK_2->iScope2->TransferringActve &&
			   !IK_2->iScope1->DataBlockActive && !IK_2->iScope1->TransferringActve)
			{
				if(IK_2->wIk->AcqBlockReady[count]) {

					IK_2->iScope1->DataBlockBegin();
					IK_2->iScope2->DataBlockBegin();

					for(i=0; i < ACQ_BLOCK_SIZE; ) {
						if(flag2) IK_2->iScope1->AddChannelData(0, IK_2->wIk->AcqBuf[count][i++]);  // Рисуем график в Х34
							else i++;
						// Проверяем на импульс в Х34
						if(IK_2->wIk->AcqBuf[count][i-1] < -4) {
							IK_2->wIk->PerepadUtil7[0] = true;
							flag1 = true;
						}

						// Измеряем минимум импульса в Х34
						if(IK_2->wIk->AcqBuf[count][i-1] < IK_2->wIk->Util7X34) IK_2->wIk->Util7X34 = IK_2->wIk->AcqBuf[count][i-1];

						// Считаем ещё один кадр для графика в Х34
						if(flag1 == true && (count_samples < IK_2->iScope1->SamplesPerSecond + 100) ) count_samples++;

						// Запрещаем рисовать в Х34
						if(count_samples == IK_2->iScope1->SamplesPerSecond + 100) flag2 = false;

						IK_2->iScope2->AddChannelData(0, IK_2->wIk->AcqBuf[count][i++]);
						if(IK_2->wIk->AcqBuf[count][i-1] < -4) IK_2->wIk->PerepadUtil7[1] = true;

						if(IK_2->wIk->AcqBuf[count][i-1] < IK_2->wIk->Util7X32) {
							IK_2->wIk->Util7X32 = IK_2->wIk->AcqBuf[count][i-1];
						}
					}
					IK_2->iScope1->DataBlockEnd();
					IK_2->iScope2->DataBlockEnd();

					IK_2->wIk->AcqBlockReady[count] = 0;
					count++;
				}
			}
		//}
	}// while(IK_1->wIk->RunFlag)

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
// Функция повторного запуска графика
void __fastcall TIK_2::CreateTh()
{
	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	Sleep(300);
	hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::ResetUtil5()
{
	sPanel10->Font->Color = clWhite;
	sPanel10->Caption = "t = ";

	sPanel30->Font->Color = clWhite;
	sPanel30->Caption = "";

	sPanel9->Font->Color = clWhite;
	sPanel9->Caption = "Порог 0 - 1\n";
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::ResetUtil6()
{
	sPanel33->Font->Color = clWhite;
	sPanel33->Caption = "t = ";

	sPanel14->Font->Color = clWhite;
	sPanel14->Caption = "Порог 0 - 1\n";
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::ResetUtil7()
{
	iScope1->DataBlockClear();
	iScope2->DataBlockClear();

	iScope1->Plot->ClearAllData();
	iScope2->Plot->ClearAllData();

	sPanel19->Font->Color = clWhite;
	sPanel19->Caption = "";

	sPanel31->Font->Color = clWhite;
	sPanel31->Caption = "";
}
//---------------------------------------------------------------------------
// Функция проверки завершения всех утилит
void __fastcall TIK_2::CheckEndIK2()
{
	for(int i=0; i<PARAMETERS_IK2; i++) {
		if(wIk->CheckIK2[i] == 0) return;
	}

	sButton3->Enabled = true;
	sButton10->Enabled = true;

	sButton4->Enabled = true;
	sButton5->Enabled = true;
	sButton6->Enabled = true;

	for(int i=0; i<PARAMETERS_IK2; i++) {
		if(wIk->CheckIK2[i] == 2 ) {
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
// Кнопка завершения проверки и отключения питания
void __fastcall TIK_2::sButton4Click(TObject *Sender)
{
//	wIk->UtilStop(1);		// Стоп с отключением питания

	this->Close();          // При закрытии отключается питание
}
//---------------------------------------------------------------------------
// Кнопка Повторить экран 2
void __fastcall TIK_2::sButton5Click(TObject *Sender)
{
	ShowMessage("Прожмите и через 5-6 секунд отпустите кнопку на контактной панели");

	TIK_2 *IK_2 = new TIK_2(0, wIk);
	if(!IK_2) return;
	IK_2->Show();
//	power = false;

	this->Close();
}
//---------------------------------------------------------------------------
// Кнопка Повторить экран 1
void __fastcall TIK_2::sButton6Click(TObject *Sender)
{
	ShowMessage("Прожмите и через 5-6 секунд отпустите кнопку на контактной панели");

	TIK_1 *IK_1 = new TIK_1(0, wIk->NumberBlock, wIk->Block);
	if(!IK_1) return;
	IK_1->Show();

	this->Close();
}
//---------------------------------------------------------------------------
// Кнопка Сохранить отчёт
void __fastcall TIK_2::sButton3Click(TObject *Sender)
{
	wIk->WriteExcel("\\RaportIK.xlsx", true);
}
//---------------------------------------------------------------------------
// Сохраняем и выводим отчёт
void __fastcall TIK_2::sButton10Click(TObject *Sender)
{
	wIk->WriteExcel("\\RaportIK.xlsx", false);

	TVsReport *vsReport = new TVsReport(this, wIk->NumberBlock);
	if(!vsReport) return;
	vsReport->ShowModal();
	delete vsReport;

	DeleteFile(GetCurrentDir() + "\\~" + wIk->NumberBlock + ".xlsx");
}
//---------------------------------------------------------------------------

//------------------------/ Обработчики графиков /---------------------------
void __fastcall TIK_2::sListBox1Click(TObject *Sender)
{
	sListBox1->Selected[sListBox1->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sListBox2Click(TObject *Sender)
{
	sListBox2->Selected[sListBox2->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sListBox3Click(TObject *Sender)
{
	sListBox3->Selected[sListBox3->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sListBox4Click(TObject *Sender)
{
	sListBox4->Selected[sListBox4->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sListBox5Click(TObject *Sender)
{
	sListBox5->Selected[sListBox5->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sListBox6Click(TObject *Sender)
{
	sListBox6->Selected[sListBox6->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sListBox4MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
	FocusControl(iScope1); // переводим фокус, чтобы колёсиком мышки нельзя было поменять значение
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sListBox4VScroll(TObject *Sender)
{
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[sListBox4->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sListBox3VScroll(TObject *Sender)
{
	iScope2->TimeBase->SecPerDiv = SecPerDivTable[sListBox3->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sListBox5VScroll(TObject *Sender)
{
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox5->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sListBox1VScroll(TObject *Sender)
{
	iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox1->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sListBox6VScroll(TObject *Sender)
{
	if(sListBox6->TopIndex == 0) iScope1->Channel[0]->Coupling = isccDC;
		else iScope1->Channel[0]->Coupling = isccAC;
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sListBox2VScroll(TObject *Sender)
{
	if(sListBox2->TopIndex == 0) iScope2->Channel[0]->Coupling = isccDC;
		else iScope2->Channel[0]->Coupling = isccAC;
}
//---------------------------------------------------------------------------

void __fastcall TIK_2::sButton11Click(TObject *Sender)
{
//	ResetUtil4K3();
	wIk->RunFlag = 1;

	wIk->Util6Step1(BLOCK_BU);
	WorkMode = MODE_UTIL6_STEP1;
	Timer1->Enabled = true;
	Timer1->Interval = 1000;                       // Длительность первого шага

//	EnableButton(false);
	sButton1->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sButton7Cl(TObject *Sender)
{
//	ResetUtil4K3();
	wIk->RunFlag = 1;

	wIk->Util6Step1(BLOCK_BU);
	WorkMode = MODE_UTIL6_STEP3;
	Timer1->Enabled = true;
	Timer1->Interval = 1000;                       // Длительность первого шага

//	EnableButton(false);
	sButton7->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sButton8Click(TObject *Sender)
{
//	ResetUtil4K3();
	wIk->RunFlag = 1;

	wIk->Util7Step1();
	WorkMode = MODE_UTIL7_STEP1;
	Timer1->Enabled = true;
	Timer1->Interval = 1000;                       // Длительность первого шага

//	EnableButton(false);
	sButton8->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TIK_2::sButton9Click(TObject *Sender)
{
//	ResetUtil4K3();
	iScope1->DataBlockClear();
	iScope2->DataBlockClear();

	iScope1->Plot->ClearAllData();
	iScope2->Plot->ClearAllData();

	wIk->RunFlag = 1;

	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

	wIk->Util7Step1();
	WorkMode = MODE_UTIL7_STEP4;
	Timer1->Enabled = true;
	Timer1->Interval = 1000;                       // Длительность первого шага

//	EnableButton(false);
	sButton9->Enabled = false;
}
//---------------------------------------------------------------------------





void __fastcall TIK_2::btnAutoClick(TObject *Sender)
{  sButton2->Enabled=false;
btnAuto->Enabled=false;
sBitBtn1->Enabled=false;  ModeAuto=0;
stepUprImp2=-1;
stepUprImp3=-1;
	ResetUtil5();
		wIk->RunFlag = 1;

		wIk->Util5Step1(wIk->Block);
		WorkMode = MODE_UTIL5_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 2000;
	//	stepUprImp2 = 1;

}
//---------------------------------------------------------------------------
  int time_step = 0;
void __fastcall TIK_2::timeZaderTimer(TObject *Sender)
{
if(stepUprImp2==1)
{if (time_step>30) {
 timeAuto->Interval = 30;
 timeAuto->Enabled = true;
 timeZader->Enabled=false;
}
 time_step++;}
 else
 {
if (time_step>5) {
 timeAuto->Interval = 10;
 timeAuto->Enabled = true;
 timeZader->Enabled=false;
}
 time_step++;}
}
//---------------------------------------------------------------------------



void __fastcall TIK_2::timeAutoTimer(TObject *Sender)
{
 if (stepUprImp2==0)
		{
		ResetUtil6();
		wIk->RunFlag = 1;

		wIk->Util6Step1(wIk->Block);
		WorkMode = MODE_UTIL6_STEP3;
		Timer1->Enabled = true;
		Timer1->Interval = 1500;                       // Длительность первого шага

//		sButton2->Enabled = false;
		stepUprImp2 = -1;
		timeAuto->Enabled=false;
	 //	stepUprImp3 = 1;
		}
	else if (stepUprImp2==1){
		ResetUtil7();
		wIk->RunFlag = 1;

//		HANDLE hnd_scope_thread;
//		DWORD scope_thread_id;
//		DWORD thread_status;
//
//		hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

		wIk->Util7Step1();
		WorkMode = MODE_UTIL7_STEP4;
		Timer1->Enabled = true;
		Timer1->Interval = 1000;                       // Длительность первого шага

//		sButton2->Enabled = false;
		stepUprImp2 = -1;
			timeAuto->Enabled=false;
	 //	stepUprImp3 = 2;
		}

 //для сброса
   if(stepUprImp3==0) {
//		ResetUtil4K3();
		wIk->RunFlag = 1;

		wIk->Util6Step1(wIk->Block);
		WorkMode = MODE_UTIL6_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 1000;                       // Длительность первого шага

//		sButton1->Enabled = false;
		stepUprImp3 = -1;
			timeAuto->Enabled=false;
	 //	stepUprImp2 = 0;
		}
	else if(stepUprImp3==1){
//		ResetUtil4K3();
		wIk->RunFlag = 1;

		wIk->Util7Step1();
		WorkMode = MODE_UTIL7_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 1000;                       // Длительность первого шага
		stepUprImp3 = -1;
//		sButton1->Enabled = false;
	  //	stepUprImp2 = 1;
		timeAuto->Enabled=false;
		}
	else if(stepUprImp3==2){
//		ResetUtil4K3();
		wIk->RunFlag = 1;

		wIk->Util6Step1(wIk->Block);
		WorkMode = MODE_UTIL6_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 1000;                       // Длительность первого шага
		 stepUprImp3 = -1;
//		sButton1->Enabled = false;
	 //	stepUprImp2 = -1;
		timeAuto->Enabled=false;

  }
}
//---------------------------------------------------------------------------

