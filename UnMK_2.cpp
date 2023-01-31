 //---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnMK_2.h"
#include "WMk.h"
#include "UnVisibleReport.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "iComponent"
#pragma link "iCustomComponent"
#pragma link "iLed"
#pragma link "iLedArrow"
#pragma link "iVCLComponent"
#pragma link "iScope"
#pragma link "sBevel"
#pragma link "sButton"
#pragma link "sLabel"
#pragma link "sListBox"
#pragma link "sPanel"
#pragma link "sRadioButton"
#pragma link "sDialogs"
#pragma link "sEdit"
#pragma link "sBitBtn"
#pragma link "iSevenSegmentDisplay"
#pragma link "iSevenSegmentInteger"
#pragma resource "*.dfm"

#include "bass.h"
#include <process.h>
#include <stdio.h>
#include <windows.h>
//TMK_2 *MK_2;
bool provT1 = false;
 int ModeAuto = 0; //0- ������, 1 ���1
bool autoPro = false, autoZap=false;
bool provPit = false;
HSAMPLE sample;
HCHANNEL channel;
const double TMK_2::SecPerDivTable[19] = {0.1, 0.05, 0.02, 0.01, 0.005, 0.002, 0.001,
0.0005, 0.0002, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06, 5E-07, 2E-07, 1E-07};

const double TMK_2::VoltPerDivTable[22] = {50, 20, 10, 5, 2, 1, 0.5, 0.2, 0.1, 0.05, 0.02,
0.01, 0.005, 0.002, 0.001, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06};
//---------------------------------------------------------------------------
__fastcall TMK_2::TMK_2(TComponent* Owner, AnsiString number_block)
	: TForm(Owner)
{
 	wMk = new WMk;

	inclPower = false;
	attempt = 1;            // �������������� ������� � ������� ��3
	inclMK1 = false;
	inclMK2 = false;
	inclMK3 = false;
	inclMK4 = false;
	inclMK5 = false;
	inclMKR = false;

	sListBox4->ItemIndex = 0;       		// ���������� �������������� �������� 0,1 �
	sListBox4->Selected[0] = false;
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[0];
	sListBox5->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
	sListBox5->Selected[6] = false;
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];
	sListBox6->ItemIndex = 1;               // ���������� AC
	sListBox6->Selected[1] = false;
	iScope1->Channel[0]->Coupling = isccAC;

	sListBox3->ItemIndex = 0;       		// ���������� �������������� �������� 0,1 �
	sListBox3->Selected[0] = false;
	iScope2->TimeBase->SecPerDiv = SecPerDivTable[0];
	sListBox1->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
	sListBox1->Selected[6] = false;
	iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];
	sListBox2->ItemIndex = 1;               // ���������� AC
	sListBox2->Selected[1] = false;
	iScope2->Channel[0]->Coupling = isccAC;

	sListBox9->ItemIndex = 0;       		// ���������� �������������� �������� 0,1 �
	sListBox9->Selected[0] = false;
	iScope3->TimeBase->SecPerDiv = SecPerDivTable[0];
	sListBox7->ItemIndex = 3;       		// ���������� �������������� �������� 5 �
	sListBox7->Selected[3] = false;
	iScope3->Channel[0]->VoltsPerDivision = VoltPerDivTable[3];

	wMk->NumberBlock = number_block;
	if(wMk->NumberBlock != "-1") {
		sBitBtn1->Enabled = true;
		sButton7->Visible = false;
		sEdit1->Visible = false;
		sLabel11->Visible = false;
		btnAuto->Enabled=true;
		sLabel13->Caption ="�" + wMk->NumberBlock;
		sLabel13->Left = (1920 - sLabel13->Width) / 2;
		sLabel13->Visible = true;

	} else {
		sButton7->Visible = true;
		sEdit1->Visible = true;
		sLabel11->Visible = true;
	}

}
//---------------------------------------------------------------------------
void __fastcall TMK_2::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
//	if(wMk->RunFlag) {
//		wMk->UtilStop(1);	// ���������� �������, ���� ��� ��������
//	}

	wMk->UtilStop(1);	// ���������� �������, ���� ��� ��������
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sBitBtn1Click(TObject *Sender)
{
	if(inclPower) {
		sBitBtn1->Down = true;

		Util0Stop();                        // ��������� �������

		// ������� �������� � �������� ����� ����������� �������
		ResetAllUtil();
		ResetUtil0();
		ResetUtil1_2();
		ResetUtil3();
		ResetUtil4();
		ResetUtil5();
		ResetUtil6();

		EnableButton(false);                // ��������� ��� ������
		sButton4->Enabled = false;
		sButton5->Enabled = false;
		sPanel11->Font->Color = clBlack;
		sPanel11->Caption = "�������� �����";
	}
		else
		{
			wMk->RunFlag = 1;
			wMk->ReplaceBuf = true;
			  autoPro=false;
			  autoZap=false;
			wMk->Util0Step1();

			Timer1->Interval = 3000;      // ����� �������� ���������� ���������� � ����
			Timer1->Enabled = true;
			inclPower = true;
			WorkMode = MODE_UTIL0_STEP1;
			sBitBtn1->Down = true;
			sBitBtn1->Enabled = false;
			 btnAuto->Enabled=false;
			wMk->ReplaceBuf2 = true;

			// �������������� ������ ����������� ����������
			for(int i=0; i<PARAMETERS_MK; i++) {
				wMk->CheckMK[i] = 0; }
		}
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sButton8Clic(TObject *Sender)
{
	if(inclMK1) {
		Util1Stop();
	} else {
		sListBox5->ItemIndex = 7;       		// ���������� �������������� �������� 200 ��
		sListBox5->Selected[7] = false;
		iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[7];

		sListBox1->ItemIndex = 7;       		// ���������� �������������� �������� 200 ��
		sListBox1->Selected[7] = false;
		iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[7];

		ResetAllUtil();
		ResetUtil1_2();
		ScopeMK1_3_4(0.31);                           // ������ ������
		wMk->RunFlag = 1;

		wMk->Util2_3_4Step1(MK1);
		WorkMode = MODE_UTIL1_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 9000;

		sButton8->Caption = "������ MK1 ������";
		inclMK1 = true;
//		sButton1->Enabled = false;
//		sButton2->Enabled = false;
//		sButton3->Enabled = false;
//		sButton9->Enabled = false;
//		sButton10->Enabled = false;

		HANDLE hnd_scope_thread;
		DWORD scope_thread_id;
		DWORD thread_status;

		scope = 1;
		hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadUtil1_2, this, 0, &scope_thread_id);

		sPanel2->Caption = "U�����. = 310 ��";
		sPanel24->Caption = "U�����. = 310 ��";

		sPanel28->Caption = "F�����. = 0,5 ��";
		sPanel27->Caption = "F�����. = 0,5 ��";
	}
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sButton9Click(TObject *Sender)
{
	if(inclMK2) {
		Util2Stop();
	} else {
		sListBox5->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
		sListBox5->Selected[6] = false;
		iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

		sListBox1->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
		sListBox1->Selected[6] = false;
		iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

		ResetAllUtil();
		ResetUtil1_2();
		ScopeMK2();                           // ������ ������
		wMk->RunFlag = 1;

		wMk->Util2_3_4Step1(MK2);
		WorkMode = MODE_UTIL2_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 9000;

		sButton9->Caption = "������ MK2 ������";
		inclMK2 = true;
		// ������ ���������� ������������
		sButton1->Enabled = false;
		sButton2->Enabled = false;
		sButton3->Enabled = false;
		sButton8->Enabled = false;
		sButton10->Enabled = false;

		HANDLE hnd_scope_thread;
		DWORD scope_thread_id;
		DWORD thread_status;

		scope = 2;
		hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadUtil1_2, this, 0, &scope_thread_id);

		sPanel2->Caption = "U�����. = 980 ��";
		sPanel24->Caption = "U�����. = 980 ��";

		sPanel28->Caption = "F�����. = 5 ��";
		sPanel27->Caption = "F�����. = 5 ��";
	}
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sButton1Click(TObject *Sender)
{
	// ��� ������������� ���� ������� ������� ��������� � ����������� ����������
	if(attempt > 3) {
		if(wMk->CheckMK[5] == 2) ShowMessage("��������� 1 ����������");
		if(wMk->CheckMK[6] == 2) ShowMessage("��������� 2 ����������");
		BASS_Init(-1,44100,0,0,NULL);
		sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
		channel =  BASS_SampleGetChannel(sample,false);
		BASS_ChannelPlay(channel,false);
		return;
	}

	if(inclMK3) {
		Util3Stop();
	} else {
		sListBox5->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
		sListBox5->Selected[6] = false;
		iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

		sListBox1->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
		sListBox1->Selected[6] = false;
		iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

		ResetAllUtil();
		ResetUtil3();
		ScopeMK1_3_4(0.98);                           // ������ ������
		wMk->RunFlag = 1;

		wMk->Util2_3_4Step1(MK3);
		WorkMode = MODE_UTIL3_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 9000;

		sButton1->Caption = "������ MK3 ������";
		inclMK3 = true;
		sButton2->Enabled = false;
		sButton3->Enabled = false;
		sButton8->Enabled = false;
		sButton9->Enabled = false;
		sButton10->Enabled = false;

		HANDLE hnd_scope_thread;
		DWORD scope_thread_id;
		DWORD thread_status;

		hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadUtil3, this, 0, &scope_thread_id);

		sPanel2->Caption = "U�����. = 980 ��";
		sPanel24->Caption = "U�����. = 980 ��";

		sPanel28->Caption = "F�����. = 0,5 ��";
		sPanel27->Caption = "F�����. = 0,5 ��";
	}
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sButton2Click(TObject *Sender)
{
	if(inclMK4) {
		Util4Stop();
	} else {
		sListBox5->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
		sListBox5->Selected[6] = false;
		iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

		sListBox1->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
		sListBox1->Selected[6] = false;
		iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

		ResetAllUtil();
		ResetUtil4();
		ScopeMK1_3_4(0.98);                           // ������ ������
		wMk->RunFlag = 1;

		HANDLE hnd_scope_thread;
		DWORD scope_thread_id;
		DWORD thread_status;

//		scope = 1;
		hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThreadUtil4_5, this, 0, &scope_thread_id);

		wMk->Util5_6Step1(MK4);
		WorkMode = MODE_UTIL4_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 3000;

		sButton2->Caption = "������ MK4 ������";
		inclMK4 = true;
		sButton1->Enabled = false;
		sButton3->Enabled = false;
		sButton8->Enabled = false;
		sButton9->Enabled = false;
		sButton10->Enabled = false;

		sPanel2->Caption = "U�����. = 980 ��";
		sPanel24->Caption = "U�����. = 980 ��";
	}
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sButton10Click(TObject *Sender)
{
	if(inclMK5) {
		Util5Stop();
	} else {
		sListBox5->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
		sListBox5->Selected[6] = false;
		iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

		sListBox1->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
		sListBox1->Selected[6] = false;
		iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

		ResetAllUtil();
		ResetUtil5();
		ScopeMK5();
		wMk->RunFlag = 1;

		HANDLE hnd_scope_thread;
		DWORD scope_thread_id;
		DWORD thread_status;

		hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThreadUtil4_5, this, 0, &scope_thread_id);

		wMk->Util5_6Step1(MK5);
		WorkMode = MODE_UTIL5_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 3500;

		sButton10->Caption = "������ MK5 ������";
		inclMK5 = true;
		sButton1->Enabled = false;
		sButton2->Enabled = false;
		sButton3->Enabled = false;
		sButton8->Enabled = false;
		sButton9->Enabled = false;

		sPanel2->Caption = "U�����. = 910 ��";
		sPanel24->Caption = "U�����. = 910 ��";
		sLabel17->Caption = "U��� = 8 �";
	}
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sButton3Click(TObject *Sender)
{
	if(inclMKR) {
		Util6Stop();
	} else {
		sOpenDialog1->InitialDir = GetCurrentDir();

		if(sOpenDialog1->Execute()) {
			ResetAllUtil();
			ResetUtil6();
			iScope3->SamplesPerSecond = 30000;
			wMk->RunFlag = 1;
			wMk->Util7Step1((AnsiString) sOpenDialog1->FileName);

			HANDLE hnd_scope_thread2;
			DWORD scope_thread_id2;
			DWORD thread_status2;

			hnd_scope_thread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThreadUtil6, this, 0, &scope_thread_id2);

			iScope1->SamplesPerSecond = wMk->LengthButhUtil6 - 1;
			iScope2->SamplesPerSecond = wMk->LengthButhUtil6 - 1;

			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread2, this, 0, &scope_thread_id);

			WorkMode = MODE_UTIL6_STEP1;
			Timer1->Enabled = true;
			Timer1->Interval = 20000;

			sButton3->Caption = "������ MK� ������";
			inclMKR = true;
			sButton1->Enabled = false;
			sButton2->Enabled = false;
			sButton8->Enabled = false;
			sButton9->Enabled = false;
			sButton10->Enabled = false;

			sLabel17->Caption = "U��� = 8 �";
		}
	}
}
//---------------------------------------------------------------------------
static char Text1[] = "T����� = %.1f �c";
static char Text2[] = "T��� = %.0f �c";
static char Text3[] = "U��+ = %.0f ��";
static char Text4[] = "U��- = %.0f ��";
static char Text5[] = "I+ = %.2f ��";
static char Text6[] = "I- = %.2f ��";
void __fastcall TMK_2::Timer1Timer(TObject *Sender)
{
//***************** Util 0 ***************************
	if(WorkMode == MODE_UTIL0_STEP1) {

		bool power1, power2, power3, power4;
		AnsiString form_string1, form_string2;

		wMk->Uplus = 0;
		wMk->Uminus = 0;

		//********* �������� ���� *******************
		wMk->currentPlus = wMk->Acq2Buf[wMk->BlockNumberThread2][32];     	// ���� 32-� �������� ��� ����
		wMk->currentMinus = wMk->Acq2Buf[wMk->BlockNumberThread2][34];      // ���� 33-� �������� ��� ����

		form_string1.printf( Text5, wMk->currentPlus );
		sPanel33->Caption = form_string1;

		form_string2.printf( Text6, wMk->currentMinus );
		sPanel36->Caption = form_string2;
		// ������������� ���
		if(wMk->currentPlus >= CURRENT_MIN	&& wMk->currentPlus <= CURRENT_MIDD) {
			sPanel33->Font->Color = clLime;
			sPanel31->Font->Color = clLime;
			sPanel31->Caption = "�����";
			power3 = true;
			wMk->note_currentPlus = "�����";
		} else {
			if(wMk->currentPlus > CURRENT_MIDD && wMk->currentPlus < CURRENT_MAX) {
				sPanel33->Font->Color = clYellow;
				sPanel31->Font->Color = clYellow;
				sPanel31->Caption = "����������";
			} else {
				sPanel33->Font->Color = clRed;
				sPanel31->Font->Color = clRed;
				sPanel31->Caption = "������";
			}
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
				power3 = false;
				wMk->note_currentMinus = "�������������";
		}
		// ������������� ���
		if(wMk->currentMinus >= CURRENT_MIN	&& wMk->currentMinus <= CURRENT_MIDD) {
			sPanel36->Font->Color = clLime;
			sPanel34->Font->Color = clLime;
			sPanel34->Caption = "�����";
			power4 = true;
		} else {
			if(wMk->currentMinus > CURRENT_MIDD && wMk->currentMinus < CURRENT_MAX) {
				sPanel36->Font->Color = clYellow;
				sPanel34->Font->Color = clYellow;
				sPanel34->Caption = "����������";
			} else {
				sPanel36->Font->Color = clRed;
				sPanel34->Font->Color = clRed;
				sPanel34->Caption = "������";
			}
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
			power4 = false;
		}
		//*************************************
		for(int i=0; i < ACQ_BLOCK_SIZE;) {
			wMk->Uplus = wMk->Uplus + wMk->AcqBuf[wMk->BlockNumberThread][i++];
			wMk->Uminus = wMk->Uminus + wMk->AcqBuf[wMk->BlockNumberThread][i++];
		}

		wMk->Uplus = wMk->Uplus / (ACQ_BLOCK_SIZE / 2) * 1000;
		form_string1.printf( Text3, wMk->Uplus );
		sPanel6->Caption = form_string1;

		wMk->Uminus = wMk->Uminus / (ACQ_BLOCK_SIZE / 2) * 1000;
		form_string2.printf( Text4, wMk->Uminus );
		sPanel30->Caption = form_string2;

		// ������������� ����������
		if( wMk->Uplus < UPLUS_MIN || wMk->Uplus > UPLUS_MAX) {
			sPanel6->Font->Color = clRed;
			sPanel5->Font->Color = clRed;
			sPanel5->Caption = "������";
//			wAsk->CheckASK1[1] = 2;
			power1 = false;
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
		} else {
			sPanel6->Font->Color = clLime;
			sPanel5->Font->Color = clLime;
			sPanel5->Caption = "�����";
			power1 = true;
//			wAsk->CheckASK1[1] = 1;
		}

		// ������������� ����������
		if( wMk->Uminus > UMINUS_MIN || wMk->Uminus < UMINUS_MAX) {
			sPanel30->Font->Color = clRed;
			sPanel19->Font->Color = clRed;
			sPanel19->Caption = "������";
//			wAsk->CheckASK1[2] = 2;
			power2 = false;
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
		} else {
			sPanel30->Font->Color = clLime;
			sPanel19->Font->Color = clLime;
			sPanel19->Caption = "�����";
			power2 = true;
//			wAsk->CheckASK1[2] = 1;
		}

		if(power1 && power2 && power3 && power4) {
//			sButton3->Enabled = true;
//			sButton4->Enabled = true;

			sBitBtn1->Font->Color = clLime;
			sBitBtn1->Caption = "��������� �������";
//			EnableButton(true);
			wMk->UtilStop();
			wMk->CheckMK[0] = 1;

			Timer1->Interval = 1000;
			WorkMode = POWER_PAUSE;
			time_step = 0;
			if (provT1) {
			   wMk->PowerPauseSecond = 25;
			} else   wMk->PowerPauseSecond = 24;	// ���� 14

//			iSevenSegmentInteger1->Visible = true;
		} else {
			Util0Stop();                         // ��� ��������� ��������
			Timer1->Enabled = false;
			WorkMode = MODE_NULL;
				if (autoPro) {    //���� �������������� �������� �������� ������
			  sBitBtn1->Enabled=false;
			  ModeAuto=0;
			 time_step = 0;
			 timeZader->Interval=100;
			 timeZader->Enabled=true;

			 } else      sBitBtn1->Enabled = true;
		  //	btnAuto->Enabled=true;
		}



		sBitBtn1->Down = false;
		wMk->ReplaceBuf = false;

	}
//***** ������� ����� ����� ��������� ������� ********
	if(WorkMode == POWER_PAUSE) {

		iSevenSegmentInteger1->Value = time_step;
		if(time_step > wMk->PowerPauseSecond) {
			WorkMode = MODE_NULL;
			Timer1->Enabled = false;
			provPit = true;
		   if (!autoPro) {
		   // ������������ ������ ��������� ������
			sButton8->Enabled = true;
//			sButton9->Enabled = true;
//			sButton1->Enabled = true;
//			sButton2->Enabled = true;
//			sButton10->Enabled = true;
//			sButton3->Enabled = true;
		   }
		   else
		   {
			  sButton8->Enabled = false;;
			  timeAuto->Interval = 10;
			  timeAuto->Enabled = true; }

//			iSevenSegmentInteger1->Visible = false;
		}
		time_step++;
    }
//***************** Util 1 ***************************
		if(WorkMode == MODE_UTIL1_STEP1) {
			// �������� � ����� �7(�9)
			if(wMk->PorogK7_X9[0] == false) {
				sPanel26->Font->Color = clLime;
				sPanel26->Caption = "��� ������.\n �����";
				wMk->CheckMK[1] = 1;
			} else {
				sPanel26->Font->Color = clRed;
				sPanel26->Caption = "����� 0 - 1\n ������";
				wMk->CheckMK[1] = 2;
					BASS_Init(-1,44100,0,0,NULL);
					sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
					channel =  BASS_SampleGetChannel(sample,false);
					BASS_ChannelPlay(channel,false);
			}
			// �������� � ����� �8(�21)
			if(wMk->PorogK8_X21[0] == false) {
				sPanel12->Font->Color = clLime;
				sPanel12->Caption = "��� ������.\n �����";
				wMk->CheckMK[2] = 1;
			} else {
				sPanel12->Font->Color = clRed;
				sPanel12->Caption = "����� 0 - 1\n ������";
				wMk->CheckMK[2] = 2;
					BASS_Init(-1,44100,0,0,NULL);
					sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
					channel =  BASS_SampleGetChannel(sample,false);
					BASS_ChannelPlay(channel,false);
			}
			Util1Stop();
			if (autoPro) {    //���� �������������� �������� �������� ������
			 ModeAuto=1;
			 time_step = 0;
			 timeZader->Interval=100;
			 timeZader->Enabled=true;

			 }
		}
//***************** Util 2 ***************************
		if(WorkMode == MODE_UTIL2_STEP1) {
			// �������� � ����� �7(�9)
			if(wMk->PorogK7_X9[1] == false) {
				sPanel26->Font->Color = clLime;
				sPanel26->Caption = "��� ������.\n �����";
				wMk->Util2X9 = 0;
				wMk->note_Util2X9 = "�����";
				wMk->CheckMK[3] = 1;
			} else {
				sPanel26->Font->Color = clRed;
				sPanel26->Caption = "����� 0 - 1\n ������";
				wMk->Util2X9 = 8.5;
				wMk->note_Util2X9 = "�������������";
				wMk->CheckMK[3] = 2;
					BASS_Init(-1,44100,0,0,NULL);
					sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
					channel =  BASS_SampleGetChannel(sample,false);
					BASS_ChannelPlay(channel,false);
			}
			// �������� � ����� �8(�21)
			if(wMk->PorogK8_X21[1] == false) {
				sPanel12->Font->Color = clLime;
				sPanel12->Caption = "��� ������.\n �����";
				wMk->Util2X21 = 0;
				wMk->note_Util2X21 = "�����";
				wMk->CheckMK[4] = 1;
			} else {
				sPanel12->Font->Color = clRed;
				sPanel12->Caption = "����� 0 - 1\n ������";
				wMk->Util2X21 = 8.5;
				wMk->note_Util2X21 = "�������������";
				wMk->CheckMK[4] = 2;
					BASS_Init(-1,44100,0,0,NULL);
					sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
					channel =  BASS_SampleGetChannel(sample,false);
					BASS_ChannelPlay(channel,false);
			}

//			Timer1->Enabled = false;
//			WorkMode = MODE_NULL;
//			wMk->UtilStop();
//			sButton9->Enabled = true;
			Util2Stop();
				if (autoPro) {    //���� �������������� �������� �������� ������
			 ModeAuto=2;
			 time_step = 0;
			 timeZader->Interval=100;
			 timeZader->Enabled=true;

			 }
		}
//***************** Util 3 ***************************
		if(WorkMode == MODE_UTIL3_STEP1) {
			bool flag_attempt = false;
			// �������� � ����� �7(�9)
			if(wMk->PerepadUtil3[0] == true && wMk->PerepadUtil3[1] == true) {
				sPanel26->Font->Color = clLime;
				sPanel26->Caption = "����� 0 - 1\n �����";
				sPanel39->Font->Color = clWhite;
				sPanel39->Caption = "";
				wMk->Util3X9 = 8.5;
				wMk->note_Util3X9 = "�����";
				wMk->CheckMK[5] = 1;
			} else {
				sPanel26->Font->Color = clRed;
				sPanel26->Caption = "��� ������.\n ������";
				sPanel39->Font->Color = clYellow;
				sPanel39->Caption = "����������, ��������� ����������";
				wMk->Util3X9 = 0;
				wMk->note_Util3X9 = "�������������";
				wMk->CheckMK[5] = 2;
                 	BASS_Init(-1,44100,0,0,NULL);
					sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
					channel =  BASS_SampleGetChannel(sample,false);
					BASS_ChannelPlay(channel,false);
				if(!flag_attempt) {
					attempt++;
					flag_attempt = true;
				}
			}
			// �������� � ����� �8(�21)
			if(wMk->PerepadUtil3[2] == true && wMk->PerepadUtil3[3] == true) {
				sPanel12->Font->Color = clLime;
				sPanel12->Caption = "����� 0 - 1\n �����";
				sPanel40->Font->Color = clWhite;
				sPanel40->Caption = "";
				wMk->Util3X21 = 8.5;
				wMk->note_Util3X21 = "�����";
				wMk->CheckMK[6] = 1;
			} else {
				sPanel12->Font->Color = clRed;
				sPanel12->Caption = "��� ������.\n ������";
				sPanel40->Font->Color = clYellow;
				sPanel40->Caption = "����������, ��������� ����������";
				wMk->Util3X21 = 0;
				wMk->note_Util3X21 = "�������������";
				wMk->CheckMK[6] = 2;
					BASS_Init(-1,44100,0,0,NULL);
					sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
					channel =  BASS_SampleGetChannel(sample,false);
					BASS_ChannelPlay(channel,false);

				if(!flag_attempt) {
					attempt++;
					flag_attempt = true;
				}
			}

//			Timer1->Enabled = false;
//			WorkMode = MODE_NULL;
//			wMk->UtilStop();
//			sButton1->Enabled = true;
			Util3Stop();

			if( flag_attempt == true && !autoPro) {
//				sBitBtn1->Click();
				Util0Stop();                   // ��������� �������
				EnableButton(false);                // ��������� ��� ������
                 	sButton6->Enabled = true;
					sButton12->Enabled = true;

					sButton4->Enabled = true;
					sButton5->Enabled = true;
				sButton1->Enabled = true;
			}
				if (autoPro) {    //���� �������������� �������� �������� ������
			 ModeAuto=3;
			 time_step = 0;
			 timeZader->Interval=100;
			 timeZader->Enabled=true;

			 }
		}
//***************** Util 4 ***************************
	if(WorkMode == MODE_UTIL4_STEP1) {
		if(wMk->PerepadUtil4[0] == true && wMk->PerepadUtil4[1] == true) {
			sPanel26->Font->Color = clLime;
			sPanel26->Caption = "����� 0 - 1 �����";
			wMk->CheckMK[7] = 1;
		} else {
			sPanel26->Font->Color = clRed;
			sPanel26->Caption = "��� ������. ������";
			wMk->CheckMK[7] = 2;
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
		}

		if(wMk->PerepadUtil4[2] == true && wMk->PerepadUtil4[3] == true) {
			sPanel12->Font->Color = clLime;
			sPanel12->Caption = "����� 0 - 1 �����";
			wMk->CheckMK[8] = 1;
		} else {
			sPanel12->Font->Color = clRed;
			sPanel12->Caption = "��� ������. ������";
			wMk->CheckMK[8] = 2;
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
		}

		if(wMk->PerepadUtil4[4] == true) {
			sPanel22->Font->Color = clLime;
			sPanel22->Caption = "����� 0 - 1\n �����";
			wMk->CheckMK[9] = 1;
		} else {
			sPanel22->Font->Color = clRed;
			sPanel22->Caption = "��� ������.\n ������";
			wMk->CheckMK[9] = 2;
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
		}
		// �������� � ����� �10(�13)
		if(wMk->PerepadUtil4[6] == false) {
			sPanel15->Font->Color = clLime;
			sPanel15->Caption = "�����";
			wMk->CheckMK[10] = 1;
		} else {
			sPanel15->Font->Color = clRed;
			sPanel15->Caption = "������";
			wMk->CheckMK[10] = 2;
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
		}

//		Timer1->Enabled = false;
//		WorkMode = MODE_NULL;
//		wMk->UtilStop();
//		sButton2->Enabled = true;
		Util4Stop();
			if (autoPro) {    //���� �������������� �������� �������� ������
			 ModeAuto=4;
			 time_step = 0;
			 timeZader->Interval=100;
			 timeZader->Enabled=true;

			 }
	}
//***************** Util 5 ***************************
	if(WorkMode == MODE_UTIL5_STEP1) {
		if(wMk->PerepadUtil4[0] == true && wMk->PerepadUtil4[1] == true) {
			sPanel26->Font->Color = clLime;
			sPanel26->Caption = "����� 0 - 1 �����";
			wMk->CheckMK[11] = 1;
		} else {
			sPanel26->Font->Color = clRed;
			sPanel26->Caption = "��� ������. ������";
			wMk->CheckMK[11] = 2;
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
		}

		if(wMk->PerepadUtil4[2] == true && wMk->PerepadUtil4[3] == true) {
			sPanel12->Font->Color = clLime;
			sPanel12->Caption = "����� 0 - 1 �����";
			wMk->CheckMK[12] = 1;
		} else {
			sPanel12->Font->Color = clRed;
			sPanel12->Caption = "��� ������. ������";
			wMk->CheckMK[12] = 2;
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
		}

//		t_start = t_end - t_start;
//		sPanel23->Caption = "T����� = " + t_start.FormatString("ss:zzz");
		AnsiString form_string;
		float timeK9;
		timeK9 = (float) counterK9 / (float) 10000 * 1000;
		form_string.printf( Text1, timeK9 );
		sPanel23->Caption = form_string;
		if(timeK9 >= TIME_X27_MIN && timeK9 <= TIME_X27_MAX) {
			sPanel23->Font->Color = clLime;

			sPanel22->Font->Color = clLime;
			sPanel22->Caption = "����� 0 - 1\n �����";
			wMk->CheckMK[13] = 1;
		} else {
			sPanel23->Font->Color = clRed;

			sPanel22->Font->Color = clRed;
			sPanel22->Caption = "��� ������.\n ������";
			wMk->CheckMK[13] = 2;
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
        }

		double time2;
		wMk->Util5TimeX13 = time2 = (double) counterK10 / (double) iScope3->SamplesPerSecond * 1000;
		form_string.printf( Text2, time2 );
		sPanel16->Caption = form_string;
		if(time2 >= TIME_X13_MIN && time2 <= TIME_X13_MAX) {
			sPanel16->Font->Color = clLime;
			sPanel15->Font->Color = clLime;
			sPanel15->Caption = "�����";
			wMk->CheckMK[14] = 1;
			wMk->note_Util5X13 = "�����";
		} else {
			sPanel16->Font->Color = clRed;
			sPanel15->Font->Color = clRed;
			sPanel15->Caption = "������";
			wMk->CheckMK[14] = 2;
			wMk->note_Util5X13 = "�������������";
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
		}
		// �������� � ����� �11(�10)
		if(wMk->PerepadUtil4[5] == true) {
			sPanel20->Font->Color = clLime;
			sPanel20->Caption = "����� 0 - 1\n �����";
			wMk->CheckMK[15] = 1;
		} else {
			sPanel20->Font->Color = clRed;
			sPanel20->Caption = "��� ������.\n ������";
			wMk->CheckMK[15] = 2;
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
        }

//		Timer1->Enabled = false;
//		WorkMode = MODE_NULL;
//		wMk->UtilStop();
//		sButton10->Enabled = true;
		Util5Stop();
			if (autoPro) {    //���� �������������� �������� �������� ������
			 ModeAuto=5;
			 time_step = 0;
			 timeZader->Interval=100;
			 timeZader->Enabled=true;

			 }
	}
//***************** Util 6 ***************************
	if(WorkMode == MODE_UTIL6_STEP1) {
		if(wMk->PerepadUtil6[1]) {
			sPanel20->Font->Color = clLime;
			sPanel20->Caption = "����� 0 - 1 �����";
			wMk->CheckMK[16] = 1;
		} else {
			sPanel20->Font->Color = clRed;
			sPanel20->Caption = "��� ������. ������";
			wMk->CheckMK[16] = 2;
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
		}

		if(wMk->PerepadUtil6[0]) {
			sPanel15->Font->Color = clLime;
			sPanel15->Caption = "�����";
			wMk->CheckMK[17] = 1;
		} else {
			sPanel15->Font->Color = clRed;
			sPanel15->Caption = "������";
			wMk->CheckMK[17] = 2;
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
		}

//		WorkMode = MODE_NULL;
//		Timer1->Enabled = false;
//		wMk->UtilStop();
//		sButton3->Enabled = true;
		Util6Stop();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::Util0Stop()
{
	wMk->UtilStop(1);                         // 1 - ��������� �������

	sBitBtn1->Font->Color = clRed;
	sBitBtn1->Caption = "������ �������";
	inclPower = false;
	sBitBtn1->Down = false;

	wMk->CheckMK[0] = 2;
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::Util1Stop()
{
	wMk->UtilStop();

	inclMK1 = false;
	Timer1->Enabled = false;
	sButton8->Caption = "������ MK1 ������";
	WorkMode = MODE_NULL;
	timeAuto->Enabled=false;
//	sButton1->Enabled = true;
//	sButton2->Enabled = true;
//	sButton3->Enabled = true;
//	sButton9->Enabled = true;
//	sButton10->Enabled = true;

	sButton8->Enabled = false;
	if (autoPro) {
	   sButton9->Enabled = false;
	}   else
	sButton9->Enabled = true;


	CheckEndMK();
	Sleep(2000);
	//sButton8->Visible=false;
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::Util2Stop()
{
	wMk->UtilStop();

	inclMK2 = false;
	Timer1->Enabled = false;
	sButton9->Caption = "������ MK2 ������";
	WorkMode = MODE_NULL;

//	sButton1->Enabled = true;
//	sButton2->Enabled = true;
//	sButton3->Enabled = true;
//	sButton8->Enabled = true;
//	sButton10->Enabled = true;

	sButton9->Enabled = false;
	if (autoPro) {
		sButton1->Enabled = false;
	}  else {
		sButton1->Enabled = true;
//		sButton2->Enabled = true;
//		sButton10->Enabled = true;
//		sButton3->Enabled = true;
	}

	CheckEndMK();
	//sButton9->Visible=false;

}
//---------------------------------------------------------------------------
void __fastcall TMK_2::Util3Stop()
{
	wMk->UtilStop();

	inclMK3 = false;
	Timer1->Enabled = false;
	sButton1->Caption = "������ MK3 ������";
	WorkMode = MODE_NULL;

//	sButton2->Enabled = true;
//	sButton3->Enabled = true;
//	sButton8->Enabled = true;
//	sButton9->Enabled = true;
//	sButton10->Enabled = true;

	sButton1->Enabled = false;
		if (autoPro) { sButton2->Enabled = false;}      else
	sButton2->Enabled = true;

	CheckEndMK();
	//sButton1->Visible=false;

}
//---------------------------------------------------------------------------
void __fastcall TMK_2::Util4Stop()
{
	wMk->UtilStop();

	inclMK4 = false;
	Timer1->Enabled = false;
	sButton2->Caption = "������ MK4 ������";
	WorkMode = MODE_NULL;

//	sButton1->Enabled = true;
//	sButton3->Enabled = true;
//	sButton8->Enabled = true;
//	sButton9->Enabled = true;
//	sButton10->Enabled = true;

	sButton2->Enabled = false;
	if (autoPro) { sButton10->Enabled = false;} else
	sButton10->Enabled = true;

	CheckEndMK();
	//sButton2->Visible=false;

}
//---------------------------------------------------------------------------
void __fastcall TMK_2::Util5Stop()
{
	wMk->UtilStop();

	inclMK5 = false;
	Timer1->Enabled = false;
	sButton10->Caption = "������ MK5 ������";
	WorkMode = MODE_NULL;

//	sButton1->Enabled = true;
//	sButton2->Enabled = true;
//	sButton3->Enabled = true;
//	sButton8->Enabled = true;
//	sButton9->Enabled = true;

	sButton10->Enabled = false;
	if (autoPro) { sButton3->Enabled = false;} else
	sButton3->Enabled = true;

	CheckEndMK();
	//sButton10->Visible=false;

}
//---------------------------------------------------------------------------
void __fastcall TMK_2::Util6Stop()
{
	wMk->UtilStop();

	inclMKR = false;
	Timer1->Enabled = false;
	sButton3->Caption = "������ MK� ������";
	WorkMode = MODE_NULL;
	 // if (ModeAuto==0) {
		 sButton8->Visible=true;
			 sButton9->Visible=true;
			 sButton1->Visible=true;
			 sButton2->Visible=true;
			 sButton10->Visible=true;
			 sButton3->Visible=true;
	//  }
   //	sButton3->Enabled = false;
//	sButton2->Enabled = true;
//	sButton8->Enabled = true;
//	sButton9->Enabled = true;
//	sButton10->Enabled = true;

	CheckEndMK();//sButton3->Visible=false;
}
//---------------------------------------------------------------------------
DWORD WINAPI TMK_2::ScopeThreadUtil4_5(TMK_2 *MK_2)
{
	int i;
	int count = 0;
	bool NeedPacket3;
	int flag0_1_K7 = 0;
	int flag0_1_K8 = 0;
	MK_2->counterK9 = 0;
	MK_2->counterK10 = 0;
	bool stop_counterK9 = true;
	int porog = 0;
	int porog_2 = 0;

	for(int i = 0; i < 7; i++) {
		MK_2->wMk->PerepadUtil4[i] = false;
	}

	MK_2->wMk->Util5MaxX13 = 0;

	if(MK_2->wMk->BlockNumberThread == 100) // �������� ������������� ����������
		{
			MK_2->CreateTh();                   // ��������� ������ ������ ����� �����
			ExitThread(0);
			return 0;
		}

	while(MK_2->wMk->RunFlag) {
		if(count == ACQ_BLOCK_QNT) count = 0;

		NeedPacket3 = MK_2->iScope3->NeedPacketsNow;

		if(!NeedPacket3) MK_2->iScope3->DataBlockClear(); // ���� ��������� �������� ����� ������ ���

		if(!MK_2->iScope3->DataBlockActive && !MK_2->iScope3->TransferringActve)
		{
			if(MK_2->wMk->AcqBlockReady[count]) {

				MK_2->iScope3->DataBlockBegin();

				for(i=0; i < ACQ_BLOCK_SIZE; ) {
//					MK_2->iScope1->AddChannelData(0, MK_2->wMk->AcqBuf[count][i++]);
					if(MK_2->wMk->AcqBuf[count][i++] > 4 ) {
                    	MK_2->wMk->PerepadUtil4[5] = true;                              // ������� 0 - 1 � K11(X10)
                    }
//					MK_2->iScope2->AddChannelData(0, MK_2->wMk->AcqBuf[count][i++]);
					i++;
					switch(flag0_1_K7) {
						case 0:{if( MK_2->wMk->AcqBuf[count][i++] > 4 ) {
										MK_2->wMk->PerepadUtil4[0] = true;              // ������� 0 - 1 � �7
										flag0_1_K7 = 1;
										stop_counterK9 = false;
									}
						break;}
						case 1:{if( MK_2->wMk->AcqBuf[count][i++] < 4 ) {
										MK_2->wMk->PerepadUtil4[1] = true;              // ������� 1 - 0 � �7
										flag0_1_K7 = 2;
								   }
						break;}
						default:{i++;}
					}
					switch(flag0_1_K8) {
						case 0:{if( MK_2->wMk->AcqBuf[count][i++] > 4 ) {
										MK_2->wMk->PerepadUtil4[2] = true;              // ������� 0 - 1 � �8
										flag0_1_K8 = 1;
									}
						break;}
						case 1:{if( MK_2->wMk->AcqBuf[count][i++] < 4 ) {
										MK_2->wMk->PerepadUtil4[3] = true;              // ������� 1 - 0 � �8
										flag0_1_K7 = 2;
								   }
						break;}
						default:{i++;}
					}

					if(!stop_counterK9)
						MK_2->counterK9++;

//					MK_2->iScope3->AddChannelData(0, MK_2->wMk->AcqBuf[count][i]);   	// ������ �9
					if( MK_2->wMk->AcqBuf[count][i++] > 4 ) {
						stop_counterK9 = true;
						MK_2->wMk->PerepadUtil4[4] = true;                              // ������� 0 - 1 � �9
					}

					if(MK_2->wMk->AcqBuf[count][i] > 4)
						porog++;
					if( MK_2->wMk->AcqBuf[count][i] < 4 && porog > 0 )
						porog_2++;
//					i++;
					MK_2->iScope3->AddChannelData(0, MK_2->wMk->AcqBuf[count][i++]);   // ������ �10
					if(MK_2->wMk->AcqBuf[count][i-1] >= 1.2 && porog_2 > 0) {
						MK_2->wMk->PerepadUtil4[6] = true;                             // ������� � �10(�13)
						MK_2->counterK10++;

						// ��������� ������������ ��������� � �13
						if(MK_2->wMk->AcqBuf[count][i-1] > MK_2->wMk->Util5MaxX13) MK_2->wMk->Util5MaxX13 = MK_2->wMk->AcqBuf[count][i-1];
					}

				}

				MK_2->iScope3->DataBlockEnd();

				MK_2->wMk->AcqBlockReady[count] = 0;
				count++;
			}
		}
//		}
	}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::CreateTh()
{
	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	Sleep(500);
	hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThreadUtil4_5, this, 0, &scope_thread_id);
}
//---------------------------------------------------------------------------
DWORD WINAPI TMK_2::ScopeThread2(TMK_2 *MK_2)
{
	int i, j;

	MK_2->iScope1->DataBlockBegin();
	MK_2->iScope2->DataBlockBegin();

	MK_2->iScope1->AddChannelData(0, -8);	// ����������� ��������
	MK_2->iScope2->AddChannelData(0, -8);


	for( i = 0; i < MK_2->wMk->LengthButhUtil6 * 4; ) {
		MK_2->iScope1->AddChannelData(0, MK_2->wMk->VoltArray[i++]);
		MK_2->iScope2->AddChannelData(0, MK_2->wMk->VoltArray[i++]);
		i += 2;
	}

	MK_2->iScope1->DataBlockEnd();
	MK_2->iScope2->DataBlockEnd();

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
DWORD WINAPI TMK_2::ScopeThreadUtil6(TMK_2 *MK_2)
{
	int count = 0;
	bool NeedPacket3;

	for(int i=0; i<2; i++) {
		MK_2->wMk->PerepadUtil6[i] = false;
	}

	while(MK_2->wMk->RunFlag)
		{
			if(count == ACQ_BLOCK_QNT) count = 0;

			NeedPacket3 = MK_2->iScope3->NeedPacketsNow;
			if(!NeedPacket3) MK_2->iScope3->DataBlockClear(); // ���� ��������� �������� ����� ������ ���

			if(!MK_2->iScope3->DataBlockActive && !MK_2->iScope3->TransferringActve) {

				if(MK_2->wMk->AcqBlockReady[count]) {

					MK_2->iScope3->DataBlockBegin();
					for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
						MK_2->iScope3->AddChannelData(0, MK_2->wMk->AcqBuf[count][i++]);	// ������ �10(�13)
						if( MK_2->wMk->AcqBuf[count][i-1] > 4 )
							MK_2->wMk->PerepadUtil6[0] = true;
						if( MK_2->wMk->AcqBuf[count][i++] > 4 )
							MK_2->wMk->PerepadUtil6[1] = true;
					}
					MK_2->iScope3->DataBlockEnd();

					MK_2->wMk->AcqBlockReady[count] = 0;
					count++;
				}
			}
		}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
DWORD WINAPI TMK_2::ThreadUtil1_2(TMK_2 *MK_2)
{
	int count = 0;

	// �������������� ����������� ���������� ���
	MK_2->wMk->PorogK7_X9[MK_2->scope-1] = false;
	MK_2->wMk->PorogK8_X21[MK_2->scope-1] = false;

	while(MK_2->wMk->RunFlag) {
		if(count == ACQ_BLOCK_QNT) count = 0;

		if(MK_2->wMk->AcqBlockReady[count]) {

			for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
				if( MK_2->wMk->AcqBuf[count][i++] > 4 )
					MK_2->wMk->PorogK7_X9[MK_2->scope-1] = true;
				if( MK_2->wMk->AcqBuf[count][i++] > 4 )
					MK_2->wMk->PorogK8_X21[MK_2->scope-1] = true;
				}
			MK_2->wMk->AcqBlockReady[count] = 0;
			count++;
		}
	}
	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
DWORD WINAPI TMK_2::ThreadUtil3(TMK_2 *MK_2)
{
	int i;
	int count = 0;
	int flag0_1_K7 = 0;
	int flag0_1_K8 = 0;

//	int countBlock_ = 0;
//	int numberBlock_ = 0;
//	double minX9 = 100.0;

	for(i = 0; i<4; i++) {
		MK_2->wMk->PerepadUtil3[i] = false;
    }

	while(MK_2->wMk->RunFlag) {
		if(count == ACQ_BLOCK_QNT) count = 0;

		if(MK_2->wMk->AcqBlockReady[count]) {

			for(i = 0; i < ACQ_BLOCK_SIZE; ) {
				switch(flag0_1_K7) {
						case 0:{if( MK_2->wMk->AcqBuf[count][i++] > 4.0 ) {
										MK_2->wMk->PerepadUtil3[0] = true;              // ������� 0 - 1 � �7(X9)
										flag0_1_K7 = 1;
									}
						break;}
						case 1:{if( MK_2->wMk->AcqBuf[count][i++] < 4.0 ) {
										MK_2->wMk->PerepadUtil3[1] = true;              // ������� 1 - 0 � �7(X9)
										flag0_1_K7 = 2;
//										numberBlock_ = countBlock_;
//										if(MK_2->wMk->AcqBuf[count][i-1] < minX9) {
//											minX9 = MK_2->wMk->AcqBuf[count][i-1];
//										}
								   }
						break;}
						default:{i++;}
					}
					switch(flag0_1_K8) {
						case 0:{if( MK_2->wMk->AcqBuf[count][i++] > 4.0 ) {
										MK_2->wMk->PerepadUtil3[2] = true;              // ������� 0 - 1 � �8(X21)
										flag0_1_K8 = 1;
									}
						break;}
						case 1:{if( MK_2->wMk->AcqBuf[count][i++] < 4.0 ) {
										MK_2->wMk->PerepadUtil3[3] = true;              // ������� 1 - 0 � �8(X21)
										flag0_1_K8 = 2;
								   }
						break;}
						default:{i++;}
					}
			}
			MK_2->wMk->AcqBlockReady[count] = 0;
//			++countBlock_;
			count++;

		}
	}
	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::ResetUtil0()
{
	sPanel6->Font->Color = clWhite;
	sPanel6->Caption = "U��+ =";
	sPanel5->Font->Color = clWhite;
	sPanel5->Caption = "";

	sPanel30->Font->Color = clWhite;
	sPanel30->Caption = "U��- =";
	sPanel19->Font->Color = clWhite;
	sPanel19->Caption = "";

	sPanel33->Font->Color = clWhite;
	sPanel33->Caption = "I+ =";
	sPanel31->Font->Color = clWhite;
	sPanel31->Caption = "";

	sPanel36->Font->Color = clWhite;
	sPanel36->Caption = "I- =";
	sPanel34->Font->Color = clWhite;
	sPanel34->Caption = "";
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::ResetUtil1_2()
{
	sPanel26->Font->Color = clWhite;
	sPanel26->Caption = "����� 0 - 1\n";

	sPanel12->Font->Color = clWhite;
	sPanel12->Caption = "����� 0 - 1\n";

	sPanel24->Caption = "U�����. = ";
	sPanel28->Caption = "F�����. = ";
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::ResetUtil3()
{
	sPanel26->Font->Color = clWhite;
	sPanel26->Caption = "����� 0 - 1\n";

	sPanel12->Font->Color = clWhite;
	sPanel12->Caption = "����� 0 - 1\n";

	sPanel39->Font->Color = clWhite;
	sPanel39->Caption = "";

	sPanel40->Font->Color = clWhite;
	sPanel40->Caption = "";

	sPanel24->Caption = "U�����. = ";
	sPanel28->Caption = "F�����. = ";
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::ResetUtil4()
{
	sPanel26->Font->Color = clWhite;
	sPanel26->Caption = "����� 0 - 1\n";

	sPanel12->Font->Color = clWhite;
	sPanel12->Caption = "����� 0 - 1\n";

	sPanel22->Font->Color = clWhite;
	sPanel22->Caption = "����� 0 - 1\n";

	sPanel15->Font->Color = clWhite;
	sPanel15->Caption = "";

	sPanel23->Font->Color = clWhite;
	sPanel23->Caption = "T����� = ";
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::ResetUtil5()
{
	sPanel26->Font->Color = clWhite;
	sPanel26->Caption = "����� 0 - 1\n";

	sPanel12->Font->Color = clWhite;
	sPanel12->Caption = "����� 0 - 1\n";

	sPanel16->Font->Color = clWhite;
	sPanel15->Font->Color = clWhite;
	sPanel16->Caption = "T��� = ";
	sPanel15->Caption = "";

	sPanel22->Font->Color = clWhite;
	sPanel22->Caption = "����� 0 - 1\n";

	sPanel20->Font->Color = clWhite;
	sPanel20->Caption = "����� 0 - 1\n";

	sPanel22->Font->Color = clWhite;
	sPanel22->Caption = "����� 0 - 1\n";

	sPanel23->Font->Color = clWhite;
	sPanel23->Caption = "T����� = ";
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::ResetUtil6()
{
	sPanel20->Font->Color = clWhite;
	sPanel20->Caption = "����� 0 - 1\n";

	sPanel15->Font->Color = clWhite;
	sPanel15->Caption = "";

	sPanel16->Font->Color = clWhite;
	sPanel16->Caption = "T��� = ";
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::ResetAllUtil()
{
	iScope1->DataBlockClear();
	iScope2->DataBlockClear();
	iScope3->DataBlockClear();

	iScope1->Plot->ClearAllData();
	iScope2->Plot->ClearAllData();
	iScope3->Plot->ClearAllData();

	sLabel17->Caption = "U��� = ";
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sButton7Click(TObject *Sender)
{
	if(sEdit1->Text != "") {
		sBitBtn1->Enabled = true;
		btnAuto->Enabled=true;
		sButton7->Visible = false;
		sEdit1->Visible = false;
		sLabel11->Visible = false;

		wMk->NumberBlock = sEdit1->Text;

		sLabel13->Caption = "�" + sEdit1->Text;
		sLabel13->Left = (1920 - sLabel13->Width) / 2;
		sLabel13->Visible = true;
	} else {
		ShowMessage("������� ����� �����!");
	}
}
//---------------------------------------------------------------------------
#include "math.h"
void __fastcall TMK_2::ScopeMK1_3_4(double amp)
{
	int i;
	double t = 0;

	iScope1->SamplesPerSecond = 7812 - 1;
	iScope2->SamplesPerSecond = 7812 - 1;

	iScope1->DataBlockBegin();
	iScope2->DataBlockBegin();

	iScope1->AddChannelData(0, -8);	// ����������� ��������
	iScope2->AddChannelData(0, -8);


	for( i = 0; i < 7812; i++ ) {
		iScope1->AddChannelData(0, amp * sin(6.28318530 * t));
		iScope2->AddChannelData(0, amp * sin(6.28318530 * t));
		t += 0.000128;
	}

	iScope1->DataBlockEnd();
	iScope2->DataBlockEnd();
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::ScopeMK2()
{
	int i;
	double t = 0;

	iScope1->SamplesPerSecond = 8000 - 1;
	iScope2->SamplesPerSecond = 8000 - 1;

	iScope1->DataBlockBegin();
	iScope2->DataBlockBegin();

	iScope1->AddChannelData(0, -8);	// ����������� ��������
	iScope2->AddChannelData(0, -8);


	for( i = 0; i < 8000; i++ ) {
		iScope1->AddChannelData(0, 0.98*sin(31.4159265358*t));
		iScope2->AddChannelData(0, 0.98*sin(31.4159265358*t));
		t += 0.0001;
	}

	iScope1->DataBlockEnd();
	iScope2->DataBlockEnd();
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::ScopeMK5()
{
	int i;
	double t1 = 0;
	double t2 = 0;

	iScope1->SamplesPerSecond = 7812 - 1;
	iScope2->SamplesPerSecond = 7812 - 1;

	iScope1->DataBlockBegin();
	iScope2->DataBlockBegin();

	iScope1->AddChannelData(0, -8);	// ����������� ��������
	iScope2->AddChannelData(0, -8);


	for( i = 0; i < 7812; i++ ) {
		iScope1->AddChannelData(0, 0.91*sin(6.28318530*t1));
		t1 += 0.000128;
		if( i < 1173 ) iScope2->AddChannelData(0, 0);
			else {
				iScope2->AddChannelData(0, 0.91*sin(6.28318530*t2));
				t2 += 0.000128;
			}
	}

	iScope1->DataBlockEnd();
	iScope2->DataBlockEnd();
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::EnableButton(bool enab)
{
	sButton1->Enabled = enab;
	sButton2->Enabled = enab;
	sButton3->Enabled = enab;
	sButton8->Enabled = enab;
	sButton9->Enabled = enab;
	sButton10->Enabled = enab;
}

//---------------------------------------------------------------------------
int provMess = -1;
void __fastcall TMK_2::CheckEndMK()
{
	for(int i=0; i<PARAMETERS_MK; i++) {
		if(wMk->CheckMK[i] == 0) return;
	}

	sButton6->Enabled = true;
	sButton12->Enabled = true;

	sButton4->Enabled = true;
	sButton5->Enabled = true;

	for(int i=0; i<PARAMETERS_MK; i++) {
		if(wMk->CheckMK[i] == 2 ) {
			sPanel11->Font->Color = clRed;
			sPanel11->Caption = "���� ����������";
			provMess++;
			if (provMess==0) {
				BASS_Init(-1,44100,0,0,NULL);
				sample = BASS_SampleLoad(false,"2.wav",0,0,1,BASS_SAMPLE_MONO) ;
				channel =  BASS_SampleGetChannel(sample,false);
				BASS_ChannelPlay(channel,false);
			}
			 //  provMess =0;

	   //		WorkModeEND = RET_ERROR;
			return;
		}
	}
//		if (provMess==-1) {
		BASS_Init(-1,44100,0,0,NULL);
		sample = BASS_SampleLoad(false,"1.wav",0,0,1,BASS_SAMPLE_MONO) ;
		channel =  BASS_SampleGetChannel(sample,false);
		BASS_ChannelPlay(channel,false);

	sPanel11->Font->Color = clLime;
	sPanel11->Caption = "��������� � �����";
	//	}
}
//---------------------------------------------------------------------------
// ������ ��������� ��������
void __fastcall TMK_2::sButton4Click(TObject *Sender)
{
//	wMk->UtilStop(1);		// ���� � ����������� �������

	this->Close();          // ��� �������� ����������� �������
}
//---------------------------------------------------------------------------
// ������ ��������� ��������
void __fastcall TMK_2::sButton5Click(TObject *Sender)
{
ModeAuto=0 ;
  btnAuto->Visible=true;
	TMK_2 *MK_2 = new TMK_2(0, wMk->NumberBlock);

	if(!MK_2) return;
	MK_2->Show();
   //	 wMk->provT = true;
//wMk->PowerPauseSecond = 25;
provT1 = true;

	this->Close();
}
//---------------------------------------------------------------------------
// ������ ������
void __fastcall TMK_2::sButton6Click(TObject *Sender)
{
	wMk->WriteExcel("\\RaportMK.xlsx", true);
}
//---------------------------------------------------------------------------
// ����� ������
void __fastcall TMK_2::sButton12Click(TObject *Sender)
{
	wMk->WriteExcel("\\RaportMK.xlsx", false);

	TVsReport *vsReport = new TVsReport(this, wMk->NumberBlock);
	if(!vsReport) return;
	vsReport->ShowModal();
	delete vsReport;

	DeleteFile(GetCurrentDir() + "\\~" + wMk->NumberBlock + ".xlsx");
}
//---------------------------------------------------------------------------

//------------------------/ ����������� �������� /--------------------------------
void __fastcall TMK_2::sListBox1Click(TObject *Sender)
{
	sListBox1->Selected[sListBox1->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox2Click(TObject *Sender)
{
	sListBox2->Selected[sListBox2->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox3Click(TObject *Sender)
{
	sListBox3->Selected[sListBox3->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox4Click(TObject *Sender)
{
	sListBox4->Selected[sListBox4->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox5Click(TObject *Sender)
{
	sListBox5->Selected[sListBox5->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox6Click(TObject *Sender)
{
	sListBox6->Selected[sListBox6->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox7Click(TObject *Sender)
{
	sListBox7->Selected[sListBox7->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox8Click(TObject *Sender)
{
	sListBox8->Selected[sListBox8->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox9Click(TObject *Sender)
{
	sListBox9->Selected[sListBox9->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
	FocusControl(iScope2); // ��������� �����, ����� �������� ����� ������ ���� �������� ��������
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox1VScroll(TObject *Sender)
{
	iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox1->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox2VScroll(TObject *Sender)
{
	if(sListBox2->TopIndex == 0) iScope2->Channel[0]->Coupling = isccDC;
		else iScope2->Channel[0]->Coupling = isccAC;
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox3VScroll(TObject *Sender)
{
	iScope2->TimeBase->SecPerDiv = SecPerDivTable[sListBox3->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox4VScroll(TObject *Sender)
{
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[sListBox4->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox5VScroll(TObject *Sender)
{
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox5->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox6VScroll(TObject *Sender)
{
	if(sListBox6->TopIndex == 0) iScope1->Channel[0]->Coupling = isccDC;
		else iScope1->Channel[0]->Coupling = isccAC;
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox7VScroll(TObject *Sender)
{
	iScope3->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox7->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox8VScroll(TObject *Sender)
{
	if(sListBox8->TopIndex == 0) iScope3->Channel[0]->Coupling = isccDC;
		else iScope3->Channel[0]->Coupling = isccAC;
}
//---------------------------------------------------------------------------
void __fastcall TMK_2::sListBox9VScroll(TObject *Sender)
{
	iScope3->TimeBase->SecPerDiv = SecPerDivTable[sListBox9->TopIndex];
}

void __fastcall TMK_2::btnAutoClick(TObject *Sender)
{
autoPro=true;
ModeAuto=0;
sBitBtn1->Enabled=false;
sButton1->Enabled=false;
sButton2->Enabled=false;
sButton3->Enabled=false;
sButton8->Enabled=false;
sButton9->Enabled=false;
sButton10->Enabled=false;
sButton11->Enabled=false;
btnAuto->Enabled=false;

  autoZap=true;
  ///----------------------------������ �������--------------

	btnAuto->Enabled=false;
		wMk->RunFlag = 1;
		wMk->ReplaceBuf = true;

		wMk->Util0Step1();

		Timer1->Interval = 2000;      // ����� �������� ���������� ����������
		Timer1->Enabled = true;
		inclPower = true;
		WorkMode = MODE_UTIL0_STEP1;
		sBitBtn1->Down = true;
		sBitBtn1->Enabled = false;

		wMk->ReplaceBuf2 = true;

		// �������������� ������ ����������� ����������
		for(int i=0; i<PARAMETERS_MK; i++) {
			wMk->CheckMK[i] = 0;
		}
		sButton8->Enabled=false;
	}


//---------------------------------------------------------------------------

void __fastcall TMK_2::timeAutoTimer(TObject *Sender)
{
	 if (ModeAuto==0)  //��-1
	  {    sButton8->Visible=true;
			 sButton9->Visible=false;
			 sButton1->Visible=false;
			 sButton2->Visible=false;
			 sButton10->Visible=false;
			 sButton3->Visible=false;
		 if (provPit){
		sListBox5->ItemIndex = 7;       		// ���������� �������������� �������� 200 ��
		sListBox5->Selected[7] = false;
		iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[7];

		sListBox1->ItemIndex = 7;       		// ���������� �������������� �������� 200 ��
		sListBox1->Selected[7] = false;
		iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[7];

		ResetAllUtil();
		ResetUtil1_2();
		ScopeMK1_3_4(0.31);                           // ������ ������
		wMk->RunFlag = 1;

		wMk->Util2_3_4Step1(MK1);
		WorkMode = MODE_UTIL1_STEP1;
		Timer1->Enabled = true;
		Timer1->Interval = 9000;

		sButton8->Caption = "������ MK1 ������";
		inclMK1 = true;
		HANDLE hnd_scope_thread;
		DWORD scope_thread_id;
		DWORD thread_status;

		scope = 1;
		hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadUtil1_2, this, 0, &scope_thread_id);

		sPanel2->Caption = "U�����. = 310 ��";
		sPanel24->Caption = "U�����. = 310 ��";

		sPanel28->Caption = "F�����. = 0,5 ��";
		sPanel27->Caption = "F�����. = 0,5 ��";

		timeAuto->Enabled = false;

	  }
		 }
		 else
			 if (ModeAuto==1)   //��-2
			 {
			 sButton8->Visible=false;
			 sButton9->Visible=true;
			 sButton1->Visible=false;
			 sButton2->Visible=false;
			 sButton10->Visible=false;
			 sButton3->Visible=false;
				sListBox5->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
				sListBox5->Selected[6] = false;
				iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

				sListBox1->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
				sListBox1->Selected[6] = false;
				iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

				ResetAllUtil();
				ResetUtil1_2();
				ScopeMK2();                           // ������ ������
				wMk->RunFlag = 1;

				wMk->Util2_3_4Step1(MK2);
				WorkMode = MODE_UTIL2_STEP1;
				Timer1->Enabled = true;
				Timer1->Interval = 9000;

				sButton9->Caption = "������ MK2 ������";
				inclMK2 = true;
				sButton1->Enabled = false;
				sButton2->Enabled = false;
				sButton3->Enabled = false;
				sButton8->Enabled = false;
				sButton10->Enabled = false;

				HANDLE hnd_scope_thread;
				DWORD scope_thread_id;
				DWORD thread_status;

				scope = 2;
				hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadUtil1_2, this, 0, &scope_thread_id);

				sPanel2->Caption = "U�����. = 980 ��";
				sPanel24->Caption = "U�����. = 980 ��";

				sPanel28->Caption = "F�����. = 5 ��";
				sPanel27->Caption = "F�����. = 5 ��";
				   timeAuto->Enabled = false;
				 }
				  else
					 if (ModeAuto==2)   //��-3
					 {
					 sButton8->Visible=false;
			 sButton9->Visible=false;
			 sButton1->Visible=true;
			 sButton2->Visible=false;
			 sButton10->Visible=false;
			 sButton3->Visible=false;
					   // ��� ������������� ���� ������� ������� ��������� � ����������� ����������
					if(attempt > 3)
					 {
					if(wMk->CheckMK[5] == 2) ShowMessage("��������� 1 ����������");
					if(wMk->CheckMK[6] == 2) ShowMessage("��������� 2 ����������");
						BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
					return;
					}


					sListBox5->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
					sListBox5->Selected[6] = false;
					iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

					sListBox1->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
					sListBox1->Selected[6] = false;
					iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

					ResetAllUtil();
					ResetUtil3();
					ScopeMK1_3_4(0.98);                           // ������ ������
					wMk->RunFlag = 1;

					wMk->Util2_3_4Step1(MK3);
					WorkMode = MODE_UTIL3_STEP1;
					Timer1->Enabled = true;
					Timer1->Interval = 9000;

					sButton1->Caption = "������ MK3 ������";
					inclMK3 = true;
					sButton2->Enabled = false;
					sButton3->Enabled = false;
					sButton8->Enabled = false;
					sButton9->Enabled = false;
					sButton10->Enabled = false;

					HANDLE hnd_scope_thread;
					DWORD scope_thread_id;
					DWORD thread_status;

					hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadUtil3, this, 0, &scope_thread_id);

					sPanel2->Caption = "U�����. = 980 ��";
					sPanel24->Caption = "U�����. = 980 ��";

					sPanel28->Caption = "F�����. = 0,5 ��";
					sPanel27->Caption = "F�����. = 0,5 ��";
					timeAuto->Enabled = false;
					 }
					  else
						 if (ModeAuto==3)     //��-4
						 {
							sButton8->Visible=false;
							 sButton9->Visible=false;
							 sButton1->Visible=false;
							 sButton2->Visible=true;
							 sButton10->Visible=false;
							 sButton3->Visible=false;
							sListBox5->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
							sListBox5->Selected[6] = false;
							iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

							sListBox1->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
							sListBox1->Selected[6] = false;
							iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

							ResetAllUtil();
							ResetUtil4();
							ScopeMK1_3_4(0.98);                           // ������ ������
							wMk->RunFlag = 1;

							HANDLE hnd_scope_thread;
							DWORD scope_thread_id;
							DWORD thread_status;

					//		scope = 1;
							hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThreadUtil4_5, this, 0, &scope_thread_id);

							wMk->Util5_6Step1(MK4);
							WorkMode = MODE_UTIL4_STEP1;
							Timer1->Enabled = true;
							Timer1->Interval = 3000;

							sButton2->Caption = "������ MK4 ������";
							inclMK4 = true;
							sButton1->Enabled = false;
							sButton3->Enabled = false;
							sButton8->Enabled = false;
							sButton9->Enabled = false;
							sButton10->Enabled = false;

							sPanel2->Caption = "U�����. = 980 ��";
							sPanel24->Caption = "U�����. = 980 ��";
							timeAuto->Enabled = false;
						 }
						 else
							 if (ModeAuto==4)        //��-5
							 {
							 sButton8->Visible=false;
							 sButton9->Visible=false;
							 sButton1->Visible=false;
							 sButton2->Visible=false;
							 sButton10->Visible=true;
							 sButton3->Visible=false;
								sListBox5->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
								sListBox5->Selected[6] = false;
								iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

								sListBox1->ItemIndex = 6;       		// ���������� �������������� �������� 500 ��
								sListBox1->Selected[6] = false;
								iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[6];

								ResetAllUtil();
								ResetUtil5();
								ScopeMK5();
								wMk->RunFlag = 1;

								HANDLE hnd_scope_thread;
								DWORD scope_thread_id;
								DWORD thread_status;

								hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThreadUtil4_5, this, 0, &scope_thread_id);

								wMk->Util5_6Step1(MK5);
								WorkMode = MODE_UTIL5_STEP1;
								Timer1->Enabled = true;
								Timer1->Interval = 3500;

								sButton10->Caption = "������ MK5 ������";
								inclMK5 = true;
								sButton1->Enabled = false;
								sButton2->Enabled = false;
								sButton3->Enabled = false;
								sButton8->Enabled = false;
								sButton9->Enabled = false;

								sPanel2->Caption = "U�����. = 910 ��";
								sPanel24->Caption = "U�����. = 910 ��";
								sLabel17->Caption = "U��� = 8 �";
								timeAuto->Enabled = false;
							 }
							  else
								if (ModeAuto==5)  //���
								{
							   //	sOpenDialog1->InitialDir = GetCurrentDir();
								  sButton8->Visible=false;
							 sButton9->Visible=false;
							 sButton1->Visible=false;
							 sButton2->Visible=false;
							 sButton10->Visible=false;
							 sButton3->Visible=true;
								//if(sOpenDialog1->Execute()) {
									ResetAllUtil();
									ResetUtil6();
									iScope3->SamplesPerSecond = 30000;
									wMk->RunFlag = 1;
									wMk->Util7Step1((AnsiString) "mk-prov.wav");

									HANDLE hnd_scope_thread2;
									DWORD scope_thread_id2;
									DWORD thread_status2;

									hnd_scope_thread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThreadUtil6, this, 0, &scope_thread_id2);

									iScope1->SamplesPerSecond = wMk->LengthButhUtil6 - 1;
									iScope2->SamplesPerSecond = wMk->LengthButhUtil6 - 1;

									HANDLE hnd_scope_thread;
									DWORD scope_thread_id;
									DWORD thread_status;

									hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread2, this, 0, &scope_thread_id);

									WorkMode = MODE_UTIL6_STEP1;
									Timer1->Enabled = true;
									Timer1->Interval = 14000;

									sButton3->Caption = "������ MK� ������";
									inclMKR = true;
									sButton1->Enabled = false;
									sButton2->Enabled = false;
									sButton8->Enabled = false;
									sButton9->Enabled = false;
									sButton10->Enabled = false;

									sLabel17->Caption = "U��� = 8 �";
							  //	}
								 timeAuto->Enabled = false;
								}
								else
								if (ModeAuto==-1)  //��������� ������
								{

								}
	///---------------------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

void __fastcall TMK_2::timeZaderTimer(TObject *Sender)
{          //   	iSevenSegmentInteger1->Value = time_step;
if (time_step>40) {
 timeAuto->Interval = 10;
 timeAuto->Enabled = true;
 timeZader->Enabled=false;
}
 time_step++;
}
//---------------------------------------------------------------------------

