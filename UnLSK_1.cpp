//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnLSK_1.h"
#include "WLsk.h"
#include "UnVisibleReport.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sBitBtn"
#pragma link "iScope"
#pragma link "sListBox"
#pragma link "sPanel"
#pragma link "sButton"
#pragma link "sLabel"
#pragma link "sRadioButton"
#pragma link "sEdit"
#pragma resource "*.dfm"
#include "bass.h"
#include <process.h>
#include <stdio.h>
#include <windows.h>
//TLSK_1 *LSK_1;
const double TLSK_1::SecPerDivTable[19] = {0.1, 0.05, 0.02, 0.01, 0.005, 0.002, 0.001,
0.0005, 0.0002, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06, 5E-07, 2E-07, 1E-07};

const double TLSK_1::VoltPerDivTable[22] = {50, 20, 10, 5, 2, 1, 0.5, 0.2, 0.1, 0.05, 0.02,
0.01, 0.005, 0.002, 0.001, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06};
HSAMPLE sample;
	HCHANNEL channel;
//---------------------------------------------------------------------------
__fastcall TLSK_1::TLSK_1(TComponent* Owner, AnsiString number_block)
	: TForm(Owner)
{
	wLsk = new WLsk;
	inclPower = false;
	power = true;
	 wLsk->autoProv = false;

	sListBox2->ItemIndex = 3;       // выставляем первоначальное значение 5 В
	sListBox2->Selected[3] = false;
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[3];
	sListBox1->ItemIndex = 3;       // выставляем первоначальное значение 10 мс
	sListBox1->Selected[3] = false;
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[3];

	wLsk->NumberBlock = number_block;
	if(wLsk->NumberBlock != "-1") {
        btnAuto->Enabled = true;
		sBitBtn1->Enabled = true;
		sButton7->Visible = false;
		sEdit1->Visible = false;
		sLabel8->Visible = false;

		sLabel10->Caption ="№" + wLsk->NumberBlock;
		sLabel10->Left = (1920 - sLabel10->Width) / 2;
		sLabel10->Visible = true;
	} else {
		sButton7->Visible = true;
		sEdit1->Visible = true;
		sLabel8->Visible = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
//	if(wLsk->RunFlag) {
//		wLsk->UtilStop(1);	// выключение модулей, если они запущены
//	}

	wLsk->UtilStop(power);	// выключение модулей, если они запущены
	power = true;
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::sBitBtn1Click(TObject *Sender)
{
//	if(wLsk->IsOpenedLTR34() == 0) return; // на случай быстрого повторного нажатия кнопки
     btnAuto->Enabled=false;
	if(inclPower)
	{
		sBitBtn1->Down = true;

//		wLsk->Util0Step1();
		Util0Stop();               // Там отключаем питание

//		// Очистка графиков и значений перед выключением питания
		ResetK9_X8();

		sButton1->Enabled = false;
		sButton2->Enabled = false;
		sButton13->Enabled = false;
		sButton14->Enabled = false;
		sPanel5->Font->Color = clBlack;
		sPanel5->Caption = "Проверка блока";
		sLabel3->Caption = "Перейти к экрану 2?";
	} else
	{
		try
		{
			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			iScope1->DataBlockClear();
			iScope1->Plot->ClearAllData();

			wLsk->RunFlag = 1;
//			wLsk->ReplaceBuf = true;
			wLsk->Util0Step1();
			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

			Timer1->Interval = 17000;        // Время ожидания результата
			Timer1->Enabled = true;
			inclPower = true;
			WorkMode = MODE_UTIL0;
			sBitBtn1->Down = true;
			sBitBtn1->Enabled = false;

			// Инициализируем массив проверочных переменных
			for(int i=0; i<PARAMETERS_LSK1; i++) {
				wLsk->CheckLSK1[i] = 0;
			}

			wLsk->ReplaceBuf2 = true;
		}
		catch( Exception &e )
		{
			MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
		}
	}
}
//---------------------------------------------------------------------------
//static char Text1[] = "F = %.1f Гц";
static char Text1[] = "T = %.2f мс";
static char Text3[] = "I+ = %.2f мА";
void __fastcall TLSK_1::Timer1Timer(TObject *Sender)
{
	try
	{
		if(WorkMode == MODE_UTIL0)
		{
			AnsiString form_string;
			bool power1, power2;

			// Проверка тока
			wLsk->currentPlus = wLsk->Acq2Buf[wLsk->BlockNumberThread2][32];     // Берём 32 значение для тока
			form_string.printf( Text3, wLsk->currentPlus );
			sPanel15->Caption = form_string;
			if(wLsk->currentPlus >= CURRENT_MIN	&& wLsk->currentPlus <= CURRENT_MIDD) {
				sPanel15->Font->Color = clLime;
				sPanel14->Font->Color = clLime;
				sPanel14->Caption = "НОРМА";
				wLsk->CheckLSK1[1] = 1;
				wLsk->note_current = "Норма";
				power1 = true;
			} else {
				if(wLsk->currentPlus > CURRENT_MIDD && wLsk->currentPlus < CURRENT_MAX) {
					sPanel15->Font->Color = clYellow;
					sPanel14->Font->Color = clYellow;
					sPanel14->Caption = "ОТКЛОНЕНИЕ";
					wLsk->CheckLSK1[1] = 2;
				} else {
					sPanel15->Font->Color = clRed;
					sPanel14->Font->Color = clRed;
					sPanel14->Caption = "ОШИБКА";
					wLsk->CheckLSK1[1] = 2;
				}
				power1 = false;
				wLsk->note_current = "Неисправность";
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			}

			wLsk->RunFlag = 0;	// Принудительное завершение потоков АЦП и scope-потока

			// Считаем количество сэмплов в одном периоде
			int porog = 0;
			double countSamples = 1;
			for(int i = 20; i < ACQ_BLOCK_SIZE; i += 2) {
				if(wLsk->AcqBuf[wLsk->BlockNumberThread][i] < 4 && wLsk->AcqBuf[wLsk->BlockNumberThread][i+2] > 4)
					porog++;

				if(porog == 1) countSamples++;
				if(porog > 1) break;
			}

			// Выводим значение
//			wLsk->frequencyK9 = 6250 / countSamples;
			wLsk->frequencyK9 = (double) countSamples / 25000.0 * 1000; // 6250.0 * 1000
			wLsk->frequencyK9 = SimpleRoundTo(wLsk->frequencyK9, -2);
//			wLsk->frequencyK9 = 1;                             // Проверка на ошибку
			form_string.printf( Text1, wLsk->frequencyK9 );
			sPanel3->Caption = form_string;
//			if(wLsk->frequencyK9 >= FREQUENCY_K9_MIN && wLsk->frequencyK9 <= FREQUENCY_K9_MAX) {
			if(wLsk->frequencyK9 >= TIME_K9_MIN && wLsk->frequencyK9 <= TIME_K9_MAX) {
				sPanel3->Font->Color = clLime;
				sPanel2->Font->Color = clLime;
				sPanel2->Caption = "НОРМА";
				wLsk->note_frequencyK9 = "Норма";
				wLsk->CheckLSK1[0] = 1;
				power2 = true;

//				sBitBtn1->Font->Color = clLime;
//				sBitBtn1->Caption = "ОТКЛЮЧИТЬ ПИТАНИЕ";
//				wLsk->UtilStop();
			} else {
				sPanel3->Font->Color = clYellow;
				sPanel2->Font->Color = clYellow;
				sPanel2->Caption = "Отклонение, требуется подстройка";
				wLsk->CheckLSK1[0] = 2;
				wLsk->note_frequencyK9 = "Неисправность";
				power2 = false;
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
//				Util0Stop();
			}

			if(power1 && power2) {
				sBitBtn1->Font->Color = clLime;
				sBitBtn1->Caption = "ОТКЛЮЧИТЬ ПИТАНИЕ";
				wLsk->UtilStop();
			} else {
            	Util0Stop();
            }

			CheckX8();
			TransitionLSK2();
//			wLsk->ReplaceBuf = false;
			Timer1->Enabled = false;
			WorkMode = MODE_NULL;
			sBitBtn1->Down = false;
			sBitBtn1->Enabled = true;
        }
	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall  TLSK_1::Util0Stop()
{
	wLsk->UtilStop(1);

	sBitBtn1->Font->Color = clRed;
	sBitBtn1->Caption = "ЗАПУСК ПИТАНИЯ";
	WorkMode = MODE_NULL;

	inclPower = false;
	sBitBtn1->Down = false;
}
//---------------------------------------------------------------------------
static char Text2[] = "U+ = %.1f В";
DWORD WINAPI TLSK_1::ScopeThread(TLSK_1 *LSK_1)
{
	int i;
	int count = 0;
	bool NeedPacket;
	AnsiString form_string;

	LSK_1->wLsk->maxValueX8 = 0;

	if(LSK_1->wLsk->BlockNumberThread == 100) // проверка инициализации переменной
		{
			LSK_1->CreateTh();                   // повторный запуск потока через паузу
			ExitThread(0);
			return 0;
		}

	while(LSK_1->wLsk->RunFlag) {

		if(count == 2) count = 0;

		NeedPacket = LSK_1->iScope1->NeedPacketsNow;
		if(!NeedPacket) {
			LSK_1->iScope1->DataBlockClear();
		} else
		{

			if(!LSK_1->iScope1->DataBlockActive && !LSK_1->iScope1->TransferringActve) {

				if(LSK_1->wLsk->AcqBlockReady[count]) {

					LSK_1->iScope1->DataBlockBegin();
					for(i=0; i < ACQ_BLOCK_SIZE; i++) {
						LSK_1->iScope1->AddChannelData(0, LSK_1->wLsk->AcqBuf[count][i++]);
						if(LSK_1->wLsk->maxValueX8 < LSK_1->wLsk->AcqBuf[count][i]) LSK_1->wLsk->maxValueX8 = LSK_1->wLsk->AcqBuf[count][i];
					}
					LSK_1->iScope1->DataBlockEnd();

					LSK_1->wLsk->AcqBlockReady[count] = 0;
					count++;
				}
			}
		}

		form_string.printf( Text2, LSK_1->wLsk->maxValueX8);
		LSK_1->sPanel22->Caption = form_string;
	}// while(LSK_1->wLsk->RunFlag)

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::CreateTh()
{
	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	Sleep(500);
	hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::CheckX8()
{
	wLsk->maxValueX8 = SimpleRoundTo(wLsk->maxValueX8, -1);
	if(wLsk->maxValueX8 >= X8_MIN && wLsk->maxValueX8 <= X8_MAX) {
		sPanel22->Font->Color = clLime;
		sPanel21->Font->Color = clLime;
		sPanel21->Caption = "НОРМА";
		wLsk->CheckLSK1[2] = 1;
	} else {
		sPanel22->Font->Color = clRed;
		sPanel21->Font->Color = clRed;
		sPanel21->Caption = "ОШИБКА";
		wLsk->CheckLSK1[2] = 2;
			BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
    }
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::ResetK9_X8()
{
	iScope1->Plot->ClearAllData();

	sPanel3->Font->Color = clWhite;
//	sPanel3->Caption = "F = ";
	sPanel3->Caption = "T = ";

	sPanel2->Caption = " ";

	sPanel22->Font->Color = clWhite;
	sPanel22->Caption = "U+ =";
	sPanel21->Caption = " ";

	sPanel15->Font->Color = clWhite;
	sPanel15->Caption = "I+ =";
	sPanel14->Font->Color = clWhite;
	sPanel14->Caption = "";
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::TransitionLSK2()
{
	for(int i=0; i<PARAMETERS_LSK1; i++) {
		if(wLsk->CheckLSK1[i] == 0) return;
	}

	sButton1->Enabled = true;
	sButton2->Enabled = true;
		sButton13->Enabled = true;
	sButton14->Enabled = true;
	for(int i=0; i<PARAMETERS_LSK1; i++) {
		if(wLsk->CheckLSK1[i] == 2 ) {
			sPanel5->Font->Color = clRed;
			sPanel5->Caption = "Блок неисправен";
			sLabel3->Caption = "Повторить проверку?";
			WorkModeEND = RET_ERROR;

			wLsk->checkBlock = false;				// Блок неисправен

				BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"2.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			return;
		}
	}

	WorkModeEND = RET_OK;
	sPanel5->Font->Color = clLime;

	wLsk->checkBlock = true;				// Всё исправно

	if (!wLsk->autoProv) {
      	sPanel5->Caption = "Параметры в норме";
	sLabel3->Caption = "Перейти к экрану \"2\"?";
		BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"1.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
	}
	else
		{
						TLSK_2 *LSK_2 = new TLSK_2(0, wLsk);
						if(!LSK_2) return;
						LSK_2->Show();
						power = false;
					  this->Close();
		}
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::sButton2Click(TObject *Sender)
{
//	wLsk->UtilStop(1);		// Стоп с отключением питания

	this->Close();
}
//---------------------------------------------------------------------------

void __fastcall TLSK_1::sButton1Click(TObject *Sender)
{
	switch(WorkModeEND) {
		case RET_OK:{	TLSK_2 *LSK_2 = new TLSK_2(0, wLsk);
						if(!LSK_2) return;
						LSK_2->Show();
						power = false;
						break;}

		case RET_ERROR:{TLSK_1 *LSK_1 = new TLSK_1(0, wLsk->NumberBlock);
						if(!LSK_1) return;
						LSK_1->Show();
						break;}
	}

	this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::sButton7Click(TObject *Sender)
{
	if(sEdit1->Text != "") {
		sBitBtn1->Enabled = true;
		btnAuto->Enabled=true;
		sButton7->Visible = false;
		sEdit1->Visible = false;
		sLabel8->Visible = false;

		wLsk->NumberBlock = sEdit1->Text;

		sLabel10->Caption = "№" + sEdit1->Text;
		sLabel10->Left = (1920 - sLabel10->Width) / 2;
		sLabel10->Visible = true;
	} else {
		ShowMessage("Введите номер блока!");
	}
}
//---------------------------------------------------------------------------

//------------------------/ Обработчики графиков /---------------------------
void __fastcall TLSK_1::sListBox1Click(TObject *Sender)
{
	sListBox1->Selected[sListBox1->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::sListBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
	FocusControl(iScope1); // переводим фокус, чтобы колёсиком мышки нельзя было поменять значение
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::sListBox1VScroll(TObject *Sender)
{
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[sListBox1->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::sListBox2Click(TObject *Sender)
{
	sListBox2->Selected[sListBox2->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::sListBox2VScroll(TObject *Sender)
{
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox2->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::sListBox3Click(TObject *Sender)
{
	sListBox3->Selected[sListBox3->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::sListBox3VScroll(TObject *Sender)
{
	if(sListBox3->TopIndex == 0) iScope1->Channel[0]->Coupling = COPLING_AC;
		else iScope1->Channel[0]->Coupling = COPLING_DC;
}
//---------------------------------------------------------------------------
void __fastcall TLSK_1::sButton9Click(TObject *Sender)
{
	TLSK_2 *LSK_2 = new TLSK_2(0, wLsk);
	if(!LSK_2) return;
	LSK_2->Show();
	power = false;

	this->Close();
}
//---------------------------------------------------------------------------

void __fastcall TLSK_1::btnAutoClick(TObject *Sender)
{
 btnAuto->Enabled=false;
 wLsk->autoProv = true;
	if(inclPower)
	{
		sBitBtn1->Down = true;

//		wLsk->Util0Step1();
		Util0Stop();               // Там отключаем питание

//		// Очистка графиков и значений перед выключением питания
		ResetK9_X8();

		sButton1->Enabled = false;
		sButton2->Enabled = false;
		sButton13->Enabled = false;
		sButton14->Enabled = false;
		sPanel5->Font->Color = clBlack;
		sPanel5->Caption = "Проверка блока";
		sLabel3->Caption = "Перейти к экрану 2?";
	} else
	{
			try
		{
			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			iScope1->DataBlockClear();
			iScope1->Plot->ClearAllData();

			wLsk->RunFlag = 1;
//			wLsk->ReplaceBuf = true;
			wLsk->Util0Step1();
			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

			Timer1->Interval = 17000;        // Время ожидания результата
			Timer1->Enabled = true;
			inclPower = true;
			WorkMode = MODE_UTIL0;
			sBitBtn1->Down = true;
			sBitBtn1->Enabled = false;

			// Инициализируем массив проверочных переменных
			for(int i=0; i<PARAMETERS_LSK1; i++) {
				wLsk->CheckLSK1[i] = 0;
			}

			wLsk->ReplaceBuf2 = true;
		}
		catch( Exception &e )
		{
			MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
		} }
}
//---------------------------------------------------------------------------

void __fastcall TLSK_1::sButton13Click(TObject *Sender)
{
	wLsk->WriteExcel_LSK_1("\\RaportLSK.xlsx", true);
}
//---------------------------------------------------------------------------

void __fastcall TLSK_1::sButton14Click(TObject *Sender)
{
	wLsk->WriteExcel_LSK_1("\\RaportLSK.xlsx", false);

	TVsReport *vsReport = new TVsReport(this, wLsk->NumberBlock);
	if(!vsReport) return;
	vsReport->ShowModal();
	delete vsReport;

	DeleteFile(GetCurrentDir() + "\\~" + wLsk->NumberBlock + ".xlsx");
}
//---------------------------------------------------------------------------

