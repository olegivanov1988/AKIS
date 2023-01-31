//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WAsk.h"
#include "UnRaport.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
// ��������� �������� ��� � �������
static const char ErrorText1[] = "������ ��� �������� ������ ������!";
static const char ErrorText2[] = "������! ���������� ������������ ������� �� ����� ���������!";
static const char ErrorText3[] =  "����������� ������";
//--------------------------/ CONSTRUCTOR /----------------------------------
WAsk::WAsk():ArrayToSend(0),VoltArray(0)
{
	BlockNumberThread = 100; // ������������� ���������� �� ������ ���� ����� ��� ����� ������ �������
	ReplaceBuf = false;      // ������������� ���������� ��������� ������ �������
	ReplaceBuf2 = false;      // ������������� ���������� ��������� ������ �������
	RunFlag = 0;
	RunFlag2 = 0;

	zRaport = new ZRaport;

//	// �������������� ������ ����������� ����������
//	for(int i=0; i<PARAMETERS; i++) {
//		CheckASK1[i] = false;
//	}
	LTR34_Init(&Conf_LTR34);
	LTR11_Init(&Conf_LTR11);
	LTR27_Init(&Conf_LTR27);
}
//--------------------------/ DESTRUCTOR /-----------------------------------
WAsk::~WAsk()
{
	delete zRaport;
//	delete ArrayToSend;
//	delete VoltArray;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
void __fastcall WAsk::Init_LTR11_LTR34()throw(Exception)
{
	int err = 0;

	//������������� ������������ �������
	err = LTR34_Init(&Conf_LTR34);
	if(err) goto GOTO_ERROR2;

	err = LTR11_Init(&Conf_LTR11);
	if(err) goto GOTO_ERROR1;
	Conf_LTR11.ModuleInfo.CbrCoef[0].Gain = 1.02;
//	Conf_LTR11.ModuleInfo.CbrCoef[0].Offset = 50; 	// �������� ��

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

void __fastcall WAsk::Init_LTR27()throw(Exception)
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

					Conf_LTR27.Mezzanine[0].CalibrCoeff[0] = 1.47;	// ����������� �������� �� 03.06.15
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
void __fastcall WAsk::StartGen_LTR11_LTR34()throw(Exception)
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
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
 DWORD WINAPI WAsk::AcquireThread (WAsk *System)
{
	int err;
	DWORD data_buf[ACQ_BLOCK_SIZE];

	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {       // ����� ��������� ������ ��� ����� ������
		System->AcqBlockReady[i] = 0;              // ������ � ������
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
	} else
	 System->RunFlag = 0;

	}

    LTR11_Stop(&System->Conf_LTR11);
	ExitThread((DWORD)err);

	return 0;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
 DWORD WINAPI WAsk::AcquireThread2 (WAsk *System)
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
void __fastcall WAsk::ErrorRelease( int err )throw(Exception)
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
int __fastcall WAsk::UtilStop( bool power_off )
{
	int size;
	int err;
	double volt_array[] = { 0,0,0,0,ASK_Ucc,0,0,0 };

	RunFlag = 0;            // ������� �� ������ ����� ������������� ���������� �� LTR11
	RunFlag2 = 0; 			// ������� �� ������ ����� ������������� ���������� �� LTR27 ( ��� )

	if( LTR34_IsOpened( &Conf_LTR34 ) == 0 )
	{
		err = LTR34_DACStop( &Conf_LTR34 );
		if(err) goto GOTO_ERROR;
	}else Init_LTR11_LTR34();


	Conf_LTR34.ChannelQnt = 8;
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ���1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ���2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ���3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(8,0);   // ���4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(1,0);   // �������
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(2,0);   // ������ �����
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(3,0);   // ������ �����
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(4,0);   // ������ �����

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
// �������� �� �.5.1.1 ��.
void __fastcall WAsk::Util0Step1()throw(Exception)
{
	int err = 0;
	int size;
	DWORD thread_status;
	DWORD acq_thread_id;
	HANDLE hnd_acq_thread;
	ArrayToSend = 0;

	Init_LTR27();
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
	VoltArray[0] = ASK_Ucc;

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
	Conf_LTR11.ADCRate.prescaler = 1;                 		// ������� ������������� - 100 ���, �� ����� - 50 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 ���

	Conf_LTR11.LChQnt = 2;                            		// ���������� ���������� ������� - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (2 << 0); // K7 - �������� - 10�, ����� - 16-���������, ���������� ����� - 2 (��������� � ���� )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (3 << 0); // K8 - �������� - 10�, ����� - 16-���������, ���������� ����� - 3 (��������� � ���� )


	// ��������� ������������ ������ ���
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	// ��������� ���������
	AcqBlockReady[0] = 0;           // ����� ���������� ��� ����� ������
	AcqBlockReady[1] = 0;           // ������ � ������
	StartGen_LTR11_LTR34();
	RunFlag2 = 1;				  	//
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
// �������� �� �.5.1.2 ��.
double WAsk::Util1ResultSet[] = { 0.245, 0.376, 0.545, 0.790, 1.15, 1.65, 2.38, 3.45 };
double WAsk::DeltaErrorUtil1[] = { 0.05, 0.05, 0.05, 0.08, 0.12, 0.17, 0.24, 0.35 };
double WAsk::VoltArrayTable[9][4] = { {0,0,0,ASK_Ucc}, {8,0,0,ASK_Ucc}, {0,8,0,ASK_Ucc}, {8,8,0,ASK_Ucc},{0,0,8,ASK_Ucc},{8,0,8,ASK_Ucc},
{0,8,8,ASK_Ucc}, {8,8,8,ASK_Ucc}, {0,0,0,ASK_Ucc}};
void __fastcall WAsk::Util1Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;

	Init_LTR11_LTR34();

	//--------- ������� ���������� ������ ��� ---------
	Conf_LTR34.ChannelQnt = 4;
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ���1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ���2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ���3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);   // �������

	Conf_LTR34.FrequencyDivisor = 40; 		 			// �������� ������� �������������

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
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
#include "math.h"
int __fastcall WAsk::Util1Step2_8()throw(Exception)
{
	int err = 0;
	int size;
	int num_step_prev;

	num_step_prev = NumStep++;	   	// ���������� ��� �������� � ���������

	if( NumStep > 8 ) { RunFlag = 0; return RET_END; }

	// ������� ������� ���������� ��������� ��� - ����������
	Util1Porog[num_step_prev] = Util1Result = AcqBuf[BlockNumberThread][512];
	// ��������� �� ���������� �������
	if(fabs(Util1Result) < 0.1) return RET_NO_U;
    // ������ ��������� �� ���������� ���������� �� ��������
	if( fabs( Util1Result - Util1ResultSet[	num_step_prev] ) > DeltaErrorUtil1[	num_step_prev] )
	return RET_ERROR;

//	if( NumStep > 7 ) { RunFlag = 0; return RET_END; }

	// ���������� ������ VoltArray
	for( register i = 0; i < 4; i++ )
	VoltArray[i] = VoltArrayTable[NumStep][i];

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 4, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 4, 5000);
	if( size != 4 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

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
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.1.3 ��.
// ��������� ��������� ������������� ��������� �� ���� ������� ( ������� ������,
// ��� ��� ���. ���. ������� ����� ���� 4 ) �������� 15,625�0,25��, 31,25�0,5��,
// 62,5�1�� �� ������� OUT5-1:1,OUT6-1:1,OUT7-1:1( �������� ���1,���2,���3 ).
void __fastcall WAsk::Util2Step1()throw(Exception)
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
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ���1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ���2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ���3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);   // �������

	Conf_LTR34.FrequencyDivisor = 32; 		// �������� ������� �������������
											// Fch=Fs/ NumberOfChannel=2 ��� / ((64 - FrequencyDivisor)* NumberOfChannel).
											// �������� Fch =  15625 ��.
//	Conf_LTR34.UseClb = true; 				// ���������� ��������� ������������� ������������
//	Conf_LTR34.AcknowledgeType = false; 		// ��� ������������� - �������������
//	Conf_LTR34.ExternalStart = false; 		// ������� ����� ��������
//	Conf_LTR34.RingMode = true; 				// ������� ����� �������������

	// ����� ������������� ����������� ��������� ����� ������
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// ��������� ������������ ������
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

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
	VoltArray[j++] = ASK_Ucc; //( (((  i/125 ) + 1 )%2) == 0 )? 8 : 0;
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 4000, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 4000, 5000);
	if( size != 4000 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
//	Conf_LTR11.StartADCMode = LTR11_STARTADCMODE_INT; 		// ����� ������ ����� ������ - ����������
//	Conf_LTR11.InpMode = LTR11_INPMODE_INT;           		// ����� ������������� ��� - ����������
//	Conf_LTR11.ADCMode = LTR11_ADCMODE_ACQ;           		// ����� ����� ������

	Conf_LTR11.ADCRate.prescaler = 64;                 		// ������� ������������� - 1,5625 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (64*(149+1)) = 1,5625 ���

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
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.1.4 ��.
// ��������� ��������� ������������� ��������� 125 �� �� ������ OUT8-1:1 (������� ���4 ),
// �� ������� OUT5-1:1,OUT6-1:1,OUT7-1:1 (�������� ���1,���2,���3) - 8�.
// �� ������ OUT1-1:1 - 8� - ������� ��������� �������
void __fastcall WAsk::Util3Step1()throw(Exception)
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
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(3,0);   // ��1 - ������ �����
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
	VoltArray[j++] = ASK_Ucc;
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	}

	// �������� ������� ���������� ����-������. ArrayToSend - ������, ������� � ��������.
	// ���������, ��� ������ ����� � �������!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 320, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 320, 5000);
	if( size != 320 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

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
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// �������� �� �.5.1.6 ��.
// ��������� ������� ���������� �.5.1.4 �� ( Util3Step1() ) + ��������� ��������������
// ������ �������� 30�� �� ������ OUT2-1:10 (������� ��1).
// ��������� ��������� ������������� ��������� �� ������ OUT8-1:1 (������� ���4 ),
// �� ������� OUT5-1:1,OUT6-1:1,OUT7-1:1 (�������� ���1,���2,���3) - 8�.
// �� ������ OUT1-1:1 - 7.5� - ������� ��������� �������
// �� ������ OUT2-1:10 - ����� 30��.
void __fastcall WAsk::Util4Step1()throw(Exception)
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
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(3,0);   // ��1 - 30 ��
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// ������ �����
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
	VoltArray[j++] = ASK_Ucc;
	VoltArray[j++] = 0.69*sin(188.4955592*t);
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
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 64;                 		// ������� ������������� - 1,5625 ���
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (64*(149+1)) = 1,5625 ���

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
//double WAsk::AmpUtil5[] = { 0.045, 0.380, 0.630, 0.880 };
double WAsk::AmpUtil5[] = { 0.09, 0.8, 1.4, 2.2 };
void __fastcall WAsk::Util5Step1()throw(Exception)
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
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(1,0);   // ��������� �������
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(3,0);   // ��1 - 30 ��
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// ������ �����
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
	VoltArray[j++] = ASK_Ucc;
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
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 64;                 		// ������� ������������� - 1,5625 ��� �� ����� - 390,625 ��
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (64*(149+1)) = 1,5625 ���

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
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
double WAsk::SwitchKomparator[4][3] = {{10,10,10}, {1,10,10}, {1,1,10}, {1,1,1}};
int __fastcall WAsk::Util5Step2_4()throw(Exception)
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
	for(int i=0;i<3;i++) Util5ResultReport[num_step_prev][i] = Util5Result[i] = resultValue[i] / 10;

	// ��������
//	for(int i=0; i<3; i++) {
//		if(SwitchKomparator[num_step_prev][i] < Util5Result[i])
//			return RET_ERROR;
//    }
	switch(NumStep - 1)
	{
		case 0:{for(int i=0,j=0; i<3; i++,j+=2) {
					if(Util5Result[i] < 4) {
						noteUtil5[j] = "�������������";
						return RET_ERROR;
					} else {
						noteUtil5[j] = "�����";
					}
				}
				break;}
		case 1:{if(Util5Result[0] > 4 || Util5Result[1] < 4 || Util5Result[2] < 4) {
					if(Util5Result[0] > 4) noteUtil5[1] = "�������������";
					if(Util5Result[1] < 4) noteUtil5[2] = "�������������";
					return RET_ERROR;
				} else {
					noteUtil5[1] = "�����";
					noteUtil5[2] = "�����";
				}
				break;}
		case 2:{if(Util5Result[0] > 4 || Util5Result[1] > 4 || Util5Result[2] < 4) {
					if(Util5Result[1] > 4) noteUtil5[3] = "�������������";
					if(Util5Result[2] < 4) noteUtil5[4] = "�������������";
					return RET_ERROR;
				} else {
					noteUtil5[3] = "�����";
					noteUtil5[4] = "�����";
				}
				break;}
		case 3:{for(int i=0,j=1; i<3; i++,j+=2) {
					if(Util5Result[i] > 4) {
						noteUtil5[j] = "�������������";
						return RET_ERROR;
					} else {
						noteUtil5[j] = "�����";
					}
				}
				break;}
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
		VoltArray[j] = AmpUtil5[NumStep]*sin(188.4955592*t);
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
	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {       // ��� ����� ������
		AcqBlockReady[i] = 0;              // ������ � ������
	}

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
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
int WAsk::KolSemplKanalUtil6[] = { 167, 200, 80,   60,   50,  42    }; 	// �����/�����
float WAsk::FreqUtil6[]  = 		{ 30,  50,  62.5, 83.3, 100, 120   };	// ��
//float WAsk::AmpUtil6[] = { 1.3, 1.56, 1.82, 2.08, 2.34, 2.6 };			// ��
float WAsk::AmpUtil6[] = { 0.66, 1.56, 1.82, 2.08, 2.34, 1.54 };			// ��
void __fastcall WAsk::Util6Step1()throw(Exception)
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
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(3,0);   // ��1 - 30 ��
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// ������ �����
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
	VoltArray[j++] = ASK_Ucc;
	VoltArray[j++] = 0.66*sin(188.4955592*t);
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
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	������������� LTR11 - ���
	//-----------------------------------------------------------------------
	//---------������� ���������� ������ ���-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// ������� ������������� - 12,5 ��� �� ����� - 4167 ��
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (8*(149+1)) = 12,5 ���

	Conf_LTR11.LChQnt = 3;                            		// ���������� ���������� ������� - 3
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (10 << 0); // K1, �������� - 10�, ����� - 16-���������, ���������� ����� - 10 ( ��������� � ���� )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (1 << 0); 	// K6, �������� - 10�, ����� - 16-���������, ���������� ����� - 1 ( ��������� � ���� )
	Conf_LTR11.LChTbl[2] = (0 << 6) | (0 << 4) | (0 << 0); 	// K5, �������� - 10�, ����� - 16-���������, ���������� ����� - 0 ( ��������� � ���� )

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
int __fastcall WAsk::Util6Step2_6()throw(Exception)
{
	int err = 0;
	int size;
	double t = 0;
	int len;

//	NumStep++;	   	// ��������� ��� ��������
	NumStep += 5;	// ����� ��������� ��� ��������

	if( NumStep >= 6 )
	{ RunFlag = 0; return RET_END; }

	// ������������ ������ VolArray[] - ���������: �������� ������� ���������
	// ������ Conf_LTR34.LChTbl[5] ( ��1 ).
	for( int i = 0,j = 5; i < KolSemplKanalUtil6[NumStep]; i++, j+=8 )
	{
		VoltArray[j] = AmpUtil6[NumStep]*sin(2*M_PI*FreqUtil6[NumStep]*t);
		t += 0.0002;			// ������ 0.0002 ��� - 5000 ��
	}

	len =  KolSemplKanalUtil6[NumStep] * 8;

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
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, len, true);
	if(err) goto GOTO_ERROR;

	// ���������� ������ � ������
	size = LTR34_Send(&Conf_LTR34, ArrayToSend,  len, 5000);
	if( size !=  len ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// ��������� ���������
	err = LTR34_DACStart(&Conf_LTR34);
	Sleep(100);
	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {       // ��� ����� ������
		AcqBlockReady[i] = 0;              // ������ � ������
	}

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
//���� �0� � ������������ ����� ����� � ������� ������ � ������.
//���� �������� ��������� - ����� �� ������
int __fastcall WAsk::IsOpenedLTR34()
{
	return LTR34_IsOpened(&Conf_LTR34);
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// FileName - ��� �����, ������� ����� ����������� ("\\RaportIK.xlsx")
// write - ���������� ��� ��� ����� ������� ���� ����
#include "math.hpp"
//static char Text1[] = "%.1f";
void __fastcall WAsk::WriteExcel(AnsiString FileName, bool write)
{
	TDateTime t;
//	AnsiString form_string;

//	NumberBlock = "22";
//	AnsiString Dir = "C:\\��������� ����\\���������\\068";
//	currentPlus = 997.789;

	if(zRaport->ExcelInit()) {
		if(zRaport->CopyRepFromSablon(FileName)) {
			zRaport->WriteData(4, 1, "� ����� " + NumberBlock);
			zRaport->WriteData(4, 3, "���� " + t.CurrentDateTime().FormatString("dd.mm.yyyy"));
			zRaport->WriteData(4, 6, "����� " + t.CurrentDateTime().FormatString("hh:nn"));

//			form_string.printf(Text1, currentPlus);
			zRaport->WriteData(9, 5, SimpleRoundTo(currentPlus, -1));    // ���
			zRaport->WriteData(9, 7, note_current);

			zRaport->WriteData(11, 5, SimpleRoundTo(Uplus, -1));         // ���������� ������������� ����������
			zRaport->WriteData(11, 7, noteUplus);
			zRaport->WriteData(12, 5, SimpleRoundTo(Uminus, -1));        // ���������� ������������� ����������
			zRaport->WriteData(12, 7, noteUminus);
			zRaport->WriteData(14, 5, SimpleRoundTo(Util1Porog[0], -2)); // ������
			zRaport->WriteData(14, 7, noteUtil1Porog[0]);
			zRaport->WriteData(15, 5, SimpleRoundTo(Util1Porog[1], -2)); //
			zRaport->WriteData(15, 7, noteUtil1Porog[1]);
			zRaport->WriteData(16, 5, SimpleRoundTo(Util1Porog[2], -2)); //
			zRaport->WriteData(16, 7, noteUtil1Porog[2]);
			zRaport->WriteData(17, 5, SimpleRoundTo(Util1Porog[3], -2)); //
			zRaport->WriteData(17, 7, noteUtil1Porog[3]);
			zRaport->WriteData(18, 5, SimpleRoundTo(Util1Porog[4], -2)); //
			zRaport->WriteData(18, 7, noteUtil1Porog[4]);
			zRaport->WriteData(19, 5, SimpleRoundTo(Util1Porog[5], -2)); //
			zRaport->WriteData(19, 7, noteUtil1Porog[5]);
			zRaport->WriteData(20, 5, SimpleRoundTo(Util1Porog[6], -2)); //
			zRaport->WriteData(20, 7, noteUtil1Porog[6]);
			zRaport->WriteData(21, 5, SimpleRoundTo(Util1Porog[7], -2)); //
			zRaport->WriteData(21, 7, noteUtil1Porog[7]);

			zRaport->WriteData(23, 5, SimpleRoundTo(Util5ResultReport[0][0], -1)); //
			zRaport->WriteData(23, 7, noteUtil5[0]);
			if(Util5ResultReport[1][0] < 0) {
				zRaport->WriteData(24, 5, 0);
			} else zRaport->WriteData(24, 5, SimpleRoundTo(Util5ResultReport[1][0], 0)); //
			zRaport->WriteData(24, 7, noteUtil5[1]);

			zRaport->WriteData(25, 5, SimpleRoundTo(Util5ResultReport[1][1], -1)); //
			zRaport->WriteData(25, 7, noteUtil5[2]);
			if(Util5ResultReport[2][1] < 0) {
				zRaport->WriteData(26, 5, 0);
			} else zRaport->WriteData(26, 5, SimpleRoundTo(Util5ResultReport[2][1], 0)); //
			zRaport->WriteData(26, 7, noteUtil5[3]);

			zRaport->WriteData(27, 5, SimpleRoundTo(Util5ResultReport[2][2], -1)); //
			zRaport->WriteData(27, 7, noteUtil5[4]);
			if(Util5ResultReport[3][2] < 0) {
				zRaport->WriteData(28, 5, 0);
			} else zRaport->WriteData(28, 5, SimpleRoundTo(Util5ResultReport[3][2], 0)); //
			zRaport->WriteData(28, 7, noteUtil5[5]);

			zRaport->WriteData(29, 5, note_CK3_5XP1_30); // ��-4
			zRaport->WriteData(29, 7, note2_CK3_5XP1_30);
			zRaport->WriteData(30, 5, note_CK3_5XP1_120); // ��-4
			zRaport->WriteData(30, 7, note2_CK3_5XP1_120);

			zRaport->WriteData(31, 5, note_CK3_7XP1_30);
			zRaport->WriteData(31, 7, note2_CK3_7XP1_30);
			zRaport->WriteData(32, 5, note_CK3_7XP1_120);
        	zRaport->WriteData(32, 7, note2_CK3_7XP1_120);

			if(write) {
				zRaport->CloseReport("C:\\��������� ����\\���������\\���-05\\068\\068-" + NumberBlock + ".xlsx");
			} else {
				zRaport->CloseReport(GetCurrentDir() + "\\~" + NumberBlock + ".xlsx");
			}
		}
	}

}
//------------------------/ MEMBER FUNCTION /--------------------------------
DWORD WINAPI WAsk::CheckUtil6Thread (WAsk *System, int step)
{
//
}
//------------------------/ MEMBER FUNCTION /--------------------------------
/*int __fastcall WAsk::Init_LTR11_LTR43()
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
int __fastcall WAsk::Config_LTR11_LTR34()
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



