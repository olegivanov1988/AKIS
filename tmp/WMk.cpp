//---------------------------------------------------------------------------
#include <vcl.h>
#include "math.h"
#pragma hdrstop
#include "WMk.h"

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
	RunFlag = 0;
//	AcqBlockSize = 1020;	 // размер массива собираемого блока данных по умолчанию

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
		(void)LTR11_Stop(&System->Conf_LTR11);
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
	double volt_array[] = { 0,0,0,9.7 };

//	AcqBlockSize = 1020;	 // размер массива собираемого блока данных по умолчанию

	RunFlag = 0;
	int err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

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
// Проверка по п.5.3.2 ТЗ.
// Проверяем напряжение на тестовых точках К5,К6.
void __fastcall WMk::Util1Step1()throw(Exception)
{
	int err;
	int size;

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
	VoltArray[0] = 9.7;

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
	Conf_LTR11.ADCRate.prescaler = 8;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 2;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (4 << 0); // K5, диапазон - 10В, режим - 16-канальный, физический канал - 4 ( нумерация с нуля )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (5 << 0); // K6, диапазон - 10В, режим - 16-канальный, физический канал - 5 ( нумерация с нуля )

	// Выполняем конфигурацию модуля АЦП
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
// Проверка по п.5.3.4 ТЗ. Сигнал "МК1"  -> параметр num_util = MK_UTIL2
// Проверка по п.5.3.5 ТЗ. Сигнал "МК2"  -> параметр num_util = MK_UTIL3
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
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 4;							// Необходимо 3 канала -> реально 4
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(3,1);   // Вх2
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(4,1);   // Вх3
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(5,0);   // ВхЦ1
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(1,0);	// Питание

	if( num_util == MK_UTIL2 )				// Для генерации синусоиды 1 Гц
	Conf_LTR34.FrequencyDivisor = 0; 		// делитель частоты дискретизации
											// Fch=Fs/ NumberOfChannel=2 МГц / ((64 - FrequencyDivisor)* NumberOfChannel).
											// Получаем Fch =  2МГц / (( 64 - 0 )*4) = 7812.5 Гц.

	if( num_util == MK_UTIL3 )              // Для генерации синусоиды 50 Гц
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

	if( num_util == MK_UTIL2 )
	{
		// Определяем массив ArrayToSend
		// Определяем массив VoltArray
		length = 31248; 					// 7812 * 4 = 31248
		ArrayToSend = new DWORD[31248];  	// 4 канала ( нужно 3, но по тех. документации можно не менее 4 )
		VoltArray = new double[31248];		// по 7812 ( max) значений для каждого - итого 31248.

		// Определяем массив VoltArray для формирования сигналов длительностью 1 сек ( 1 Гц )
		// Рассчет значений для формирования частоты 1 Гц на канале OUT3-1:10 ( сигнал Вх2 )
		// и OUT4-1:10 ( сигнал Вх3 ) sin( omega*t );
		// omega = 2*PI*1 = 6.28318530 рад/сек
		for (int i = 0,j = 0; i < 7812; i++)
		{
			VoltArray[j++] = 0.91*sin(6.28318530*t);       	// Вх2
			VoltArray[j++] = 0.91*sin(6.28318530*t);       	// Вх3
			VoltArray[j++] = 0;       						// ВхЦ1
			VoltArray[j++] = 9.7;	  						// Питание
			t += 0.000128;									// период 0.000128 сек - 7812.5 Гц
		}
	}
	if( num_util == MK_UTIL3 )
	{
		// Определяем массив ArrayToSend
		// Определяем массив VoltArray
		length = 800;						// 200 * 4 = 800
		ArrayToSend = new DWORD[800];  		// 4 канала ( нужно 3, но по тех. документации можно не менее 4 )
		VoltArray = new double[800];		// по 200 ( max) значений для каждого - итого 800.

		// Определяем массив VoltArray для формирования сигналов длительностью 0.02 сек ( 50 Гц )
		// Рассчет значений для формирования частоты 1 Гц на канале OUT3-1:10 ( сигнал Вх2 )
		// и OUT4-1:10 ( сигнал Вх3 ) sin( omega*t );
		// omega = 2*PI*50 = 6.28318530 * 50  = 314.159265358 рад/сек
		for (int i = 0,j = 0; i < 200; i++)
		{
			VoltArray[j++] = 0.91*sin(314.159265358*t);    	// Вх2
			VoltArray[j++] = 0.91*sin(314.159265358*t);    	// Вх3
			VoltArray[j++] = 0;       						// ВхЦ1
			VoltArray[j++] = 9.7;	  						// Питание
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
	Conf_LTR11.ADCRate.prescaler = 8;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 4;                            		// количество логических каналов - 4
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (0 << 0); // K1, диапазон - 10В, режим - 16-канальный, физический канал - 3 ( нумерация с нуля )
	Conf_LTR11.LChTbl[1] = (0 << 6) | (0 << 4) | (1 << 0); // K2, диапазон - 10В, режим - 16-канальный, физический канал - 4 ( нумерация с нуля )
	Conf_LTR11.LChTbl[2] = (0 << 6) | (0 << 4) | (2 << 0); // K3, диапазон - 10В, режим - 16-канальный, физический канал - 5 ( нумерация с нуля )
	Conf_LTR11.LChTbl[3] = (0 << 6) | (0 << 4) | (3 << 0); // K4, диапазон - 10В, режим - 16-канальный, физический канал - 6 ( нумерация с нуля )

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
// Проверка по п.5.3.6 ТЗ. Сигнал "МК3"  -> параметр num_util = MK_UTIL4
// Проверка по п.5.3.7 ТЗ. Сигнал "МК4"  -> параметр num_util = MK_UTIL5
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

	if( num_util == MK_UTIL4 )
	{
		// Определяем массив ArrayToSend
		// Определяем массив VoltArray
		length = 31248; 					// 7812 * 4 = 31248
		ArrayToSend = new DWORD[31248];  	// 4 канала ( нужно 3, но по тех. документации можно не менее 4 )
		VoltArray = new double[31248];		// по 7812 ( max) значений для каждого - итого 31248.

		// Определяем массив VoltArray для формирования сигналов длительностью 1 сек ( 1 Гц )
		// Рассчет значений для формирования частоты 1 Гц на канале OUT3-1:10 ( сигнал Вх2 )
		// и OUT4-1:10 ( сигнал Вх3 ) sin( omega*t );
		// omega = 2*PI*1 = 6.28318530 рад/сек
		for (int i = 0,j = 0; i < 7812; i++)
		{
			VoltArray[j++] = 0.91*sin(6.28318530*t1);      	// Вх2
			VoltArray[j++] = 0.91*sin(6.28318530*t1);      	// Вх3
			VoltArray[j++] = 0;       						// ВхЦ1
			VoltArray[j++] = 9.7;	  						// Питание
			t1 += 0.000128;									// период 0.000128 сек - 7812.5 Гц
		}
	}
	if( num_util == MK_UTIL5 )
	{
		// Определяем массив ArrayToSend
		// Определяем массив VoltArray
		length = 35936; 					// (7812 + 1172) * 4 = 35936
		ArrayToSend = new DWORD[35936];  	// 4 канала ( нужно 3, но по тех. документации можно не менее 4 )
		VoltArray = new double[35936];		// по 7812 ( max) значений для каждого - итого 31248.

		// Определяем массив VoltArray для формирования сигналов длительностью 1 сек ( 1 Гц )
		// Рассчет значений для формирования частоты 1 Гц на канале OUT3-1:10 ( сигнал Вх2 )
		// и OUT4-1:10 ( сигнал Вх3 ) sin( omega*t ) с задержкой 150 мс ( 1172 отсчета ЦАП )
		// omega = 2*PI*1 = 6.28318530 рад/сек
		// Для формирования сигнала 1 Гц необходимо 7812 отсчетов
		// Плюс задержка для Dх3 150 мс - 1172 отсчета
		// Итого:  7812 + 1172 = 8984 jncxtnjd
		for (int i = 0,j = 0; i < 8984; i++)
		{
			if( i < 1173 )   VoltArray[j++] = 0;                //** Вх2
			else { VoltArray[j++] = 0.91*sin(6.28318530*t1);   // *
			t1 += 0.000128;										//** период 0.000128 сек - 7812.5 Гц
			}
			if( i < 7812 )                                      //** Вх3
			{  VoltArray[j++] = 0.91*sin(6.28318530*t2);       // *
				t2 += 0.000128;									// * период 0.000128 сек - 7812.5 Гц
			}else  VoltArray[j++] = 0;                          //**
			VoltArray[j++] = 8;       							// ВхЦ1
			VoltArray[j++] = 9.7;	  							// Питание
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
	Conf_LTR11.ADCRate.prescaler = 8;                 		// частота дискретизации - 12,5 кГц  на канал 2,083 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (8*(149+1)) = 12,5 кГц

	Conf_LTR11.LChQnt = 6;                            		// количество логических каналов - 6
	Conf_LTR11.LChTbl[0] = (0 << 6) | (0 << 4) | (2 << 0); // K3, диапазон - 10В, режим - 16-канальный, физический канал - 2 ( нумерация с нуля )
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

//-----------------------------/ END /---------------------------------------
