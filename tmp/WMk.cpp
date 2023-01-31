//---------------------------------------------------------------------------
#include <vcl.h>
#include "math.h"
#pragma hdrstop
#include "WMk.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
static const char ErrorText1[] = "������ ��� �������� ������ ������!";
static const char ErrorText2[] = "������! ���������� ������������ ������� �� ����� ���������!";
static const char ErrorText3[] =  "����������� ������";
//--------------------------/ CONSTRUCTOR /----------------------------------
WMk::WMk():ArrayToSend(0),VoltArray(0)
{
	BlockNumberThread = 100; // ������������� ���������� �� ������ ���� ����� ��� ����� ������ �������
	ReplaceBuf = false;      // ������������� ���������� ��������� ������ �������
	RunFlag = 0;
//	AcqBlockSize = 1020;	 // ������ ������� ����������� ����� ������ �� ���������

	for(int i = 0; i < 2; i++) {
//		PerepadUtil1[i] = false;
//		PerepadUtil2[i] = false;
//		PerepadUtil3[i] = false;
		PerepadUtil4[i] = false;
//		PerepadUtil5[i] = false;
	}
}
//--------------------------/ DESTRUCTOR /-----------------------------------
WMk::~WMk()
{
}
//------------------------/ MEMBER FUNCTION /--------------------------------
void __fastcall WMk::Init_LTR11_LTR34()throw(Exception)
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
void __fastcall WMk::StartGen_LTR11_LTR34()throw(Exception)
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
DWORD WINAPI WMk::AcquireThread (WMk *System)
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

	ExitThread((DWORD)err);

	return 0;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
void __fastcall WMk::ErrorRelease( int err )throw(Exception)
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
int __fastcall WMk::IsOpenedLTR34()
{
	return LTR34_IsOpened(&Conf_LTR34);
}
//------------------------/ MEMBER FUNCTION /--------------------------------
int __fastcall WMk::UtilStop( bool power_off )
{
	int size;
	double volt_array[] = { 0,0,0,9.7 };

//	AcqBlockSize = 1020;	 // ������ ������� ����������� ����� ������ �� ���������

	RunFlag = 0;
	int err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	Conf_LTR34.ChannelQnt = 4;

	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(3,1);   // ��2
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(4,1);   // ��3
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(5,0);   // ���1
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);   // ��������� �������

	Conf_LTR34.RingMode = false; 				// ������� ��������� �����
	Conf_LTR34.FrequencyDivisor = 0;			// �������� �� ����������� �������

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	if( power_off == true ) volt_array[3] = 0;
	delete ArrayToSend;
	ArrayToSend = new DWORD[4];
	err = LTR34_ProcessData(&Conf_LTR34, volt_array, ArrayToSend, 4, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 4, 5000);
	if( size != 4 )  { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

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
	}
	return RET_OK;
}
//-----------------------/ END COMMON FUNCTIONS /----------------------------



//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.3.2 ��.
// ��������� ���������� �� �������� ������ �5,�6.
void __fastcall WMk::Util1Step1()throw(Exception)
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
											// �������� Fch =  2��� / (( 64 - 0 )*8) = 3906.25 ��.
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
	VoltArray[0] = 9.7;

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
	Conf_LTR11.ADCRate.prescaler = 8;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 2;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (4 << 0); // K5, �������� - 10�, ����� - 16-���������, ���������� ����� - 4 ( ��������� � ���� )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (5 << 0); // K6, �������� - 10�, ����� - 16-���������, ���������� ����� - 5 ( ��������� � ���� )

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
// �������� �� �.5.3.4 ��. ������ "��1"  -> �������� num_util = MK_UTIL2
// �������� �� �.5.3.5 ��. ������ "��2"  -> �������� num_util = MK_UTIL3
void __fastcall WMk::Util2_3Step1(int num_util)throw(Exception)
{
	int err = 0;
	int length = 0;
	int size;
	double t = 0;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 4;							// ���������� 3 ������ -> ������� 4
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(3,1);   // ��2
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(4,1);   // ��3
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(5,0);   // ���1
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);	// �������

	if( num_util == MK_UTIL2 )				// ��� ��������� ��������� 1 ��
	Conf_LTR34.FrequencyDivisor = 0; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 0 )*4) = 7812.5 ��.

	if( num_util == MK_UTIL3 )              // ��� ��������� ��������� 50 ��
	Conf_LTR34.FrequencyDivisor = 14; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 14 )*4) = 10000 ��.


	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0

	if( num_util == MK_UTIL2 )
	{
		// ���������� ������ ArrayToSend
		// ���������� ������ VoltArray
		length = 31248; 					// 7812 * 4 = 31248
		ArrayToSend = new DWORD[31248];  	// 4 ������ ( ����� 3, �� �� ���. ������������ ����� �� ����� 4 )
		VoltArray = new double[31248];		// �� 7812 ( max) �������� ��� ������� - ����� 31248.

		// ���������� ������ VoltArray ��� ������������ �������� ������������� 1 ��� ( 1 �� )
		// ������� �������� ��� ������������ ������� 1 �� �� ������ OUT3-1:10 ( ������ ��2 )
		// � OUT4-1:10 ( ������ ��3 ) sin( omega*t );
		// omega = 2*PI*1 = 6.28318530 ���/���
		for (int i = 0,j = 0; i < 7812; i++)
		{
			VoltArray[j++] = 0.91*sin(6.28318530*t);       	// ��2
			VoltArray[j++] = 0.91*sin(6.28318530*t);       	// ��3
			VoltArray[j++] = 0;       						// ���1
			VoltArray[j++] = 9.7;	  						// �������
			t += 0.000128;									// ������ 0.000128 ��� - 7812.5 ��
		}
	}
	if( num_util == MK_UTIL3 )
	{
		// ���������� ������ ArrayToSend
		// ���������� ������ VoltArray
		length = 800;						// 200 * 4 = 800
		ArrayToSend = new DWORD[800];  		// 4 ������ ( ����� 3, �� �� ���. ������������ ����� �� ����� 4 )
		VoltArray = new double[800];		// �� 200 ( max) �������� ��� ������� - ����� 800.

		// ���������� ������ VoltArray ��� ������������ �������� ������������� 0.02 ��� ( 50 �� )
		// ������� �������� ��� ������������ ������� 1 �� �� ������ OUT3-1:10 ( ������ ��2 )
		// � OUT4-1:10 ( ������ ��3 ) sin( omega*t );
		// omega = 2*PI*50 = 6.28318530 * 50  = 314.159265358 ���/���
		for (int i = 0,j = 0; i < 200; i++)
		{
			VoltArray[j++] = 0.91*sin(314.159265358*t);    	// ��2
			VoltArray[j++] = 0.91*sin(314.159265358*t);    	// ��3
			VoltArray[j++] = 0;       						// ���1
			VoltArray[j++] = 9.7;	  						// �������
			t += 0.0001;								   	// ������ 0.0001 ��� - 10000 ��
		}
	}
	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, length, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, length, 5000);
	if( size != length ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 4;                            		// ���������� ���������� ������� - 4
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (0 << 0); // K1, �������� - 10�, ����� - 16-���������, ���������� ����� - 3 ( ��������� � ���� )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (1 << 0); // K2, �������� - 10�, ����� - 16-���������, ���������� ����� - 4 ( ��������� � ���� )
	Conf_LTR11.LChTbl[2] = (0 << 6) | (0 << 4) | (2 << 0); // K3, �������� - 10�, ����� - 16-���������, ���������� ����� - 5 ( ��������� � ���� )
	Conf_LTR11.LChTbl[3] = (0 << 6) | (0 << 4) | (3 << 0); // K4, �������� - 10�, ����� - 16-���������, ���������� ����� - 6 ( ��������� � ���� )

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.3.6 ��. ������ "��3"  -> �������� num_util = MK_UTIL4
// �������� �� �.5.3.7 ��. ������ "��4"  -> �������� num_util = MK_UTIL5
void __fastcall WMk::Util4_5Step1(int num_util)throw(Exception)
{
	int err = 0;
	int length = 0;
	int size;
	double t1 = 0, t2 = 0;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 4;							// ���������� 3 ������ -> ������� 4
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(3,1);   // ��2
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(4,1);   // ��3
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(5,0);   // ���1
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);	// �������

	Conf_LTR34.RingMode = false; 			// ������� ��������� �����
	Conf_LTR34.FrequencyDivisor = 0;        // ��� ��������� ��������� 1 ��
											// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  2��� / (( 64 - 0 )*4) = 7812.5 ��.

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;							// ����� ���� �������� = 0

	if( num_util == MK_UTIL4 )
	{
		// ���������� ������ ArrayToSend
		// ���������� ������ VoltArray
		length = 31248; 					// 7812 * 4 = 31248
		ArrayToSend = new DWORD[31248];  	// 4 ������ ( ����� 3, �� �� ���. ������������ ����� �� ����� 4 )
		VoltArray = new double[31248];		// �� 7812 ( max) �������� ��� ������� - ����� 31248.

		// ���������� ������ VoltArray ��� ������������ �������� ������������� 1 ��� ( 1 �� )
		// ������� �������� ��� ������������ ������� 1 �� �� ������ OUT3-1:10 ( ������ ��2 )
		// � OUT4-1:10 ( ������ ��3 ) sin( omega*t );
		// omega = 2*PI*1 = 6.28318530 ���/���
		for (int i = 0,j = 0; i < 7812; i++)
		{
			VoltArray[j++] = 0.91*sin(6.28318530*t1);      	// ��2
			VoltArray[j++] = 0.91*sin(6.28318530*t1);      	// ��3
			VoltArray[j++] = 0;       						// ���1
			VoltArray[j++] = 9.7;	  						// �������
			t1 += 0.000128;									// ������ 0.000128 ��� - 7812.5 ��
		}
	}
	if( num_util == MK_UTIL5 )
	{
		// ���������� ������ ArrayToSend
		// ���������� ������ VoltArray
		length = 35936; 					// (7812 + 1172) * 4 = 35936
		ArrayToSend = new DWORD[35936];  	// 4 ������ ( ����� 3, �� �� ���. ������������ ����� �� ����� 4 )
		VoltArray = new double[35936];		// �� 7812 ( max) �������� ��� ������� - ����� 31248.

		// ���������� ������ VoltArray ��� ������������ �������� ������������� 1 ��� ( 1 �� )
		// ������� �������� ��� ������������ ������� 1 �� �� ������ OUT3-1:10 ( ������ ��2 )
		// � OUT4-1:10 ( ������ ��3 ) sin( omega*t ) � ��������� 150 �� ( 1172 ������� ��� )
		// omega = 2*PI*1 = 6.28318530 ���/���
		// ��� ������������ ������� 1 �� ���������� 7812 ��������
		// ���� �������� ��� D�3 150 �� - 1172 �������
		// �����:  7812 + 1172 = 8984 jncxtnjd
		for (int i = 0,j = 0; i < 8984; i++)
		{
			if( i < 1173 )   VoltArray[j++] = 0;                //** ��2
			else { VoltArray[j++] = 0.91*sin(6.28318530*t1);   // *
			t1 += 0.000128;										//** ������ 0.000128 ��� - 7812.5 ��
			}
			if( i < 7812 )                                      //** ��3
			{  VoltArray[j++] = 0.91*sin(6.28318530*t2);       // *
				t2 += 0.000128;									// * ������ 0.000128 ��� - 7812.5 ��
			}else  VoltArray[j++] = 0;                          //**
			VoltArray[j++] = 8;       							// ���1
			VoltArray[j++] = 9.7;	  							// �������
		}
	}
	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, length, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, length, 5000);
	if( size != length ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// ������� ������������� - 12,5 ���  �� ����� 2,083 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (8*(149+1)) = 12,5 ���

	Conf_LTR11.LChQnt = 6;                            		// ���������� ���������� ������� - 6
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (2 << 0); // K3, �������� - 10�, ����� - 16-���������, ���������� ����� - 2 ( ��������� � ���� )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (3 << 0); // K4, �������� - 10�, ����� - 16-���������, ���������� ����� - 3 ( ��������� � ���� )
	Conf_LTR11.LChTbl[2] = (0 << 6) | (0 << 4) | (6 << 0); // K7, �������� - 10�, ����� - 16-���������, ���������� ����� - 6 ( ��������� � ���� )
	Conf_LTR11.LChTbl[3] = (0 << 6) | (0 << 4) | (7 << 0); // K8, �������� - 10�, ����� - 16-���������, ���������� ����� - 7 ( ��������� � ���� )
	Conf_LTR11.LChTbl[4] = (0 << 6) | (0 << 4) | (8 << 0); // K9, �������� - 10�, ����� - 16-���������, ���������� ����� - 8 ( ��������� � ���� )
	Conf_LTR11.LChTbl[5] = (0 << 6) | (0 << 4) | (9 << 0); // K10, �������� - 10�, ����� - 16-���������, ���������� ����� - 9 ( ��������� � ���� )

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
}

//-----------------------------/ END /---------------------------------------
