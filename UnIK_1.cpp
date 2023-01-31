//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnIK_1.h"
#include "WIk.h"
#include "UnVisibleReport.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "iScope"
#pragma link "sBitBtn"
#pragma link "sButton"
#pragma link "sLabel"
#pragma link "sListBox"
#pragma link "sPanel"
#pragma link "sRadioButton"
#pragma link "acProgressBar"
#pragma link "sGauge"
#pragma link "sEdit"
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
//TIK_1 *IK_1;
const double TIK_1::SecPerDivTable[19] = {0.1, 0.05, 0.02, 0.01, 0.005, 0.002, 0.001,
0.0005, 0.0002, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06, 5E-07, 2E-07, 1E-07};

const double TIK_1::VoltPerDivTable[22] = {50, 20, 10, 5, 2, 1, 0.5, 0.2, 0.1, 0.05, 0.02,
0.01, 0.005, 0.002, 0.001, 0.0001, 5E-05, 2E-05, 1E-05, 5E-06, 2E-06, 1E-06};
HSAMPLE sample;
	HCHANNEL channel;
//---------------------------------------------------------------------------
__fastcall TIK_1::TIK_1(TComponent* Owner, AnsiString number_block, int type_block)
	: TForm(Owner)
{
	wIk = new WIk;

	inclPower = false;                      // ����������, ������� ��������� �������
	inclX8X10 = false;                      // ������ ��������
	inclOptions = false;

	sListBox4->ItemIndex = 2;       		// ���������� �������������� �������� 20 ��
	sListBox4->Selected[2] = false;
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[2];
	sListBox5->ItemIndex = 4;       		// ���������� �������������� �������� 2 �
	sListBox5->Selected[4] = false;
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[4];

	sListBox3->ItemIndex = 2;       		// ���������� �������������� �������� 20 ��
	sListBox3->Selected[2] = false;
	iScope2->TimeBase->SecPerDiv = SecPerDivTable[2];
	sListBox1->ItemIndex = 4;       		// ���������� �������������� �������� 2 �
	sListBox1->Selected[4] = false;
	iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[4];

	// ��������� �������� �� �� ���� ������ ��� ��� � ������� ����� �����
	wIk->NumberBlock = number_block;
	if(wIk->NumberBlock != "-1") {
		sBitBtn1->Enabled = true;
		sButton11->Visible = false;
		sEdit1->Visible = false;
		sLabel15->Visible = false;

		switch(type_block){
			case BLOCK_BU:{sLabel16->Caption ="���� �� �" + wIk->NumberBlock;	
						   break;}
			case BLOCK_IP:{sLabel16->Caption ="���� �� �" + wIk->NumberBlock;	
						   break;} 
			default:{sLabel16->Caption ="�" + wIk->NumberBlock;}
		}
		sLabel16->Left = (1920 - sLabel16->Width) / 2;
		sLabel16->Visible = true;

		sPanel30->Visible = false;
		sPanel31->Visible = false;
	}
}
//---------------------------------------------------------------------------
__fastcall TIK_1::~TIK_1()
{
//	delete wIk;
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
//	if(wIk->RunFlag) {
//		wIk->UtilStop(1);	// ���������� �������, ���� ��� ��������
//	}

	wIk->UtilStop(1);	// ���������� �������, ���� ��� ��������
}
//---------------------------------------------------------------------------
// ������ �������
void __fastcall TIK_1::sBitBtn1Click(TObject *Sender)
{
	if(inclPower)
	{
		sBitBtn1->Down = true;

//		wIk->UtilStop(1);		// ��������� �������
		Util0Stop();            // � ���� ������� ��������� �������

		// ������� �������� � �������� ����� ����������� �������
		ResetUtil0();

		sButton1->Enabled = false;
		sButton2->Enabled = false;
		sPanel11->Font->Color = clBlack;
		sPanel11->Caption = "�������� �����";
		sLabel3->Caption = "������� � ������ 2?";
	} else
	{
		try
		{
			wIk->RunFlag = 1;
			wIk->ReplaceBuf2 = true;
			wIk->Util0Step1();

			Timer1->Interval = 4500;            // ����� �������� ���������� // ���� 3500 16.06.17
			Timer1->Enabled = true;
			inclPower = true;
			WorkMode = MODE_UTIL0_STEP1;
			sBitBtn1->Down = true;
			sBitBtn1->Enabled = false;

			// �������������� ������ ����������� ����������
			for(int i=0; i<PARAMETERS_IK1; i++) {
				wIk->CheckIK1[i] = 0;
			}

		}
		catch( Exception &e )
		{
			MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sButton9Click(TObject *Sender)
{
//	if(wIk->IsOpenedLTR34() == 0)	 return; 			// ���� �������� ������ ��������

	if(inclX8X10) {
		Util1Stop();
	} else {
		ResetK1K2();
		wIk->RunFlag = 1;

		wIk->Util1Step1();
		WorkMode = MODE_UTIL1_STEP1;
//		Timer1->Enabled = true;
//		Timer1->Interval = 60000;
		inclX8X10 = true;

//		sButton9->Enabled = false;
		sButton9->Caption = "����";

		HANDLE hnd_scope_thread;
		DWORD scope_thread_id;
		DWORD thread_status;

		hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);

		sButton4->Enabled = false;
		sButton5->Enabled = false;
		sButton10->Enabled = false;
		sButton14->Enabled = false;

		Timer2->Enabled = true;
		Timer2->Interval = 1500;
		WorkMode2 = MODE_UTIL1_STEP1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sButton14Click(TObject *Sender)
{
	if(inclOptions) {
		Util7Stop();
	} else {
//		ResetUtil2K3();
		wIk->RunFlag = 1;

		wIk->Util2Step0();
		WorkMode = MODE_UTIL2_STEP0;
		Timer1->Enabled = true;
		Timer1->Interval = 1000;           // ������ ������� ��������� �����
		inclOptions = true;

		sButton14->Caption = "����";
//		sButton14->Enabled = false;

		HANDLE hnd_scope_thread;
		DWORD scope_thread_id;
		DWORD thread_status;

		hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wIk->CheckUtil3_7Thread, wIk, 0, &scope_thread_id);

		sButton5->Enabled = false;
		sButton4->Enabled = false;
		sButton9->Enabled = false;
		sButton10->Enabled = false;
	}

}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sButton10Click(TObject *Sender)
{
//	if(wIk->IsOpenedLTR34() == 0) return; 			// ���� �������� ������ ��������

	ResetUtil2K3();
	wIk->RunFlag = 1;

	wIk->Util2Step1();
	WorkMode = MODE_UTIL2_STEP1;
	Timer1->Enabled = true;
	Timer1->Interval = 700;

//	EnableButton(false);
	sButton10->Enabled = false;

	sButton4->Enabled = false;
	sButton5->Enabled = false;
	sButton9->Enabled = false;
	sButton14->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sButton5Click(TObject *Sender)
{
//	if(wIk->IsOpenedLTR34() == 0) return; 			// ���� �������� ������ ��������

	ResetUtil3K3();
	wIk->RunFlag = 1;

	wIk->Util3Step1();
	WorkMode = MODE_UTIL3_STEP1;
	Timer1->Enabled = true;
	Timer1->Interval = 700;

//	EnableButton(false);
	sButton4->Enabled = false;
	sButton5->Enabled = false;
	sButton9->Enabled = false;
	sButton10->Enabled = false;
	sButton14->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sButton4Click(TObject *Sender)
{
//	if(wIk->IsOpenedLTR34() == 0) return; 			// ���� �������� ������ ��������

	ResetUtil4K3();
	wIk->RunFlag = 1;

	wIk->Util4Step1();
	WorkMode = MODE_UTIL4_STEP1;
	Timer1->Enabled = true;
	Timer1->Interval = 5000;                       // ����������� 5 �

	sButton4->Enabled = false;
	sButton5->Enabled = false;
	sButton9->Enabled = false;
	sButton10->Enabled = false;
	sButton14->Enabled = false;

	sGauge1->Enabled = true;
	sGauge1->Progress = 0;
	Timer2->Enabled = true;
	Timer2->Interval = 1000;
	WorkMode2 = MODE_UTIL4_STEP1;

	// ������� �����������
	sLabel17->Caption = "���������� ������� � �������� 5 �";
	sLabel17->Left = 989;
	sLabel17->Visible = true;
}
//---------------------------------------------------------------------------
//static char Text1[] = "F = %.1f ��";
static char Text1[] = "T = %.2f ��";
static char Text2[] = "U����.���.= %.1f �";
static char Text3[] = "K4 = %.1f �";
static char Text4[] = "K5 = %.1f �";
static char Text6[] = "I+ = %.2f ��";
static char Text7[] = "T = %.3f ��";
void __fastcall TIK_1::Timer1Timer(TObject *Sender)
{

	AnsiString form_string;

	try
	{
//***************** Util 0 ***************************
		if(WorkMode == MODE_UTIL0_STEP1) {
			bool power1;

			wIk->currentPlus = wIk->Acq2Buf[wIk->BlockNumberThread2][32];     	// ���� 32-� �������� ��� ����

			form_string.printf( Text6, wIk->currentPlus );
			sPanel18->Caption = form_string;

			// ������������� ���
			if(wIk->currentPlus >= CURRENT_MIN	&& wIk->currentPlus <= CURRENT_MIDD) {
				sPanel18->Font->Color = clLime;
				sPanel16->Font->Color = clLime;
				sPanel16->Caption = "�����";
				wIk->note_current = "�����";
				power1 = true;
			} else {
				if(wIk->currentPlus > CURRENT_MIDD && wIk->currentPlus < CURRENT_MAX) {
					sPanel18->Font->Color = clYellow;
					sPanel16->Font->Color = clYellow;
					sPanel16->Caption = "����������";
				} else {
					sPanel18->Font->Color = clRed;
					sPanel16->Font->Color = clRed;
					sPanel16->Caption = "������";
				}
				power1 = false;
					BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
                wIk->note_current = "�������������";
			}

			if(power1) {
				sButton14->Enabled = true;

				sBitBtn1->Font->Color = clLime;
				sBitBtn1->Caption = "��������� �������";
				wIk->UtilStop();
			} else Util0Stop();

			Timer1->Enabled = false;
			WorkMode = MODE_NULL;
			sBitBtn1->Down = false;
			sBitBtn1->Enabled = true;
		}
//***************** Util 1 ***************************
//		if(WorkMode == MODE_UTIL1_STEP1) {
//			wIk->RunFlag = 0;									// �������������� ���������� ������� ��� � scope-������
//
//			// ������� ���������� ������� � ����� ������� ��� �1
//			int porog = 0;
//			double countSamples = 1;
//			for(int i = 20; i < ACQ_BLOCK_SIZE; i += 2) {
//				if(wIk->AcqBuf[wIk->BlockNumberThread][i] < 4 && wIk->AcqBuf[wIk->BlockNumberThread][i+2] > 4)
//					porog++;
//
//				if(porog == 1) countSamples++;
//				if(porog > 1) break;
//			}
//			wIk->Util1frequencyK1 = countSamples / iScope1->SamplesPerSecond * 1000;
//
//			// ������� ���������� ������� � ����� ������� ��� �2
//			porog = 0;
//			countSamples = 1;
//			for(int i = 19; i < ACQ_BLOCK_SIZE; i += 2) {
//				if(wIk->AcqBuf[wIk->BlockNumberThread][i] < 4 && wIk->AcqBuf[wIk->BlockNumberThread][i+2] > 4)
//					porog++;
//
//				if(porog == 1) countSamples++;
//				if(porog > 1) break;
//			}
//			wIk->Util1frequencyK2 = countSamples / iScope2->SamplesPerSecond * 1000;
//
//			// ������� �������� ��� �1
////			wLsk->frequencyK9 = 1;                             // �������� �� ������
//			wIk->Util1frequencyK1 = SimpleRoundTo(wIk->Util1frequencyK1, -2);
//			form_string.printf( Text1, wIk->Util1frequencyK1 );
//			sPanel3->Caption = form_string;
////			if(wIk->Util1frequencyK1 >= FREQUENCY_K1_MIN && wIk->Util1frequencyK1 <= FREQUENCY_K1_MAX) {
//			if(wIk->Util1frequencyK1 >= TIME_X8_MIN && wIk->Util1frequencyK1 <= TIME_X8_MAX) {
//				sPanel3->Font->Color = clLime;
//				sPanel2->Font->Color = clLime;
//				sPanel2->Caption = "�����";
//				wIk->note_timeX8 = "�����";
//				wIk->CheckIK1[0] = 1;
//			} else {
//				if(wIk->Util1frequencyK1 == iScope1->SamplesPerSecond) {
//					sPanel3->Font->Color = clRed;
//					sPanel2->Font->Color = clRed;
//					sPanel2->Caption = "������";
//				} else {
//					sPanel3->Font->Color = clYellow;
//					sPanel2->Font->Color = clYellow;
//					sPanel2->Caption = "����������, ��������� ����������";
//				}
//					wIk->note_timeX8 = "�������������";
//				wIk->CheckIK1[0] = 2;
//			}
//
//			// ������� �������� ��� �2
////			wLsk->frequencyK9 = 1;                             // �������� �� ������
//			wIk->Util1frequencyK2 = SimpleRoundTo(wIk->Util1frequencyK2, -3);
//			form_string.printf( Text7, wIk->Util1frequencyK2 );
//			sPanel9->Caption = form_string;
////			if(wIk->Util1frequencyK2 >= FREQUENCY_K2_MIN && wIk->Util1frequencyK2 <= FREQUENCY_K2_MAX) {
//			if(wIk->Util1frequencyK2 >= TIME_X10_MIN && wIk->Util1frequencyK2 <= TIME_X10_MAX) {
//				sPanel9->Font->Color = clLime;
//				sPanel7->Font->Color = clLime;
//				sPanel7->Caption = "�����";
//				wIk->note_timeX10 = "�����";
//				wIk->CheckIK1[1] = 1;
//			} else {
//				if(wIk->Util1frequencyK2 == iScope1->SamplesPerSecond) {
//					sPanel9->Font->Color = clRed;
//					sPanel7->Font->Color = clRed;
//					sPanel7->Caption = "������";
//				} else {
//					sPanel9->Font->Color = clYellow;
//					sPanel7->Font->Color = clYellow;
//					sPanel7->Caption = "����������, ��������� ����������";
//				}
//				wIk->CheckIK1[1] = 2;
//				wIk->note_timeX10 = "�������������";
//			}
//
//			Util1Stop();
//		}
//***************** Util 2 Step 0 ********************
	if(WorkMode == MODE_UTIL2_STEP0) {
		if(wIk->Util3K3) {
			sPanel14->Font->Color = clLime;
			sPanel14->Caption = "����� 0 - 1 �����";
			Util7Stop();
		}

//		wIk->UtilStop();
//		WorkMode = MODE_NULL;
//		Timer1->Enabled = false;
//		sButton14->Enabled = true;
	}
//***************** Util 2 ***************************
		if(WorkMode == MODE_UTIL2_STEP1) {
			switch(wIk->Util2Step2_12())
			{
				case RET_OK:{double pressureK3 = SimpleRoundTo(wIk->PressureK3, -1);
							if(pressureK3 >= Util2_K3_MIN && pressureK3 <= Util2_K3_MAX) {
								sPanel24->Font->Color = clLime;
								sPanel22->Font->Color = clLime;
								sPanel22->Caption = "�����";
								wIk->CheckIK1[2] = 1;
								wIk->note_PressureK3 = "�����";
							 } else {
								sPanel24->Font->Color = clYellow;
								sPanel22->Font->Color = clYellow;
								sPanel22->Caption = "����������, ��������� ����������";
								wIk->CheckIK1[2] = 2;
								wIk->note_PressureK3 = "�������������";
								BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
							 }

							 sPanel14->Font->Color = clLime;
							 sPanel14->Caption = "����� 0 - 1 �����";
							 //form_string.printf( Text2, SimpleRoundTo(wIk->PressureK3, -1) );
							 //sPanel24->Caption = form_string;
							 sPanel24->Caption = "U����.���.= " + FloatToStr(pressureK3) + " �";

							 Util2Stop();
							 break;}

				case RET_END:{sPanel14->Font->Color = clRed;
							  sPanel14->Caption = "��� ������. ������";
							  wIk->CheckIK1[2] = 2;
							  wIk->note_PressureK3 = "�������������";
							  BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);

							  Util2Stop();
							  break;}
			}
		}
//***************** Util 3 ***************************
		if(WorkMode == MODE_UTIL3_STEP1) {
			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;

			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wIk->CheckUtil3_7Thread, wIk, 0, &scope_thread_id);

			WorkMode = MODE_UTIL3_STEP2;
			wIk->Util3Step2();
			Timer1->Interval = 700;                // ������������ ������� ����
			return;
		}

		if(WorkMode == MODE_UTIL3_STEP2) {
			if(wIk->Util3K3 == true) {
				sPanel33->Font->Color = clLime;
				sPanel33->Caption = "����� 0 - 1 �����";
				wIk->CheckIK1[3] = 1;
			} else {
				sPanel33->Font->Color = clRed;
				sPanel33->Caption = "��� ������. ������";
				BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				wIk->CheckIK1[3] = 2;
			}

			Timer1->Enabled = false;
			WorkMode = MODE_NULL;
			wIk->UtilStop();

			sButton4->Enabled = true;
			sButton5->Enabled = true;
			sButton9->Enabled = true;
			sButton10->Enabled = true;
			sButton14->Enabled = true;
		}
//***************** Util 4 ***************************
		if(WorkMode == MODE_UTIL4_STEP1) {

//			WorkMode = MODE_UTIL4_STEP2;
			sLabel17->Caption = "������ ��������� �� X19";
			sLabel17->Left = 1032;
			iSevenSegmentInteger1->Visible = true;
			iSevenSegmentInteger1->Value = 0;

			WorkMode = MODE_UTIL4_STEP2_1;
			t_start = Time();                        // �������� ������ �������
			wIk->Util4Step2();

//			Timer1->Interval = 35000;                // ������������ ������� ����
			Timer1->Interval = 27800;
			return;
		}

		if(WorkMode == MODE_UTIL4_STEP2_1) {
			for(int i = 0; i < ACQ_BLOCK_QNT; i++) {                       // ����� ��������� ������ ��� ����� ������
				wIk->AcqBlockReady[i] = 0;                                 // ������ � ������
			}

			HANDLE hnd_scope_thread;
			DWORD scope_thread_id;
			DWORD thread_status;
//          t_start = Time();
			hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheckUtil4Step2Thread, this, 0, &scope_thread_id);

			Timer1->Interval = 2000;
			WorkMode = MODE_UTIL4_STEP2;
			return;
		}

		if(WorkMode == MODE_UTIL4_STEP2) {
			WorkMode = MODE_UTIL4_STEP3;

			sLabel17->Caption = "���������� ������� � �������� 4 �";
			sLabel17->Left = 989;

//			wIk->RunFlag = 0;                        // ������������� ��� ������
//			WorkMode = MODE_UTIL4_STEP3;
			wIk->Util4Step3();                       // ��������� ������� � ��� 4 �
			Timer1->Interval = 4;                	 // ��� 4 �
			return;
		}

		if(WorkMode == MODE_UTIL4_STEP3) {
			wIk->UtilStop(1);

			WorkMode = MODE_NULL;
			Timer1->Enabled = false;

			sButton4->Enabled = true;
			sButton5->Enabled = true;
			sButton9->Enabled = true;
			sButton10->Enabled = true;
			sButton14->Enabled = true;

			sGauge1->Progress = 100;
			sGauge1->Enabled = false;
			Timer2->Enabled = false;
            WorkMode2 = MODE_NULL;

			// ������� �����������
			sLabel17->Visible = false;

			TransitionIK2();
			ShowMessage("�������� � ����� 5-6 ������ ��������� ������ �� ���������� ������");
		}

//		if(WorkMode == MODE_UTIL4_STEP3) {
//			WorkMode = MODE_UTIL4_STEP4;
//			wIk->Util4Step4();
//			Timer1->Interval = 1000;                 // ��� 1 �
//			step = 1;
////			wIk->Util4K4 = 0;                        // �������������� ����������
////			wIk->Util4K5 = 0;
//			return;
//		}
//
//		if(WorkMode == MODE_UTIL4_STEP4) {
//			bool thread = true;
//			int count = 0;
//
//			wIk->Util4K4 = 0;                        // �������� ����������
//			wIk->Util4K5 = 0;
//
//			while(thread) {
//				if(count == ACQ_BLOCK_QNT) break;
//
//				if(wIk->AcqBlockReady[count]) {
//
//					for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
//						if( wIk->AcqBuf[count][i] > wIk->Util4K4 ) wIk->Util4K4 = wIk->AcqBuf[count][i];
//						i++;
//						if( wIk->AcqBuf[count][i] > wIk->Util4K5 ) wIk->Util4K5 = wIk->AcqBuf[count][i];
//						i++;
//					}
//				}
//				wIk->AcqBlockReady[count] = 0;
//				count++;
//			}
//
//			form_string.printf( Text3, wIk->Util4K4 );
//			sPanel34->Caption = form_string;
//			form_string.printf( Text4, wIk->Util4K5 );
//			sPanel35->Caption = form_string;
//			sLabel12->Caption = IntToStr(step);
//			step++;
//
//			if(step > 640) {
//				Timer1->Enabled = false;
//				wIk->UtilStop();
//				WorkMode = MODE_NULL;
//				sButton12->Enabled = true;
//			}
//		}
	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::Timer2Timer(TObject *Sender)
{
	try
	{
//***************** Util 1 ***************************
		if(WorkMode2 == MODE_UTIL1_STEP1) {
			AnsiString form_string;

			// ������� �������� ��� �1
			wIk->Util1TimeK1 = SimpleRoundTo(wIk->Util1frequencyK1, -2);
			form_string.printf( Text1, wIk->Util1frequencyK1 );
			sPanel3->Caption = form_string;
//			if(wIk->Util1frequencyK1 >= FREQUENCY_K1_MIN && wIk->Util1frequencyK1 <= FREQUENCY_K1_MAX) {
			if(wIk->Util1TimeK1 >= TIME_X8_MIN && wIk->Util1TimeK1 <= TIME_X8_MAX) {
				sPanel3->Font->Color = clLime;
				sPanel2->Font->Color = clLime;
				sPanel2->Caption = "�����";
				wIk->note_timeX8 = "�����";
				wIk->CheckIK1[0] = 1;
			} else {
				if(wIk->Util1frequencyK1 == iScope1->SamplesPerSecond) {
					sPanel3->Font->Color = clRed;
					sPanel2->Font->Color = clRed;
					sPanel2->Caption = "������";
				} else {
					sPanel3->Font->Color = clYellow;
					sPanel2->Font->Color = clYellow;
					sPanel2->Caption = "����������, ��������� ����������";
				}
				wIk->CheckIK1[0] = 2;
				BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				wIk->note_timeX8 = "�������������";
			}

			// ������� �������� ��� �2
			wIk->Util1TimeK2 = SimpleRoundTo(wIk->Util1frequencyK2, -3);
			form_string.printf( Text7, wIk->Util1frequencyK2 );
			sPanel9->Caption = form_string;
//			if(wIk->Util1frequencyK2 >= FREQUENCY_K2_MIN && wIk->Util1frequencyK2 <= FREQUENCY_K2_MAX) {
			if(wIk->Util1TimeK2 >= TIME_X10_MIN && wIk->Util1TimeK2 <= TIME_X10_MAX) {
				sPanel9->Font->Color = clLime;
				sPanel7->Font->Color = clLime;
				sPanel7->Caption = "�����";
				wIk->note_timeX10 = "�����";
				wIk->CheckIK1[1] = 1;
			} else {
				if(wIk->Util1frequencyK2 == iScope1->SamplesPerSecond) {
					sPanel9->Font->Color = clRed;
					sPanel7->Font->Color = clRed;
					sPanel7->Caption = "������";
				} else {
					sPanel9->Font->Color = clYellow;
					sPanel7->Font->Color = clYellow;
					sPanel7->Caption = "����������, ��������� ����������";
				}
				BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
				wIk->CheckIK1[1] = 2;
				wIk->note_timeX10 = "�������������";
			}
        }
//***************** Util 4 ***************************
		if(WorkMode2 == MODE_UTIL4_STEP1) {
			sGauge1->Progress = sGauge1->Progress + 2;
			if(WorkMode == MODE_UTIL4_STEP2_1 || WorkMode == MODE_UTIL4_STEP2) {
				iSevenSegmentInteger1->Value = iSevenSegmentInteger1->Value + 1;
			}
		}
	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall  TIK_1::Util0Stop()
{
	wIk->UtilStop(1);

	sBitBtn1->Font->Color = clRed;
	sBitBtn1->Caption = "������ �������";
	WorkMode = MODE_NULL;

	inclPower = false;
	sBitBtn1->Down = false;
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::Util1Stop()
{
	WorkMode = MODE_NULL;
//	Timer1->Enabled = false;
	wIk->UtilStop();
	sButton9->Caption = "F�����. ������";
	inclX8X10 = false;

	Timer2->Enabled = false;
	WorkMode2 = MODE_NULL;

	sButton5->Enabled = true;
	sButton10->Enabled = true;
	sButton14->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::Util2Stop()
{
	Timer1->Enabled = false;
	WorkMode = MODE_NULL;
	wIk->UtilStop();

	sButton9->Enabled = true;
	sButton10->Enabled = true;
	sButton14->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::Util7Stop()
{
	Timer1->Enabled = false;
	WorkMode = MODE_NULL;
	wIk->UtilStop();
//	sButton14->Enabled = true;
	sButton14->Caption = "��������� ������";
	inclOptions = false;

	sButton10->Enabled = true;
}
//---------------------------------------------------------------------------
// ��������� ������� ��������� �������
DWORD WINAPI TIK_1::ScopeThread(TIK_1 *IK_1)
{
	int i;
	int count = 0;
	bool NeedPacket1, NeedPacket2;
	AnsiString form_string;

	int countSamplesK1 = 1;
	int countSamplesK2 = 1;
	int countForScreenK1 = 0;
	int countForScreenK2 = 0;
	int porog_K1 = 0;
	int porog_K2 = 0;

	if(IK_1->wIk->BlockNumberThread == 100) // �������� ������������� ����������
		{
			IK_1->CreateTh();                   // ��������� ������ ������ ����� �����
			ExitThread(0);
			return 0;
		}

	while(IK_1->wIk->RunFlag) {

		if(count == ACQ_BLOCK_QNT) count = 0;

		NeedPacket1 = IK_1->iScope1->NeedPacketsNow;
		NeedPacket2 = IK_1->iScope2->NeedPacketsNow;

		if(!NeedPacket2 || !NeedPacket1) {
			if(!NeedPacket1) IK_1->iScope1->DataBlockClear();
			if(!NeedPacket2) IK_1->iScope2->DataBlockClear();
		}// else {

			if(!IK_1->iScope2->DataBlockActive && !IK_1->iScope2->TransferringActve &&
			   !IK_1->iScope1->DataBlockActive && !IK_1->iScope1->TransferringActve)
			{

				if(IK_1->wIk->AcqBlockReady[count]) {

					IK_1->iScope1->DataBlockBegin();
					IK_1->iScope2->DataBlockBegin();

					for(i=0; i < ACQ_BLOCK_SIZE; ) {
						IK_1->iScope1->AddChannelData(0, IK_1->wIk->AcqBuf[count][i++]);
						IK_1->iScope2->AddChannelData(0, IK_1->wIk->AcqBuf[count][i++]);
					}
					IK_1->iScope1->DataBlockEnd();
					IK_1->iScope2->DataBlockEnd();
//***************************���������� ���������� �������************************
//			// ������� ���������� ������� � ����� ������� ��� �1
//			int porog = 0;
//			double countSamples = 1;
//			for(int i = 20; i < ACQ_BLOCK_SIZE; i += 2) {
//				if(IK_1->wIk->AcqBuf[IK_1->wIk->BlockNumberThread][i] < 4 && IK_1->wIk->AcqBuf[IK_1->wIk->BlockNumberThread][i+2] > 4)
//					porog++;
//
//				if(porog == 1) countSamples++;
//				if(porog > 1) break;
//			}
////			IK_1->wIk->Util1frequencyK1 = IK_1->iScope1->SamplesPerSecond / countSamples;
//			IK_1->wIk->Util1frequencyK1 = countSamples / IK_1->iScope1->SamplesPerSecond * 1000;
//
//			// ������� ���������� ������� � ����� ������� ��� �2
//			porog = 0;
//			countSamples = 1;
//			for(int i = 19; i < ACQ_BLOCK_SIZE; i += 2) {
//				if(IK_1->wIk->AcqBuf[IK_1->wIk->BlockNumberThread][i] < 4 && IK_1->wIk->AcqBuf[IK_1->wIk->BlockNumberThread][i+2] > 4)
//					porog++;
//
//				if(porog == 1) countSamples++;
//				if(porog > 1) break;
//			}
////			IK_1->wIk->Util1frequencyK2 = IK_1->iScope2->SamplesPerSecond / countSamples;
//			IK_1->wIk->Util1frequencyK2 = countSamples / IK_1->iScope2->SamplesPerSecond * 1000;

	for(i = 2; i < ACQ_BLOCK_SIZE; ++i)
	{
		if( !(i % 2) )
		{
			// ����� ������
			if(IK_1->wIk->AcqBuf[IK_1->wIk->BlockNumberThread][i] < 4 && IK_1->wIk->AcqBuf[IK_1->wIk->BlockNumberThread][i-2] > 4)
				++porog_K1;

			if(porog_K1 == 1)
				++countSamplesK1;

			if(porog_K1 > 1)
			{
				porog_K1 = 0;
				countForScreenK1 = countSamplesK1;	// �������� ��� �������
				countSamplesK1 = 1;
			}
		}
		else
		{
			// ����� ��������
			if(IK_1->wIk->AcqBuf[IK_1->wIk->BlockNumberThread][i] < 4 && IK_1->wIk->AcqBuf[IK_1->wIk->BlockNumberThread][i-2] > 4)
				++porog_K2;

			if(porog_K2 == 1)
				++countSamplesK2;

			if(porog_K2 > 1)
			{
				porog_K2 = 0;
				countForScreenK2 = countSamplesK2;	// �������� ��� �������
				countSamplesK2 = 1;
			}
		}
	}

	if(countSamplesK1 != 1)
		++countSamplesK1;

	if(countSamplesK2 != 1)
		++countSamplesK2;

	if(countForScreenK1 != 0)
		IK_1->wIk->Util1frequencyK1 = (double) countForScreenK1 / IK_1->iScope1->SamplesPerSecond * 1000;
	if(countForScreenK2 != 0)
		IK_1->wIk->Util1frequencyK2 = (double) countForScreenK2 / IK_1->iScope2->SamplesPerSecond * 1000;

//***************************************************************************

					IK_1->wIk->AcqBlockReady[count] = 0;
					count++;
				}
			}
		//}
	}// while(IK_1->wIk->RunFlag)

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
// ������� ���������� ������� �������
void __fastcall TIK_1::CreateTh()
{
	HANDLE hnd_scope_thread;
	DWORD scope_thread_id;
	DWORD thread_status;

	Sleep(500);
	hnd_scope_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScopeThread, this, 0, &scope_thread_id);
}
//---------------------------------------------------------------------------

// ���������� ���������� ������� ��� F�����. ������
void __fastcall TIK_1::Frequency_Calculation()
{
/*	// ������� ���������� ������� � ����� ������� ��� �1
	int porog = 0;
	double countSamples = 1;

	// ������ � 20?
	for(int i = 20; i < ACQ_BLOCK_SIZE; i += 2)
	{
		if(wIk->AcqBuf[IK_1->wIk->BlockNumberThread][i] < 4 && IK_1->wIk->AcqBuf[IK_1->wIk->BlockNumberThread][i+2] > 4)
			porog++;

			if(porog == 1) countSamples++;
			if(porog > 1) break;
	}
//	IK_1->wIk->Util1frequencyK1 = IK_1->iScope1->SamplesPerSecond / countSamples;
	IK_1->wIk->Util1frequencyK1 = countSamples / IK_1->iScope1->SamplesPerSecond * 1000;

	// ������� ���������� ������� � ����� ������� ��� �2
	porog = 0;
	countSamples = 1;

	// ������ � 19?
	for(int i = 19; i < ACQ_BLOCK_SIZE; i += 2)
	{
		if(IK_1->wIk->AcqBuf[IK_1->wIk->BlockNumberThread][i] < 4 && IK_1->wIk->AcqBuf[IK_1->wIk->BlockNumberThread][i+2] > 4)
			porog++;

		if(porog == 1) countSamples++;
		if(porog > 1) break;
	}
//	IK_1->wIk->Util1frequencyK2 = IK_1->iScope2->SamplesPerSecond / countSamples;
	IK_1->wIk->Util1frequencyK2 = countSamples / IK_1->iScope2->SamplesPerSecond * 1000;
*/
}
//---------------------------------------------------------------------------

static char Text5[] = "K3 = %.1f �";
//��������� ������� ���������������� ��������
DWORD WINAPI TIK_1::CheckUtil4Step2Thread (TIK_1 *IK_1)
{
//	AnsiString form_string;
	int count = 0;
	IK_1->wIk->Util4K3 = false;				// �������������� ����������

	while(IK_1->wIk->RunFlag)
		{
			if(count == ACQ_BLOCK_QNT) count = 0;

			if(IK_1->wIk->AcqBlockReady[count]) {

				for(int i = 0; i < ACQ_BLOCK_SIZE; i++) {
					if( IK_1->wIk->AcqBuf[count][i] > 4 )
//						IK_1->form_string6.printf( Text5, IK_1->wIk->AcqBuf[count][i] );
//						IK_1->sPanel36->Caption = IK_1->form_string6;
						IK_1->wIk->Util4K3 = true;
						IK_1->t_end = Time();
						IK_1->wIk->RunFlag = 0;
						break;
				}

			IK_1->wIk->AcqBlockReady[count] = 0;
			count++;
			}
		}
//	IK_1->form_string6.printf( Text5, IK_1->wIk->AcqBuf[count][i] );
//	IK_1->sPanel36->Caption = IK_1->form_string6;

	IK_1->t_start = IK_1->t_end - IK_1->t_start;
	IK_1->wIk->TimeUtil4X6 = IK_1->t_start;

	if(IK_1->t_start >= EncodeTime(0, 0, Util4_SS_MIN, Util4_ZZZ_MIN) && IK_1->t_start <= EncodeTime(0, 0, Util4_SS_MAX, Util4_ZZZ_MAX)) {
		IK_1->sPanel26->Caption = "T����. ��. = " + IK_1->t_start.FormatString("ss.zzz");
		IK_1->sPanel26->Font->Color = clLime;
		IK_1->sPanel25->Font->Color = clLime;
		IK_1->sPanel25->Caption = "�����";
		IK_1->wIk->note_Util4X6 = "�����";
		IK_1->wIk->CheckIK1[4] = 1;
	}	else {
		IK_1->sPanel26->Caption = "T����. ��. = ";
		IK_1->sPanel26->Font->Color = clRed;
		IK_1->sPanel25->Font->Color = clRed;
		IK_1->sPanel25->Caption = "������";
		IK_1->wIk->note_Util4X6 = "�������������";
		BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"3.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
		IK_1->wIk->CheckIK1[2] = 1;
    }


	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::ResetUtil0()
{
	sPanel18->Font->Color = clWhite;
	sPanel16->Font->Color = clWhite;

	sPanel18->Caption = "I+ =";
    sPanel16->Caption = "";
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::ResetK1K2()
{
	iScope1->Plot->ClearAllData();
	iScope2->Plot->ClearAllData();

	sPanel3->Font->Color = clWhite;
	sPanel9->Font->Color = clWhite;

	sPanel3->Caption = "T = ";
	sPanel9->Caption = "T = ";

	sPanel2->Caption = "";
	sPanel7->Caption = "";
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::ResetUtil2K3()
{
	sPanel14->Font->Color = clWhite;
	sPanel14->Caption = "����� 0 - 1\n";

	sPanel24->Font->Color = clWhite;
	sPanel24->Caption = "U����.���.=";

	sPanel22->Font->Color = clWhite;
	sPanel22->Caption = "";
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::ResetUtil3K3()
{
	sPanel33->Font->Color = clWhite;
	sPanel33->Caption = "����� 0 - 1\n";
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::ResetUtil4K3()
{
	sPanel26->Font->Color = clWhite;
	sPanel26->Caption = "T����. ��. = ";
	sPanel25->Font->Color = clWhite;
	sPanel25->Caption = "";

}
//---------------------------------------------------------------------------
// ������� �������� ���������� �������� �� ������ �����
void __fastcall TIK_1::TransitionIK2()
{
	for(int i=0; i<PARAMETERS_IK1; i++) {

		if(wIk->CheckIK1[i] == 0) return;
	}

	sButton1->Enabled = true;
	sButton2->Enabled = true;
		sButton6->Enabled=true;
	sButton12->Enabled=true;

	for(int i=0; i<PARAMETERS_IK1; i++) {
		if(wIk->CheckIK1[i] == 2 ) {
			sPanel11->Font->Color = clRed;
			sPanel11->Caption = "���� ����������";
			sLabel3->Caption = "��������� ��������?";
			BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"2.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
			WorkModeEND = RET_ERROR;
			return;
        }
	}

	WorkModeEND = RET_OK;
	sPanel11->Font->Color = clLime;
	sPanel11->Caption = "��������� � �����";
	sLabel3->Caption = "������� � ������ \"2\"?";
	BASS_Init(-1,44100,0,0,NULL);
	  sample = BASS_SampleLoad(false,"1.wav",0,0,1,BASS_SAMPLE_MONO) ;
	channel =  BASS_SampleGetChannel(sample,false);
	BASS_ChannelPlay(channel,false);
}
//---------------------------------------------------------------------------
// ������ �������� �� ������ ��� ������ �����
void __fastcall TIK_1::sButton1Click(TObject *Sender)
{
//	WorkModeEND = RET_OK; 	// ��� �������

	switch(WorkModeEND) {
		case RET_OK:{	TIK_2 *IK_2 = new TIK_2(0, wIk);
						if(!IK_2)
							return;
						IK_2->Show();
						break;}

		case RET_ERROR:{TIK_1 *IK_1 = new TIK_1(this, wIk->NumberBlock, wIk->Block);
						if(!IK_1) return;
						IK_1->Show();
						break;}
	}

	this->Close();
}
//---------------------------------------------------------------------------
// ������ ������ ���������� ��������
void __fastcall TIK_1::sButton2Click(TObject *Sender)
{
//	wIk->UtilStop(1);		// ���� � ����������� �������

	this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sButton11Click(TObject *Sender)
{
	UnicodeString str;
	char *s = NULL;
	s = NULL;
	try {
		if(sEdit1->Text != "") {
			if(wIk->Block == BLOCK_IP) {
				int len = sEdit1->Text.Length();                    // ���������� ����� ������
				s = new char[len+1];
				for(int i=0; i<len; i++) {
					if(sEdit1->Text.t_str()[i] != '-') s[i] = sEdit1->Text.t_str()[i];
						else break;
				}
				wIk->number_block = StrToInt(s);
				if(wIk->number_block < 1 || wIk->number_block > 24 || wIk->number_block == 7 || wIk->number_block == 19) {
					ShowMessage("������ ������������ ����� �����");
					sEdit1->Text="";
					return;
				}
			} else
				if(wIk->Block == BLOCK_BU) {
					int len = sEdit1->Text.Length();                    // ���������� ����� ������
					s = new char[len+1];
					for(int i=0; i<len; i++) {
						if(sEdit1->Text.t_str()[i] != '-') s[i] = sEdit1->Text.t_str()[i];
							else break;
					}
				wIk->number_block = StrToInt(s);
				if(wIk->number_block != 7 && wIk->number_block != 19) {
					ShowMessage("������ ������������ ����� �����");
					sEdit1->Text="";
					return;
				}
			}

			sBitBtn1->Enabled = true;
			sButton11->Visible = false;
			sEdit1->Visible = false;
			sLabel15->Visible = false;

			wIk->NumberBlock = sEdit1->Text;

			if(wIk->Block == BLOCK_IP) {
				sLabel16->Caption = "���� �� �" + sEdit1->Text;
			} else {
            	sLabel16->Caption = "���� �� �" + sEdit1->Text;
            }
			sLabel16->Left = (1920 - sLabel16->Width) / 2;
			sLabel16->Visible = true;
		}
		else {
			ShowMessage("������� ����� �����!");
		}
	}
	catch( Exception &e )
	{
		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
//		MessageDlg( "������� ������������ �����", mtError, TMsgDlgButtons() << mbOK, 0);
	}
	delete s;
}
//---------------------------------------------------------------------------
// ������ ������ ������������ ����� - ��
void __fastcall TIK_1::sButton7Click(TObject *Sender)
{
	wIk->Block = BLOCK_BU;
	sPanel31->Visible = false;
	sPanel30->Visible = false;

	sButton11->Visible = true;
	sEdit1->Visible = true;
	sLabel15->Visible = true;

	sEdit1->SetFocus();
}
//---------------------------------------------------------------------------
// ������ ������ ������������ ����� - ��
void __fastcall TIK_1::sButton8Click(TObject *Sender)
{
	wIk->Block = BLOCK_IP;
	sPanel31->Visible = false;
	sPanel30->Visible = false;

	sButton11->Visible = true;
	sEdit1->Visible = true;
	sLabel15->Visible = true;

	sEdit1->SetFocus();
}
//---------------------------------------------------------------------------

//------------------------/ ����������� �������� /---------------------------
void __fastcall TIK_1::sListBox1Click(TObject *Sender)
{
	sListBox1->Selected[sListBox1->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sListBox2Click(TObject *Sender)
{
	sListBox2->Selected[sListBox2->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sListBox3Click(TObject *Sender)
{
	sListBox3->Selected[sListBox3->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sListBox4Click(TObject *Sender)
{
	sListBox4->Selected[sListBox4->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sListBox5Click(TObject *Sender)
{
	sListBox5->Selected[sListBox5->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sListBox6Click(TObject *Sender)
{
	sListBox6->Selected[sListBox6->TopIndex] = false; // ������ �� ��������� ������
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sListBox4MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
	FocusControl(iScope1); // ��������� �����, ����� �������� ����� ������ ���� �������� ��������
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sListBox4VScroll(TObject *Sender)
{
	iScope1->TimeBase->SecPerDiv = SecPerDivTable[sListBox4->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sListBox3VScroll(TObject *Sender)
{
	iScope2->TimeBase->SecPerDiv = SecPerDivTable[sListBox3->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sListBox5VScroll(TObject *Sender)
{
	iScope1->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox5->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sListBox1VScroll(TObject *Sender)
{
	iScope2->Channel[0]->VoltsPerDivision = VoltPerDivTable[sListBox1->TopIndex];
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sListBox6VScroll(TObject *Sender)
{
	if(sListBox6->TopIndex == 0) iScope1->Channel[0]->Coupling = COPLING_AC;
		else iScope1->Channel[0]->Coupling = COPLING_DC;
}
//---------------------------------------------------------------------------
void __fastcall TIK_1::sListBox2VScroll(TObject *Sender)
{
	if(sListBox2->TopIndex == 0) iScope2->Channel[0]->Coupling = COPLING_AC;
		else iScope2->Channel[0]->Coupling = COPLING_DC;
}
//---------------------------------------------------------------------------
// ������ �������� �������� �� ������ ����
void __fastcall TIK_1::sButton3Click(TObject *Sender)
{
	TIK_2 *IK_2 = new TIK_2(0, wIk);
	if(!IK_2) return;
	IK_2->Show();

    this->Close();
}
//---------------------------------------------------------------------------





void __fastcall TIK_1::sButton6Click(TObject *Sender)
{
wIk->WriteExcel("\\RaportIK.xlsx", true);
}
//---------------------------------------------------------------------------

void __fastcall TIK_1::sButton12Click(TObject *Sender)
{
	wIk->WriteExcel("\\RaportIK.xlsx", false);

	TVsReport *vsReport = new TVsReport(this, wIk->NumberBlock);
	if(!vsReport) return;
	vsReport->ShowModal();
	delete vsReport;

	DeleteFile(GetCurrentDir() + "\\~" + wIk->NumberBlock + ".xlsx");
}
//---------------------------------------------------------------------------


