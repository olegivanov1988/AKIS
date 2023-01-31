//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnMK_1.h"
#include "WMk.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sBitBtn"
#pragma link "sLabel"
#pragma link "sPanel"
#pragma link "sRadioButton"
#pragma link "iScope"
#pragma link "sButton"
#pragma link "sListBox"
#pragma link "sBevel"
#pragma link "sEdit"
#pragma resource "*.dfm"
//TMK_1 *MK_1;

const double TMK_1::SecPerDivTable[19] = {0.1, 0.05, 0.02, 0.01, 0.005, 0.002, 0.001,
0.0005, 0.0002, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06, 5E-07, 2E-07, 1E-07};

const double TMK_1::VoltPerDivTable[22] = {50, 20, 10, 5, 2, 1, 0.5, 0.2, 0.1, 0.05, 0.02,
0.01, 0.005, 0.002, 0.001, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06};
//---------------------------------------------------------------------------
__fastcall TMK_1::TMK_1(TComponent* Owner)
	: TForm(Owner)
{
	wMk = new WMk;
	inclPower = false;

	sListBox4->ItemIndex = 0;       		// выставляем первоначальное значение 0,1 с
	sListBox4->Selected[0] = false;
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[0];
	sListBox5->ItemIndex = 2;       		// выставляем первоначальное значение 10 В
	sListBox5->Selected[2] = false;
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[2];
	sListBox6->ItemIndex = 1;               // выставляем AC
	sListBox6->Selected[1] = false;
	iScope1->Channel[0]->Coupling = isccAC;
//	int a = iScope1->Channel[0]->Coupling;

	sListBox1->ItemIndex = 0;       		// выставляем первоначальное значение 0,1 с
	sListBox1->Selected[0] = false;
	iScope2->TimeBase->SecPerDiv = SecPerDivTable[0];
	sListBox2->ItemIndex = 2;       		// выставляем первоначальное значение 10 В
	sListBox2->Selected[2] = false;
	iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[2];

	sListBox7->ItemIndex = 0;       		// выставляем первоначальное значение 0,1 с
	sListBox7->Selected[0] = false;
	iScope3->TimeBase->SecPerDiv = SecPerDivTable[0];
	sListBox8->ItemIndex = 2;       		// выставляем первоначальное значение 10 В
	sListBox8->Selected[2] = false;
	iScope3->Channel[0]->VoltsPerDivision = VoltPerDivTable[2];

	sListBox10->ItemIndex = 0;       		// выставляем первоначальное значение 0,1 с
	sListBox10->Selected[0] = false;
	iScope4->TimeBase->SecPerDiv = SecPerDivTable[0];
	sListBox11->ItemIndex = 2;       		// выставляем первоначальное значение 10 В
	sListBox11->Selected[2] = false;
	iScope4->Channel[0]->VoltsPerDivision = VoltPerDivTable[2];
	sListBox12->ItemIndex = 1;               // выставляем AC
	sListBox12->Selected[1] = false;
	iScope4->Channel[0]->Coupling = isccAC;
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
	if(wMk->RunFlag) {
		wMk->UtilStop(1);	// выключение модулей, если они запущены
	}
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sBitBtn1Click(TObject *Sender)
{
	if(inclPower) {
		sBitBtn1->Down = true;

		wMk->UtilStop(1);                   // Выключаем питание
//		Util0Stop();

//		// Очистка графиков и значений перед выключением питания
//		ResetK9_X8();
//
//		sButton1->Enabled = false;
//		sButton2->Enabled = false;
//		sPanel5->Font->Color = clBlack;
//		sPanel5->Caption = "Проверка блока";
//		sLabel3->Caption = "Перейти к экрану 2?";
	}else {

		wMk->RunFlag = 1;
		wMk->ReplaceBuf = true;

//		wMk->Util1Step1();

		Timer1->Interval = 1500;            // Время ожидания результата напряжений
		Timer1->Enabled = true;
		inclPower = true;
		WorkMode = MODE_UTIL1_STEP1;
		sBitBtn1->Down = true;
		sBitBtn1->Enabled = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sButton3Click(TObject *Sender)
{
	ResetUtil2_3();
	wMk->RunFlag = 1;

//	wMk->Util2_3Step1(MK_UTIL2);
	WorkMode = MODE_UTIL2_STEP1;
	Timer1->Enabled = true;
	Timer1->Interval = 9000;

//	EnableButton(false);
	sButton3->Enabled = false;

	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

	sListBox5->ItemIndex = 6;       		// выставляем первоначальное значение 500 мВ
	sListBox5->Selected[6] = false;
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

	sListBox11->ItemIndex = 6;       		// выставляем первоначальное значение 500 мВ
	sListBox11->Selected[6] = false;
	iScope4->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

	sPanel24->Caption = "Uвходн. = 910 мВ";
	sPanel37->Caption = "Uвходн. = 910 мВ";
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sButton4Click(TObject *Sender)
{
	ResetUtil2_3();
	wMk->RunFlag = 1;

//	wMk->Util2_3Step1(MK_UTIL3);
	WorkMode = MODE_UTIL3_STEP1;
	Timer1->Enabled = true;
	Timer1->Interval = 9000;

//	EnableButton(false);
	sButton4->Enabled = false;

	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

	sListBox5->ItemIndex = 9;       		// выставляем первоначальное значение 50 мВ
	sListBox5->Selected[9] = false;
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[9];

	sListBox11->ItemIndex = 9;       		// выставляем первоначальное значение 50 мВ
	sListBox11->Selected[9] = false;
	iScope4->Channel[0]->VoltsPerDivision = VoltPerDivTable[9];

	sPanel24->Caption = "Uвходн. = 910 мВ";
	sPanel37->Caption = "Uвходн. = 910 мВ";
}
//---------------------------------------------------------------------------
static char Text1[] = "Uоп+ = %.0f мВ";
static char Text2[] = "Uоп- = %.0f мВ";
static char Text3[] = "U = %.0f мВ";
void __fastcall TMK_1::Timer1Timer(TObject *Sender)
{
	AnsiString form_string1, form_string2;

	try
	{
//***************** Util 1 ***************************
		if(WorkMode == MODE_UTIL1_STEP1) {

			bool power1, power2;
//			AnsiString form_string1, form_string2;
			wMk->Uplus = 0;
			wMk->Uminus = 0;

			for(int i=0; i < ACQ_BLOCK_SIZE;) {
				wMk->Uplus = wMk->Uplus + wMk->AcqBuf[wMk->BlockNumberThread][i++];
				wMk->Uminus = wMk->Uminus + wMk->AcqBuf[wMk->BlockNumberThread][i++];
            }

//			wMk->Uplus = wMk->AcqBuf[wMk->BlockNumberThread][512] * 1000;
			wMk->Uplus = wMk->Uplus / (ACQ_BLOCK_SIZE / 2) * 1000;
			form_string1.printf( Text1, wMk->Uplus );
			sPanel12->Caption = form_string1;

//			wMk->Uminus = wMk->AcqBuf[wMk->BlockNumberThread][513] * 1000;
			wMk->Uminus = wMk->Uminus / (ACQ_BLOCK_SIZE / 2) * 1000;
			form_string2.printf( Text2, wMk->Uminus );
			sPanel2->Caption = form_string2;

			// Положительное напряжение
			if( wMk->Uplus < UPLUS_MIN || wMk->Uplus > UPLUS_MAX) {
				sPanel12->Font->Color = clRed;
				sPanel11->Font->Color = clRed;
				sPanel11->Caption = "ОШИБКА";
//				wAsk->CheckASK1[1] = 2;
				power1 = false;
			} else {
				sPanel12->Font->Color = clLime;
				sPanel11->Font->Color = clLime;
				sPanel11->Caption = "НОРМА";
				power1 = true;
//				wAsk->CheckASK1[1] = 1;
			}

			// Отрицательное напряжение
			if( wMk->Uminus > UMINUS_MIN || wMk->Uminus < UMINUS_MAX) {
				sPanel2->Font->Color = clRed;
				sPanel1->Font->Color = clRed;
				sPanel1->Caption = "ОШИБКА";
//				wAsk->CheckASK1[2] = 2;
				power2 = false;
			} else {
				sPanel2->Font->Color = clLime;
				sPanel1->Font->Color = clLime;
				sPanel1->Caption = "НОРМА";
				power2 = true;
//				wAsk->CheckASK1[2] = 1;
			}

			if(power1 && power2) {
				sButton3->Enabled = true;
				sButton4->Enabled = true;

				sBitBtn1->Font->Color = clLime;
				sBitBtn1->Caption = "ОТКЛЮЧИТЬ ПИТАНИЕ";
				wMk->UtilStop();
			} else Util1Stop();                         // При неудачной проверке

			Timer1->Enabled = false;
			WorkMode = MODE_NULL;
			sBitBtn1->Enabled = true;
			wMk->ReplaceBuf = false;
//			wMk->UtilStop();
        }
//***************** Util 2 ***************************
		if(WorkMode == MODE_UTIL2_STEP1) {
			form_string1.printf( Text3, wMk->Util2MaxK3 * 1000 );
			sPanel30->Caption = form_string1;

			form_string2.printf( Text3, wMk->Util2MaxK4 * 1000 );
			sPanel39->Caption = form_string2;

			if(wMk->Util2MaxK3 >= UTIL2_3_MIN) {
				sPanel30->Font->Color = clLime;
				sPanel21->Font->Color = clLime;
				sPanel21->Caption = "НОРМА";
			} else {
				sPanel30->Font->Color = clRed;
				sPanel21->Font->Color = clRed;
				sPanel21->Caption = "ОШИБКА";
			}

			if(wMk->Util2MaxK4 >= UTIL2_3_MIN) {
				sPanel39->Font->Color = clLime;
				sPanel38->Font->Color = clLime;
				sPanel38->Caption = "НОРМА";
			} else {
				sPanel39->Font->Color = clRed;
				sPanel38->Font->Color = clRed;
                sPanel38->Caption = "ОШИБКА";
            }


			Timer1->Enabled = false;
			WorkMode = MODE_NULL;
			wMk->UtilStop();
			sButton3->Enabled = true;
		}
//***************** Util 3 ***************************
		if(WorkMode == MODE_UTIL3_STEP1) {
			Timer1->Enabled = false;
			WorkMode = MODE_NULL;
			wMk->UtilStop();
			sButton4->Enabled = true;
		}
	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::Util1Stop()
{
	wMk->UtilStop(1);                         // 1 - отключаем питание

	sBitBtn1->Font->Color = clRed;
	sBitBtn1->Caption = "ЗАПУСК ПИТАНИЯ";
	inclPower = false;
	sBitBtn1->Down = false;
}
//---------------------------------------------------------------------------
DWORD WINAPI TMK_1::ScopeThread(TMK_1 *MK_1)
{
	int i;
	int count = 0;
	bool NeedPacket1, NeedPacket2, NeedPacket3, NeedPacket4;
	MK_1->wMk->Util2MaxK3 = 0;
	MK_1->wMk->Util2MaxK4 = 0;

	if(MK_1->wMk->BlockNumberThread == 100) // проверка инициализации переменной
		{
			MK_1->CreateTh();                   // повторный запуск потока через паузу
			ExitThread(0);
			return 0;
		}

	while(MK_1->wMk->RunFlag) {

		if(count == ACQ_BLOCK_QNT) count = 0;

		NeedPacket1 = MK_1->iScope1->NeedPacketsNow;
//		NeedPacket2 = MK_1->iScope2->NeedPacketsNow;
//		NeedPacket3 = MK_1->iScope3->NeedPacketsNow;
		NeedPacket4 = MK_1->iScope4->NeedPacketsNow;

		if(!NeedPacket1 || !NeedPacket4 /* || !NeedPacket2 || !NeedPacket3*/) {
			if(!NeedPacket1) MK_1->iScope1->DataBlockClear();
//			if(!NeedPacket2) MK_1->iScope2->DataBlockClear();
//			if(!NeedPacket3) MK_1->iScope3->DataBlockClear();
			if(!NeedPacket4) MK_1->iScope4->DataBlockClear();
		}// else {

			if(!MK_1->iScope1->DataBlockActive && !MK_1->iScope1->TransferringActve &&
//			   !MK_1->iScope2->DataBlockActive && !MK_1->iScope2->TransferringActve &&
//			   !MK_1->iScope3->DataBlockActive && !MK_1->iScope3->TransferringActve &&
			   !MK_1->iScope4->DataBlockActive && !MK_1->iScope4->TransferringActve)
			{

				if(MK_1->wMk->AcqBlockReady[count]) {

					MK_1->iScope1->DataBlockBegin();
//					MK_1->iScope2->DataBlockBegin();
//					MK_1->iScope3->DataBlockBegin();
					MK_1->iScope4->DataBlockBegin();
					for(i=2; i < ACQ_BLOCK_SIZE; ) {
//						MK_1->iScope2->AddChannelData(0, MK_1->wMk->AcqBuf[count][i++]);
//						MK_1->iScope3->AddChannelData(0, MK_1->wMk->AcqBuf[count][i++]);
						MK_1->iScope1->AddChannelData(0, MK_1->wMk->AcqBuf[count][i++]);
						if(MK_1->wMk->AcqBuf[count][i-1] > MK_1->wMk->Util2MaxK3) MK_1->wMk->Util2MaxK3 = MK_1->wMk->AcqBuf[count][i-1];
						MK_1->iScope4->AddChannelData(0, MK_1->wMk->AcqBuf[count][i++]);
						if(MK_1->wMk->AcqBuf[count][i-1] > MK_1->wMk->Util2MaxK4) MK_1->wMk->Util2MaxK4 = MK_1->wMk->AcqBuf[count][i-1];
						i += 2;
					}
					MK_1->iScope1->DataBlockEnd();
//					MK_1->iScope2->DataBlockEnd();
//					MK_1->iScope3->DataBlockEnd();
					MK_1->iScope4->DataBlockEnd();

					MK_1->wMk->AcqBlockReady[count] = 0;
					count++;
				}
			}
//		}
	}// while(MK_1->wMk->RunFlag)

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::CreateTh()
{
	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	Sleep(500);
	hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::ResetUtil2_3()
{
	iScope1->Plot->ClearAllData();
	iScope4->Plot->ClearAllData();
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sButton7Click(TObject *Sender)
{
//	if(sEdit1->Text != "") {
//		sBitBtn1->Enabled = true;
//		sButton7->Visible = false;
//		sEdit1->Visible = false;
//		sLabel8->Visible = false;
//
//		wLsk->NumberBlock = sEdit1->Text;
//
//		sLabel10->Caption = sEdit1->Text;
//		sLabel10->Left = (1920 - sLabel10->Width) / 2;
//		sLabel10->Visible = true;
//	} else {
//		ShowMessage("Введите номер блока!");
//	}
}
//---------------------------------------------------------------------------

//------------------------/ Обработчики графиков /---------------------------
void __fastcall TMK_1::sListBox1Click(TObject *Sender)
{
	sListBox1->Selected[sListBox1->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox2Click(TObject *Sender)
{
	sListBox2->Selected[sListBox2->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox3Click(TObject *Sender)
{
	sListBox3->Selected[sListBox3->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox4Click(TObject *Sender)
{
	sListBox4->Selected[sListBox4->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox5Click(TObject *Sender)
{
	sListBox5->Selected[sListBox5->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox6Click(TObject *Sender)
{
	sListBox6->Selected[sListBox6->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox7Click(TObject *Sender)
{
	sListBox7->Selected[sListBox7->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox8Click(TObject *Sender)
{
	sListBox8->Selected[sListBox8->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox9Click(TObject *Sender)
{
	sListBox9->Selected[sListBox9->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox10Click(TObject *Sender)
{
	sListBox10->Selected[sListBox10->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox11Click(TObject *Sender)
{
	sListBox11->Selected[sListBox11->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox12Click(TObject *Sender)
{
	sListBox12->Selected[sListBox12->TopIndex] = false; // запрет на выделение строки
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
	FocusControl(iScope2); // переводим фокус, чтобы колёсиком мышки нельзя было поменять значение
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox1VScroll(TObject *Sender)
{
	iScope2->TimeBase->SecPerDiv = SecPerDivTable[sListBox1->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox4VScroll(TObject *Sender)
{
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[sListBox4->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox7VScroll(TObject *Sender)
{
	iScope3->TimeBase->SecPerDiv = SecPerDivTable[sListBox7->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox10VScroll(TObject *Sender)
{
	iScope4->TimeBase->SecPerDiv = SecPerDivTable[sListBox10->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox2VScroll(TObject *Sender)
{
	iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox2->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox5VScroll(TObject *Sender)
{
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox5->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox8VScroll(TObject *Sender)
{
	iScope3->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox8->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox11VScroll(TObject *Sender)
{
	iScope4->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox11->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox3VScroll(TObject *Sender)
{
	if(sListBox3->TopIndex == 0) iScope2->Channel[0]->Coupling = isccDC;
		else iScope2->Channel[0]->Coupling = isccAC;
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox6VScroll(TObject *Sender)
{
	if(sListBox6->TopIndex == 0) iScope1->Channel[0]->Coupling = isccDC;
		else iScope1->Channel[0]->Coupling = isccAC;
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox9VScroll(TObject *Sender)
{
	if(sListBox9->TopIndex == 0) iScope3->Channel[0]->Coupling = isccDC;
		else iScope3->Channel[0]->Coupling = isccAC;
}
//---------------------------------------------------------------------------
void __fastcall TMK_1::sListBox12VScroll(TObject *Sender)
{
	if(sListBox12->TopIndex == 0) iScope4->Channel[0]->Coupling = isccDC;
		else iScope4->Channel[0]->Coupling = isccAC;
}
//---------------------------------------------------------------------------



