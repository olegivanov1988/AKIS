//---------------------------------------------------------------------------
#include <vcl.h>

#pragma hdrstop

#include "WIk.h"
#include "UnRaport.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
static const char ErrorText1[] = "������ ��� �������� ������ ������!";
static const char ErrorText2[] = "������! ���������� ������������ ������� �� ����� ���������!";
static const char ErrorText3[] =  "����������� ������";
// 6 � 18 ���� - ����� �7 � �19 �����������
float WIk::TimeMinUtil5[] = { -0.01, 1.9, 3.8, 1.42, 2.56, 4.95, 0, 2.18, 4.28, -0.01, 7.0, 5.44, 1.43, 6.95, 4.56,
0.19, 6.95, 6.18, 0, 2.28, 4.66, 0.67, 3.04, 6.65 };
float WIk::TimeMaxUtil5[] = {  0.01, 2.1, 4.2, 1.58, 2.84, 5.45, 0, 2.42, 4.72,  0.01, 7.6, 5.96, 1.57, 7.65, 5.04,
0.21, 7.65, 6.82, 0, 2.52, 5.14, 0.73, 3.36, 7.35 };
float WIk::TimeMinUtil6[] = { 0.19, 2.28, 4.18, 1.8, 2.95, 5.32, 0, 2.57, 4.65, 0.28, 7.3, 5.8,
1.8, 7.3, 4.94, 0.57, 7.3, 6.55, 0, 2.66, 5.04, 1.04, 3.42, 7.03 };
float WIk::TimeMaxUtil6[] = { 0.21, 2.52, 4.62, 2.0, 3.25, 5.88, 0, 2.83, 5.15, 0.32, 8.1, 6.4,
2.0, 8.1, 5.46, 0.63, 8.1, 7.25, 0, 2.94, 5.56, 1.16, 3.78, 7.77 };

//--------------------------/ CONSTRUCTOR /----------------------------------
WIk::WIk():ArrayToSend(0),VoltArray(0)
{
	BlockNumberThread = 100; // ������������� ���������� �� ������ ���� ����� ��� ����� ������ �������
	ReplaceBuf = false;      // ������������� ���������� ��������� ������ �������
	ReplaceBuf2 = false;     // ������������� ���������� ��������� ������ �������
	RunFlag = 0;
	RunFlag2 = 0;

	zRaport = new ZRaport;


	LTR34_Init(&Conf_LTR34);
	LTR11_Init(&Conf_LTR11);
	LTR27_Init(&Conf_LTR27);
}
//--------------------------/ DESTRUCTOR /-----------------------------------
WIk::~WIk()
{
}
//------------------------/ MEMBER FUNCTION /--------------------------------
void __fastcall WIk::Init_LTR11_LTR34()throw(Exception)
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

void __fastcall WIk::Init_LTR27()throw(Exception)
{
	int res;

	// �������������� ���� ��������� ���������� �� ���������
	res = LTR27_Init(&Conf_LTR27);
	if( res == LTR_OK )
	{
		// ������������� ���������� � ������� ����������� � ������ ����� ������.
		// ��� �������� ������, �������� ����� ltr-������� � ��������� ������
		// ������ ���������� �������� �� ���������
		res = LTR27_Open(&Conf_LTR27, SADDR_DEFAULT, SPORT_DEFAULT, "", CC_MODULE5);
		if( res == LTR_OK )
		{	// �������� ������������ ������
			res = LTR27_GetConfig(&Conf_LTR27);
			if( res == LTR_OK )
			{
        		// ��������� �������� ������ � ���������
				res = LTR27_GetDescription( &Conf_LTR27, LTR27_ALL_DESCRIPTION );
				if( res == LTR_OK )
				{
					// �������� ������� ������������� 100��
					Conf_LTR27.FrequencyDivisor = 9;
					// �������� ������������� ������������
					for( int i = 0; i < LTR27_MEZZANINE_NUMBER; i++ )
					for( int j = 0; j < 4; j++ )
					Conf_LTR27.Mezzanine[i].CalibrCoeff[j] = Conf_LTR27.ModuleInfo.Mezzanine[i].Calibration[j];
					// �������� ��������� ����� ������ � ������
					res = LTR27_SetConfig( &Conf_LTR27 );
					//if(res==LTR_OK) {
                }
			}
		}
	}

    // ������� ��������� �� ������
	if(res != LTR_OK )
	throw Exception(  LTR_GetErrorString( res ) );
}
//------------------------/ MEMBER FUNCTION /--------------------------------
void __fastcall WIk::StartGen_LTR11_LTR34()throw(Exception)
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
	hnd_acq_thread2 = hnd_acq_thread;
	(void)GetExitCodeThread(hnd_acq_thread, &thread_status);

	if(err)
	{
		ErrorRelease( err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
DWORD WINAPI WIk::AcquireThread (WIk *System)
{
	int err;
	DWORD data_buf[ACQ_BLOCK_SIZE];
//	DWORD data_buf2[ACQ_BLOCK_SIZE_2];

	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {                       // ����� ��������� ������ ��� ����� ������
		System->AcqBlockReady[i] = 0;                              // ������ � ������
	}

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
//	switch(System->AcqBlockSize) {
//		case ACQ_BLOCK_SIZE:{   err = LTR_Recv(&System->Conf_LTR11.Channel, data_buf, NULL, ACQ_BLOCK_SIZE, acq_time_out);
//								break;}
//		case ACQ_BLOCK_SIZE_2:{ err = LTR_Recv(&System->Conf_LTR11.Channel, data_buf2, NULL, ACQ_BLOCK_SIZE_2, acq_time_out);
//		int a = 0;
//								break;}
//	}
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
//			switch(System->AcqBlockSize) {
//				case ACQ_BLOCK_SIZE:{   int data_size = ACQ_BLOCK_SIZE;
//										err = LTR11_ProcessData(&System->Conf_LTR11, data_buf, System->AcqBuf[block_number], &data_size, TRUE, TRUE);
//										break;}
//				case ACQ_BLOCK_SIZE_2:{ int data_size = ACQ_BLOCK_SIZE_2;
//										err = LTR11_ProcessData(&System->Conf_LTR11, data_buf2, System->AcqBuf[block_number], &data_size, TRUE, TRUE);
//										break;}
//			}
			err = LTR11_ProcessData(&System->Conf_LTR11, data_buf, System->AcqBuf[block_number], &data_size, TRUE, TRUE);
			System->BlockNumberThread = block_number; // ����� ����� ������ ��� scope-������
			if(err) {
				System->RunFlag = 0;
			} else {
				if(System->ReplaceBuf == false) System->AcqBlockReady[block_number] = 1;             // true - ����� �������� � ������
			}
		}
	} else
	 System->RunFlag = 0;

	}

	LTR11_Stop(&System->Conf_LTR11);
	ExitThread((DWORD)err);

	return 0;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
 DWORD WINAPI WIk::AcquireThread2 (WIk *System)
{
	int res;
	int size;
	DWORD data_buf[ACQ2_BLOCK_SIZE];
//	double data_fis[ACQ2_BLOCK_SIZE];	// ������ ���������� ��������

	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {       // ����� ��������� ������ ��� ����� ������
		System->Acq2BlockReady[i] = 0;              // ������ � ������
	}

	// ����-��� �������� ������ ����� ������ � ��
//	const DWORD acq_time_out = (DWORD)(ACQ_BLOCK_SIZE / (System->Conf_LTR11.ChRate * System->Conf_LTR11.LChQnt) + 1000);

	// ������ ����� ������
	res = LTR27_ADCStart(&System->Conf_LTR27);
	if( res != LTR_OK )
	{
		(void)LTR27_ADCStop(&System->Conf_LTR27);
		ExitThread(res);
		return 0;
	}

	while( System->RunFlag2 )
	{
	// ��������� ������ �� LTR11
	size = LTR27_Recv(&System->Conf_LTR27, data_buf, NULL, ACQ2_BLOCK_SIZE, 2000 );
	if( size > 0 &&  size == ACQ2_BLOCK_SIZE)
	{
		int block_number;
		// ��������� ���������� � ��������� � ����������
//		res = LTR27_ProcessData(&ltr27, data_fis, data_buf, &size, 1, 1);
		// ���� ��� ������ ������� ��������� ���������� ������
//		if( res == LTR_OK )
//		{
			// ����� ���������� ������
			block_number = 0;
			while (block_number < ACQ_BLOCK_QNT && System->Acq2BlockReady[block_number])
			block_number++;


		if (block_number < ACQ_BLOCK_QNT)
		{
			DWORD data_size = ACQ2_BLOCK_SIZE;
			// ���������� �������� � ������������ ������ � ������
			res = LTR27_ProcessData(&System->Conf_LTR27, data_buf, System->Acq2Buf[block_number], &data_size, true, true );
			System->BlockNumberThread2 = block_number; // ����� ����� ������ ��� scope-������
			if(res) {
				System->RunFlag2 = 0;
			} else {
				if(System->ReplaceBuf2 == false) System->Acq2BlockReady[block_number] = 1;             // true - ����� �������� � ������
			}
		}
//		}
	} else
	 System->RunFlag2 = 0;

	}

    (void)LTR27_ADCStop(&System->Conf_LTR27);
	ExitThread((DWORD)res);

	return 0;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
void __fastcall WIk::ErrorRelease( int err )throw(Exception)
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
int __fastcall WIk::IsOpenedLTR34()
{
	return LTR34_IsOpened(&Conf_LTR34);
}
//------------------------/ MEMBER FUNCTION /--------------------------------
int __fastcall WIk::UtilStop( bool power_off )
{
	int size;
	int err;
	double volt_array[] = { 0,0,0,0,0,IK_Ucc,0,0 };

//	AcqBlockSize = 1020;	 // ������ ������� ����������� ����� ������ �� ���������

	RunFlag = 0;
	RunFlag2 = 0; 			// ������� �� ������ ����� ������������� ���������� �� LTR27 ( ��� )

	if( LTR34_IsOpened( &Conf_LTR34 ) == 0 )
	{
		err = LTR34_DACStop( &Conf_LTR34 );
		if(err) goto GOTO_ERROR;
	}else Init_LTR11_LTR34();

	Conf_LTR34.ChannelQnt = 8;

	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);   // ���2
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ���3-1
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ���3-2
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(3,0);   // ���3-3
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(4,0);   // ���4
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(1,0);   // ��������� �������
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);   // ������ �����
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(8,0);   // ������ �����

	Conf_LTR34.RingMode = false; 				// ������� ��������� �����
	Conf_LTR34.FrequencyDivisor = 0;			// �������� �� ����������� �������

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	if( power_off == true ) volt_array[5] = 0;
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
    LTR27_Close(&Conf_LTR27);
	delete ArrayToSend; ArrayToSend = 0;
	delete VoltArray; VoltArray = 0;

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
	return RET_OK;
}
//-----------------------/ END COMMON FUNCTIONS /----------------------------


//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.1 � �.5.4.1� ��.
// �������� ������� � ��������� ��� �����������.
void __fastcall WIk::Util0Step1()throw(Exception)
{
	int err;
	int size;

	DWORD thread_status;
	DWORD acq_thread_id;
	HANDLE hnd_acq_thread;

	Init_LTR27();
	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 1;							// ���������� 1 ����� -> ������� 1
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(1,0);   // ��������� �������

	Conf_LTR34.FrequencyDivisor = 0; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 0 )*1) = 31250 ��.
	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[1];  	// 1 �����
	VoltArray = new double[1];		//
	VoltArray[0] = IK_Ucc_Low;				// ���������� ���������� �������

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1, 5000);
	if( size != 1 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ��������� ���
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;


	RunFlag2 = 1;
	// �������� ������ ����� ������ �� ��� LTR27 ������
	hnd_acq_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AcquireThread2, this, 0, &acq_thread_id );
	(void)GetExitCodeThread(hnd_acq_thread, &thread_status);

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.3 � �.5.4.3� ��.
// ��������� ������� ������������� ��������� � �������� ������ �1,�2.
void __fastcall WIk::Util1Step1()throw(Exception)
{
	int err;
	int size;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 1;							// ���������� 1 ����� -> ������� 1
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(1,0);   // ��������� �������

	Conf_LTR34.FrequencyDivisor = 0; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 0 )*1) = 31250 ��.
	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[1];  	// 1 �����
	VoltArray = new double[1];		//
	VoltArray[0] = IK_Ucc;					// ���������� ���������� �������

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
	Conf_LTR11.ADCRate.prescaler = 1;                 		// ������� ������������� - 375 ���  �� ����� - 187,5 ���
	Conf_LTR11.ADCRate.divider = 39;//124                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(39+1)) = 375 ���

	// ������ �������� 5.05.15
	Conf_LTR11.LChQnt = 2;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (5 << 0); // K1, �������� - 10�, ����� - 16-���������, ���������� ����� - 0 ( ��������� � ���� )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (6 << 0); // K2, �������� - 10�, ����� - 16-���������, ���������� ����� - 1 ( ��������� � ���� )

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}

}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.2 � �.5.4.2� ��.
// ��������� ������� �������� � ����� K3 ��� ������� ���������� �������.
// ��� 1.
void __fastcall WIk::Util2Step1()throw(Exception)
{
	int err;
	int size;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 1;							// ���������� 1 ����� -> ������� 1
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(1,0);   // ��������� �������

	Conf_LTR34.FrequencyDivisor = 0; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 0 )*1) = 31250 ��.
	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[1];  			// 1 �����
	VoltArray = new double[1];				//
	VoltArray[0] = IK_Ucc;					// ���������� ���������� �������

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
	Conf_LTR11.ADCRate.prescaler = 64;                 		// ������� ������������� - 1,5625 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (64*(149+1)) = 1,5625 ���

	Conf_LTR11.LChQnt = 1;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (13 << 0); // K3, �������� - 10�, ����� - 16-���������, ���������� ����� - 13 ( ��������� � ���� )

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}

}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.2 � �.5.4.2� ��.
// ��������� ������� �������� � ����� K3 ��� ������� ���������� �������.
// ��� 2 - 12.
int __fastcall WIk::Util2Step2_12()throw(Exception)
{
	int err = 0;
	int size;
	int num_step_prev;
	bool Run = true;

	num_step_prev = NumStep++;	   	// ���������� ��� �������� � ���������
	if( NumStep >= 18 ) { RunFlag = 0; return RET_END; }                  // 01.02.17 ���� 13 �����
																		  // 15.05.17 ���� 17 �����
	// ������� ������� ���������� ��������� ���
	do {
		if(AcqBlockReady[0]) {
			Run = false;

			for(int i=0; i<ACQ_BLOCK_SIZE; i += 2) {
				if(AcqBuf[0][i] > 4) {

					PressureK3 = VoltArray[0] - 0.7;
					return RET_OK;
				}
			}
		}
	} while(Run);

	// ��������� ����� ����������
	VoltArray[0] -= 0.1;

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1, 5000);
	if( size != 1 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ���������
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

//	Sleep(100);
	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {
		AcqBlockReady[i] = 0;
    }

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}

	return RET_ERROR;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.4 � �.5.4.4� ��.
// ��� 1
void __fastcall WIk::Util3Step1()throw(Exception)
{
	int err;
	int size;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 2;							// ���������� 1 ����� -> ������� 1
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(4,0);   // ���4
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(1,0);   // ��������� �������

	Conf_LTR34.FrequencyDivisor = 0; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 0 )*2) = 15625 ��.
	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[156];
	VoltArray = new double[156];
	VoltArray[0] = 7.0;
	VoltArray[1] = IK_Ucc;					// ���������� �������

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 2, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 2, 5000);
	if( size != 2 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }


	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// ������� ������������� - 12,5 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (8*(149+1)) = 12,5 ���

	Conf_LTR11.LChQnt = 1;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (13 << 0); // K3, �������� - 10�, ����� - 16-���������, ���������� ����� - 13 ( ��������� � ���� )

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}

}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.4 � � �.5.4.4� ��.
// ��������� ��� 2 ��� ������������ �������������� �������� 5 �� �� ���4
// ��� 2
int __fastcall WIk::Util3Step2()throw(Exception)
{
	int err = 0;
	int size;
	bool Run = true;

	NumStep++;

	// ��������� ������������� ������� �� 5 ��
	for (int i = 0,j = 0; i < 78; i++)
	{
		VoltArray[j++] = 0;                     // ���4
		VoltArray[j++] = IK_Ucc;					// ���������� �������
	}

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	Conf_LTR34.RingMode = false; 			// ������� ��������� �����
	// ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 156, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 156, 5000);
	if( size != 156 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ��������� �� 5 �� � ��������� ������
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ������� ������ ��� ������������ �������� ������� �������� �� 10 ����
	for( register i = 0, j = 0; i < 40; i++ )
	{
		VoltArray[j++] = 7.;                    // ���4
		VoltArray[j++] = IK_Ucc;				// ���������� �������
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 80, true);
	if(err) goto GOTO_ERROR;

	// ��������� ��������� ������� �� 10 ����
	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 5000);
	if( size != 80 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

//	Sleep(100);
//	AcqBlockReady[0] = 0;
//	AcqBlockReady[1] = 0;

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}

	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.5 � �.5.4.5� ��.
// ��� 1
// ������� ��������� ������� �����

void __fastcall WIk::Util4Step1()throw(Exception)
{
	int err;
	int size;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 2;							// ���������� 1 ����� -> ������� 1
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(3,0);   // ���3-3
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(1,0);   // ��������� �������

	Conf_LTR34.FrequencyDivisor = 0; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 0 )*2) = 15625 ��.
	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[6250];
	VoltArray = new double[6250];
	VoltArray[0] = 0;               // ���3-3
	VoltArray[1] = 0;               // ���������� �������

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 2, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 2, 5000);
	if( size != 2 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ��������� ���
	err = LTR34_DACStart(&Conf_LTR34);

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}

}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.5 � � �.5.4.5� ��.
// ��� 2
// �������� ������� ����� � ������ �������� �� ���� ���3-3
//
int __fastcall WIk::Util4Step2()throw(Exception)
{
	int err = 0;
	int size;

	NumStep++;						// ��� 2

	for (int i = 0,j = 0; i < 3124; i++ )
	{
		VoltArray[j++] = (( i < 1562 )? 7.0 : 0.0);   	// ���3-3
		VoltArray[j++] = IK_Ucc;						// ���������� �������
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
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 6248, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 6248, 5000);
	if( size != 6248 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 1;                 		// ������� ������������� - 12,5 ���
	Conf_LTR11.ADCRate.divider = 249;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (8*(149+1)) = 12,5 ���

	Conf_LTR11.LChQnt = 1;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (13 << 0); // K3, �������� - 10�, ����� - 16-���������, ���������� ����� - 13 ( ��������� � ���� )

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();
//	Sleep(100);
 //	AcqBlockReady[0] = 0;
 //	AcqBlockReady[1] = 0;

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.5 � � �.5.4.5� ��.
// ��� 3
// ��������� ������� �����
//
int __fastcall WIk::Util4Step3()throw(Exception)
{
	int err = 0;
	int size;

	NumStep++;						// ��� 3

	VoltArray[0] = 0.0;   			// ���3-3
	VoltArray[1] = 0.0;				// ���������� �������

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

//	// ����� ������������� ����������� ��������� ����� ������
//	err = LTR34_Reset(&Conf_LTR34);
//	if(err) goto GOTO_ERROR;
//
//	// ��������� ������������ ������
//	err = LTR34_Config(&Conf_LTR34);
//	if(err) goto GOTO_ERROR;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 2, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 2, 5000);
	if( size != 2 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

    // ��������� ��������� ���
	err = LTR34_DACStart(&Conf_LTR34);

 GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
	return RET_OK;
 }
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.6 � � �.5.4.6� ��.
// ��� 4
// �������� ������� ����� � ������������� LTR11 �� ��������� � ������ �4,�5
//
int __fastcall WIk::Util4Step4()throw(Exception)
{
	int err = 0;
	int size;

	NumStep++;						// ��� 3

	if( LTR34_IsOpened( &Conf_LTR34 ) == 0 )
	{
		err = LTR34_DACStop( &Conf_LTR34 );
		if(err) goto GOTO_ERROR;
	} else
	{
		Init_LTR11_LTR34();
		//-----------------------------------------------------------------------
		//	������������� LTR34 - ���
		//-----------------------------------------------------------------------
		Conf_LTR34.ChannelQnt = 2;							// ���������� 1 ����� -> ������� 1
		Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(3,0);   // ���3-3
		Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(1,0);   // ��������� �������

		Conf_LTR34.FrequencyDivisor = 0; 		// �������� ������� �������������
												// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
												// �������� Fch =  2��� / (( 64 - 0 )*2) = 15625 ��.
		// ����� ������������� ����������� ��������� ����� ������
		err = LTR34_Reset(&Conf_LTR34);
		if(err) goto GOTO_ERROR;

		// ��������� ������������ ������
		err = LTR34_Config(&Conf_LTR34);
		if(err) goto GOTO_ERROR;
		// ���������� ������ ArrayToSend � VoltArray
		delete ArrayToSend; delete VoltArray;   // �� ������ ������
		ArrayToSend = new DWORD[2];
		VoltArray = new double[2];
	 }

	VoltArray[0] = 0.0;   			// ���3-3
	VoltArray[1] = IK_Ucc; 			// ���������� �������
	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 2, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 2, 5000);
	if( size != 2 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	LTR11_Close(&Conf_LTR11);
	err = LTR11_Open(&Conf_LTR11, SADDR_DEFAULT, SPORT_DEFAULT, "", 1);
	//---------������� ���������� ������ ���-------

	Conf_LTR11.ADCRate.prescaler = 8;                 		// ������� ������������� - 12,5 ���, �� ����� - 6,25 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (8*(149+1)) = 12,5 ���

	Conf_LTR11.LChQnt = 2;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (2 << 0); // K4, �������� - 10�, ����� - 16-���������, ���������� ����� - 2 ( ��������� � ���� )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (3 << 0); // K5, �������� - 10�, ����� - 16-���������, ���������� ����� - 3 ( ��������� � ���� )
	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();
//	Sleep(100);

 GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.7 � � �.5.4.7� ��.
// ��� 1
// ������� ����������� ������ LTR34 � LTR11

void __fastcall WIk::Util5Step1( int num_block )throw(Exception)
{
	int err;
	int size;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 2;							// ���������� 2 ������ -> ������� 2
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);   // ���2
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(1,0);   // ��������� �������

	Conf_LTR34.FrequencyDivisor = 0; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 0 )*2) = 15625 ��.
	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[2190];
	VoltArray = new double[2190];
	VoltArray[0] = 0;               // ���2
	VoltArray[1] = IK_Ucc;          // ���������� �������

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 2, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 2, 5000);
	if( size != 2 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// ������� ������������� - 12,5 ���, �� ����� - 6,25 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (8*(149+1)) = 12,5 ���

	Conf_LTR11.LChQnt = 2;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (11 << 0); // K6, �������� - 10�, ����� - 16-���������, ����� X12
	if( num_block == BLOCK_BU )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (12 << 0); // K7, �������� - 10�, ����� - 16-���������, ����� X13
	else
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (15 << 0); // K11, �������� - 10�, ����� - 16-���������, ����� X16
	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

//	SuspendThread(hnd_acq_thread2);       // ���������������� ����� ������ �� ���

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.7 � � �.5.4.7� ��.
// ��� 2
// ������ ������� �� ���2 � ������� �������� �� K6, K7

int __fastcall WIk::Util5Step2()throw(Exception)
{
	int err;
	int size;

	NumStep++;						// ��� 2

	// ��������� ������� ������������� 70 �� ( ������������ ������ ������ ��� - 0.064 �� )
	// ��������� �������� - �������� �������� -> ������� ���������
	for (int i = 0,j = 0; i < 1095; i++ )
	{
		VoltArray[j++] = ( i != 1094 )? 7.2 : 0.0;      // ���2
		VoltArray[j++] = IK_Ucc;						// ���������� �������
	}

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	Conf_LTR34.RingMode = false; 			// �������� ��������� �����
	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 2190, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 2190, 5000);
	if( size != 2190 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ��������� �������� �� ���2 ������������� 70 ��
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

//	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {               // ��� ����� ������
//		AcqBlockReady[i] = 0;                              // ������ � ������
//	}

//	ResumeThread(hnd_acq_thread2);						   // ���������� ����� ������ �� ���

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.8 � �.5.4.8� ��.
// ��� 1
// ������� ����������� ������ LTR34 � LTR11

void __fastcall WIk::Util6Step1(  int num_block )throw(Exception)
{
	int err;
	int size;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 4;							// ���������� 2 ������ -> ������� 2
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);   // ���2
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ���3-1
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ���3-2
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);   // ��������� �������

	Conf_LTR34.FrequencyDivisor = 0; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 0 )*4) = 7812.5 ��.
	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[21876];
	VoltArray = new double[21876];
	VoltArray[0] = 0;               // ���2
	VoltArray[1] = 0;               // ���3-1
	VoltArray[2] = 0;               // ���3-2
	VoltArray[3] = IK_Ucc;          // ���������� �������

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
	Conf_LTR11.ADCRate.prescaler = 8;                 		// ������� ������������� - 12,5 ���, �� ����� - 6,25 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (8*(149+1)) = 12,5 ���

	Conf_LTR11.LChQnt = 2;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (11 << 0); // K6, �������� - 10�, ����� - 16-���������, ����� X12
	if( num_block == BLOCK_BU )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (14 << 0); // K8, �������� - 10�, ����� - 16-���������, ����� X15
	else
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (14 << 0); // K7, �������� - 10�, ����� - 16-���������, ����� X15
	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.8 � �.5.4.9. ��.
// ��� 2
// ������ ������� �� ���3-1 � ���3-2 � ������� �������� �� K6, K8

int __fastcall WIk::Util6and7Step2()throw(Exception)
{
	int err;
	int size;

	NumStep++;						// ��� 2

	// ��������� ������� ������������� 700 �� ( ������������ ������ ������ ��� - 0.128 �� )
	// ��������� �������� - �������� �������� -> ������� ���������
	for (int i = 0,j = 0; i < 5469; i++ )
	{
		VoltArray[j++] = 0.0;		                    // ���2
		VoltArray[j++] = ( i != 5468 )? 7.0 : 0.0;      // ���3-1
		VoltArray[j++] = ( i != 5468 )? 7.0 : 0.0;      // ���3-2
		VoltArray[j++] = IK_Ucc;   						// ���������� �������
	}

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	Conf_LTR34.RingMode = false; 			// ������� ��������� �����
	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 21876, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 21876, 5000);
	if( size != 21876 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ��������� �������� �� ���2 ������������� 70 ��
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.8 �.5.4.9 ��.
// ��� 3
// ������ ������� �� ���2 � ������� �������� �� K6, K8

int __fastcall WIk::Util6and7Step3()throw(Exception)
{
	int err;
	int size;

	NumStep++;						// ��� 2

	// ��������� ������� ������������� 70 �� ( ������������ ������ ������ ��� - 0.128 �� )
	// ��������� �������� - �������� �������� -> ������� ���������
	for (int i = 0,j = 0; i < 547; i++ )
	{
		VoltArray[j++] = ( i != 5468 )? 7.0 : 0.0;      // ���2
		VoltArray[j++] = 0.0;      						// ���3-1
		VoltArray[j++] = 0.0;      						// ���3-2
		VoltArray[j++] = IK_Ucc;   						// ���������� �������
	}

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	Conf_LTR34.RingMode = false; 			// ������� ��������� �����
	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 2188, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 2188, 5000);
	if( size != 2188 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ��������� �������� �� ���2 ������������� 70 ��
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.9 ��.
// ��� 1
// ������������� LTR11 �� ��������� � ������ K9, K10

void __fastcall WIk::Util7Step1()throw(Exception)
{
	int err;
	int size;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 4;							// ���������� 2 ������ -> ������� 2
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);   // ���2
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ���3-1
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ���3-2
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);   // ��������� �������

	Conf_LTR34.FrequencyDivisor = 0; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 0 )*4) = 7812.5 ��.
	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[21876];
	VoltArray = new double[21876];
	VoltArray[0] = 0;               // ���2
	VoltArray[1] = 0;               // ���3-1
	VoltArray[2] = 0;               // ���3-2
	VoltArray[3] = IK_Ucc;          // ���������� �������

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
	Conf_LTR11.ADCRate.prescaler = 8;                 		// ������� ������������� - 12,5 ���, �� ����� - 6,25 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (8*(149+1)) = 12,5 ���

	Conf_LTR11.LChQnt = 2;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (9 << 0); 	// K9, �������� - 10�, ����� - 16-���������, ����� X10
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (8 << 0); 	// K10, �������� - 10�, ����� - 16-���������, ����� X9

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}
}
//	int err;
//
//	NumStep++;						// ��� 3
//	//-----------------------------------------------------------------------
//	//	����������������� LTR11 - ���
//	//-----------------------------------------------------------------------
//	SuspendThread(hnd_acq_thread2);       // ���������������� ����� ������ �� ���
//	LTR11_Stop(&Conf_LTR11);
//	//---------������� ���������� ������ ���-------
//	Conf_LTR11.ADCRate.prescaler = 8;                 		// ������� ������������� - 100 ���
//	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���
//
//	Conf_LTR11.LChQnt = 2;                            		// ���������� ���������� ������� - 1
//	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (9 << 0); 	// K9, �������� - 10�, ����� - 16-���������, ���������� ����� - 9 ( ��������� � ���� )
//	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (8 << 0); 	// K10, �������� - 10�, ����� - 16-���������, ���������� ����� - 8 ( ��������� � ���� )
//
//	// ��������� ������������ ������ ���
//	err = LTR11_SetADC(&Conf_LTR11);
//	if(err) goto GOTO_ERROR;
//
//	// ������ ����� ������
//	err = LTR11_Start(&Conf_LTR11);
//	ResumeThread(hnd_acq_thread2);						   // ���������� ����� ������ �� ���
//
//GOTO_ERROR:
//	if(err)
//	{
//		ErrorRelease( err );
//	}
//	return RET_OK;
//}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.4.3 ��. ( ������ ����� ������ )
// ���������� ���������� ������� 6.2� � ������������ ������� � ����� �3
// ��� 1.
void __fastcall WIk::Util2Step0()throw(Exception)
{
	int err;
	int size;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 1;							// ���������� 1 ����� -> ������� 1
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(1,0);   // ��������� �������

	Conf_LTR34.FrequencyDivisor = 0; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 0 )*1) = 31250 ��.
	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	// ���������� ������ VoltArray
	ArrayToSend = new DWORD[1];  			// 1 �����
	VoltArray = new double[1];				//
	VoltArray[0] = (6.2+0.7); 				// ���������� ���������� �������
//	VoltArray[0] = (6.2); 				// ���������� ���������� �������

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
	Conf_LTR11.ADCRate.prescaler = 64;                 		// ������� ������������� - 1,5625 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (64*(149+1)) = 1,5625 ���

	Conf_LTR11.LChQnt = 1;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (13 << 0); // K3, �������� - 10�, ����� - 16-���������, ���������� ����� - 13 ( ��������� � ���� )

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}

}
//------------------------------/ END /--------------------------------------
//��������� ������� ���������������� ��������
DWORD WINAPI WIk::CheckUtil3_7Thread (WIk *System)
{
	int count = 0;
	System->Util3K3 = false;				// �������������� ����������

	while(System->RunFlag)
		{
			if(count == ACQ_BLOCK_QNT) count = 0;

			if(System->AcqBlockReady[count]) {

				for(int i = 0; i < ACQ_BLOCK_SIZE; i++) {
					if( System->AcqBuf[count][i] > 4 )
						System->Util3K3 = true;
				}

			System->AcqBlockReady[count] = 0;
			count++;
			}
		}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
// �. 5.4.7
// �������� ������� ����� ����������
DWORD WINAPI WIk::CheckUtil5Thread (WIk *System)
{
	int count = 0;
	int i;
//	System->Util5K6 = false;				// �������������� ����������
//	System->Util5K7 = false;				// �������������� ����������

	int K6, K7;

	int count_sampl = 0;
	bool perepadK6 = false;
	bool perepadK7 = false;

	while(System->RunFlag)
		{
			if(count == ACQ_BLOCK_QNT) count = 0;

			if(System->AcqBlockReady[count]) {

				for(i=0; i <ACQ_BLOCK_SIZE;) {
					count_sampl++;
					if( System->AcqBuf[count][i++] > 4 && perepadK6 == false ) {
						perepadK6 = true;
						K6 = count_sampl;
					}
					if(System->AcqBuf[count][i++] > 4 && perepadK6 == true && perepadK7 == false ) {
						perepadK7 = true;
						K7 = count_sampl;
						System->TimeUtil5 = ( (double) K7 - (double) K6) / 6250.0;
					}
				}

			System->AcqBlockReady[count] = 0;
			count++;
			}
		}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
#include "math.hpp"
#include <math.h>
// �. 5.4.8
// �������� ������� ����� ����������
DWORD WINAPI WIk::CheckUtil6Step3Thread (WIk *System)
{
	int count = 0;
	int i;

	int count_sampl = 0;
	bool perepadK6 = false;
	bool perepadK8 = false;

	int K6, K7;

//	System->timeUtil6K6 = 0;
//	System->timeUtil6K8 = 0;

	while(System->RunFlag)
		{
			if(count == ACQ_BLOCK_QNT) count = 0;

			if(System->AcqBlockReady[count]) {

				for(i=0; i <ACQ_BLOCK_SIZE;) {
					count_sampl++;
					if( System->AcqBuf[count][i++] > 4 && perepadK6 == false ) {
						perepadK6 = true;
//						System->timeUtil6K6 = Time();
						K6 = count_sampl;
					}

					if(System->AcqBuf[count][i++] > 4 && perepadK6 == true && perepadK8 == false ) {
						perepadK8 = true;
//						System->timeUtil6K8 = Time();
                    	K7 = count_sampl;
						System->TimeUtil6 = SimpleRoundTo(( (double) K7 - (double) K6) / 6250.0 ,-2);
					}
				}

			System->AcqBlockReady[count] = 0;
			count++;
			}
		}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
// FileName - ��� �����, ������� ����� ����������� ("\\RaportIK.xlsx")
// write - ���������� ��� ��� ����� ������� ���� ����
void __fastcall WIk::WriteExcel(AnsiString FileName, bool write)
{
	TDateTime t;


	if(zRaport->ExcelInit()) {
		if(zRaport->CopyRepFromSablon(FileName)) {
			zRaport->WriteData(4, 1, "� ����� " + NumberBlock);
			zRaport->WriteData(4, 3, "���� " + t.CurrentDateTime().FormatString("dd.mm.yyyy"));
			zRaport->WriteData(4, 6, "����� " + t.CurrentDateTime().FormatString("hh:nn"));

			zRaport->WriteData(9, 5, SimpleRoundTo(currentPlus, -2));    // ���
			zRaport->WriteData(9, 7, note_current);

			zRaport->WriteData(10, 5, SimpleRoundTo(PressureK3, -1));
			zRaport->WriteData(10, 7, note_PressureK3);

			zRaport->WriteData(12, 5, Util1TimeK1);
			zRaport->WriteData(12, 7, note_timeX8);

			zRaport->WriteData(14, 5, Util1TimeK2);
			zRaport->WriteData(14, 7, note_timeX10);

			zRaport->WriteData(15, 5,time1);    //secK4
			zRaport->WriteData(15, 7, note_Util4X30);

			zRaport->WriteData(16, 5, time2);
			zRaport->WriteData(16, 7, note_Util4X31);

			zRaport->WriteData(17, 5, TimeUtil4X6.FormatString("ss.zzz"));
			zRaport->WriteData(17, 7, note_Util4X6);

			zRaport->WriteData(18, 5, SimpleRoundTo(Util4K4, -1));
			zRaport->WriteData(18, 7, note_Util4X30);

			zRaport->WriteData(19, 5, SimpleRoundTo(Util4K5, -1));
			zRaport->WriteData(19, 7, note_Util4X31);

			zRaport->WriteData(20, 5, TimeUtil5);
			zRaport->WriteData(20, 7, note_TimeUtil5);

			zRaport->WriteData(21, 5, TimeUtil6);
			zRaport->WriteData(21, 7, note_TimeUtil6);

			zRaport->WriteData(22, 5, SimpleRoundTo(fabs(Util7X34), -1));
			zRaport->WriteData(22, 7, note_Util7X34);

			if(write) {
				zRaport->CloseReport("C:\\��������� ����\\���������\\���-05\\067\\067-" + NumberBlock + ".xlsx");
			} else {
				zRaport->CloseReport(GetCurrentDir() + "\\~" + NumberBlock + ".xlsx");
			}
        }
    }
}
//---------------------------------/ END /-----------------------------------
