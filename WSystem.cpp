//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WSystem.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
//static DWORD WINAPI AcquireThread (LPVOID param);
// ��������� �������� ��� � �������
static const char ErrorText1[] = "������ ��� �������� ������ ������!";
static const char ErrorText2[] = "������! ���������� ������������ ������� �� ����� ���������!";
static const char ErrorText3[] =  "����������� ������";
//--------------------------/ CONSTRUCTOR /----------------------------------
WSystem::WSystem():ArrayToSend(0),VoltArray(0)
{
	BlockNumberThread = 100; // ������������� ���������� �� ������ ���� ����� ��� ����� ������ �������
	ReplaceBuf = false;      // ������������� ���������� ��������� ������ �������
	RunFlag = 0;
}
//--------------------------/ DESTRUCTOR /-----------------------------------
WSystem::~WSystem()
{
//	delete ArrayToSend;
//	delete VoltArray;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
void __fastcall WSystem::Init_LTR11_LTR34()throw(Exception)
{
	int err = 0;

	//������������� ������������ �������
	err = LTR34_Init(&Conf_LTR34);
	if(err) goto GOTO_ERROR2;

	err = LTR11_Init(&Conf_LTR11);
	if(err) goto GOTO_ERROR1;

	//�������� ������� ����� � ��������
	// ����� ����� � ������ LTR34 - 3
	err = LTR34_Open(&Conf_LTR34, SADDR_DEFAULT, SPORT_DEFAULT, "", 3);
	if(err) goto GOTO_ERROR1;
	// ����� ����� � ������ LTR11 - 1
	err = LTR11_Open(&Conf_LTR11, SADDR_DEFAULT, SPORT_DEFAULT, "", 1);

	// ��������� ����� ���� ������������ ������ LTR34
	Conf_LTR34.UseClb = true; 				// ���������� ��������� ������������� ������������
	Conf_LTR34.AcknowledgeType = false; 	// ��� ������������� - �������������
	Conf_LTR34.ExternalStart = false; 		// ������� ����� ��������
	Conf_LTR34.RingMode = true; 			// ������� ����� �������������
	// ��������� ����� ���� ������������ ������ LTR11
	Conf_LTR11.StartADCMode = LTR11_STARTADCMODE_INT; 		// ����� ������ ����� ������ - ����������
	Conf_LTR11.InpMode = LTR11_INPMODE_INT;           		// ����� ������������� ��� - ����������
	Conf_LTR11.ADCMode = LTR11_ADCMODE_ACQ;           		// ����� ����� ������

GOTO_ERROR1:
	if( err )
	{
		LTR11_Close(&Conf_LTR11);
GOTO_ERROR2:
		LTR34_Close(&Conf_LTR34);
		const char *str_err;
		if( err < (-3000) ) str_err = LTR34_GetErrorString(err);
		else
		if( err < (-1000 ) ) str_err = LTR11_GetErrorString(err);
		else str_err = ErrorText3;
		throw Exception( str_err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
void __fastcall WSystem::StartGen_LTR11_LTR34()throw(Exception)
{
	int err;
	HANDLE hnd_acq_thread;
	DWORD acq_thread_id;
	DWORD thread_status;

	RunFlag = 1;

	// ��������� ��������� ���
	err = LTR34_DACStart(&Conf_LTR34);

	// �������� ������ ����� ������ �� ��� ������
	hnd_acq_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AcquireThread, this, 0, &acq_thread_id);
	(void)GetExitCodeThread(hnd_acq_thread, &thread_status);

	if(err)
	{
		ErrorRelease( err );
//		RunFlag = 0;
////		MessageDlg((char *) LTR34_GetErrorString(err), mtError, TMsgDlgButtons() << mbOK, 0);
//		LTR34_Close(&Conf_LTR34);
//		LTR11_Close(&Conf_LTR11);
//
//		delete ArrayToSend;  ArrayToSend = 0;
//		delete VoltArray;  VoltArray = 0;
//		//return RET_ERROR;
//    	const char *str_err;
//		if( err < (-3000) ) str_err = LTR34_GetErrorString(err);
//		else
//		if( err < (-1000 ) ) str_err = LTR11_GetErrorString(err);
//		else str_err = ErrorText3;
//		throw Exception( str_err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
 DWORD WINAPI WSystem::AcquireThread (WSystem *System)
{
	int err;
	DWORD data_buf[ACQ_BLOCK_SIZE];

	System->AcqBlockReady[0] = 0;                                  // ����� ��������� ������ ��� ����� ������
	System->AcqBlockReady[1] = 0;                                  // ������ � ������

	// ����-��� �������� ������ ����� ������ � ��
	const DWORD acq_time_out = (DWORD)(ACQ_BLOCK_SIZE / (System->Conf_LTR11.ChRate * System->Conf_LTR11.LChQnt) + 1000);

	// ������ ����� ������
	err = LTR11_Start(&System->Conf_LTR11);
	if(err) {
		(void)LTR11_Stop(&System->Conf_LTR11);
		ExitThread(err);
		return 0;
	}

	while (System->RunFlag)
	{
	// ��������� ������ �� LTR11
	err = LTR_Recv(&System->Conf_LTR11.Channel, data_buf, NULL, ACQ_BLOCK_SIZE, acq_time_out);
	if (err > 0 && err == ACQ_BLOCK_SIZE)
	{
		int block_number;
		// ��� ������ ������� ��������� ���������� ������

		// ����� ���������� ������
		block_number = 0;
		while (block_number < ACQ_BLOCK_QNT && System->AcqBlockReady[block_number])
			block_number++;


		if (block_number < ACQ_BLOCK_QNT)
		{
			int data_size = ACQ_BLOCK_SIZE;
			// ���������� �������� � ������������ ������ � ������
			err = LTR11_ProcessData(&System->Conf_LTR11, data_buf, System->AcqBuf[block_number], &data_size, TRUE, TRUE);
			System->BlockNumberThread = block_number; // ����� ����� ������ ��� scope-������
			if(err) {
				System->RunFlag = 0;
			} else {
				if(System->ReplaceBuf == false) System->AcqBlockReady[block_number] = 1;             // true - ����� �������� � ������
			}
		}
	} else System->RunFlag = 0;

	}

	ExitThread((DWORD)err);

	return 0;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.1.1 ��.
void __fastcall WSystem::Util0Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;

	Init_LTR11_LTR34();

	//--------- ������� ���������� ������ ��� ---------
	Conf_LTR34.ChannelQnt = 1;		  			// ���������� ���� ����� - ����� ��������� �������
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(1,0);

	Conf_LTR34.FrequencyDivisor = 40; 		 	// �������� ������� �������������

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	ArrayToSend = new DWORD[1];
	VoltArray = new double[1];
	VoltArray[0] = 7.5;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1, 5000);
	if( size != 1 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 1;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 2;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (2 << 0); // �������� - 10�, ����� - 16-���������, ���������� ����� - 2 (��������� � ���� )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (3 << 0); // �������� - 10�, ����� - 16-���������, ���������� ����� - 3 (��������� � ���� )

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	// ��������� ���������
	AcqBlockReady[0] = 0;                                  // ����� ���������� ��� ����� ������
	AcqBlockReady[1] = 0;                                  // ������ � ������
	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
//		const char *str_err;
//		LTR34_Close(&Conf_LTR34);
//		LTR11_Close(&Conf_LTR11);
//		if( err == 1 )  str_err = ErrorText1;
//		else
//		if( err == 2 )  str_err = ErrorText2;
//		else
//		{
//			if( err < (-3000) ) str_err = LTR34_GetErrorString(err);
//			else
//			if( err < (-1000 ) ) str_err = LTR11_GetErrorString(err);
//			else str_err = ErrorText3;
//		}
//		delete ArrayToSend; ArrayToSend = 0;
//		delete VoltArray; VoltArray = 0;
//		throw Exception(  str_err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.1.2 ��.
double WSystem::Util1ResultSet[] = { 0.245, 0.376, 0.545, 0.790, 1.15, 1.65, 2.38, 3.45 };
double WSystem::DeltaErrorUtil1[] = { 0.05, 0.05, 0.06, 0.08, 0.12, 0.17, 0.24, 0.35 };
double WSystem::VoltArrayTable[9][4] = { {0,0,0,7.5}, {8,0,0,7.5}, {0,8,0,7.5}, {8,8,0,7.5},{0,0,8,7.5},{8,0,8,7.5},
{0,8,8,7.5}, {8,8,8,7.5}, {0,0,0,7.5}};
void __fastcall WSystem::Util1Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;

	Init_LTR11_LTR34();

	//--------- ������� ���������� ������ ��� ---------
	Conf_LTR34.ChannelQnt = 4;						// ���������� ��� ������, ���� ����� ������
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	//
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);

	Conf_LTR34.FrequencyDivisor = 40; 		 	// �������� ������� �������������

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);                               // !!!!!! ��� ���� ��������
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	ArrayToSend = new DWORD[4];
	VoltArray = new double[4];
	for( register i = 0; i < 4; i++ )
	VoltArray[i] = VoltArrayTable[0][i];

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 4, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 4, 5000);
	if( size != 4 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 1;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 1;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (4 << 0); // �������� - 10�, ����� - 16-���������, ���������� ����� - 4 (��������� � ���� )

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	// ��������� ���������
	AcqBlockReady[0] = 0;                                  // ����� ���������� ��� ����� ������
	AcqBlockReady[1] = 0;                                  // ������ � ������
	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
//		const char *str_err;
//		LTR34_Close(&Conf_LTR34);
//		LTR11_Close(&Conf_LTR11);
//		if( err == 1 )  str_err = ErrorText1;
//		else
//		if( err == 2 )  str_err = ErrorText2;
//		else
//		{
//			if( err < (-3000) ) str_err = LTR34_GetErrorString(err);
//			else
//			if( err < (-1000 ) ) str_err = LTR11_GetErrorString(err);
//			else str_err = ErrorText3;
//		}
//		delete ArrayToSend; ArrayToSend = 0;
//		delete VoltArray; VoltArray = 0;
//		throw Exception(  str_err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
#include "math.h"
int __fastcall WSystem::Util1Step2_8()throw(Exception)
{
	int err = 0;
	int size;
	int num_step_prev;

	num_step_prev = NumStep++;	   	// ���������� ��� �������� � ���������

	if( NumStep > 8 ) { RunFlag = 0; return RET_END; }

	// ������� ������� ���������� ��������� ��� - ����������
	Util1Result = AcqBuf[BlockNumberThread][512];
	if( fabs( Util1Result - Util1ResultSet[	num_step_prev] ) > DeltaErrorUtil1[	num_step_prev] )
	return RET_ERROR;

	if(fabs(Util1Result) < 0.01) 		   // !!!
		return RET_NO_U;

//	if( NumStep > 7 ) { RunFlag = 0; return RET_END; }

	// ���������� ������ VoltArray
	for( register i = 0; i < 4; i++ )
	VoltArray[i] = VoltArrayTable[NumStep][i];

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 4, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 4, 5000);
	if( size != 4 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }//throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	// ��������� ���������
	err = LTR34_DACStart(&Conf_LTR34);
//		if(err) throw Exception(  LTR34_GetErrorString(err) );
	Sleep(100);
	AcqBlockReady[0] = 0;
	AcqBlockReady[1] = 0;

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
//		const char *str_err;
//		RunFlag = 0;
//		LTR34_Close(&Conf_LTR34);
//		LTR11_Close(&Conf_LTR11);
//		if( err == 1 )  str_err = ErrorText1;
//		else
//		if( err == 2 )  str_err = ErrorText2;
//		else str_err = LTR34_GetErrorString(err);
//
//		MessageDlg( str_err, mtError, TMsgDlgButtons() << mbOK, 0);
////		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
//		delete ArrayToSend; ArrayToSend = 0;
//		delete VoltArray; VoltArray = 0;
//		throw Exception(  str_err );
////		return RET_ERROR;
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
int __fastcall WSystem::UtilStop( bool power_off )
{
	int size;
	double volt_array[] = { 0,0,0,0,7.5,0,0,0 };

	RunFlag = 0;
	int err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	Conf_LTR34.ChannelQnt = 8;
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(8,0);
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(1,0);
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(2,0);
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(3,0);
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(4,0);

	// �������� ��������� �����
	Conf_LTR34.RingMode = false; 				// ������� ��������� �����
	Conf_LTR34.FrequencyDivisor = 0;			// �������� �� ������������ �������

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	if( power_off == true ) volt_array[4] = 0;
	delete ArrayToSend;
	ArrayToSend = new DWORD[8];
	err = LTR34_ProcessData(&Conf_LTR34, volt_array, ArrayToSend, 8, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 8, 5000);
	if( size != 8 )  { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ���������
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;
		// ���� ��������� ��������� ��������
//		do
//		{
//			DWORD data[1],tmark[1];
//			err = LTR34_Recv( &Conf_LTR34, data, tmark, 1, 100 );
//			if( err < 0 ) throw Exception(  LTR34_GetErrorString(err) );
//		}
//		while( Conf_LTR34.BufferEmpty == false );
	Sleep(300);
	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	LTR34_Close(&Conf_LTR34);
	LTR11_Close(&Conf_LTR11);
	delete ArrayToSend; ArrayToSend = 0;
	delete VoltArray; VoltArray = 0;

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
//		LTR34_Close(&Conf_LTR34);
//		LTR11_Close(&Conf_LTR11);
//		RunFlag = 0;
//		const char *str_err;
//		if( err == 1 )  str_err = ErrorText1;
//		else
//		if( err == 2 )  str_err = ErrorText2;
//		else str_err = LTR34_GetErrorString(err);
//
//		MessageDlg( str_err, mtError, TMsgDlgButtons() << mbOK, 0);
////		MessageDlg( e.Message.c_str(), mtError, TMsgDlgButtons() << mbOK, 0);
//		delete ArrayToSend; ArrayToSend = 0;
//		delete VoltArray; VoltArray = 0;
//		return RET_ERROR;
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.1.3 ��.
// ��������� ��������� ������������� ��������� �� ���� ������� ( ������� ������,
// ��� ��� ���. ���. ������� ����� ���� 4 ) �������� 15,625�0,25��, 31,25�0,5��,
// 62,5�1�� �� ������� OUT5-1:1,OUT6-1:1,OUT7-1:1( �������� ���1,���2,���3 ).
void __fastcall WSystem::Util2Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;
	VoltArray = 0;

	// ���������� ������� ������
	for(int i=0; i<ACQ_BLOCK_QNT; i++) {
		for(int j=0; j<ACQ_BLOCK_SIZE; j++) {
			AcqBuf[i][j] = 0;
        }
    }

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 4;						// ���������� ��� ������, ���� ����� ������
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	//
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);

	Conf_LTR34.FrequencyDivisor = 32; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  15625 ��.
//	Conf_LTR34.UseClb = true; 				// ���������� ��������� ������������� ������������
//	Conf_LTR34.AcknowledgeType = false; 		// ��� ������������� - �������������
//	Conf_LTR34.ExternalStart = false; 		// ������� ����� ��������
//	Conf_LTR34.RingMode = true; 				// ������� ����� �������������

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[4000];  	// 4 ������ ( ����� 3, �� �� ���. ������������ ����� �� ����� 4 )
	VoltArray = new double[4000];		// �� 1000 �������� ��� ������� - ����� 4000

	// ��������� �������� ������� ��� ������� - ����� ���������� �������� � ��������;
	// 1 ����� - 15,625 ��, 2 ����� - 31,25 ��, 3 ����� - 62,5 ��
	for (int i = 0,j = 0; i < 1000; i++) {
	VoltArray[j++] = ( (((  i/125 ) + 1 )%2) == 0 )? 8 : 0;
	VoltArray[j++] = ( (((  i/250 ) + 1 )%2) == 0 )? 8 : 0;
	VoltArray[j++] = ( (((  i/500 ) + 1 )%2) == 0 )? 8 : 0;
	VoltArray[j++] = 7.5; //( (((  i/125 ) + 1 )%2) == 0 )? 8 : 0;
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 4000, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 4000, 5000);
	if( size != 4000 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
//	Conf_LTR11.StartADCMode = LTR11_STARTADCMODE_INT; 		// ����� ������ ����� ������ - ����������
//	Conf_LTR11.InpMode = LTR11_INPMODE_INT;           		// ����� ������������� ��� - ����������
//	Conf_LTR11.ADCMode = LTR11_ADCMODE_ACQ;           		// ����� ����� ������

	Conf_LTR11.ADCRate.prescaler = 64;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 1;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (4 << 0); // �������� - 10�, ����� - 16-���������, ���������� ����� - 4 ( ��������� � ���� )

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	// ��������� ���������
//	err = LTR34_DACStart(&Conf_LTR34);
//		if(err) throw Exception(  LTR34_GetErrorString(err) );
//	}
	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
//		const char *str_err;
//		LTR34_Close(&Conf_LTR34);
//		LTR11_Close(&Conf_LTR11);
//		if( err == 1 )  str_err = ErrorText1;
//		else
//		if( err == 2 )  str_err = ErrorText2;
//		else
//		{
//			if( err < (-3000) ) str_err = LTR34_GetErrorString(err);
//			else
//			if( err < (-1000 ) ) str_err = LTR11_GetErrorString(err);
//			else str_err = ErrorText3;
//		}
//		delete ArrayToSend; ArrayToSend = 0;
//		delete VoltArray; VoltArray = 0;
//		throw Exception(  str_err );
	}
//	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.1.4 ��.
// ��������� ��������� ������������� ��������� 125 �� �� ������ OUT8-1:1 (������� ���4 ),
// �� ������� OUT5-1:1,OUT6-1:1,OUT7-1:1 (�������� ���1,���2,���3) - 8�.
// �� ������ OUT1-1:1 - 8� - ������� ��������� �������
void __fastcall WSystem::Util3Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// ���������� ���� ������� ( ������� 8 )
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ���1 - 8�
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ���2 - 8�
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ���3 - 8�
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(8,0);   // ���4 - 125 ��
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(1,0);   // ��������� ������� - 7.5�
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(2,0);	// ������ �����
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(3,0);   // ������ �����
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(4,0);   // ������ �����

	Conf_LTR34.FrequencyDivisor = 14; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 14 )*8) = 5000 ��.
//	Conf_LTR34.UseClb = true; 				// ���������� ��������� ������������� ������������
//	Conf_LTR34.AcknowledgeType = false; 	// ��� ������������� - �������������
//	Conf_LTR34.ExternalStart = false; 		// ������� ����� ��������
//	Conf_LTR34.RingMode = true; 			// ������� ����� �������������

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[320];  		// 8 ������ ( ����� 5, �� �� ���. ������������ ����� �� ����� 8 )
	VoltArray = new double[320];		// �� 40 �������� ��� ������� - ����� 320

	// ��������� �������� ������� ��� ������� - ����� ���������� �������� � ��������;
	// 4 ���������� ����� ( ���4 - LTR34\OUT8-1:1 )- 125 ��
	for (int i = 0,j = 0; i < 40; i++) {
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = ( (((  i/20 ) + 1 )%2) == 0 )? 8 : 0;
	VoltArray[j++] = 7.5;
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 320, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 320, 5000);
	if( size != 320 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
//	Conf_LTR11.StartADCMode = LTR11_STARTADCMODE_INT; 		// ����� ������ ����� ������ - ����������
//	Conf_LTR11.InpMode = LTR11_INPMODE_INT;           		// ����� ������������� ��� - ����������
//	Conf_LTR11.ADCMode = LTR11_ADCMODE_ACQ;           		// ����� ����� ������

	Conf_LTR11.ADCRate.prescaler = 64;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 1;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (5 << 0); // �������� - 10�, ����� - 16-���������, ���������� ����� - 4 ( ��������� � ���� )

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	// ��������� ���������
//	err = LTR34_DACStart(&Conf_LTR34);
//		if(err) throw Exception(  LTR34_GetErrorString(err) );
//	}
	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
//		const char *str_err;
//		LTR34_Close(&Conf_LTR34);
//		LTR11_Close(&Conf_LTR11);
//		if( err == 1 )  str_err = ErrorText1;
//		else
//		if( err == 2 )  str_err = ErrorText2;
//		else
//		{
//			if( err < (-3000) ) str_err = LTR34_GetErrorString(err);
//			else
//			if( err < (-1000 ) ) str_err = LTR11_GetErrorString(err);
//			else str_err = ErrorText3;
//		}
//		delete ArrayToSend; ArrayToSend = 0;
//		delete VoltArray; VoltArray = 0;
//		throw Exception(  str_err );
	}
//	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.1.6 ��.
// ��������� ������� ���������� �.5.1.4 �� ( Util3Step1() ) + ��������� ��������������
// ������ �������� 30�� �� ������ OUT2-1:10 (������� ��1).
// ��������� ��������� ������������� ��������� �� ������ OUT8-1:1 (������� ���4 ),
// �� ������� OUT5-1:1,OUT6-1:1,OUT7-1:1 (�������� ���1,���2,���3) - 8�.
// �� ������ OUT1-1:1 - 7.5� - ������� ��������� �������
// �� ������ OUT2-1:10 - ����� 30��.
void __fastcall WSystem::Util4Step1()throw(Exception)
{
	int err = 0;
	int size;
	double t = 0;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// ���������� 6 ������� (������� 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ���1 - 8�
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ���2 - 8�
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ���3 - 8�
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(8,0);   // ���4 - 125 ��
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(1,0);   // ��������� ������� - 7.5�
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(2,1);   // ��1 - 30 ��
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(3,0);	// ������ �����
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(4,0);   // ������ �����

	Conf_LTR34.FrequencyDivisor = 14; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 14 )*8) = 5000 ��.
	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[1336];  	// 8 ������ ( ����� 5, �� �� ���. ������������ ����� �� ����� 8 )
	VoltArray = new double[1336];		// �� 167 �������� ��� ������� - ����� 1336

	// ������� �������� ��� ������������ ������� 30 �� �� ������ OUT2-1:10 ( ������� ��1 )
	// sin( omega*t );
	// omega = 2*PI*30 = 188.4955592 ���/���
	// ��������� �������� ������� ��� ������� - ����� ���������� �������� � ��������;
	// 4 ���������� ����� ( ���4 - LTR34\OUT8-1:1 )- 125 ��
	for (int i = 0,j = 0; i < 167; i++) {
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = ( (((  i/20 ) + 1 )%2) == 0 )? 8 : 0;
	VoltArray[j++] = 7.5;
	VoltArray[j++] = 0.424*sin(188.4955592*t);
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	t += 0.0002;			// ������ 0.0002 ��� - 5000 ��
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1336, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1336, 5000);
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 64;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 1;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (10 << 0); // �������� - 10�, ����� - 16-���������, ���������� ����� - 10 ( ��������� � ���� )

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
//		const char *str_err;
//		LTR34_Close(&Conf_LTR34);
//		LTR11_Close(&Conf_LTR11);
//		if( err == 1 )  str_err = ErrorText1;
//		else
//		if( err == 2 )  str_err = ErrorText2;
//		else
//		{
//			if( err < (-3000) ) str_err = LTR34_GetErrorString(err);
//			else
//			if( err < (-1000 ) ) str_err = LTR11_GetErrorString(err);
//			else str_err = ErrorText3;
//		}
//		delete ArrayToSend; ArrayToSend = 0;
//		delete VoltArray; VoltArray = 0;
//		throw Exception(  str_err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.1.7 ��.
// ��������� ������� ���������� �.5.1.4 �� ( Util3Step1() ) + ��������� ��������������
// ������ �������� 30�� �� ������ OUT2-1:10 (������� ��1).
// ��������� ��������� ������������� ��������� �� ������ OUT8-1:1 (������� ���4 ),
// �� ������� OUT5-1:1,OUT6-1:1,OUT7-1:1 (�������� ���1,���2,���3) - 8�.
// �� ������ OUT1-1:1 - 7.5� - ������� ��������� �������
// �� ������ OUT2-1:10 - ����� 30��. ��������� ������ �������� ���������� �
//                       �������� Util5Step2_4();
//double WSystem::AmpUtil5[] = { 0.045, 0.380, 0.630, 0.880 };
double WSystem::AmpUtil5[] = { 0.06363, 0.53732, 0.89082, 1.0 /*(1.24432*/ };
void __fastcall WSystem::Util5Step1()throw(Exception)
{
	int err = 0;
	int size;
	double t = 0;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// ���������� 6 ������� (������� 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ���1 - 8�
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ���2 - 8�
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ���3 - 8�
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(8,0);   // ���4 - 125 ��
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(1,0);   // ��������� ������� - 7.5�
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(2,1);   // ��1 - 30 ��
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(3,0);	// ������ �����
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(4,0);   // ������ �����

	Conf_LTR34.FrequencyDivisor = 14; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 14 )*8) = 5000 ��.

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;							// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[1336];  		// 8 ������ ( ����� 5, �� �� ���. ������������ ����� �� ����� 8 )
	VoltArray = new double[1336];			// �� 167 �������� ��� ������� - ����� 1336

	// ������� �������� ��� ������������ ������� 30 �� �� ������ OUT2-1:10 ( ������� ��1 )
	// sin( omega*t );
	// omega = 2*PI*30 = 188.4955592 ���/���
	// ��������� �������� ������� ��� ������� - ����� ���������� �������� � ��������;
	// 4 ���������� ����� ( ���4 - LTR34\OUT8-1:1 )- 125 ��
	for (int i = 0,j = 0; i < 167; i++) {
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = ( (((  i/20 ) + 1 )%2) == 0 )? 8 : 0;
	VoltArray[j++] = 7.5;
	VoltArray[j++] = AmpUtil5[0]*sin(188.4955592*t);
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	t += 0.0002;			// ������ 0.0002 ��� - 5000 ��
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1336, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1336, 5000);
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 64;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 4;                            		// ���������� ���������� ������� - 4
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (10 << 0); // �������� - 10�, ����� - 16-���������, ���������� ����� - 10 ( ��������� � ���� )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (6 << 0); 	// �������� - 10�, ����� - 16-���������, ���������� ����� - 6 ( ��������� � ���� )
	Conf_LTR11.LChTbl[2] = (0 << 6) | (0 << 4) | (7 << 0); 	// �������� - 10�, ����� - 16-���������, ���������� ����� - 7 ( ��������� � ���� )
	Conf_LTR11.LChTbl[3] = (0 << 6) | (0 << 4) | (8 << 0); 	// �������� - 10�, ����� - 16-���������, ���������� ����� - 8 ( ��������� � ���� )


	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
//		const char *str_err;
//		LTR34_Close(&Conf_LTR34);
//		LTR11_Close(&Conf_LTR11);
//		if( err == 1 )  str_err = ErrorText1;
//		else
//		if( err == 2 )  str_err = ErrorText2;
//		else
//		{
//			if( err < (-3000) ) str_err = LTR34_GetErrorString(err);
//			else
//			if( err < (-1000 ) ) str_err = LTR11_GetErrorString(err);
//			else str_err = ErrorText3;
//		}
//		delete ArrayToSend; ArrayToSend = 0;
//		delete VoltArray; VoltArray = 0;
//		throw Exception(  str_err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
double WSystem::SwitchKomparator[4][3] = {{10,10,10}, {1,10,10}, {1,1,10}, {1,1,1}};
int __fastcall WSystem::Util5Step2_4()throw(Exception)
{
	int err = 0;
	int size;
	double t = 0;
	int num_step_prev;
 //	double k;
	double resultValue[3];

	num_step_prev = NumStep++;	   	// ���������� ��� �������� � ���������

	// ������� ������� ���������� ��������� ��� - ����������
	for(int i=0;i<3;i++) resultValue[i] = 0;                            // ���������
	for(int i=0, j=513; i<10; i++) {
		resultValue[0] = resultValue[0] + AcqBuf[BlockNumberThread][j++];
		resultValue[1] = resultValue[1] + AcqBuf[BlockNumberThread][j++];
		resultValue[2] = resultValue[2] + AcqBuf[BlockNumberThread][j++];
		j++;
	}
	for(int i=0;i<3;i++) Util5Result[i] = resultValue[i] / 10;

	// ��������
	for(int i=0; i<3; i++) {
		if(SwitchKomparator[num_step_prev][i] < Util5Result[i])
			return RET_ERROR;
    }

	if( NumStep >= 4 ) { RunFlag = 0; return RET_END; }
//	if( fabs( Util1Result - Util1ResultSet[	num_step_prev] ) > DeltaErrorUtil1[	num_step_prev] )
//	return RET_ERROR;

	// ������������ ������ VolArray[] - ���������: �������� ��������� ���������
	// ������ Conf_LTR34.LChTbl[5] ( ��1 - 30 �� ).
//	k = AmpUtil5[NumStep]/AmpUtil5[num_step_prev];
//	for (int i = 0,j = 5; i < 167; i++,j+=8)
//	{
//		VoltArray[j] *= k;
//	}
	for (int i = 0,j = 5; i < 167; i++,j += 8 )
	{
//		VoltArray[j++] = 8;
//		VoltArray[j++] = 8;
//		VoltArray[j++] = 8;
//		VoltArray[j++] = ( (((  i/20 ) + 1 )%2) == 0 )? 8 : 0;
//		VoltArray[j++] = 7.5;
		VoltArray[j++] = AmpUtil5[NumStep]*sin(188.4955592*t);
//		VoltArray[j++] = 0;
//		VoltArray[j++] = 0;
		t += 0.0002;			// ������ 0.0002 ��� - 5000 ��
	}

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1336, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1336, 5000);
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ���������
	err = LTR34_DACStart(&Conf_LTR34);
	Sleep(100);
	AcqBlockReady[0] = 0;
	AcqBlockReady[1] = 0;

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
//		const char *str_err;
//		RunFlag = 0;
//		LTR34_Close(&Conf_LTR34);
//		LTR11_Close(&Conf_LTR11);
//		if( err == 1 )  str_err = ErrorText1;
//		else
//		if( err == 2 )  str_err = ErrorText2;
//		else str_err = LTR34_GetErrorString(err);
//		MessageDlg( str_err, mtError, TMsgDlgButtons() << mbOK, 0);
//		delete ArrayToSend; ArrayToSend = 0;
//		delete VoltArray; VoltArray = 0;
//		throw Exception(  str_err );
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.1.8 ��.
// ��������� ������� ���������� �.5.1.4 �� ( Util3Step1() ) + ��������� ��������������
// ������ �������� 30�� �� ������ OUT2-1:10 (������� ��1).
// ��������� ��������� ������������� ��������� �� ������ OUT8-1:1 (������� ���4 ),
// �� ������� OUT5-1:1,OUT6-1:1,OUT7-1:1 (�������� ���1,���2,���3) - 8�.
// �� ������ OUT1-1:1 - 7.5� - ������� ��������� �������
// �� ������ OUT2-1:10 - ����� 30��. ������� ������ �������� ���������� �
//                       �������� Util6Step2_6();
// ���� ����� - 0.2 ���� ( 5000 �� )
// ������������� �������� 125 �� -> 40 ������� -> ������ 8 ����
// ����� 30 �� -> 167 ������� -> 1 ������
// ����� 50 �� -> 200 ������� -> 2 �������
// ����� 62.5 �� -> 80 ������� -> 1 ������
// ����� 83.3(3) �� -> 60 ������� -> 1 ������
// ����� 100 �� -> 50 ������� -> 1 ������
// ����� 120 �� -> 42 ������ -> 1 ������
int WSystem::KolSemplKanalUtil6[] = { 167, 200, 80,   60,   50,  42    }; 	// �����/�����
float WSystem::FreqUtil6[]  = 		{ 30,  50,  62.5, 83.3, 100, 120   };	// ��
void __fastcall WSystem::Util6Step1()throw(Exception)
{
	int err = 0;
	int size;
	double t = 0;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// ���������� 6 ������� (������� 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ���1 - 8�
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ���2 - 8�
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ���3 - 8�
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(8,0);   // ���4 - 125 ��
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(1,0);   // ��������� ������� - 7.5�
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(2,1);   // ��1 - 30 ��
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(3,0);	// ������ �����
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(4,0);   // ������ �����

	Conf_LTR34.FrequencyDivisor = 14; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 14 )*8) = 5000 ��.

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;							// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[1600];  		// 8 ������ ( ����� 5, �� �� ���. ������������ ����� �� ����� 8 )
	VoltArray = new double[1600];			// �� 200 �������� ( ����.) ��� ������� - ����� 1600

	// ������� �������� ��� ������������ ������� 30 �� �� ������ OUT2-1:10 ( ������� ��1 )
	// sin( omega*t );
	// omega = 2*PI*30 = 188.4955592 ���/���
	// ��������� �������� ������� ��� ������� - ����� ���������� �������� � ��������;
	// 4 ���������� ����� ( ���4 - LTR34\OUT8-1:1 )- 125 ��
	for (int i = 0,j = 0; i < 200; i++) {
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = ( (((  i/20 ) + 1 )%2) == 0 )? 8 : 0;
	VoltArray[j++] = 7.5;
	VoltArray[j++] = 1.0*sin(188.4955592*t);
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	t += 0.0002;			// ������ 0.0002 ��� - 5000 ��
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1336, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1336, 5000);
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 64;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 3;                            		// ���������� ���������� ������� - 3
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (10 << 0); // �������� - 10�, ����� - 16-���������, ���������� ����� - 10 ( ��������� � ���� )
//	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (6 << 0); 	// �������� - 10�, ����� - 16-���������, ���������� ����� - 6 ( ��������� � ���� )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (1 << 0); 	// �������� - 10�, ����� - 16-���������, ���������� ����� - 1 ( ��������� � ���� )
	Conf_LTR11.LChTbl[2] = (0 << 6) | (0 << 4) | (0 << 0); 	// �������� - 10�, ����� - 16-���������, ���������� ����� - 0 ( ��������� � ���� )


	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if( err != NULL )
	{

		ErrorRelease( err );
//		const char *str_err;
//		LTR34_Close(&Conf_LTR34);
//		LTR11_Close(&Conf_LTR11);
//		if( err == 1 )  str_err = ErrorText1;
//		else
//		if( err == 2 )  str_err = ErrorText2;
//		else
//		{
//			if( err < (-3000) ) str_err = LTR34_GetErrorString(err);
//			else
//			if( err < (-1000 ) ) str_err = LTR11_GetErrorString(err);
//			else str_err = ErrorText3;
//		}
//		delete ArrayToSend; ArrayToSend = 0;
//		delete VoltArray; VoltArray = 0;
//		throw Exception(  str_err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
int __fastcall WSystem::Util6Step2_6()throw(Exception)
{
	int err = 0;
	int size;
	int num_step_prev;
	double t = 0;
	int len;

	num_step_prev = NumStep++;	   	// ���������� ��� �������� � ���������

	if( NumStep >= 6 ) { RunFlag = 0; return RET_END; }

//    // ������� ������� ���������� ��������� ��� - ����������
//	Util1Result = AcqBuf[BlockNumberThread][512];
//	if( fabs( Util1Result - Util1ResultSet[	num_step_prev] ) > DeltaErrorUtil1[	num_step_prev] )
//	return RET_ERROR;

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// ������������ ������ VolArray[] - ���������: �������� ������� ���������
	// ������ Conf_LTR34.LChTbl[5] ( ��1 ).
	for( int i = 0,j = 5; i < KolSemplKanalUtil6[NumStep]; i++, j+=8 )
	{
		VoltArray[j] = 1.0*sin(2*M_PI*FreqUtil6[NumStep]*t);
		t += 0.0002;			// ������ 0.0002 ��� - 5000 ��
	}

	len =  KolSemplKanalUtil6[NumStep] * 8;

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, len, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend,  len, 5000);
	if( size !=  len ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ���������
	err = LTR34_DACStart(&Conf_LTR34);
	Sleep(100);
	AcqBlockReady[0] = 0;
	AcqBlockReady[1] = 0;

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
//		const char *str_err;
//		RunFlag = 0;
//		LTR34_Close(&Conf_LTR34);
//		LTR11_Close(&Conf_LTR11);
//		if( err == 1 )  str_err = ErrorText1;
//		else
//		if( err == 2 )  str_err = ErrorText2;
//		else str_err = LTR34_GetErrorString(err);
//		MessageDlg( str_err, mtError, TMsgDlgButtons() << mbOK, 0);
//		delete ArrayToSend; ArrayToSend = 0;
//		delete VoltArray; VoltArray = 0;
//		throw Exception(  str_err );
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
void __fastcall WSystem::ErrorRelease( int err )throw(Exception)
{
	const char *str_err;
	RunFlag = 0;
	LTR34_Close(&Conf_LTR34);
	LTR11_Close(&Conf_LTR11);
	if( err == 1 )  str_err = ErrorText1;
	else
	if( err == 2 )  str_err = ErrorText2;
	else
	{
		if( err < (-3000) ) str_err = LTR34_GetErrorString(err);
		else
		if( err < (-1000 ) ) str_err = LTR11_GetErrorString(err);
		else str_err = ErrorText3;
	}
	delete ArrayToSend; ArrayToSend = 0;
	delete VoltArray; VoltArray = 0;
	throw Exception(  str_err );
}

//------------------------/ MEMBER FUNCTION /--------------------------------
//���� �0� � ������������ ����� ����� � ������� ������ � ������.
//���� �������� ��������� - ����� �� ������
int __fastcall WSystem::IsOpenedLTR34()
{
	return LTR34_IsOpened(&Conf_LTR34);
}
//------------------------/ MEMBER FUNCTION /--------------------------------
/*int __fastcall WSystem::Init_LTR11_LTR43()
{
	int err;

	//������������� ������������ �������
	err = LTR43_Init(&Conf_LTR43);
	if(err) {
		MessageDlg((char *) LTR43_GetErrorString(err), mtError, TMsgDlgButtons() << mbOK, 0);
		return RET_ERROR;
	}

	//�������� ������� ����� � ��������
	// ����� ����� � ������ LTR43 - 4
	err = LTR43_Open(&Conf_LTR43, SADDR_DEFAULT, SPORT_DEFAULT, "", 4);
	if(err) {
		MessageDlg((char *) LTR43_GetErrorString(err), mtError, TMsgDlgButtons() << mbOK, 0);
		return RET_ERROR;
	}

	return RET_OK;
} */

//------------------------/ MEMBER FUNCTION /--------------------------------
/*
int __fastcall WSystem::Config_LTR11_LTR34()
{
	int err;
	int size;

	//---------������� ���������� ������ ���-------
	Conf_LTR34.ChannelQnt = 4;						 // ���������� ��� ������, ���� ����� ������
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(1,0);
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(2,0);
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(3,0);
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(4,0);

	Conf_LTR34.FrequencyDivisor = 32; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  15625 ��.
	Conf_LTR34.UseClb = true; 				// ���������� ��������� ������������� ������������
	Conf_LTR34.AcknowledgeType = false; 	// ��� ������������� - �������������
	Conf_LTR34.ExternalStart = false; 		// ������� ����� ��������
	Conf_LTR34.RingMode = true; 			// ������� ����� �������������

	//---------������� ���������� ������ ���-------
	Conf_LTR11.StartADCMode = LTR11_STARTADCMODE_INT; 		// ����� ������ ����� ������ - ����������
	Conf_LTR11.InpMode = LTR11_INPMODE_INT;           		// ����� ������������� ��� - ����������
	Conf_LTR11.ADCMode = LTR11_ADCMODE_ACQ;           		// ����� ����� ������

	Conf_LTR11.ADCRate.prescaler = 1;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 1;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (11 << 0); // �������� - 10�, ����� - 16-���������, ���������� ����� - 12

	// ����� ������������� ����������� ��������� ����� ������ ���
	err = LTR34_Reset(&Conf_LTR34);
	if(err) {
		MessageDlg((char *) LTR34_GetErrorString(err), mtError, TMsgDlgButtons() << mbOK, 0);
		LTR34_Close(&Conf_LTR34);
		return RET_ERROR;
	}

	// ��������� ������������ ������ ���
	err = LTR34_Config(&Conf_LTR34);
	if(err) {
		MessageDlg((char *) LTR34_GetErrorString(err), mtError, TMsgDlgButtons() << mbOK, 0);
		LTR34_Close(&Conf_LTR34);
		return RET_ERROR;
	}

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) {
		MessageDlg((char *) LTR11_GetErrorString(err), mtError, TMsgDlgButtons() << mbOK, 0);
		LTR11_Close(&Conf_LTR11);
		return RET_ERROR;
	}

	//------------���������� ������ ��� ���-------------
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[4000];  	// 4 ������ ( ����� 3, �� �� ���. ������������ ����� �� ����� 4 )
	VoltArray = new double[4000];		// �� 1000 �������� ��� ������� - ����� 4000

	// ��������� �������� ������� ��� ������� - ����� ���������� �������� � ��������;
	// 1 ����� - 15,625 ��, 2 ����� - 31,25 ��, 3 ����� - 62,5 ��
	for (int i = 0,j = 0; i < 1000; i++) {
		VoltArray[j++] = ( (((  i/500 ) + 1 )%2) == 0 )? 8 : 0;
		VoltArray[j++] = ( (((  i/250 ) + 1 )%2) == 0 )? 8 : 0;
		VoltArray[j++] = ( (((  i/125 ) + 1 )%2) == 0 )? 8 : 0;
		VoltArray[j++] = ( (((  i/125 ) + 1 )%2) == 0 )? 8 : 0;
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 4000, true);
	if(err) {
		MessageDlg((char *) LTR34_GetErrorString(err), mtError, TMsgDlgButtons() << mbOK, 0);
		LTR34_Close(&Conf_LTR34);
		delete ArrayToSend;
		delete VoltArray;
		return RET_ERROR;
	}

	// ���������� ������ � ������ ���
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 4000, 5000);
	if( size != 4000 ) {
		err = ((size < 0 )? 1 : 2);
	}
	if(err) {
		MessageDlg((char *) LTR34_GetErrorString(err), mtError, TMsgDlgButtons() << mbOK, 0);
		LTR34_Close(&Conf_LTR34);
		delete ArrayToSend;
		delete VoltArray;
		return RET_ERROR;
	}

	return RET_OK;
}*/
//------------------------------/ END /--------------------------------------



