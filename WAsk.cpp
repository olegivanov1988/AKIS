//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WAsk.h"
#include "UnRaport.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
// Выводимые значения АЦП в вольтах
static const char ErrorText1[] = "Ошибка при отправке данных модулю!";
static const char ErrorText2[] = "Ошибка! Количество отправленных сэмплов не равно заданному!";
static const char ErrorText3[] =  "Неизвестная ошибка";
//--------------------------/ CONSTRUCTOR /----------------------------------
WAsk::WAsk():ArrayToSend(0),VoltArray(0)
{
	BlockNumberThread = 100; // инициализация переменной на случай если поток АЦП будет создан позднее
	ReplaceBuf = false;      // иництализация переменной повторной записи буферов
	ReplaceBuf2 = false;      // иництализация переменной повторной записи буферов
	RunFlag = 0;
	RunFlag2 = 0;

	zRaport = new ZRaport;

//	// Инициализируем массив проверочных переменных
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

	//инициализация дескрипторов модулей
	err = LTR34_Init(&Conf_LTR34);
	if(err) goto GOTO_ERROR2;

	err = LTR11_Init(&Conf_LTR11);
	if(err) goto GOTO_ERROR1;
	Conf_LTR11.ModuleInfo.CbrCoef[0].Gain = 1.02;
//	Conf_LTR11.ModuleInfo.CbrCoef[0].Offset = 50; 	// Смкшкние мВ

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

void __fastcall WAsk::Init_LTR27()throw(Exception)
{
	int res;

	// Инициализируем поля структуры значениями по умолчанию
	res = LTR27_Init(&Conf_LTR27);
	if( res == LTR_OK )
	{
		// Устанавливаем соединение с модулем находящемся в первом слоте крейта.
		// для сетевого адреса, сетевого порта ltr-сервера и серийного номера
		// крейта используем значения по умолчанию
		res = LTR27_Open(&Conf_LTR27, SADDR_DEFAULT, SPORT_DEFAULT, "", CC_MODULE5);
		if( res == LTR_OK )
		{	// получаем конфигурацию модуля
			res = LTR27_GetConfig(&Conf_LTR27);
			if( res == LTR_OK )
			{
        		// считываем описание модуля и мезонинов
				res = LTR27_GetDescription( &Conf_LTR27, LTR27_ALL_DESCRIPTION );
				if( res == LTR_OK )
				{
					// выбираем частоту дискретизации 100Гц
					Conf_LTR27.FrequencyDivisor = 9;
					// копируем калибровочные коэффициенты
					for( int i = 0; i < LTR27_MEZZANINE_NUMBER; i++ )
						for( int j = 0; j < 4; j++ )
							Conf_LTR27.Mezzanine[i].CalibrCoeff[j] = Conf_LTR27.ModuleInfo.Mezzanine[i].Calibration[j];

					Conf_LTR27.Mezzanine[0].CalibrCoeff[0] = 1.47;	// Коэффициент масштаба от 03.06.15
					// передаем параметры сбора данных в модуль
					res = LTR27_SetConfig( &Conf_LTR27 );
					//if(res==LTR_OK) {
                }
			}
		}
	}

    // выводим сообщение об ошибке
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

	// Запускаем генерацию ЦАП
	err = LTR34_DACStart(&Conf_LTR34);

	// создание потока сбора данных от АЦП модуля
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

	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {       // перед созданием потока все блоки данных
		System->AcqBlockReady[i] = 0;              // готовы к записи
	}

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
//	double data_fis[ACQ2_BLOCK_SIZE];	// Массив физических значений

	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {       // перед созданием потока все блоки данных
		System->Acq2BlockReady[i] = 0;              // готовы к записи
	}

	// тайм-аут ожидания одного блока данных в мс
//	const DWORD acq_time_out = (DWORD)(ACQ_BLOCK_SIZE / (System->Conf_LTR11.ChRate * System->Conf_LTR11.LChQnt) + 1000);

	// запуск сбора данных
	res = LTR27_ADCStart(&System->Conf_LTR27);
	if( res != LTR_OK )
	{
		(void)LTR27_ADCStop(&System->Conf_LTR27);
		ExitThread(res);
		return 0;
	}

	while( System->RunFlag2 )
	{
	// получение данных от LTR11
	size = LTR27_Recv(&System->Conf_LTR27, data_buf, NULL, ACQ2_BLOCK_SIZE, 2000 );
	if( size > 0 &&  size == ACQ2_BLOCK_SIZE)
	{
		int block_number;
		// применяем калибровку и переводим в милиамперы
//		res = LTR27_ProcessData(&ltr27, data_fis, data_buf, &size, 1, 1);
		// если без ошибок принято ожидаемое количество данных
//		if( res == LTR_OK )
//		{
			// поиск свободного буфера
			block_number = 0;
			while (block_number < ACQ_BLOCK_QNT && System->Acq2BlockReady[block_number])
			block_number++;


		if (block_number < ACQ_BLOCK_QNT)
		{
			DWORD data_size = ACQ2_BLOCK_SIZE;
			// сохранение принятых и обработанных данных в буфере
			res = LTR27_ProcessData(&System->Conf_LTR27, data_buf, System->Acq2Buf[block_number], &data_size, true, true );
			System->BlockNumberThread2 = block_number; // номер блока данных для scope-потока
			if(res) {
				System->RunFlag2 = 0;
			} else {
				if(System->ReplaceBuf2 == false) System->Acq2BlockReady[block_number] = 1;             // true - пишем повторно в буферы
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

	RunFlag = 0;            // Выходим из потока сбора измерительной информации от LTR11
	RunFlag2 = 0; 			// Выходим из потока сбора измерительной информации от LTR27 ( ток )

	if( LTR34_IsOpened( &Conf_LTR34 ) == 0 )
	{
		err = LTR34_DACStop( &Conf_LTR34 );
		if(err) goto GOTO_ERROR;
	}else Init_LTR11_LTR34();


	Conf_LTR34.ChannelQnt = 8;
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ВхУ1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ВхУ2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ВхУ3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(8,0);   // ВхУ4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(1,0);   // Питание
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(2,0);   // лишний канал
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(3,0);   // лишний канал
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(4,0);   // лишний канал

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
// Проверка по п.5.1.1 ТЗ.
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
	VoltArray[0] = ASK_Ucc;

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
	Conf_LTR11.ADCRate.prescaler = 1;                 		// частота дискретизации - 100 кГц, на канал - 50 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 2;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (2 << 0); // K7 - диапазон - 10В, режим - 16-канальный, физический канал - 2 (нумерация с нуля )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (3 << 0); // K8 - диапазон - 10В, режим - 16-канальный, физический канал - 3 (нумерация с нуля )


	// Выполняем конфигурацию модуля АЦП
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	// Запускаем генерацию
	AcqBlockReady[0] = 0;           // перед генерацией все блоки данных
	AcqBlockReady[1] = 0;           // готовы к записи
	StartGen_LTR11_LTR34();
	RunFlag2 = 1;				  	//
	// Создание потока сбора данных от АЦП LTR27 модуля
	hnd_acq_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AcquireThread2, this, 0, &acq_thread_id );
	(void)GetExitCodeThread(hnd_acq_thread, &thread_status);


GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// Проверка по п.5.1.2 ТЗ.
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

	//--------- Задание параметров работы ЦАП ---------
	Conf_LTR34.ChannelQnt = 4;
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ВхУ1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ВхУ2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ВхУ3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);   // Питание

	Conf_LTR34.FrequencyDivisor = 40; 		 			// делитель частоты дискретизации

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
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
#include "math.h"
int __fastcall WAsk::Util1Step2_8()throw(Exception)
{
	int err = 0;
	int size;
	int num_step_prev;

	num_step_prev = NumStep++;	   	// Предыдущий шаг проверки и следующий

	if( NumStep > 8 ) { RunFlag = 0; return RET_END; }

	// Сначала смотрим предыдущий результат АЦП - напряжения
	Util1Porog[num_step_prev] = Util1Result = AcqBuf[BlockNumberThread][512];
	// Проверяем на отсутствие сигнала
	if(fabs(Util1Result) < 0.1) return RET_NO_U;
    // Теперь проверяем на отклонение напряжения от номинала
	if( fabs( Util1Result - Util1ResultSet[	num_step_prev] ) > DeltaErrorUtil1[	num_step_prev] )
	return RET_ERROR;

//	if( NumStep > 7 ) { RunFlag = 0; return RET_END; }

	// Определяем массив VoltArray
	for( register i = 0; i < 4; i++ )
	VoltArray[i] = VoltArrayTable[NumStep][i];

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 4, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 4, 5000);
	if( size != 4 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

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
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// Проверка по п.5.1.3 ТЗ.
// Запускаем генерацию прямоугольных импульсов по трем каналам ( реально четыре,
// так как мин. кол. каналов может быть 4 ) частотой 15,625±0,25Гц, 31,25±0,5Гц,
// 62,5±1Гц на каналах OUT5-1:1,OUT6-1:1,OUT7-1:1( контакты ВхУ1,ВхУ2,ВхУ3 ).
void __fastcall WAsk::Util2Step1()throw(Exception)
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
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ВхУ1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ВхУ2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ВхУ3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);   // Питание

	Conf_LTR34.FrequencyDivisor = 32; 		// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  15625 Гц.
//	Conf_LTR34.UseClb = true; 				// Используем заводские калибровочные коэффициенты
//	Conf_LTR34.AcknowledgeType = false; 		// тип подтверждения - периодические
//	Conf_LTR34.ExternalStart = false; 		// внешний старт отключен
//	Conf_LTR34.RingMode = true; 				// включен режим автогенерации

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

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
	VoltArray[j++] = ASK_Ucc; //( (((  i/125 ) + 1 )%2) == 0 )? 8 : 0;
	}

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 4000, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 4000, 5000);
	if( size != 4000 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
//	Conf_LTR11.StartADCMode = LTR11_STARTADCMODE_INT; 		// режим старта сбора данных - внутренний
//	Conf_LTR11.InpMode = LTR11_INPMODE_INT;           		// режим синхронизации АЦП - внутренний
//	Conf_LTR11.ADCMode = LTR11_ADCMODE_ACQ;           		// режим сбора данных

	Conf_LTR11.ADCRate.prescaler = 64;                 		// частота дискретизации - 1,5625 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (64*(149+1)) = 1,5625 кГц

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
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// Проверка по п.5.1.4 ТЗ.
// Запускаем генерацию прямоугольных импульсов 125 Гц на канале OUT8-1:1 (контакт ВхУ4 ),
// на каналах OUT5-1:1,OUT6-1:1,OUT7-1:1 (контакты ВхУ1,ВхУ2,ВхУ3) - 8В.
// На канале OUT1-1:1 - 8В - контакт включения питания
void __fastcall WAsk::Util3Step1()throw(Exception)
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
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(3,0);   // Вх1 - лишний канал
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
	VoltArray[j++] = ASK_Ucc;
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	}

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 320, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 320, 5000);
	if( size != 320 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

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
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// Проверка по п.5.1.6 ТЗ.
// Создаются сигналы аналогично п.5.1.4 ТЗ ( Util3Step1() ) + создается синусоидальный
// сигнал частотой 30Гц на канале OUT2-1:10 (контакт Вх1).
// Запускаем генерацию прямоугольных импульсов на канале OUT8-1:1 (контакт ВхУ4 ),
// на каналах OUT5-1:1,OUT6-1:1,OUT7-1:1 (контакты ВхУ1,ВхУ2,ВхУ3) - 8В.
// На канале OUT1-1:1 - 7.5В - контакт включения питания
// На канале OUT2-1:10 - синус 30Гц.
void __fastcall WAsk::Util4Step1()throw(Exception)
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
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(3,0);   // Вх1 - 30 Гц
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// лишний канал
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
	VoltArray[j++] = ASK_Ucc;
	VoltArray[j++] = 0.69*sin(188.4955592*t);
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
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 64;                 		// частота дискретизации - 1,5625 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (64*(149+1)) = 1,5625 кГц

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
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// Необходимо 6 каналов (реально 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(5,0);	// ВхУ1 - 8В
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);   // ВхУ2 - 8В
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // ВхУ3 - 8В
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(8,0);   // ВхУ4 - 125 Гц
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(1,0);   // Включение питания
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(3,0);   // Вх1 - 30 Гц
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// лишний канал
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
	VoltArray[j++] = ASK_Ucc;
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
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 64;                 		// частота дискретизации - 1,5625 кГц на канал - 390,625 Гц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (64*(149+1)) = 1,5625 кГц

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

	num_step_prev = NumStep++;	   	// Предыдущий шаг проверки и следующий

	// Сначала смотрим предыдущий результат АЦП - напряжения
	for(int i=0;i<3;i++) resultValue[i] = 0;                            // Обнуление
	for(int i=0, j=513; i<10; i++) {
		resultValue[0] = resultValue[0] + AcqBuf[BlockNumberThread][j++];
		resultValue[1] = resultValue[1] + AcqBuf[BlockNumberThread][j++];
		resultValue[2] = resultValue[2] + AcqBuf[BlockNumberThread][j++];
		j++;
	}
	for(int i=0;i<3;i++) Util5ResultReport[num_step_prev][i] = Util5Result[i] = resultValue[i] / 10;

	// Проверка
//	for(int i=0; i<3; i++) {
//		if(SwitchKomparator[num_step_prev][i] < Util5Result[i])
//			return RET_ERROR;
//    }
	switch(NumStep - 1)
	{
		case 0:{for(int i=0,j=0; i<3; i++,j+=2) {
					if(Util5Result[i] < 4) {
						noteUtil5[j] = "Неисправность";
						return RET_ERROR;
					} else {
						noteUtil5[j] = "Норма";
					}
				}
				break;}
		case 1:{if(Util5Result[0] > 4 || Util5Result[1] < 4 || Util5Result[2] < 4) {
					if(Util5Result[0] > 4) noteUtil5[1] = "Неисправность";
					if(Util5Result[1] < 4) noteUtil5[2] = "Неисправность";
					return RET_ERROR;
				} else {
					noteUtil5[1] = "Норма";
					noteUtil5[2] = "Норма";
				}
				break;}
		case 2:{if(Util5Result[0] > 4 || Util5Result[1] > 4 || Util5Result[2] < 4) {
					if(Util5Result[1] > 4) noteUtil5[3] = "Неисправность";
					if(Util5Result[2] < 4) noteUtil5[4] = "Неисправность";
					return RET_ERROR;
				} else {
					noteUtil5[3] = "Норма";
					noteUtil5[4] = "Норма";
				}
				break;}
		case 3:{for(int i=0,j=1; i<3; i++,j+=2) {
					if(Util5Result[i] > 4) {
						noteUtil5[j] = "Неисправность";
						return RET_ERROR;
					} else {
						noteUtil5[j] = "Норма";
					}
				}
				break;}
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
		VoltArray[j] = AmpUtil5[NumStep]*sin(188.4955592*t);
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
	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {       // все блоки данных
		AcqBlockReady[i] = 0;              // готовы к записи
	}

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
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
int WAsk::KolSemplKanalUtil6[] = { 167, 200, 80,   60,   50,  42    }; 	// Сэмпл/канал
float WAsk::FreqUtil6[]  = 		{ 30,  50,  62.5, 83.3, 100, 120   };	// Гц
//float WAsk::AmpUtil6[] = { 1.3, 1.56, 1.82, 2.08, 2.34, 2.6 };			// мВ
float WAsk::AmpUtil6[] = { 0.66, 1.56, 1.82, 2.08, 2.34, 1.54 };			// мВ
void __fastcall WAsk::Util6Step1()throw(Exception)
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
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(3,0);   // Вх1 - 30 Гц
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// лишний канал
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
	VoltArray[j++] = ASK_Ucc;
	VoltArray[j++] = 0.66*sin(188.4955592*t);
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
	if( size != 1336 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// частота дискретизации - 12,5 кГц на канал - 4167 Гц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (8*(149+1)) = 12,5 кГц

	Conf_LTR11.LChQnt = 3;                            		// количество логических каналов - 3
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (10 << 0); // K1, диапазон - 10В, режим - 16-канальный, физический канал - 10 ( нумерация с нуля )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (1 << 0); 	// K6, диапазон - 10В, режим - 16-канальный, физический канал - 1 ( нумерация с нуля )
	Conf_LTR11.LChTbl[2] = (0 << 6) | (0 << 4) | (0 << 0); 	// K5, диапазон - 10В, режим - 16-канальный, физический канал - 0 ( нумерация с нуля )

	// Выполняем конфигурацию модуля АЦП
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

//	NumStep++;	   	// Следующий шаг проверки
	NumStep += 5;	// Сразу последний шаг проверки

	if( NumStep >= 6 )
	{ RunFlag = 0; return RET_END; }

	// Модифицируем массив VolArray[] - конкретно: изменяем частоту синусоиды
	// канала Conf_LTR34.LChTbl[5] ( Вх1 ).
	for( int i = 0,j = 5; i < KolSemplKanalUtil6[NumStep]; i++, j+=8 )
	{
		VoltArray[j] = AmpUtil6[NumStep]*sin(2*M_PI*FreqUtil6[NumStep]*t);
		t += 0.0002;			// период 0.0002 сек - 5000 Гц
	}

	len =  KolSemplKanalUtil6[NumStep] * 8;

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
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, len, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend,  len, 5000);
	if( size !=  len ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// Запускаем генерацию
	err = LTR34_DACStart(&Conf_LTR34);
	Sleep(100);
	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {       // все блоки данных
		AcqBlockReady[i] = 0;              // готовы к записи
	}

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
//Если “0” – интерфейсный канал связи с модулем создан и открыт.
//Если значение ненулевое - канал не создан
int __fastcall WAsk::IsOpenedLTR34()
{
	return LTR34_IsOpened(&Conf_LTR34);
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// FileName - имя файла, которое будем присваивать ("\\RaportIK.xlsx")
// write - записываем его или потом стираем этот файл
#include "math.hpp"
//static char Text1[] = "%.1f";
void __fastcall WAsk::WriteExcel(AnsiString FileName, bool write)
{
	TDateTime t;
//	AnsiString form_string;

//	NumberBlock = "22";
//	AnsiString Dir = "C:\\Документы АКИС\\Протоколы\\068";
//	currentPlus = 997.789;

	if(zRaport->ExcelInit()) {
		if(zRaport->CopyRepFromSablon(FileName)) {
			zRaport->WriteData(4, 1, "№ блока " + NumberBlock);
			zRaport->WriteData(4, 3, "Дата " + t.CurrentDateTime().FormatString("dd.mm.yyyy"));
			zRaport->WriteData(4, 6, "Время " + t.CurrentDateTime().FormatString("hh:nn"));

//			form_string.printf(Text1, currentPlus);
			zRaport->WriteData(9, 5, SimpleRoundTo(currentPlus, -1));    // Ток
			zRaport->WriteData(9, 7, note_current);

			zRaport->WriteData(11, 5, SimpleRoundTo(Uplus, -1));         // Напряжение положительной полярности
			zRaport->WriteData(11, 7, noteUplus);
			zRaport->WriteData(12, 5, SimpleRoundTo(Uminus, -1));        // Напряжение отрицательной полярности
			zRaport->WriteData(12, 7, noteUminus);
			zRaport->WriteData(14, 5, SimpleRoundTo(Util1Porog[0], -2)); // Пороги
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

			zRaport->WriteData(29, 5, note_CK3_5XP1_30); // ПУ-4
			zRaport->WriteData(29, 7, note2_CK3_5XP1_30);
			zRaport->WriteData(30, 5, note_CK3_5XP1_120); // ПУ-4
			zRaport->WriteData(30, 7, note2_CK3_5XP1_120);

			zRaport->WriteData(31, 5, note_CK3_7XP1_30);
			zRaport->WriteData(31, 7, note2_CK3_7XP1_30);
			zRaport->WriteData(32, 5, note_CK3_7XP1_120);
        	zRaport->WriteData(32, 7, note2_CK3_7XP1_120);

			if(write) {
				zRaport->CloseReport("C:\\Документы АКИС\\Протоколы\\КИБ-05\\068\\068-" + NumberBlock + ".xlsx");
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
int __fastcall WAsk::Config_LTR11_LTR34()
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



