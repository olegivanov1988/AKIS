//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WSystem.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
//static DWORD WINAPI AcquireThread (LPVOID param);
// Выводимые значения АЦП в вольтах
static const char ErrorText1[] = "Ошибка при отправке данных модулю!";
static const char ErrorText2[] = "Ошибка! Количество отправленных сэмплов не равно заданному!";
static const char ErrorText3[] =  "Неизвестная ошибка";
//--------------------------/ CONSTRUCTOR /----------------------------------
WSystem::WSystem():ArrayToSend(0),VoltArray(0)
{
	BlockNumberThread = 100; // инициализация переменной на случай если поток АЦП будет создан позднее
	ReplaceBuf = false;      // иництализация переменной повторной записи буферов
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

	//инициализация дескрипторов модулей
	err = LTR34_Init(&Conf_LTR34);
	if(err) goto GOTO_ERROR2;

	err = LTR11_Init(&Conf_LTR11);
	if(err) goto GOTO_ERROR1;

	//открытие каналов связи с модулями
	// Номер слота с платой LTR34 - 3
	err = LTR34_Open(&Conf_LTR34, SADDR_DEFAULT, SPORT_DEFAULT, "", 3);
	if(err) goto GOTO_ERROR1;
	// Номер слота с платой LTR11 - 1
	err = LTR11_Open(&Conf_LTR11, SADDR_DEFAULT, SPORT_DEFAULT, "", 1);

	// Заполняем общие поля конфигурации модуля LTR34
	Conf_LTR34.UseClb = true; 				// Используем заводские калибровочные коэффициенты
	Conf_LTR34.AcknowledgeType = false; 	// тип подтверждения - периодические
	Conf_LTR34.ExternalStart = false; 		// внешний старт отключен
	Conf_LTR34.RingMode = true; 			// включен режим автогенерации
	// Заполняем общие поля конфигурации модуля LTR11
	Conf_LTR11.StartADCMode = LTR11_STARTADCMODE_INT; 		// режим старта сбора данных - внутренний
	Conf_LTR11.InpMode = LTR11_INPMODE_INT;           		// режим синхронизации АЦП - внутренний
	Conf_LTR11.ADCMode = LTR11_ADCMODE_ACQ;           		// режим сбора данных

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

	// Запускаем генерацию ЦАП
	err = LTR34_DACStart(&Conf_LTR34);

	// создание потока сбора данных от АЦП модуля
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

	System->AcqBlockReady[0] = 0;                                  // перед созданием потока все блоки данных
	System->AcqBlockReady[1] = 0;                                  // готовы к записи

	// тайм-аут ожидания одного блока данных в мс
	const DWORD acq_time_out = (DWORD)(ACQ_BLOCK_SIZE / (System->Conf_LTR11.ChRate * System->Conf_LTR11.LChQnt) + 1000);

	// запуск сбора данных
	err = LTR11_Start(&System->Conf_LTR11);
	if(err) {
		(void)LTR11_Stop(&System->Conf_LTR11);
		ExitThread(err);
		return 0;
	}

	while (System->RunFlag)
	{
	// получение данных от LTR11
	err = LTR_Recv(&System->Conf_LTR11.Channel, data_buf, NULL, ACQ_BLOCK_SIZE, acq_time_out);
	if (err > 0 && err == ACQ_BLOCK_SIZE)
	{
		int block_number;
		// без ошибок принято ожидаемое количество данных

		// поиск свободного буфера
		block_number = 0;
		while (block_number < ACQ_BLOCK_QNT && System->AcqBlockReady[block_number])
			block_number++;


		if (block_number < ACQ_BLOCK_QNT)
		{
			int data_size = ACQ_BLOCK_SIZE;
			// сохранение принятых и обработанных данных в буфере
			err = LTR11_ProcessData(&System->Conf_LTR11, data_buf, System->AcqBuf[block_number], &data_size, TRUE, TRUE);
			System->BlockNumberThread = block_number; // номер блока данных для scope-потока
			if(err) {
				System->RunFlag = 0;
			} else {
				if(System->ReplaceBuf == false) System->AcqBlockReady[block_number] = 1;             // true - пишем повторно в буферы
			}
		}
	} else System->RunFlag = 0;

	}

	ExitThread((DWORD)err);

	return 0;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// Проверка по п.5.1.1 ТЗ.
void __fastcall WSystem::Util0Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;

	Init_LTR11_LTR34();

	//--------- Задание параметров работы ЦАП ---------
	Conf_LTR34.ChannelQnt = 1;		  			// Используем один канал - канал включения питания
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(1,0);

	Conf_LTR34.FrequencyDivisor = 40; 		 	// делитель частоты дискретизации

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// Номер шага проверки = 0
	// Определяем массив ArrayToSend
	ArrayToSend = new DWORD[1];
	VoltArray = new double[1];
	VoltArray[0] = 7.5;

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1, 5000);
	if( size != 1 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 1;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 2;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (2 << 0); // диапазон - 10В, режим - 16-канальный, физический канал - 2 (нумерация с нуля )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (3 << 0); // диапазон - 10В, режим - 16-канальный, физический канал - 3 (нумерация с нуля )

	// Выполняем конфигурацию модуля АЦП
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	// Запускаем генерацию
	AcqBlockReady[0] = 0;                                  // перед генерацией все блоки данных
	AcqBlockReady[1] = 0;                                  // готовы к записи
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
// Проверка по п.5.1.2 ТЗ.
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

	//--------- Задание параметров работы ЦАП ---------
	Conf_LTR34.ChannelQnt = 4;						// Необходимо три канала, один канал лишний
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	//
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);

	Conf_LTR34.FrequencyDivisor = 40; 		 	// делитель частоты дискретизации

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);                               // !!!!!! без тоже работает
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// Номер шага проверки = 0
	// Определяем массив ArrayToSend
	ArrayToSend = new DWORD[4];
	VoltArray = new double[4];
	for( register i = 0; i < 4; i++ )
	VoltArray[i] = VoltArrayTable[0][i];

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 4, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 4, 5000);
	if( size != 4 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 1;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 1;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (4 << 0); // диапазон - 10В, режим - 16-канальный, физический канал - 4 (нумерация с нуля )

	// Выполняем конфигурацию модуля АЦП
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	// Запускаем генерацию
	AcqBlockReady[0] = 0;                                  // перед генерацией все блоки данных
	AcqBlockReady[1] = 0;                                  // готовы к записи
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

	num_step_prev = NumStep++;	   	// Предыдущий шаг проверки и следующий

	if( NumStep > 8 ) { RunFlag = 0; return RET_END; }

	// Сначала смотрим предыдущий результат АЦП - напряжения
	Util1Result = AcqBuf[BlockNumberThread][512];
	if( fabs( Util1Result - Util1ResultSet[	num_step_prev] ) > DeltaErrorUtil1[	num_step_prev] )
	return RET_ERROR;

	if(fabs(Util1Result) < 0.01) 		   // !!!
		return RET_NO_U;

//	if( NumStep > 7 ) { RunFlag = 0; return RET_END; }

	// Определяем массив VoltArray
	for( register i = 0; i < 4; i++ )
	VoltArray[i] = VoltArrayTable[NumStep][i];

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 4, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 4, 5000);
	if( size != 4 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }//throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	// Запускаем генерацию
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

	// Включаем потоковый режим
	Conf_LTR34.RingMode = false; 				// Включен потоковый режим
	Conf_LTR34.FrequencyDivisor = 0;			// Работаем на максимальной частоте

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	if( power_off == true ) volt_array[4] = 0;
	delete ArrayToSend;
	ArrayToSend = new DWORD[8];
	err = LTR34_ProcessData(&Conf_LTR34, volt_array, ArrayToSend, 8, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 8, 5000);
	if( size != 8 )  { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// Запускаем генерацию
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;
		// Ждем окончания потоковой передачи
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
// Проверка по п.5.1.3 ТЗ.
// Запускаем генерацию прямоугольных импульсов по трем каналам ( реально четыре,
// так как мин. кол. каналов может быть 4 ) частотой 15,625±0,25Гц, 31,25±0,5Гц,
// 62,5±1Гц на каналах OUT5-1:1,OUT6-1:1,OUT7-1:1( контакты ВхУ1,ВхУ2,ВхУ3 ).
void __fastcall WSystem::Util2Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;
	VoltArray = 0;

	// Заполнение массива нулями
	for(int i=0; i<ACQ_BLOCK_QNT; i++) {
		for(int j=0; j<ACQ_BLOCK_SIZE; j++) {
			AcqBuf[i][j] = 0;
        }
    }

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 4;						// Необходимо три канала, один канал лишний
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	//
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);

	Conf_LTR34.FrequencyDivisor = 32; 		// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  15625 Гц.
//	Conf_LTR34.UseClb = true; 				// Используем заводские калибровочные коэффициенты
//	Conf_LTR34.AcknowledgeType = false; 		// тип подтверждения - периодические
//	Conf_LTR34.ExternalStart = false; 		// внешний старт отключен
//	Conf_LTR34.RingMode = true; 				// включен режим автогенерации

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	NumStep = 0;		// Номер шага проверки = 0
	// Определяем массив ArrayToSend
	// Определяем массив VoltArray
	ArrayToSend = new DWORD[4000];  	// 4 канала ( нужно 3, но по тех. документации можно не менее 4 )
	VoltArray = new double[4000];		// по 1000 значений для каждого - итого 4000

	// заполняем значение массива для отсылки - чтобы генерились импульсы с частотой;
	// 1 канал - 15,625 Гц, 2 канал - 31,25 Гц, 3 канал - 62,5 Гц
	for (int i = 0,j = 0; i < 1000; i++) {
	VoltArray[j++] = ( (((  i/125 ) + 1 )%2) == 0 )? 8 : 0;
	VoltArray[j++] = ( (((  i/250 ) + 1 )%2) == 0 )? 8 : 0;
	VoltArray[j++] = ( (((  i/500 ) + 1 )%2) == 0 )? 8 : 0;
	VoltArray[j++] = 7.5; //( (((  i/125 ) + 1 )%2) == 0 )? 8 : 0;
	}

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 4000, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 4000, 5000);
	if( size != 4000 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
//	Conf_LTR11.StartADCMode = LTR11_STARTADCMODE_INT; 		// режим старта сбора данных - внутренний
//	Conf_LTR11.InpMode = LTR11_INPMODE_INT;           		// режим синхронизации АЦП - внутренний
//	Conf_LTR11.ADCMode = LTR11_ADCMODE_ACQ;           		// режим сбора данных

	Conf_LTR11.ADCRate.prescaler = 64;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 1;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (4 << 0); // диапазон - 10В, режим - 16-канальный, физический канал - 4 ( нумерация с нуля )

	// Выполняем конфигурацию модуля АЦП
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	// Запускаем генерацию
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
// Проверка по п.5.1.4 ТЗ.
// Запускаем генерацию прямоугольных импульсов 125 Гц на канале OUT8-1:1 (контакт ВхУ4 ),
// на каналах OUT5-1:1,OUT6-1:1,OUT7-1:1 (контакты ВхУ1,ВхУ2,ВхУ3) - 8В.
// На канале OUT1-1:1 - 8В - контакт включения питания
void __fastcall WSystem::Util3Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// Необходимо пять каналов ( реально 8 )
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ВхУ1 - 8В
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ВхУ2 - 8В
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ВхУ3 - 8В
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(8,0);   // ВхУ4 - 125 Гц
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(1,0);   // Включение питания - 7.5В
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(2,0);	// лишний канал
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(3,0);   // лишний канал
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(4,0);   // лишний канал

	Conf_LTR34.FrequencyDivisor = 14; 		// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  2МГц / (( 64 - 14 )*8) = 5000 Гц.
//	Conf_LTR34.UseClb = true; 				// Используем заводские калибровочные коэффициенты
//	Conf_LTR34.AcknowledgeType = false; 	// тип подтверждения - периодические
//	Conf_LTR34.ExternalStart = false; 		// внешний старт отключен
//	Conf_LTR34.RingMode = true; 			// включен режим автогенерации

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// Номер шага проверки = 0
	// Определяем массив ArrayToSend
	// Определяем массив VoltArray
	ArrayToSend = new DWORD[320];  		// 8 канала ( нужно 5, но по тех. документации можно не менее 8 )
	VoltArray = new double[320];		// по 40 значений для каждого - итого 320

	// заполняем значение массива для отсылки - чтобы генерились импульсы с частотой;
	// 4 логический канал ( ВхУ4 - LTR34\OUT8-1:1 )- 125 Гц
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

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 320, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 320, 5000);
	if( size != 320 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
//	Conf_LTR11.StartADCMode = LTR11_STARTADCMODE_INT; 		// режим старта сбора данных - внутренний
//	Conf_LTR11.InpMode = LTR11_INPMODE_INT;           		// режим синхронизации АЦП - внутренний
//	Conf_LTR11.ADCMode = LTR11_ADCMODE_ACQ;           		// режим сбора данных

	Conf_LTR11.ADCRate.prescaler = 64;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 1;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (5 << 0); // диапазон - 10В, режим - 16-канальный, физический канал - 4 ( нумерация с нуля )

	// Выполняем конфигурацию модуля АЦП
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	// Запускаем генерацию
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
// Проверка по п.5.1.6 ТЗ.
// Создаются сигналы аналогично п.5.1.4 ТЗ ( Util3Step1() ) + создается синусоидальный
// сигнал частотой 30Гц на канале OUT2-1:10 (контакт Вх1).
// Запускаем генерацию прямоугольных импульсов на канале OUT8-1:1 (контакт ВхУ4 ),
// на каналах OUT5-1:1,OUT6-1:1,OUT7-1:1 (контакты ВхУ1,ВхУ2,ВхУ3) - 8В.
// На канале OUT1-1:1 - 7.5В - контакт включения питания
// На канале OUT2-1:10 - синус 30Гц.
void __fastcall WSystem::Util4Step1()throw(Exception)
{
	int err = 0;
	int size;
	double t = 0;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// Необходимо 6 каналов (реально 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ВхУ1 - 8В
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ВхУ2 - 8В
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ВхУ3 - 8В
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(8,0);   // ВхУ4 - 125 Гц
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(1,0);   // Включение питания - 7.5В
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(2,1);   // Вх1 - 30 Гц
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(3,0);	// лишний канал
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(4,0);   // лишний канал

	Conf_LTR34.FrequencyDivisor = 14; 		// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  2МГц / (( 64 - 14 )*8) = 5000 Гц.
	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// Номер шага проверки = 0
	// Определяем массив ArrayToSend
	// Определяем массив VoltArray
	ArrayToSend = new DWORD[1336];  	// 8 канала ( нужно 5, но по тех. документации можно не менее 8 )
	VoltArray = new double[1336];		// по 167 значений для каждого - итого 1336

	// Рассчет значений для формирования частоты 30 Гц на канале OUT2-1:10 ( контакт Вх1 )
	// sin( omega*t );
	// omega = 2*PI*30 = 188.4955592 рад/сек
	// заполняем значение массива для отсылки - чтобы генерились импульсы с частотой;
	// 4 логический канал ( ВхУ4 - LTR34\OUT8-1:1 )- 125 Гц
	for (int i = 0,j = 0; i < 167; i++) {
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = ( (((  i/20 ) + 1 )%2) == 0 )? 8 : 0;
	VoltArray[j++] = 7.5;
	VoltArray[j++] = 0.424*sin(188.4955592*t);
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	t += 0.0002;			// период 0.0002 сек - 5000 Гц
	}

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1336, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1336, 5000);
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 64;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 1;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (10 << 0); // диапазон - 10В, режим - 16-канальный, физический канал - 10 ( нумерация с нуля )

	// Выполняем конфигурацию модуля АЦП
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
// Проверка по п.5.1.7 ТЗ.
// Создаются сигналы аналогично п.5.1.4 ТЗ ( Util3Step1() ) + создается синусоидальный
// сигнал частотой 30Гц на канале OUT2-1:10 (контакт Вх1).
// Запускаем генерацию прямоугольных импульсов на канале OUT8-1:1 (контакт ВхУ4 ),
// на каналах OUT5-1:1,OUT6-1:1,OUT7-1:1 (контакты ВхУ1,ВхУ2,ВхУ3) - 8В.
// На канале OUT1-1:1 - 7.5В - контакт включения питания
// На канале OUT2-1:10 - синус 30Гц. Амплитуда синуса меняется ступенчато в
//                       функциях Util5Step2_4();
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
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// Необходимо 6 каналов (реально 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ВхУ1 - 8В
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ВхУ2 - 8В
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ВхУ3 - 8В
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(8,0);   // ВхУ4 - 125 Гц
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(1,0);   // Включение питания - 7.5В
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(2,1);   // Вх1 - 30 Гц
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(3,0);	// лишний канал
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(4,0);   // лишний канал

	Conf_LTR34.FrequencyDivisor = 14; 		// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  2МГц / (( 64 - 14 )*8) = 5000 Гц.

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;							// Номер шага проверки = 0
	// Определяем массив ArrayToSend
	// Определяем массив VoltArray
	ArrayToSend = new DWORD[1336];  		// 8 канала ( нужно 5, но по тех. документации можно не менее 8 )
	VoltArray = new double[1336];			// по 167 значений для каждого - итого 1336

	// Рассчет значений для формирования частоты 30 Гц на канале OUT2-1:10 ( контакт Вх1 )
	// sin( omega*t );
	// omega = 2*PI*30 = 188.4955592 рад/сек
	// заполняем значение массива для отсылки - чтобы генерились импульсы с частотой;
	// 4 логический канал ( ВхУ4 - LTR34\OUT8-1:1 )- 125 Гц
	for (int i = 0,j = 0; i < 167; i++) {
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = ( (((  i/20 ) + 1 )%2) == 0 )? 8 : 0;
	VoltArray[j++] = 7.5;
	VoltArray[j++] = AmpUtil5[0]*sin(188.4955592*t);
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	t += 0.0002;			// период 0.0002 сек - 5000 Гц
	}

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1336, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1336, 5000);
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 64;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 4;                            		// количество логических каналов - 4
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (10 << 0); // диапазон - 10В, режим - 16-канальный, физический канал - 10 ( нумерация с нуля )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (6 << 0); 	// диапазон - 10В, режим - 16-канальный, физический канал - 6 ( нумерация с нуля )
	Conf_LTR11.LChTbl[2] = (0 << 6) | (0 << 4) | (7 << 0); 	// диапазон - 10В, режим - 16-канальный, физический канал - 7 ( нумерация с нуля )
	Conf_LTR11.LChTbl[3] = (0 << 6) | (0 << 4) | (8 << 0); 	// диапазон - 10В, режим - 16-канальный, физический канал - 8 ( нумерация с нуля )


	// Выполняем конфигурацию модуля АЦП
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

	num_step_prev = NumStep++;	   	// Предыдущий шаг проверки и следующий

	// Сначала смотрим предыдущий результат АЦП - напряжения
	for(int i=0;i<3;i++) resultValue[i] = 0;                            // Обнуление
	for(int i=0, j=513; i<10; i++) {
		resultValue[0] = resultValue[0] + AcqBuf[BlockNumberThread][j++];
		resultValue[1] = resultValue[1] + AcqBuf[BlockNumberThread][j++];
		resultValue[2] = resultValue[2] + AcqBuf[BlockNumberThread][j++];
		j++;
	}
	for(int i=0;i<3;i++) Util5Result[i] = resultValue[i] / 10;

	// Проверка
	for(int i=0; i<3; i++) {
		if(SwitchKomparator[num_step_prev][i] < Util5Result[i])
			return RET_ERROR;
    }

	if( NumStep >= 4 ) { RunFlag = 0; return RET_END; }
//	if( fabs( Util1Result - Util1ResultSet[	num_step_prev] ) > DeltaErrorUtil1[	num_step_prev] )
//	return RET_ERROR;

	// Модифицируем массив VolArray[] - конкретно: изменяем амплитуду синусоиды
	// канала Conf_LTR34.LChTbl[5] ( Вх1 - 30 Гц ).
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
		t += 0.0002;			// период 0.0002 сек - 5000 Гц
	}

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1336, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1336, 5000);
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// Запускаем генерацию
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
// Проверка по п.5.1.8 ТЗ.
// Создаются сигналы аналогично п.5.1.4 ТЗ ( Util3Step1() ) + создается синусоидальный
// сигнал частотой 30Гц на канале OUT2-1:10 (контакт Вх1).
// Запускаем генерацию прямоугольных импульсов на канале OUT8-1:1 (контакт ВхУ4 ),
// на каналах OUT5-1:1,OUT6-1:1,OUT7-1:1 (контакты ВхУ1,ВхУ2,ВхУ3) - 8В.
// На канале OUT1-1:1 - 7.5В - контакт включения питания
// На канале OUT2-1:10 - синус 30Гц. Частота синуса меняется ступенчато в
//                       функциях Util6Step2_6();
// Один сэмпл - 0.2 мсек ( 5000 Гц )
// прямоугольные импульсы 125 Гц -> 40 сэмплов -> период 8 мсек
// синус 30 Гц -> 167 сэмплов -> 1 период
// синус 50 Гц -> 200 сэмплов -> 2 периода
// синус 62.5 Гц -> 80 сэмплов -> 1 период
// синус 83.3(3) Гц -> 60 сэмплов -> 1 период
// синус 100 Гц -> 50 сэмплов -> 1 период
// синус 120 Гц -> 42 сэмпла -> 1 период
int WSystem::KolSemplKanalUtil6[] = { 167, 200, 80,   60,   50,  42    }; 	// Сэмпл/канал
float WSystem::FreqUtil6[]  = 		{ 30,  50,  62.5, 83.3, 100, 120   };	// Гц
void __fastcall WSystem::Util6Step1()throw(Exception)
{
	int err = 0;
	int size;
	double t = 0;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// Необходимо 6 каналов (реально 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ВхУ1 - 8В
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ВхУ2 - 8В
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ВхУ3 - 8В
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(8,0);   // ВхУ4 - 125 Гц
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(1,0);   // Включение питания - 7.5В
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(2,1);   // Вх1 - 30 Гц
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(3,0);	// лишний канал
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(4,0);   // лишний канал

	Conf_LTR34.FrequencyDivisor = 14; 		// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  2МГц / (( 64 - 14 )*8) = 5000 Гц.

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;							// Номер шага проверки = 0
	// Определяем массив ArrayToSend
	// Определяем массив VoltArray
	ArrayToSend = new DWORD[1600];  		// 8 канала ( нужно 5, но по тех. документации можно не менее 8 )
	VoltArray = new double[1600];			// по 200 значений ( макс.) для каждого - итого 1600

	// Рассчет значений для формирования частоты 30 Гц на канале OUT2-1:10 ( контакт Вх1 )
	// sin( omega*t );
	// omega = 2*PI*30 = 188.4955592 рад/сек
	// заполняем значение массива для отсылки - чтобы генерились импульсы с частотой;
	// 4 логический канал ( ВхУ4 - LTR34\OUT8-1:1 )- 125 Гц
	for (int i = 0,j = 0; i < 200; i++) {
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = 8;
	VoltArray[j++] = ( (((  i/20 ) + 1 )%2) == 0 )? 8 : 0;
	VoltArray[j++] = 7.5;
	VoltArray[j++] = 1.0*sin(188.4955592*t);
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	t += 0.0002;			// период 0.0002 сек - 5000 Гц
	}

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1336, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1336, 5000);
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 64;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 3;                            		// количество логических каналов - 3
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (10 << 0); // диапазон - 10В, режим - 16-канальный, физический канал - 10 ( нумерация с нуля )
//	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (6 << 0); 	// диапазон - 10В, режим - 16-канальный, физический канал - 6 ( нумерация с нуля )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (1 << 0); 	// диапазон - 10В, режим - 16-канальный, физический канал - 1 ( нумерация с нуля )
	Conf_LTR11.LChTbl[2] = (0 << 6) | (0 << 4) | (0 << 0); 	// диапазон - 10В, режим - 16-канальный, физический канал - 0 ( нумерация с нуля )


	// Выполняем конфигурацию модуля АЦП
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

	num_step_prev = NumStep++;	   	// Предыдущий шаг проверки и следующий

	if( NumStep >= 6 ) { RunFlag = 0; return RET_END; }

//    // Сначала смотрим предыдущий результат АЦП - напряжения
//	Util1Result = AcqBuf[BlockNumberThread][512];
//	if( fabs( Util1Result - Util1ResultSet[	num_step_prev] ) > DeltaErrorUtil1[	num_step_prev] )
//	return RET_ERROR;

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// Модифицируем массив VolArray[] - конкретно: изменяем частоту синусоиды
	// канала Conf_LTR34.LChTbl[5] ( Вх1 ).
	for( int i = 0,j = 5; i < KolSemplKanalUtil6[NumStep]; i++, j+=8 )
	{
		VoltArray[j] = 1.0*sin(2*M_PI*FreqUtil6[NumStep]*t);
		t += 0.0002;			// период 0.0002 сек - 5000 Гц
	}

	len =  KolSemplKanalUtil6[NumStep] * 8;

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, len, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend,  len, 5000);
	if( size !=  len ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// Запускаем генерацию
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
//Если “0” – интерфейсный канал связи с модулем создан и открыт.
//Если значение ненулевое - канал не создан
int __fastcall WSystem::IsOpenedLTR34()
{
	return LTR34_IsOpened(&Conf_LTR34);
}
//------------------------/ MEMBER FUNCTION /--------------------------------
/*int __fastcall WSystem::Init_LTR11_LTR43()
{
	int err;

	//инициализация дескрипторов модулей
	err = LTR43_Init(&Conf_LTR43);
	if(err) {
		MessageDlg((char *) LTR43_GetErrorString(err), mtError, TMsgDlgButtons() << mbOK, 0);
		return RET_ERROR;
	}

	//открытие каналов связи с модулями
	// Номер слота с платой LTR43 - 4
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

	//---------задание параметров работы ЦАП-------
	Conf_LTR34.ChannelQnt = 4;						 // Необходимо три канала, один канал лишний
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(1,0);
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(2,0);
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(3,0);
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(4,0);

	Conf_LTR34.FrequencyDivisor = 32; 		// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  15625 Гц.
	Conf_LTR34.UseClb = true; 				// Используем заводские калибровочные коэффициенты
	Conf_LTR34.AcknowledgeType = false; 	// тип подтверждения - периодические
	Conf_LTR34.ExternalStart = false; 		// внешний старт отключен
	Conf_LTR34.RingMode = true; 			// включен режим автогенерации

	//---------задание параметров работы АЦП-------
	Conf_LTR11.StartADCMode = LTR11_STARTADCMODE_INT; 		// режим старта сбора данных - внутренний
	Conf_LTR11.InpMode = LTR11_INPMODE_INT;           		// режим синхронизации АЦП - внутренний
	Conf_LTR11.ADCMode = LTR11_ADCMODE_ACQ;           		// режим сбора данных

	Conf_LTR11.ADCRate.prescaler = 1;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 1;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (11 << 0); // диапазон - 10В, режим - 16-канальный, физический канал - 12

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля ЦАП
	err = LTR34_Reset(&Conf_LTR34);
	if(err) {
		MessageDlg((char *) LTR34_GetErrorString(err), mtError, TMsgDlgButtons() << mbOK, 0);
		LTR34_Close(&Conf_LTR34);
		return RET_ERROR;
	}

	// Выполняем конфигурацию модуля ЦАП
	err = LTR34_Config(&Conf_LTR34);
	if(err) {
		MessageDlg((char *) LTR34_GetErrorString(err), mtError, TMsgDlgButtons() << mbOK, 0);
		LTR34_Close(&Conf_LTR34);
		return RET_ERROR;
	}

	// Выполняем конфигурацию модуля АЦП
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) {
		MessageDlg((char *) LTR11_GetErrorString(err), mtError, TMsgDlgButtons() << mbOK, 0);
		LTR11_Close(&Conf_LTR11);
		return RET_ERROR;
	}

	//------------Определяем данные для ЦАП-------------
	// Определяем массив ArrayToSend
	// Определяем массив VoltArray
	ArrayToSend = new DWORD[4000];  	// 4 канала ( нужно 3, но по тех. документации можно не менее 4 )
	VoltArray = new double[4000];		// по 1000 значений для каждого - итого 4000

	// заполняем значение массива для отсылки - чтобы генерились импульсы с частотой;
	// 1 канал - 15,625 Гц, 2 канал - 31,25 Гц, 3 канал - 62,5 Гц
	for (int i = 0,j = 0; i < 1000; i++) {
		VoltArray[j++] = ( (((  i/500 ) + 1 )%2) == 0 )? 8 : 0;
		VoltArray[j++] = ( (((  i/250 ) + 1 )%2) == 0 )? 8 : 0;
		VoltArray[j++] = ( (((  i/125 ) + 1 )%2) == 0 )? 8 : 0;
		VoltArray[j++] = ( (((  i/125 ) + 1 )%2) == 0 )? 8 : 0;
	}

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 4000, true);
	if(err) {
		MessageDlg((char *) LTR34_GetErrorString(err), mtError, TMsgDlgButtons() << mbOK, 0);
		LTR34_Close(&Conf_LTR34);
		delete ArrayToSend;
		delete VoltArray;
		return RET_ERROR;
	}

	// Отправляем данные в модуль ЦАП
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



