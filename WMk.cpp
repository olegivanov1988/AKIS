//---------------------------------------------------------------------------
#include <vcl.h>
#include "bass.h"
#include "math.h"
#pragma hdrstop
#include "WMk.h"
#include "UnRaport.h"


//---------------------------------------------------------------------------

#pragma package(smart_init)

static const char ErrorText1[] = "Ошибка при отправке данных модулю!";
static const char ErrorText2[] = "Ошибка! Количество отправленных сэмплов не равно заданному!";
static const char ErrorText3[] =  "Неизвестная ошибка";
//--------------------------/ CONSTRUCTOR /----------------------------------
WMk::WMk():ArrayToSend(0),VoltArray(0)
{
	BlockNumberThread = 100; // инициализация переменной на случай если поток АЦП будет создан позднее
	ReplaceBuf = false;      // иництализация переменной повторной записи буферов
	ReplaceBuf2 = false;     // иництализация переменной повторной записи буферов
	RunFlag = 0;
	RunFlag2 = 0;
   //	PowerPauseSecond = 14;    // Первоначальная пауза после вкл питания

	zRaport = new ZRaport;
//	for(int i = 0; i < 7; i++) {
//		PerepadUtil4[i] = false;
//	}

	LTR34_Init(&Conf_LTR34);
	LTR11_Init(&Conf_LTR11);
	LTR27_Init(&Conf_LTR27);
}
//--------------------------/ DESTRUCTOR /-----------------------------------
WMk::~WMk()
{
}
//------------------------/ MEMBER FUNCTION /--------------------------------
void __fastcall WMk::Init_LTR11_LTR34()throw(Exception)
{
	int err = 0;

	//инициализация дескрипторов модулей
	err = LTR34_Init(&Conf_LTR34);
	if(err) goto GOTO_ERROR2;

	err = LTR11_Init(&Conf_LTR11);
	if(err) goto GOTO_ERROR1;
	Conf_LTR11.ModuleInfo.CbrCoef[2].Gain = 1.06;

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

void __fastcall WMk::Init_LTR27()throw(Exception)
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
void __fastcall WMk::StartGen_LTR11_LTR34()throw(Exception)
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

	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {                       // перед созданием потока все блоки данных
		System->AcqBlockReady[i] = 0;                              // готовы к записи
	}

	// тайм-аут ожидания одного блока данных в мс
	const DWORD acq_time_out = (DWORD)(ACQ_BLOCK_SIZE / (System->Conf_LTR11.ChRate * System->Conf_LTR11.LChQnt) + 1000);

	// запуск сбора данных
	err = LTR11_Start(&System->Conf_LTR11);
	if(err) {
		LTR11_Stop(&System->Conf_LTR11);
		ExitThread(err);
		return 0;
	}

	while (System->RunFlag)
	{
	// получение данных от LTR11
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
		// без ошибок принято ожидаемое количество данных

		// поиск свободного буфера
		block_number = 0;
		while (block_number < ACQ_BLOCK_QNT && System->AcqBlockReady[block_number])
			block_number++;


		if (block_number < ACQ_BLOCK_QNT)
		{
			int data_size = ACQ_BLOCK_SIZE;
			// сохранение принятых и обработанных данных в буфере
//			switch(System->AcqBlockSize) {
//				case ACQ_BLOCK_SIZE:{   int data_size = ACQ_BLOCK_SIZE;
//										err = LTR11_ProcessData(&System->Conf_LTR11, data_buf, System->AcqBuf[block_number], &data_size, TRUE, TRUE);
//										break;}
//				case ACQ_BLOCK_SIZE_2:{ int data_size = ACQ_BLOCK_SIZE_2;
//										err = LTR11_ProcessData(&System->Conf_LTR11, data_buf2, System->AcqBuf[block_number], &data_size, TRUE, TRUE);
//										break;}
//			}
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
 DWORD WINAPI WMk::AcquireThread2 (WMk *System)
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
//Если “0” – интерфейсный канал связи с модулем создан и открыт.
//Если значение ненулевое - канал не создан
int __fastcall WMk::IsOpenedLTR34()
{
	return LTR34_IsOpened(&Conf_LTR34);
}
//------------------------/ MEMBER FUNCTION /--------------------------------
int __fastcall WMk::UtilStop( bool power_off )
{
	int size;
	int err;
	double volt_array[] = { 0,0,0,MK_Ucc };

//	AcqBlockSize = 1020;	 // размер массива собираемого блока данных по умолчанию

	RunFlag = 0;
	RunFlag2 = 0; 			// Выходим из потока сбора измерительной информации от LTR27 ( ток )

    if( LTR34_IsOpened( &Conf_LTR34 ) == 0 )
	{
		err = LTR34_DACStop( &Conf_LTR34 );
		if(err) goto GOTO_ERROR;
	}else Init_LTR11_LTR34();

	Conf_LTR34.ChannelQnt = 4;

	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(3,1);   // Вх2
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(4,1);   // Вх3
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(5,0);   // ВхЦ1
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);   // Включение питания

	Conf_LTR34.RingMode = false; 				// Включен потоковый режим
	Conf_LTR34.FrequencyDivisor = 0;			// Работаем на минимальной частоте

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	if( power_off == true ) volt_array[3] = 0;
	delete ArrayToSend;
	ArrayToSend = new DWORD[4];
	err = LTR34_ProcessData(&Conf_LTR34, volt_array, ArrayToSend, 4, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 4, 5000);
	if( size != 4 )  { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

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
// Проверка по п.5.3.1 ТЗ.
// Включаем питание и проверяем ток потребления по положительному и отрицательному питанию.
void __fastcall WMk::Util0Step1()throw(Exception)
{
	int err;
	int size;

	DWORD thread_status;
	DWORD acq_thread_id;
	HANDLE hnd_acq_thread;

	Init_LTR27();
	Init_LTR11_LTR34();

	//-----------------------------------------------------------------------
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 1;							// Необходимо 1 канал -> реально 1
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(1,0);   // Включение питания

	Conf_LTR34.FrequencyDivisor = 0; 		// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  2МГц / (( 64 - 0 )*8) = 3906.25 Гц.
	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// Номер шага проверки = 0
	// Определяем массив ArrayToSend
	// Определяем массив VoltArray
	ArrayToSend = new DWORD[1];  	// 1 канал
	VoltArray = new double[1];		//
	VoltArray[0] = MK_Ucc;

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1, 5000);
	if( size != 1 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

//    // Запускаем генерацию ЦАП
//	err = LTR34_DACStart(&Conf_LTR34);
//	if(err) goto GOTO_ERROR;

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// частота дискретизации - 12,5 кГц, на канал - 6,25 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (8*(149+1)) = 12,5 кГц

	Conf_LTR11.LChQnt = 2;                            		// количество логических каналов - 2
	Conf_LTR11.LChTbl[0] = (2 << 6) | (0 << 4) | (4 << 0); // K5, диапазон - 0,6В, режим - 16-канальный, физический канал - 4 ( нумерация с нуля )
	Conf_LTR11.LChTbl[1] = (2 << 6) | (0 << 4) | (5 << 0); // K6, диапазон - 0,6В, режим - 16-канальный, физический канал - 5 ( нумерация с нуля )

	// Выполняем конфигурацию модуля АЦП LTR27
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

    RunFlag2 = 1;
	// Создание потока сбора данных от АЦП LTR27 модуля
	hnd_acq_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AcquireThread2, this, 0, &acq_thread_id );
	(void)GetExitCodeThread(hnd_acq_thread, &thread_status);

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}

}
/*
//------------------------/ MEMBER FUNCTION /--------------------------------
// Проверка по п.5.3.2 ТЗ.
// Проверяем напряжение на тестовых точках К5,К6.
void __fastcall WMk::Util1Step1()throw(Exception)
{
	int err;
	int size;

	WAV *wav = new WAV();
	wav->Open( AnsiString("1.wav") );
	delete wav;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 1;							// Необходимо 1 канал -> реально 1
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(1,0);   // Включение питания

	Conf_LTR34.FrequencyDivisor = 0; 		// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  2МГц / (( 64 - 0 )*8) = 3906.25 Гц.
	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// Номер шага проверки = 0
	// Определяем массив ArrayToSend
	// Определяем массив VoltArray
	ArrayToSend = new DWORD[1];  	// 1 канал
	VoltArray = new double[1];		//
	VoltArray[0] = MK_Ucc;

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
	Conf_LTR11.ADCRate.prescaler = 8;                 		// частота дискретизации - 12,5 кГц, на канал - 6,25 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (8*(149+1)) = 12,5 кГц

	Conf_LTR11.LChQnt = 2;                            		// количество логических каналов - 2
	Conf_LTR11.LChTbl[0] = (2 << 6) | (0 << 4) | (4 << 0); // K5, диапазон - 0,6В, режим - 16-канальный, физический канал - 4 ( нумерация с нуля )
	Conf_LTR11.LChTbl[1] = (2 << 6) | (0 << 4) | (5 << 0); // K6, диапазон - 0,6В, режим - 16-канальный, физический канал - 5 ( нумерация с нуля )

	// Выполняем конфигурацию модуля АЦП
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	StartGen_LTR11_LTR34();

GOTO_ERROR:
	if(err)
	{
		ErrorRelease( err );
	}

}    */
//------------------------/ MEMBER FUNCTION /--------------------------------
// Проверка по п.5.3.4 ТЗ. Сигнал "МК1"  -> параметр num_util = MK1
// Проверка по п.5.3.5 ТЗ. Сигнал "МК2"  -> параметр num_util = MK2
// Проверка по п.5.3.6 ТЗ. Сигнал "МК3"  -> параметр num_util = MK3
void __fastcall WMk::Util2_3_4Step1(int num_util)throw(Exception)
{
	int err = 0;
	int length = 0;
	int size;
	double t = 0;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 4;							// Необходимо 3 канала -> реально 4
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(3,1);   // Вх2
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(4,1);   // Вх3
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(5,0);   // ВхЦ1
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);	// Питание

	if( num_util == MK1 || num_util == MK3 )				// Для генерации синусоиды 1 Гц
	Conf_LTR34.FrequencyDivisor = 0; 		// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  2МГц / (( 64 - 0 )*4) = 7812.5 Гц.

	if( num_util == MK2 )              // Для генерации синусоиды 50 Гц
	Conf_LTR34.FrequencyDivisor = 14; 		// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  2МГц / (( 64 - 14 )*4) = 10000 Гц.


	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// Номер шага проверки = 0

	if( num_util == MK1 || num_util == MK3)
	{
		// Определяем массив ArrayToSend
		// Определяем массив VoltArray
		length = 62496; 					// (7812 * 2) * 4 = 62496
		ArrayToSend = new DWORD[62496];  	// 4 канала ( нужно 3, но по тех. документации можно не менее 4 )
		VoltArray = new double[62496];		// по 15624 ( max) значений для каждого - итого 62496.

		// Определяем массив VoltArray для формирования сигналов длительностью 1 сек ( 1 Гц )
		// Рассчет значений для формирования частоты 1 Гц на канале OUT3-1:10 ( сигнал Вх2 )
		// и OUT4-1:10 ( сигнал Вх3 ) sin( omega*t );
		// omega = 2*PI*0.5 = 3,1415926535897 рад/сек
		float amp = ( num_util == MK1 )? 0.31 : 0.98;
		for (int i = 0,j = 0; i < 15624; i++)
		{
			VoltArray[j++] = amp*sin(3.1415926535897*t);   	// Вх2
			VoltArray[j++] = amp*sin(3.1415926535897*t);   	// Вх3
			VoltArray[j++] = 0;       						// ВхЦ1
			VoltArray[j++] = MK_Ucc;	  			 		// Питание
			t += 0.000128;									// период 0.000128 сек - 7812.5 Гц
		}
	}
	if( num_util == MK2 )
	{
		// Определяем массив ArrayToSend
		// Определяем массив VoltArray
		length = 8000;						// 2000 * 4 = 8000
		ArrayToSend = new DWORD[8000];    	// 4 канала ( нужно 3, но по тех. документации можно не менее 4 )
		VoltArray = new double[8000];		// по 2000 ( max) значений для каждого - итого 8000.

		// Определяем массив VoltArray для формирования сигналов длительностью 0.2 сек ( 5 Гц )
		// Рассчет значений для формирования частоты 5 Гц на канале OUT3-1:10 ( сигнал Вх2 )
		// и OUT4-1:10 ( сигнал Вх3 ) sin( omega*t );
		// omega = 2*PI*5 = 6.28318530 * 5  = 31.4159265358 рад/сек
		for (int i = 0,j = 0; i < 2000; i++)
		{
			VoltArray[j++] = 0.98*sin(31.4159265358*t);    	// Вх2
			VoltArray[j++] = 0.98*sin(31.4159265358*t);    	// Вх3
			VoltArray[j++] = 0;       						// ВхЦ1
			VoltArray[j++] = MK_Ucc;  						// Питание
			t += 0.0001;								   	// период 0.0001 сек - 10000 Гц
		}
	}
	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, length, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, length, 5000);
	if( size != length ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// частота дискретизации - 12,5 кГц, на канал 6,25 кГц
	Conf_LTR11.ADCRate.divider = 149; //149                		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (8*(149+1)) = 12,5 кГц

	Conf_LTR11.LChQnt = 2;                            		// количество логических каналов - 2
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (6 << 0); // K7, диапазон - 10В, режим - 16-канальный, физический канал - 6 ( нумерация с нуля )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (7 << 0); // K8, диапазон - 10В, режим - 16-канальный, физический канал - 7 ( нумерация с нуля )
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
// Проверка по п.5.3.6 ТЗ. Сигнал "МК4"  -> параметр num_util = MK4
// Проверка по п.5.3.7 ТЗ. Сигнал "МК5"  -> параметр num_util = MK5
void __fastcall WMk::Util5_6Step1(int num_util)throw(Exception)
{
	int err = 0;
	int length = 0;
	int size;
	double t1 = 0, t2 = 0;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 4;							// Необходимо 3 канала -> реально 4
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(3,1);   // Вх2
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(4,1);   // Вх3
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(5,0);   // ВхЦ1
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);	// Питание

	Conf_LTR34.RingMode = false; 			// Включен потоковый режим
	Conf_LTR34.FrequencyDivisor = 0;        // Для генерации синусоиды 1 Гц
											// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  2МГц / (( 64 - 0 )*4) = 7812.5 Гц.

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;							// Номер шага проверки = 0

	if( num_util == MK4 )
	{
		// Определяем массив ArrayToSend
		// Определяем массив VoltArray
		length =  62496; 					// (7812*2) * 4 = 31248
		ArrayToSend = new DWORD[ 62496];  	// 4 канала ( нужно 3, но по тех. документации можно не менее 4 )
		VoltArray = new double[62496];		// по 15624 ( max) значений для каждого - итого 62496.

		// Определяем массив VoltArray для формирования сигналов длительностью 2 сек ( 0.5 Гц )
		// Рассчет значений для формирования частоты 0.5 Гц на канале OUT3-1:10 ( сигнал Вх2 )
		// и OUT4-1:10 ( сигнал Вх3 ) sin( omega*t );
		// omega = 2*PI*0.5 = 3.1415926535897 рад/сек
		for (int i = 0,j = 0; i < 15624; i++ )
		{
			VoltArray[j++] = 0.98*sin(3.1415926535897*t1); 	// Вх2
			VoltArray[j++] = 0.98*sin(3.1415926535897*t1); 	// Вх3
			VoltArray[j++] = 0;       						// ВхЦ1
			VoltArray[j++] = MK_Ucc;   						// Питание
			t1 += 0.000128;									// период 0.000128 сек - 7812.5 Гц
		}
	}
	if( num_util == MK5 )
	{
		// Определяем массив ArrayToSend
		// Определяем массив VoltArray
		length = 67184; 					// (15624 + 1172) * 4 = 67184
		ArrayToSend = new DWORD[67184];  	// 4 канала ( нужно 3, но по тех. документации можно не менее 4 )
		VoltArray = new double[67184];		// по 7812 ( max) значений для каждого - итого 31248.

		// Определяем массив VoltArray для формирования сигналов длительностью 2 сек ( 0.5 Гц )
		// Рассчет значений для формирования частоты 0.5 Гц на канале OUT3-1:10 ( сигнал Вх2 )
		// и OUT4-1:10 ( сигнал Вх3 ) sin( omega*t ) с задержкой 150 мс ( 1172 отсчета ЦАП )
		// omega = 2*PI*0.5 = 3.1415926535897 рад/сек
		// Для формирования сигнала 0.5 Гц необходимо 15624 отсчетов
		// Плюс задержка для Dх3 150 мс - 1172 отсчета
		// Итого:  15624 + 1172 = 16798 отсчетов
		for (int i = 0,j = 0; i < 16798; i++)
		{
			if( i < 15624 )                                      //** Вх2
			{  VoltArray[j++] = 0.91*sin(3.1415926535897*t2);   // *
				t2 += 0.000128;									// * период 0.000128 сек - 7812.5 Гц
			}else  VoltArray[j++] = 0;                          //**

			if( i < 1173 )   VoltArray[j++] = 0;                //** Вх3
			else { VoltArray[j++] = 0.91*sin(3.1415926535897*t1);	// *
			t1 += 0.000128;										//** период 0.000128 сек - 7812.5 Гц
			}
			VoltArray[j++] = 8;       							// ВхЦ1
			VoltArray[j++] = MK_Ucc;  							// Питание
		}
	}
	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, length, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, length, 5000);
	if( size != length ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 1;                 		// частота дискретизации - 60 кГц  на канал 10 кГц
	Conf_LTR11.ADCRate.divider = 249;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(249+1)) = 60 кГц

	Conf_LTR11.LChQnt = 6;                            		// количество логических каналов - 6
//	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (2 << 0); // K3, диапазон - 10В, режим - 16-канальный, физический канал - 2 ( нумерация с нуля )
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (10 << 0); // K11, диапазон - 10В, режим - 16-канальный, физический канал - 10 ( нумерация с нуля )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (3 << 0); // K4, диапазон - 10В, режим - 16-канальный, физический канал - 3 ( нумерация с нуля )
	Conf_LTR11.LChTbl[2] = (0 << 6) | (0 << 4) | (6 << 0); // K7, диапазон - 10В, режим - 16-канальный, физический канал - 6 ( нумерация с нуля )
	Conf_LTR11.LChTbl[3] = (0 << 6) | (0 << 4) | (7 << 0); // K8, диапазон - 10В, режим - 16-канальный, физический канал - 7 ( нумерация с нуля )
	Conf_LTR11.LChTbl[4] = (0 << 6) | (0 << 4) | (8 << 0); // K9, диапазон - 10В, режим - 16-канальный, физический канал - 8 ( нумерация с нуля )
	Conf_LTR11.LChTbl[5] = (0 << 6) | (0 << 4) | (9 << 0); // K10, диапазон - 10В, режим - 16-канальный, физический канал - 9 ( нумерация с нуля )

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
void __fastcall WMk::Util7Step1( AnsiString &filename )throw(Exception)
{
	int err = 0;
	int length1,length2;
	int size;
	float k_length;

	WAV *wav = new WAV();
	err = wav->Open( filename );
	if( err != WAV_OK )
	{
		const char *str_err = wav->GetErrorString( err );
		delete wav;
		throw Exception(  str_err );
	}
	// Определяем длину выборки одного канала в сэмплах ( 1 сэмпл - 16 бит )
	length1 = wav->TitleWave.len_data/4;		// Длина выборки одного канала WAV файла
	// Находим макс. значение 1 и 2 канала WAV файла
	short dat,max_kanal1wav,max_kanal2wav;
	for( register i = 0; i < length1; i++ )
	{
		// Смотрим первый канал
		dat = ((short(*)[2])wav->BufferWave)[i][0];
		if( i == 0 ) max_kanal1wav = dat;
		else { if( dat > max_kanal1wav ) max_kanal1wav = dat; }
		// Смотрим второй канал
		dat = ((short(*)[2])wav->BufferWave)[i][1];
		if( i == 0 ) max_kanal2wav = dat;
		else { if( dat > max_kanal2wav ) max_kanal2wav = dat; }
	}
	// Коэффициенты преобразования данных WAV файла ( коды АЦП ) в милливольты.
	// Считаем, что максимальное значение WAV файла ( коды АЦП ) соответствует 1 вольту
	float k_kanal1wav,k_kanal2wav;
	k_kanal1wav =  1.0/max_kanal1wav; k_kanal2wav = 1.0/max_kanal2wav;
	// Определяем длину буфера для LTR34 ( length2 != length1 так как различны частоты
	// дискретизации для WAV файла ( wav->TitleWave.freq Гц) и LTR34 ( 7812.5 Гц.)
	k_length = wav->TitleWave.freq/7812.5;
	length2 = length1/k_length;

	ArrayToSend = new DWORD[length2*4];  	// 4 канала
	VoltArray = new double[length2*4];		// по length2 значений для каждого

	LengthButhUtil6 = length2;
	for (int i = 0,j = 0; i < length2; i++)
	{

		VoltArray[j++] = ((short(*)[2])wav->BufferWave)[(int)(i*k_length)][0]*k_kanal1wav;	// Вх2
		VoltArray[j++] = ((short(*)[2])wav->BufferWave)[(int)(i*k_length)][1]*k_kanal2wav;	// Вх3
		VoltArray[j++] = 8.0;      			// ВхЦ1
		VoltArray[j++] = MK_Ucc;  			// Питание
	}

	delete wav;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 4;							// Необходимо 3 канала -> реально 4
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(4,1);   // Вх2
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(3,1);   // Вх3
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(5,0);   // ВхЦ1
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);	// Питание

	Conf_LTR34.RingMode = false; 			// Включен потоковый режим
	Conf_LTR34.FrequencyDivisor = 0;        // Для генерации синусоиды 1 Гц
											// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  2МГц / (( 64 - 0 )*4) = 7812.5 Гц.
	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;							// Номер шага проверки = 0

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	length2 *= 4;
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, length2, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, length2, 5000);
	if( size != length2 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 1;                 		// частота дискретизации - 12,5 кГц  на канал 2,083 кГц
	Conf_LTR11.ADCRate.divider = 249;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(249+1)) = 12,5 кГц

	Conf_LTR11.LChQnt = 2;                            		// количество логических каналов - 6
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (9 << 0); 	// K10, диапазон - 10В, режим - 16-канальный, физический канал - 9 ( нумерация с нуля )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (10 << 0); // K11, диапазон - 10В, режим - 16-канальный, физический канал - 10 ( нумерация с нуля )
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
// FileName - имя файла, которое будем присваивать ("\\RaportIK.xlsx")
// write - записываем его или потом стираем этот файл
#include "math.hpp"
void __fastcall WMk::WriteExcel(AnsiString FileName, bool write)
{
	TDateTime t;

	if(zRaport->ExcelInit()) {
		if(zRaport->CopyRepFromSablon(FileName)) {
			zRaport->WriteData(4, 1, "№ блока " + NumberBlock);
			zRaport->WriteData(4, 3, "Дата " + t.CurrentDateTime().FormatString("dd.mm.yyyy"));
			zRaport->WriteData(4, 6, "Время " + t.CurrentDateTime().FormatString("hh:nn"));

			zRaport->WriteData(9, 5, SimpleRoundTo(currentPlus, -2));    // Ток положительной полярности
			zRaport->WriteData(9, 7, note_currentPlus);

			zRaport->WriteData(11, 5, SimpleRoundTo(currentMinus, -2));    // Ток отрицательной полярности полярности
			zRaport->WriteData(11, 7, note_currentMinus);

			zRaport->WriteData(13, 5, Util3X9);
			zRaport->WriteData(13, 7, note_Util3X9);

			zRaport->WriteData(14, 5, Util3X21);
			zRaport->WriteData(14, 7, note_Util3X21);

			zRaport->WriteData(16, 5, Util2X9);
			zRaport->WriteData(16, 7, note_Util2X9);

			zRaport->WriteData(17, 5, Util2X21);
			zRaport->WriteData(17, 7, note_Util2X21);

			// п.5 Амплитуда
			zRaport->WriteData(18, 5, SimpleRoundTo(Util5MaxX13, -1));
			if(Util5MaxX13 >= 8) zRaport->WriteData(18, 7, "Норма");
				else	zRaport->WriteData(18, 7, "Неисправность");

			// п.5 Время
			zRaport->WriteData(19, 5, SimpleRoundTo(Util5TimeX13, 0));
            zRaport->WriteData(19, 7, note_Util5X13);

			if(write) {
				zRaport->CloseReport("C:\\Документы АКИС\\Протоколы\\КИБ-05\\102\\102-" + NumberBlock + ".xlsx");
			} else {
				zRaport->CloseReport(GetCurrentDir() + "\\~" + NumberBlock + ".xlsx");
			}
        }
    }
}
//------------------------/ MEMBER FUNCTION /--------------------------------
/*DWORD WINAPI WMk::CheckUtil6Thread (WMk *System)
{
	int count = 0;

	for(int i=0; i<2; i++) {
		System->PerepadUtil6[i] = false;
	}

	while(System->RunFlag)
		{
			if(count == ACQ_BLOCK_QNT) count = 0;

			if(System->AcqBlockReady[count]) {

				for(int i = 0; i < ACQ_BLOCK_SIZE; ) {
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil6[0] = true;
					if( System->AcqBuf[count][i++] > 4 )
						System->PerepadUtil6[1] = true;
				}
			System->AcqBlockReady[count] = 0;
			count++;
			}
		}

	ExitThread(0);
	return 0;
}*/
//----------------------------/ END CLASS WMk /------------------------------

//-------------------------------/ CLASS WAV /-------------------------------
#include <stdio.h>
//-------------------------------/ CONSTRUCTOR /-----------------------------
WAV::WAV()
{
	BufferWave = NULL;
}
//-------------------------------/ DESTRUCTOR /------------------------------
WAV::~WAV()
{
	delete BufferWave;
}
//----------------------------/ MEMBER FUCTION /-----------------------------
int __fastcall WAV::Open( AnsiString &filename )
{
	FILE *f;

	f = fopen( filename.c_str(),"rb");
	if ( f == 0 )  return WAV_ERR_FILEOPEN;
	fread( &TitleWave,sizeof(TITLE_WAVE),1,f);
	// Проверяем заголовок
	if ( strncmp(TitleWave.id_riff,"RIFF",4) != 0 ) return WAV_ERR_ID_RIFF;
	if ( strncmp(TitleWave.id_chuck,"WAVE",4) != 0 ) return WAV_ERR_ID_CHUCK;
	if ( strncmp(TitleWave.fmt,"fmt ",4) != 0 ) return WAV_ERR_ID_FMT;

	if( BufferWave != NULL ) delete BufferWave;
	BufferWave = new short [TitleWave.len_data/2];
	fread( BufferWave, 2, TitleWave.len_data/2, f);

	fclose( f );
	return WAV_OK;
}
//----------------------------/ MEMBER FUCTION /-----------------------------
const char* __fastcall WAV::GetErrorString( int error )
{
	const char *error_string[] = { "Ошибка открытия файла", "Не совпал идентификатор RIFF", "Не совпал идентификатор CHUCK",
	"Не совпал идентификатор FMT" };
	if( error > 0 && error <= 4 )
	return error_string[error-1];
	else return "Неизвестная ошибка WAV-файла";
}
//-----------------------------/ END /---------------------------------------
//----------------------------------ЗВУКОВОЕ СОПРОВОЖДЕНИЕ-------------------
//void Voice(char filename)
//{
//	BASS_Init(-1,44100,0,0,NULL);
//	HSAMPLE sample;
//	sample = BASS_SampleLoad(false,filename,0,0,1,BASS_SAMPLE_MONO) ;
//	HCHANNEL channel =  BASS_ChannelPlay(sample,false);
//	BASS_ChannelPlay(channel,false);
//}


