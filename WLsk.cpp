//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "WLsk.h"
#include "UnRaport.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
static const char ErrorText1[] = "������ ��� �������� ������ ������!";
static const char ErrorText2[] = "������! ���������� ������������ ������� �� ����� ���������!";
static const char ErrorText3[] =  "����������� ������";
//--------------------------/ CONSTRUCTOR /----------------------------------
WLsk::WLsk():ArrayToSend(0),VoltArray(0)
{
	BlockNumberThread = 100; // ������������� ���������� �� ������ ���� ����� ��� ����� ������ �������
	ReplaceBuf = false;      // ������������� ���������� ��������� ������ �������
	ReplaceBuf2 = false;     // ������������� ���������� ��������� ������ �������
	RunFlag = 0;
	RunFlag2 = 0;

	zRaport = new ZRaport;

	for(int i = 0; i<5; i++) {
        PerepadUtil1[i] = false;
		PerepadUtil2[i] = false;
		PerepadUtil3[i] = false;
		PerepadUtil4[i] = false;
		PerepadUtil5[i] = false;
	}
	LTR34_Init(&Conf_LTR34);
	LTR11_Init(&Conf_LTR11);
	LTR27_Init(&Conf_LTR27);
}
//--------------------------/ DESTRUCTOR /-----------------------------------
WLsk::~WLsk()
{
}
//------------------------/ MEMBER FUNCTION /--------------------------------
void __fastcall WLsk::Init_LTR11_LTR34()throw(Exception)
{
	int err = 0;

	//������������� ������������ �������
	err = LTR34_Init(&Conf_LTR34);
	if(err) goto GOTO_ERROR2;

	err = LTR11_Init(&Conf_LTR11);
	if(err) goto GOTO_ERROR1;
	Conf_LTR11.ModuleInfo.CbrCoef[0].Gain = 1.03;

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
void __fastcall WLsk::Init_LTR27()throw(Exception)
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
void __fastcall WLsk::StartGen_LTR11_LTR34()throw(Exception)
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
DWORD WINAPI WLsk::AcquireThread (WLsk *System)
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
 DWORD WINAPI WLsk::AcquireThread2 (WLsk *System)
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

	LTR27_ADCStop(&System->Conf_LTR27);
	ExitThread((DWORD)res);

	return 0;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
void __fastcall WLsk::ErrorRelease( int err )throw(Exception)
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
int __fastcall WLsk::IsOpenedLTR34()
{
	return LTR34_IsOpened(&Conf_LTR34);
}
//------------------------/ MEMBER FUNCTION /--------------------------------
int __fastcall WLsk::UtilStop( bool power_off )
{
	int size;
	int err;
	double volt_array[] = { 9,9,0,0,9,LSK_Ucc,0,7 };

//	AcqBlockSize = 1020;	 // ������ ������� ����������� ����� ������ �� ���������

	RunFlag = 0;            // ������� �� ������ ����� ������������� ���������� �� LTR11
	RunFlag2 = 0; 			// ������� �� ������ ����� ������������� ���������� �� LTR27 ( ��� )

	if( LTR34_IsOpened( &Conf_LTR34 ) == 0 )
	{
		err = LTR34_DACStop( &Conf_LTR34 );
		if(err) goto GOTO_ERROR;
	}else
		Init_LTR11_LTR34();

	Conf_LTR34.ChannelQnt = 8;

	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(6,0);	// ��1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(7,0);   // ��2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(8,0);   // ��3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(3,0);   // ��4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(4,0);   // ��5
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(1,0);   // ��������� �������
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// ������ �����
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(5,0);   // ����

	Conf_LTR34.RingMode = false; 				// ������� ��������� �����
	Conf_LTR34.FrequencyDivisor = 0;			// �������� �� ������������ �������

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	if( power_off == true ) {
		volt_array[0] = 0;
		volt_array[1] = 0;
		volt_array[4] = 0;
		volt_array[5] = 0;
		volt_array[7] = 0;
	}

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
// �������� �� �.5.2.1 - 5.2.2 ��.
void __fastcall WLsk::Util0Step1()throw(Exception)
{
	int err = 0;
	int size;
	DWORD thread_status;
	DWORD acq_thread_id;
	HANDLE hnd_acq_thread;
	ArrayToSend = 0;

	Init_LTR27();
	Init_LTR11_LTR34();

	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// ���������� 6 ������� (������� 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(6,0);	// ��1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(7,0);   // ��2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(8,0);   // ��3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(3,0);   // ��4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(4,0);   // ��5
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(1,0);   // ��������� �������
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// ������ �����
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(5,0);   // ����

//	//--------- ������� ���������� ������ ��� ---------
//	Conf_LTR34.ChannelQnt = 1;		  			// ���������� ���� ����� - ����� ��������� �������
//	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(1,0);
//	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);	// ��1
////	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ��2
////	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(4,0);   // ��5

	Conf_LTR34.FrequencyDivisor = 40; 		 	// �������� ������� �������������

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// ����� ���� �������� = 0
	// ���������� ������ ArrayToSend
	ArrayToSend = new DWORD[8];
	VoltArray = new double[8];
	VoltArray[0] =  LSK_Uh;   		// ��1
	VoltArray[1] =  LSK_Uh;         // ��2
	VoltArray[2] =  LSK_Uh;         // ��3
	VoltArray[3] =  0.0;            // ��4
	VoltArray[4] =  LSK_Uh;         // ��5
	VoltArray[5] = 	LSK_Ucc;        // ��������� �������
	VoltArray[6] =  0.0;            // ������ �����
	VoltArray[7] =  LSK_InpUl;      // ����

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 8, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 8, 5000);
	if( size != 8 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 1;// 8                 		// ������� ������������� - 50 ��� �� ����� - 25 ���
	Conf_LTR11.ADCRate.divider = 299;// 149                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(299+1)) = 50 ���

	Conf_LTR11.LChQnt = 2;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (2 << 4) | (8 << 0); 	// K9 - �������� - 10�, ����� - 32-���������, ����� X9
	Conf_LTR11.LChTbl[1] = (0 << 6) | (2 << 4) | (14 << 0); // K10 - �������� - 10�, ����� - 32-���������, ����� X15
	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	// ��������� ���������
	StartGen_LTR11_LTR34();
	RunFlag2 = 1;				  							//
	// �������� ������ ����� ������ �� ��� LTR27 ������
	hnd_acq_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AcquireThread2, this, 0, &acq_thread_id );
	(void)GetExitCodeThread(hnd_acq_thread, &thread_status);


GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.2.3 ��. ��������� ������ "��1"

void __fastcall WLsk::Util1Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// ���������� 6 ������� (������� 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(6,0);	// ��1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(7,0);   // ��2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(8,0);   // ��3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(3,0);   // ��4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(4,0);   // ��5
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(1,0);   // ��������� �������
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// ������ �����
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(5,0);   // ����

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
	ArrayToSend = new DWORD[24000];  	// 8 ������ ( ����� 6, �� �� ���. ������������ ����� �� ����� 8 )
	VoltArray = new double[24000];		// �� 2926 ( max) �������� ��� ������� - ����� 24000 ( max ).

	// ���������� ���������� ������ �������� � ������ �� �������� � ������� 0.5 ���
	for (int i = 0,j = 0; i < 10; i++) {
	VoltArray[j++] = LSK_Uh;
	VoltArray[j++] = LSK_Uh;
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	VoltArray[j++] = LSK_Uh;
	VoltArray[j++] = LSK_Ucc;		// �������
	VoltArray[j++] = 0;
	VoltArray[j++] = LSK_InpUl;
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 80, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 5000);
	if( size != 80 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 5;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (2 << 4) | (3 << 0); // K4, �������� - 10�, ����� - 32-���������, ����� X4
	Conf_LTR11.LChTbl[1] = (0 << 6) | (3 << 4) | (0 << 0); // K5, �������� - 10�, ����� - 32-���������, ����� Y1
	Conf_LTR11.LChTbl[2] = (0 << 6) | (3 << 4) | (1 << 0); // K6, �������� - 10�, ����� - 32-���������, ����� Y2
	Conf_LTR11.LChTbl[3] = (0 << 6) | (3 << 4) | (2 << 0); // K7, �������� - 10�, ����� - 32-���������, ����� Y3
	Conf_LTR11.LChTbl[4] = (0 << 6) | (2 << 4) | (7 << 0); // K8, �������� - 10�, ����� - 32-���������, ����� X8

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
// �������� �� �.5.2.3 ��.  ��������� ������ "��1"

int __fastcall WLsk::Util1Step2()throw(Exception)
{
	int err = 0;
	int size;

	// ���������� ������ VoltArray ��� ������������ �������� ������������� 32 ��
	for( register i = 0, j = 0; i < 125; i++ )
	{
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = LSK_Ucc; 		// �������
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_InpUl;
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
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1000, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1000, 5000);
	if( size != 1000 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ��������� ������� �� 32 ����
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ����� ������� ������ ��� ������������ ������� �������� �� 500 ����
	for( register i = 0, j = 0; i < 1953; i++ )
	{
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = LSK_Ucc; 		// �������
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_InpUl;
	}
	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 15624, true);
	if(err) goto GOTO_ERROR;

	// ��������� ��������� ������� �� 468 ����
	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 15624, 5000);
	if( size != 15624 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ����� ������� ������ ��� ������������ ������� �������� �� (500 - 32 = 468) ����
//	for( register i = 0, j = 0; i < 1828; i++ )
	// ����� ������� ������ ��� ������������ ������� �������� �� 750 ����
	for( register i = 0, j = 0; i < 2926; i++ )
	{
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = LSK_Ucc; 		// �������
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_InpUl;
	}
	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
//	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 14624, true);
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 23408, true);
	if(err) goto GOTO_ERROR;

	// ��������� ��������� ������� �� 468 ����
	// ���������� ������ � ������
//	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 14624, 5000);
//	if( size != 14624 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
	// ��������� ��������� ������� �� 750 ����
	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 23408, 5000);
	if( size != 23408 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ����� ������� ������ ��� ������������ ������� �������� �� 10 ����
	for( register i = 0, j = 0; i < 40; i++ )
	{
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = LSK_Ucc; 		// �������
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_InpUl;
	}
	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 320, true);
	if(err) goto GOTO_ERROR;

	// ��������� ��������� ������� �� 10 ����
	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 320, 5000);
	if( size != 320 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

//	err = LTR34_DACStart(&Conf_LTR34);
//	if(err) goto GOTO_ERROR;

//	Sleep(100);
	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {       // ��� ����� ������
		AcqBlockReady[i] = 0;              		   // ������ � ������
	}

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.2.4 �� � �� �.5.2.7 ��.

int __fastcall WLsk::Util1_2_3Step3()throw(Exception)
{
	int err = 0;

	TerminateThread(hnd_acq_thread2, 0);

	Conf_LTR11.LChQnt = 3;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (2 << 4) | (0 << 0); // K1, �������� - 10�, ����� - 32-���������, ����� X1
	Conf_LTR11.LChTbl[1] = (0 << 6) | (2 << 4) | (1 << 0); // K2, �������� - 10�, ����� - 32-���������, ����� X2
	Conf_LTR11.LChTbl[2] = (0 << 6) | (2 << 4) | (2 << 0); // K3, �������� - 10�, ����� - 32-���������, ����� X3

	LTR11_Close(&Conf_LTR11);

	// ����� ����� � ������ LTR11 - 1
	err = LTR11_Open(&Conf_LTR11, SADDR_DEFAULT, SPORT_DEFAULT, "", 1);

	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

//	StartGen_LTR11_LTR34();
	HANDLE hnd_acq_thread;
	DWORD acq_thread_id;
	DWORD thread_status;

	hnd_acq_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AcquireThread, this, 0, &acq_thread_id);

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.2.5 ��. ������ "��2"

void __fastcall WLsk::Util2Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// ���������� 6 ������� (������� 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(6,0);	// ��1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(7,0);   // ��2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(8,0);   // ��3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(3,0);   // ��4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(4,0);   // ��5
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(1,0);   // ��������� �������
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// ������ �����
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(5,0);   // ����

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
	ArrayToSend = new DWORD[5208];  	// 8 ������ ( ����� 6, �� �� ���. ������������ ����� �� ����� 8 )
	VoltArray = new double[5208];		// �� 651 ( max) �������� ��� ������� - ����� 5208.

	// ���������� ���������� ������ �������� � ������ �� �������� � ������� 0.5 ���
	for (int i = 0,j = 0; i < 10; i++) {
	VoltArray[j++] = LSK_Uh;       	// ��1 - ���������� �������
	VoltArray[j++] = LSK_Uh;       	// ��2 - ���������� �������
	VoltArray[j++] = 0;       		// ��3 - 0�
	VoltArray[j++] = 0;       		// ��4 - 0� - ��������
	VoltArray[j++] = LSK_Uh;       	// ��5 - ���������� �������
	VoltArray[j++] = LSK_Ucc; 		// �������
	VoltArray[j++] = 0;       		// ������ �����
	VoltArray[j++] = LSK_InpUl;    	// ���� - ���������� �������
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 80, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 5000);
	if( size != 80 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 1;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 249;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(49+1)) = 100 ���

	Conf_LTR11.LChQnt = 5;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (2 << 4) | (3 << 0); // K4, �������� - 10�, ����� - 32-���������, ����� X4
	Conf_LTR11.LChTbl[1] = (0 << 6) | (3 << 4) | (0 << 0); // K5, �������� - 10�, ����� - 32-���������, ����� Y1
	Conf_LTR11.LChTbl[2] = (0 << 6) | (3 << 4) | (1 << 0); // K6, �������� - 10�, ����� - 32-���������, ����� Y2
	Conf_LTR11.LChTbl[3] = (0 << 6) | (3 << 4) | (2 << 0); // K7, �������� - 10�, ����� - 32-���������, ����� Y3
	Conf_LTR11.LChTbl[4] = (0 << 6) | (2 << 4) | (7 << 0); // K8, �������� - 10�, ����� - 32-���������, ����� X8

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
// �������� �� �.5.2.5 ��.  "��2"
int __fastcall WLsk::Util2Step2()throw(Exception)
{
	int err = 0;
	int size;
	int num_step_prev;

	// ���������� ������ VoltArray ��� ������������ �������� ������������� 1/6 ���
	for( register i = 0, j = 0; i < 651; i++ )
	{
		VoltArray[j++] = LSK_Uh;		// ��1 - ���������� �������
		VoltArray[j++] = 0;     		// ��2 - 0�
		VoltArray[j++] = 0;     		// ��3 - 0�
		VoltArray[j++] = 0;     		// ��4 - 0� - ��������
		VoltArray[j++] = LSK_Uh;     	// ��5 - ���������� �������
		VoltArray[j++] = LSK_Ucc; 		// �������
		VoltArray[j++] = 0;     		// ������ �����
		VoltArray[j++] = LSK_InpUl;    	// ���� - ���������� �������
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
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 5208, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 5208, 5000);
	if( size != 5208 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ��������� ������� �� 32 ����
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ���������� ������ ������ � ������ ��� ������������ �������� �� ��2
	// ������������� 1 ���
	for( register i = 0; i < 5; i++ )
	{
		size = LTR34_Send(&Conf_LTR34, ArrayToSend, 5208, 5000);
		if( size != 5208 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
	}

	// ��������� ������� �� ��2
	for( register i = 0, j = 1; i < 651; i++,j += 8 )
	{
		VoltArray[j] = LSK_Uh;     // ��2 - ���������� �������
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 5208, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 5208, 5000);
	if( size != 5208 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ������� �� ��3
	// ������� ������ ��� ������������ �������� ������� �������� �� 10 ����
	for( register i = 0, j = 2; i < 40; i++,j += 8 )
	{
//		VoltArray[j] = LSK_Uh;     	// ��3 - ���������� �������
    	VoltArray[j] = 0;     		// ��3 - 0
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 320, true);
	if(err) goto GOTO_ERROR;

	// ��������� ��������� ������� �� 10 ����
	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 320, 5000);
	if( size != 320 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}

}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.2.7 ��.  ������ "��3"

void __fastcall WLsk::Util3Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// ���������� 6 ������� (������� 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(6,0);	// ��1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(7,0);   // ��2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(8,0);   // ��3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(3,0);   // ��4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(4,0);   // ��5
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(1,0);   // ��������� �������
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// ������ �����
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(5,0);   // ����

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
	ArrayToSend = new DWORD[2000];  	// 8 ������ ( ����� 6, �� �� ���. ������������ ����� �� ����� 8 )
	VoltArray = new double[2000];		// �� 250 ( max) �������� ��� ������� - ����� 2000.

	// ���������� ���������� ������ �������� � ������ �� �������� � ������� 0.5 ���
	for (int i = 0,j = 0; i < 10; i++) {
	VoltArray[j++] = LSK_Uh;   			// ��1 - ���������� �������
	VoltArray[j++] = LSK_Uh;   			// ��2 - ���������� �������
	VoltArray[j++] = LSK_Uh;   			// ��3 - ���������� �������
	VoltArray[j++] = 0;        			// ��4 - 0� ��������
	VoltArray[j++] = LSK_Uh;   			// ��5 - 9�
	VoltArray[j++] = LSK_Ucc;  			// �������
	VoltArray[j++] = 0;        			// ������ �����
	VoltArray[j++] = LSK_InpUl; 		// ���� - ���������� �������
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 80, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 5000);
	if( size != 80 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 5;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (2 << 4) | (3 << 0); // K4, �������� - 10�, ����� - 32-���������, ����� X4
	Conf_LTR11.LChTbl[1] = (0 << 6) | (3 << 4) | (0 << 0); // K5, �������� - 10�, ����� - 32-���������, ����� Y1
	Conf_LTR11.LChTbl[2] = (0 << 6) | (3 << 4) | (1 << 0); // K6, �������� - 10�, ����� - 32-���������, ����� Y2
	Conf_LTR11.LChTbl[3] = (0 << 6) | (3 << 4) | (2 << 0); // K7, �������� - 10�, ����� - 32-���������, ����� Y3
	Conf_LTR11.LChTbl[4] = (0 << 6) | (2 << 4) | (7 << 0); // K8, �������� - 10�, ����� - 32-���������, ����� X8
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
// �������� �� �.5.2.7 ��.
int __fastcall WLsk::Util3Step2()throw(Exception)
{
	int err = 0;
	int size;
	int num_step_prev;

	// ���������� ������ VoltArray ��� ������������ �������� ������������� (1/3906.25)*244=62.464 ��
	for( register i = 0, j = 0; i < 244; i++ )
	{
		VoltArray[j++] = LSK_Uh;  	// ��1 - ���������� �������
		VoltArray[j++] = 0; 		// ��2 - 0�
		VoltArray[j++] = LSK_Uh;   	// ��3 - ���������� �������
		VoltArray[j++] = 0;         // ��4 - 0� - ��������
		VoltArray[j++] = LSK_Uh;   	// ��5 - ���������� �������
		VoltArray[j++] = LSK_Ucc;	// �������
		VoltArray[j++] = 0;         // ������ �����
		VoltArray[j++] = LSK_InpUl;	// ����
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
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1952, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);
	if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ��������� ������� �� 32+32 = 64 ����
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ���������� ������ ������ � ������ ��� ������������ ��������
	// ������������� 0.5 ���
	for( register i = 0; i < 7; i++ )
	{
		size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);
		if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
//		err = LTR34_DACStart(&Conf_LTR34);
//		if(err) goto GOTO_ERROR;
	}

//	// ������� ������ ��� ������������ ���-�� ��������. �� ������� ��1 = 0
//	// ������������� (1/3906.25)*244=62.464 ��
//	for( register i = 0, j = 0; i < 244; i++, j += 8 )
//	{
//		VoltArray[j] = 0;			// ��1 - 0�
//	}
	// ������� ������ ��� ������������ ���-�� ��������. �� ������� ��1 = 0
	// ������������� (1/3906.25)*98 = 25 ��
	for( register i = 0, j = 0; i < 98; i++, j += 8 )
	{
		VoltArray[j] = 0;			// ��1 - 0�
	}
	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 784/*1952*/, true);
	if(err) goto GOTO_ERROR;

//	// ��������� ��������� 2 ������ �� 62.464 �� * 2 = 124.928 ��
//	// ���������� ������ � ������
//	for( register i = 0; i < 2; i++ )
//	{
//		size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);
//		if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
//	}
	// ��������� ��������� �������� �� 25 ��
	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 784, 5000);
	if( size != 784 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ������� ������ ��� ������������ ���-�� ��������. �� ������� ��1 = ���������� �������
	// ������������� (1/3906.25)*225 = 57.6 ��
	for( register i = 0, j = 0; i < 225; i++, j += 8 )
	{
		VoltArray[j] = LSK_Uh;			// ��1 - ���������� �������
	}
	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1800, true);
	if(err) goto GOTO_ERROR;

//	// ��������� ��������� 4 ������ �� 62.464 �� * 4 = 249.856 ��
//	// ���������� ������ � ������
//	for( register i = 0; i < 4; i++ )
//	{
//		size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);
//		if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
//	}
	// ��������� ��������� 6 ������ �� 57.6 �� * 6 = 346,2 ��
	// ���������� ������ � ������
	for( register i = 0; i < 6; i++ )
	{
		size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1800, 5000);
		if( size != 1800 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
	}

//	// ������� ������ ��� ������������ ���-�� ��������. �� ������� ��2 = 0
//	// ������������� (1/3906.25)*244=62.464 ��
//	for( register i = 0, j = 1; i < 244; i++, j += 8 )
//	{
//		VoltArray[j] = 0;			// ��1 - 0�
//	}
//	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
//	// ���������, ��� ������ ����� � �������!
//	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1952, true);
//	if(err) goto GOTO_ERROR;
//
//	// ��������� ��������� 2 ������ �� 62.464 �� * 2 = 124.928 ��
//	// ���������� ������ � ������
//	for( register i = 0; i < 2; i++ )
//	{
//		size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);
//		if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
//	}

	// ����� ������� ������ ��� ������������ �������� ������� �������� �� 10 ����
	for( register i = 0, j = 0; i < 40; i++ )
	{
		VoltArray[j++] = LSK_Uh;   		// ��1 - ���������� �������
		VoltArray[j++] = LSK_Uh;       	// ��2 - 0�
		VoltArray[j++] = LSK_Uh;       	// ��3 - 0�

		VoltArray[j++] = 0;        		// ��4 - 0� - ��������
		VoltArray[j++] = LSK_Uh;       	// ��5 - ���������� �������
		VoltArray[j++] = LSK_Ucc; 		// �������
		VoltArray[j++] = 0;         	// ������ �����
		VoltArray[j++] = LSK_InpUl;    	// ����
	}
	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 320, true);
	if(err) goto GOTO_ERROR;

	// ��������� ��������� ������� �� 10 ����
	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 320, 5000);
	if( size != 320 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.2.8 � �.5.2.9 ��.

void __fastcall WLsk::Util4_5Step1(int num_util)throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;
	VoltArray = 0;

//	AcqBlockSize = ACQ_BLOCK_SIZE_2;	 // ���������� ������ ������� ����������� ����� ������ 1024, �. �. 8 ������� ���

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	������������� LTR34 - ���
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// ���������� 6 ������� (������� 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(6,0);	// ��1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(7,0);   // ��2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(8,0);   // ��3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(3,0);   // ��4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(4,0);   // ��5
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(1,0);   // ��������� �������
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// ������ �����
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(5,0);   // ����

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
	ArrayToSend = new DWORD[2000];  	// 8 ������ ( ����� 6, �� �� ���. ������������ ����� �� ����� 8 )
	VoltArray = new double[2000];		// �� 250 ( max) �������� ��� ������� - ����� 2000.

	// ���������� ���������� ������ �������� � ������ �� �������� � ������� 0.5 ���

	for (int i = 0,j = 0; i < 10; i++) {
	VoltArray[j++] = LSK_Uh;        // ��1 - 9�
	VoltArray[j++] = LSK_Uh;        // ��2 - 9�
	VoltArray[j++] = LSK_Uh;        // ��3 - 9�
	VoltArray[j++] = ((num_util == UTIL_4 )? LSK_Uh:0); // ��� �.5.2.8 ��4 - LSK_Uh
														// ��� �.5.2.9 ��4 - ��������
	VoltArray[j++] = ((num_util == UTIL_4 )? 0:LSK_Uh); // ��� �.5.2.8 ��5 - ��������
														// ��� �.5.2.9 ��5 - LSK_Uh
	VoltArray[j++] = LSK_Ucc;  		// �������
	VoltArray[j++] = 0;        		// ������ �����
	VoltArray[j++] = ((num_util == UTIL_4 )? LSK_InpUl:0);  // ����
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 80, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// ���������� ������ � ������
//	if (num_util == UTIL_4 ) {                                        //!!!!!!!!!!!!!!!!!!
//	   size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 2500);
//	}
//  else
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 5000);
	if( size != 80 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// ������� ������������� - 100 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 8;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (2 << 4) | (3 << 0); // K4, �������� - 10�, ����� - 32-���������, ����� X4
	Conf_LTR11.LChTbl[1] = (0 << 6) | (3 << 4) | (0 << 0); // K5, �������� - 10�, ����� - 32-���������, ����� Y1
	Conf_LTR11.LChTbl[2] = (0 << 6) | (3 << 4) | (1 << 0); // K6, �������� - 10�, ����� - 32-���������, ����� Y2
	Conf_LTR11.LChTbl[3] = (0 << 6) | (3 << 4) | (2 << 0); // K7, �������� - 10�, ����� - 32-���������, ����� Y3
	Conf_LTR11.LChTbl[4] = (0 << 6) | (2 << 4) | (7 << 0); // K8, �������� - 10�, ����� - 32-���������, ����� X8
	Conf_LTR11.LChTbl[5] = (0 << 6) | (2 << 4) | (0 << 0); // K1, �������� - 10�, ����� - 32-���������, ����� X1
	Conf_LTR11.LChTbl[6] = (0 << 6) | (2 << 4) | (1 << 0); // K2, �������� - 10�, ����� - 32-���������, ����� X2
	Conf_LTR11.LChTbl[7] = (0 << 6) | (2 << 4) | (2 << 0); // K3, �������� - 10�, ����� - 32-���������, ����� X3

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
// �������� �� �.5.2.8 ��.
int __fastcall WLsk::Util4_5Step2(int num_util)throw(Exception)
{
	int err = 0;
	int size;
	int num_step_prev;

	// ���������� ������ VoltArray ��� ������������ �������� ������������� (1/3906.25)*244=62.464 ��
	for( register i = 0, j = 0; i < 244; i++ )
	{
		VoltArray[j++] = LSK_Uh;        // ��1 - 9�
		VoltArray[j++] = 0;        		// ��2 - 0�
		VoltArray[j++] = LSK_Uh;        // ��3 - 9�
		VoltArray[j++] = ((num_util == UTIL_4 )? 9:0); // ��� �.5.2.8 ��4 - 9�
												   // ��� �.5.2.9 ��4 - ��������
		VoltArray[j++] = ((num_util == UTIL_4 )? 0:9); // ��� �.5.2.8 ��5 - ��������
												   // ��� �.5.2.9 ��5 - 9�
		VoltArray[j++] = LSK_Ucc;  		// �������
		VoltArray[j++] = 0;        		// ������ �����
		VoltArray[j++] =( num_util == UTIL_5 )? 0 : LSK_InpUl;    // ����

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
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1952, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
//		if (num_util == UTIL_4 ) {                                        //!!!!!!!!!!!!!!!!!!
//	   size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 2500);
//	} else
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);
	if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ��������� ������� �� 62.464 ����
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ���������� ��� 7 ������ ������ � ������ ��� ������������ ����� ��������� �� ��2
	// ������������� 0.5 ���

	for( register i = 0; i < 7; i++ )
	{
//		if (num_util == UTIL_4 ) {                                        //!!!!!!!!!!!!!!!!!!
//				  size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 2500);
//			}else
					size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);

		if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
	}

	// ����� ������� ������ ��� ������������ ���-�� ��������. �� ������� ��1 = 0
	for( register i = 0, j = 0; i < 122; i++, j += 8 )
	{
		VoltArray[j] = 0;			// ��1 - 0�
	}
	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1952, true);
	if(err) goto GOTO_ERROR;

	// ��������� ��������� 8 ������ �� 62.464 ���� = 499.712 ��
	// ���������� ������ � ������
	for( register i = 0; i < 8; i++ )
	{
//		if (num_util == UTIL_4 )
//	 {                                        //!!!!!!!!!!!!!!!!!!
//	   size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 2500);
//	}
//	else
	  size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);

		if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
	}

	// ����� ������� ������ ��� ������������ �������� ������� �������� �� 10 ����
	for( register i = 0, j = 0; i < 40; i++ )
	{
		VoltArray[j++] = LSK_Uh;			// ��1 - 9�
		VoltArray[j++] = ((num_util == UTIL_4 )? LSK_Uh:0); 	// ��� �.5.2.8 ��2 - 9�
														// ��� �.5.2.9 ��2 - 0�
		VoltArray[j++] = LSK_Uh;       		// ��3 - 0�
		VoltArray[j++] = LSK_Uh;         	// ��4 - 9�
		VoltArray[j++] = 0;       			// ��5 - ��������
		VoltArray[j++] = LSK_Ucc;			// �������
		VoltArray[j++] = 0;         		// ������ �����
		VoltArray[j++] = 0;//LSK_InpUl; 		// ����
	}
	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 320, true);
	if(err) goto GOTO_ERROR;

	// ��������� ��������� ������� �� 10 ����
	// ���������� ������ � ������
	//size = LTR34_Send(&Conf_LTR34, ArrayToSend, 320, 5000);
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 320, 5000);
	if( size != 320 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }



GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
}
//---------------------------------------------------------------------------
//void __fastcall WLsk::CheckUtil1()
//{
//	for(int i = 0; i<5; i++) {
//		checkUtil1[i] = false;
//	}
//
//	for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
//		if( AcqBuf[BlockNumberThread][i++] > 4 ) checkUtil1[0] = true;
//		if( AcqBuf[BlockNumberThread][i++] < 4 ) checkUtil1[1] = true;
//		if( AcqBuf[BlockNumberThread][i++] < 4 ) checkUtil1[2] = true;
//		if( AcqBuf[BlockNumberThread][i++] < 4 ) checkUtil1[3] = true;
//		if( AcqBuf[BlockNumberThread][i++] < 4 ) checkUtil1[4] = true;
//	}
//}//---------------------------------------------------------------------------
//void __fastcall WLsk::CheckUtil2()
//{
//	for(int i = 0; i<5; i++) {
//		checkUtil2[i] = false;
//	}
//
//	for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
//		if( AcqBuf[BlockNumberThread][i++] < 4 ) checkUtil2[0] = true;
//		if( AcqBuf[BlockNumberThread][i++] > 4 ) checkUtil2[1] = true;
//		if( AcqBuf[BlockNumberThread][i++] < 4 ) checkUtil2[2] = true;
//		if( AcqBuf[BlockNumberThread][i++] < 4 ) checkUtil2[3] = true;
//		if( AcqBuf[BlockNumberThread][i++] < 4 ) checkUtil2[4] = true;
//	}
//}
//---------------------------------------------------------------------------
DWORD WINAPI WLsk::CheckUtil1Thread (WLsk *System)
{
	int count = 0;

	while(System->RunFlag)
		{
			if(count == 2) count = 0;

			if(System->AcqBlockReady[count]) {

				for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil1[0] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil1[1] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil1[2] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil1[3] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil1[4] = true;
				}

			System->AcqBlockReady[count] = 0;
			count++;
			}
		}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
DWORD WINAPI WLsk::CheckUtil1Step3Thread (WLsk *System)
{
	int count = 0;

	for(int i=0; i<3; i++) {
		System->maxK1_K3[i] = 0;
	}

	while(System->RunFlag)
		{
			if(count == 2) count = 0;

			if(System->AcqBlockReady[count]) {

				for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
					if( System->AcqBuf[count][i++] >  System->maxK1_K3[0] )
						System->maxK1_K3[0] = System->AcqBuf[count][i-1];
					if( System->AcqBuf[count][i++] >  System->maxK1_K3[1] )
						System->maxK1_K3[1] = System->AcqBuf[count][i-1];
					if( System->AcqBuf[count][i++] >  System->maxK1_K3[2] )
						System->maxK1_K3[2] = System->AcqBuf[count][i-1];
				}

			System->AcqBlockReady[count] = 0;
			count++;
			}
		}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
DWORD WINAPI WLsk::CheckUtil2Thread (WLsk *System)
{
	int count = 0;

	while(System->RunFlag)
		{
			if(count == ACQ_BLOCK_QNT) count = 0;

			if(System->AcqBlockReady[count]) {

				for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil2[0] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil2[1] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil2[2] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil2[3] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil2[4] = true;
				}

			System->AcqBlockReady[count] = 0;
			count++;
			}
		}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
DWORD WINAPI WLsk::CheckUtil2Step3Thread (WLsk *System)
{
	int count = 0;

	for(int i=0; i<3; i++) {
		System->maxK1_K3_Util2[i] = 0;
	}

	while(System->RunFlag)
		{
			if(count == 2) count = 0;

			if(System->AcqBlockReady[count]) {

				for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
					if( System->AcqBuf[count][i++] >  System->maxK1_K3_Util2[0] )
						System->maxK1_K3_Util2[0] = System->AcqBuf[count][i-1];
					if( System->AcqBuf[count][i++] >  System->maxK1_K3_Util2[1] )
						System->maxK1_K3_Util2[1] = System->AcqBuf[count][i-1];
					if( System->AcqBuf[count][i++] >  System->maxK1_K3_Util2[2] )
						System->maxK1_K3_Util2[2] = System->AcqBuf[count][i-1];
				}

			System->AcqBlockReady[count] = 0;
			count++;
			}
		}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
DWORD WINAPI WLsk::CheckUtil3Thread (WLsk *System)
{
	int count = 0;

	while(System->RunFlag)
		{
			if(count == 2) count = 0;

			if(System->AcqBlockReady[count]) {

				for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil3[0] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil3[1] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil3[2] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil3[3] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil3[4] = true;
				}

			System->AcqBlockReady[count] = 0;
			count++;
			}
		}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
DWORD WINAPI WLsk::CheckUtil3Step3Thread (WLsk *System)
{
	int count = 0;

	for(int i=0; i<3; i++) {
		System->maxK1_K3_Util3[i] = 0;
	}

	while(System->RunFlag)
		{
			if(count == ACQ_BLOCK_QNT) count = 0;

			if(System->AcqBlockReady[count]) {

				for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
					if( System->AcqBuf[count][i++] >  System->maxK1_K3_Util2[0] )
						System->maxK1_K3_Util2[0] = System->AcqBuf[count][i-1];
					if( System->AcqBuf[count][i++] >  System->maxK1_K3_Util2[1] )
						System->maxK1_K3_Util2[1] = System->AcqBuf[count][i-1];
					if( System->AcqBuf[count][i++] >  System->maxK1_K3_Util2[2] )
						System->maxK1_K3_Util2[2] = System->AcqBuf[count][i-1];
				}

			System->AcqBlockReady[count] = 0;
			count++;
			}
		}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
DWORD WINAPI WLsk::CheckUtil4Thread (WLsk *System)
{
	int count = 0;

	for(int i=0; i<3; i++) {
		System->maxK1_K3_Util4[i] = 0;
	}

	while(System->RunFlag)
		{
			if(count == 2) count = 0;

			if(System->AcqBlockReady[count]) {

				for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil4[0] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil4[1] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil4[2] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil4[3] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil4[4] = true;
//					if( System->AcqBuf[count][i++] >  System->maxK1_K3_Util4[0] )
//						System->maxK1_K3_Util4[0] = System->AcqBuf[count][i-1];
//					if( System->AcqBuf[count][i++] >  System->maxK1_K3_Util4[1] )
//						System->maxK1_K3_Util4[1] = System->AcqBuf[count][i-1];
//					if( System->AcqBuf[count][i++] >  System->maxK1_K3_Util4[2] )
//						System->maxK1_K3_Util4[2] = System->AcqBuf[count][i-1];
					i += 3;
				}

			System->AcqBlockReady[count] = 0;
			count++;
			}
		}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
DWORD WINAPI WLsk::CheckUtil4Step3Thread (WLsk *System)
{
	int count = 0;

	for(int i=0; i<3; i++) {
		System->maxK1_K3_Util4[i] = 0;
	}

	while(System->RunFlag)
		{
			if(count == 2) count = 0;

			if(System->AcqBlockReady[count]) {

				for(int i = 5; i < ACQ_BLOCK_SIZE; ) {
					if( System->AcqBuf[count][i++] >  System->maxK1_K3_Util4[0] )
						System->maxK1_K3_Util4[0] = System->AcqBuf[count][i-1];
					if( System->AcqBuf[count][i++] >  System->maxK1_K3_Util4[1] )
						System->maxK1_K3_Util4[1] = System->AcqBuf[count][i-1];
					if( System->AcqBuf[count][i++] >  System->maxK1_K3_Util4[2] )
						System->maxK1_K3_Util4[2] = System->AcqBuf[count][i-1];
                    i += 5;
				}

			System->AcqBlockReady[count] = 0;
			count++;
			}
		}

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------
DWORD WINAPI WLsk::CheckUtil5Thread (WLsk *System)
{
	int count = 0;

//	for(int i=0; i<3; i++) {
//		System->maxK1_K3_Util4[i] = 0;
//	}

	while(System->RunFlag)
		{
			if(count == 2) count = 0;

			if(System->AcqBlockReady[count]) {

				for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil5[0] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil5[1] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil5[2] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil5[3] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil5[4] = true;
					i += 3;
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
#include "math.hpp"
void __fastcall WLsk::WriteExcel(AnsiString FileName, bool write)
{
	TDateTime t;

	if(zRaport->ExcelInit()) {
		if(zRaport->CopyRepFromSablon(FileName)) {
			zRaport->WriteData(4, 1, "� ����� " + NumberBlock);
			zRaport->WriteData(4, 3, "���� " + t.CurrentDateTime().FormatString("dd.mm.yyyy"));
			zRaport->WriteData(4, 6, "����� " + t.CurrentDateTime().FormatString("hh:nn"));

			zRaport->WriteData(9, 5, SimpleRoundTo(currentPlus, -2));    // ���
			zRaport->WriteData(9, 7, note_current);

			zRaport->WriteData(10, 5, frequencyK9);    					 // ����� �������
			zRaport->WriteData(10, 7, note_frequencyK9);

			// �.3
		 //  maxK1_K3[0] = 0; maxK1_K3[1] = 0; maxK1_K3[2] = 0;    //���������������� 16,08
			zRaport->WriteData(12, 5, SimpleRoundTo(maxK1_K3[0], -1));
			if(CheckLSK2_2[0][0] == 1) zRaport->WriteData(12, 7, "�����");
				else zRaport->WriteData(12, 7, "�������������");

			zRaport->WriteData(13, 5, SimpleRoundTo(maxK1_K3[1], -1));
			if(CheckLSK2_2[0][1] == 1) zRaport->WriteData(13, 7, "�����");
				else zRaport->WriteData(13, 7, "�������������");

			zRaport->WriteData(14, 5, SimpleRoundTo(maxK1_K3[2], -1));
			if(CheckLSK2_2[0][2] == 1) zRaport->WriteData(14, 7, "�����");
				else zRaport->WriteData(14, 7, "�������������");

			// �.4
		   	maxK1_K3_Util4[0] = 0; maxK1_K3_Util4[1] = 0; maxK1_K3_Util4[2] = 0;   //���������������� 16,08
			zRaport->WriteData(16, 5, SimpleRoundTo(maxK1_K3_Util4[0], -1));
			if(CheckLSK2_2[3][0] == 1) zRaport->WriteData(16, 7, "�����");
				else zRaport->WriteData(16, 7, "�������������");

			zRaport->WriteData(17, 5, SimpleRoundTo(maxK1_K3_Util4[1], -1));
			if(CheckLSK2_2[3][1] == 1) zRaport->WriteData(17, 7, "�����");
				else zRaport->WriteData(17, 7, "�������������");

			zRaport->WriteData(18, 5, SimpleRoundTo(maxK1_K3_Util4[2], -1));
			if(CheckLSK2_2[3][2] == 1) zRaport->WriteData(18, 7, "�����");
				else zRaport->WriteData(18, 7, "�������������");

			// �.5
			if(PerepadUtil5[4] == true) {
				zRaport->WriteData(19, 5, "���� �������");
				zRaport->WriteData(19, 7, "�����");
			} else {
				zRaport->WriteData(19, 5, "��� ��������");
				zRaport->WriteData(19, 7, "�������������");
			}

			// �.6
			if(checkBlock) {
				zRaport->WriteData(20, 2, "���� ��������");
			}	else {
				zRaport->WriteData(20, 2, "���� ����������");
			}


			if(write) {
				zRaport->CloseReport("C:\\��������� ����\\���������\\���-05\\069\\069-" + NumberBlock + ".xlsx");
			} else {
				zRaport->CloseReport(GetCurrentDir() + "\\~" + NumberBlock + ".xlsx");
			}
        }
    }
}
//---------------------------------------------------------------------------
// �������� ������ ��� ������ ��� 1
// FileName - ��� �����, ������� ����� ����������� ("\\RaportIK.xlsx")
// write - ���������� ��� ��� ����� ������� ���� ����
#include "math.hpp"
void __fastcall WLsk::WriteExcel_LSK_1(AnsiString FileName, bool write)
{
	TDateTime t;

	if(zRaport->ExcelInit()) {
		if(zRaport->CopyRepFromSablon(FileName)) {
			zRaport->WriteData(4, 1, "� ����� " + NumberBlock);
			zRaport->WriteData(4, 3, "���� " + t.CurrentDateTime().FormatString("dd.mm.yyyy"));
			zRaport->WriteData(4, 6, "����� " + t.CurrentDateTime().FormatString("hh:nn"));

			zRaport->WriteData(9, 5, SimpleRoundTo(currentPlus, -2));    // ���
			zRaport->WriteData(9, 7, note_current);

			zRaport->WriteData(10, 5, frequencyK9);    					 // ����� �������
			zRaport->WriteData(10, 7, note_frequencyK9);

			// �.6
			if(checkBlock) {
				zRaport->WriteData(20, 2, "���� ��������");
			}	else {
				zRaport->WriteData(20, 2, "���� ����������");
			}

			if(write) {
				zRaport->CloseReport("C:\\��������� ����\\���������\\���-05\\069\\069-" + NumberBlock + ".xlsx");
			} else {
				zRaport->CloseReport(GetCurrentDir() + "\\~" + NumberBlock + ".xlsx");
			}

		}
	}
}
//-------------------------------/ END /-------------------------------------
