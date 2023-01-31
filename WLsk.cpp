//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "WLsk.h"
#include "UnRaport.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
static const char ErrorText1[] = "Ошибка при отправке данных модулю!";
static const char ErrorText2[] = "Ошибка! Количество отправленных сэмплов не равно заданному!";
static const char ErrorText3[] =  "Неизвестная ошибка";
//--------------------------/ CONSTRUCTOR /----------------------------------
WLsk::WLsk():ArrayToSend(0),VoltArray(0)
{
	BlockNumberThread = 100; // инициализация переменной на случай если поток АЦП будет создан позднее
	ReplaceBuf = false;      // иництализация переменной повторной записи буферов
	ReplaceBuf2 = false;     // иництализация переменной повторной записи буферов
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

	//инициализация дескрипторов модулей
	err = LTR34_Init(&Conf_LTR34);
	if(err) goto GOTO_ERROR2;

	err = LTR11_Init(&Conf_LTR11);
	if(err) goto GOTO_ERROR1;
	Conf_LTR11.ModuleInfo.CbrCoef[0].Gain = 1.03;

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
void __fastcall WLsk::Init_LTR27()throw(Exception)
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
void __fastcall WLsk::StartGen_LTR11_LTR34()throw(Exception)
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
DWORD WINAPI WLsk::AcquireThread (WLsk *System)
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
//Если “0” – интерфейсный канал связи с модулем создан и открыт.
//Если значение ненулевое - канал не создан
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

//	AcqBlockSize = 1020;	 // размер массива собираемого блока данных по умолчанию

	RunFlag = 0;            // Выходим из потока сбора измерительной информации от LTR11
	RunFlag2 = 0; 			// Выходим из потока сбора измерительной информации от LTR27 ( ток )

	if( LTR34_IsOpened( &Conf_LTR34 ) == 0 )
	{
		err = LTR34_DACStop( &Conf_LTR34 );
		if(err) goto GOTO_ERROR;
	}else
		Init_LTR11_LTR34();

	Conf_LTR34.ChannelQnt = 8;

	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(6,0);	// Вх1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(7,0);   // Вх2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(8,0);   // Вх3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(3,0);   // Вх4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(4,0);   // Вх5
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(1,0);   // Включение питания
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// лишний канал
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(5,0);   // ВхУл

	Conf_LTR34.RingMode = false; 				// Включен потоковый режим
	Conf_LTR34.FrequencyDivisor = 0;			// Работаем на максимальной частоте

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
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
// Проверка по п.5.2.1 - 5.2.2 ТЗ.
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
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// Необходимо 6 каналов (реально 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(6,0);	// Вх1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(7,0);   // Вх2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(8,0);   // Вх3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(3,0);   // Вх4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(4,0);   // Вх5
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(1,0);   // Включение питания
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// лишний канал
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(5,0);   // ВхУл

//	//--------- Задание параметров работы ЦАП ---------
//	Conf_LTR34.ChannelQnt = 1;		  			// Используем один канал - канал включения питания
//	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(1,0);
//	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(6,0);	// Вх1
////	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(7,0);   // Вх2
////	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(4,0);   // Вх5

	Conf_LTR34.FrequencyDivisor = 40; 		 	// делитель частоты дискретизации

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Выполняем конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	NumStep = 0;		// Номер шага проверки = 0
	// Определяем массив ArrayToSend
	ArrayToSend = new DWORD[8];
	VoltArray = new double[8];
	VoltArray[0] =  LSK_Uh;   		// Вх1
	VoltArray[1] =  LSK_Uh;         // Вх2
	VoltArray[2] =  LSK_Uh;         // Вх3
	VoltArray[3] =  0.0;            // Вх4
	VoltArray[4] =  LSK_Uh;         // Вх5
	VoltArray[5] = 	LSK_Ucc;        // Включение питания
	VoltArray[6] =  0.0;            // лишний канал
	VoltArray[7] =  LSK_InpUl;      // ВхУл

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 8, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 8, 5000);
	if( size != 8 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 1;// 8                 		// частота дискретизации - 50 кГц на канал - 25 кГЦ
	Conf_LTR11.ADCRate.divider = 299;// 149                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(299+1)) = 50 кГц

	Conf_LTR11.LChQnt = 2;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (2 << 4) | (8 << 0); 	// K9 - диапазон - 10В, режим - 32-канальный, канал X9
	Conf_LTR11.LChTbl[1] = (0 << 6) | (2 << 4) | (14 << 0); // K10 - диапазон - 10В, режим - 32-канальный, канал X15
	// Выполняем конфигурацию модуля АЦП
	err = LTR11_SetADC(&Conf_LTR11);
	if(err) goto GOTO_ERROR;

	// Запускаем генерацию
	StartGen_LTR11_LTR34();
	RunFlag2 = 1;				  							//
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
// Проверка по п.5.2.3 ТЗ. Формируем сигнал "ЛС1"

void __fastcall WLsk::Util1Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// Необходимо 6 каналов (реально 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(6,0);	// Вх1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(7,0);   // Вх2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(8,0);   // Вх3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(3,0);   // Вх4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(4,0);   // Вх5
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(1,0);   // Включение питания
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// лишний канал
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(5,0);   // ВхУл

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
	ArrayToSend = new DWORD[24000];  	// 8 канала ( нужно 6, но по тех. документации можно не менее 8 )
	VoltArray = new double[24000];		// по 2926 ( max) значений для каждого - итого 24000 ( max ).

	// Выставляем постоянные уровни сигналов и держим их примерно в течение 0.5 сек
	for (int i = 0,j = 0; i < 10; i++) {
	VoltArray[j++] = LSK_Uh;
	VoltArray[j++] = LSK_Uh;
	VoltArray[j++] = 0;
	VoltArray[j++] = 0;
	VoltArray[j++] = LSK_Uh;
	VoltArray[j++] = LSK_Ucc;		// Питание
	VoltArray[j++] = 0;
	VoltArray[j++] = LSK_InpUl;
	}

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 80, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 5000);
	if( size != 80 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 5;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (2 << 4) | (3 << 0); // K4, диапазон - 10В, режим - 32-канальный, канал X4
	Conf_LTR11.LChTbl[1] = (0 << 6) | (3 << 4) | (0 << 0); // K5, диапазон - 10В, режим - 32-канальный, канал Y1
	Conf_LTR11.LChTbl[2] = (0 << 6) | (3 << 4) | (1 << 0); // K6, диапазон - 10В, режим - 32-канальный, канал Y2
	Conf_LTR11.LChTbl[3] = (0 << 6) | (3 << 4) | (2 << 0); // K7, диапазон - 10В, режим - 32-канальный, канал Y3
	Conf_LTR11.LChTbl[4] = (0 << 6) | (2 << 4) | (7 << 0); // K8, диапазон - 10В, режим - 32-канальный, канал X8

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
// Проверка по п.5.2.3 ТЗ.  Формируем сигнал "ЛС1"

int __fastcall WLsk::Util1Step2()throw(Exception)
{
	int err = 0;
	int size;

	// Определяем массив VoltArray для формирования сигналов длительностью 32 мс
	for( register i = 0, j = 0; i < 125; i++ )
	{
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = LSK_Ucc; 		// Питание
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_InpUl;
	}

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	Conf_LTR34.RingMode = false; 			// Включен потоковый режим
	// конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1000, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1000, 5000);
	if( size != 1000 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// Запускаем генерацию уровней на 32 мсек
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Сразу готовим данные для формирования уровней сигналов на 500 мсек
	for( register i = 0, j = 0; i < 1953; i++ )
	{
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = LSK_Ucc; 		// Питание
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_InpUl;
	}
	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 15624, true);
	if(err) goto GOTO_ERROR;

	// Запускаем генерацию уровней на 468 мсек
	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 15624, 5000);
	if( size != 15624 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// Сразу готовим данные для формирования уровней сигналов на (500 - 32 = 468) мсек
//	for( register i = 0, j = 0; i < 1828; i++ )
	// Сразу готовим данные для формирования уровней сигналов на 750 мсек
	for( register i = 0, j = 0; i < 2926; i++ )
	{
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = LSK_Ucc; 		// Питание
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_InpUl;
	}
	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
//	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 14624, true);
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 23408, true);
	if(err) goto GOTO_ERROR;

	// Запускаем генерацию уровней на 468 мсек
	// Отправляем данные в модуль
//	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 14624, 5000);
//	if( size != 14624 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
	// Запускаем генерацию уровней на 750 мсек
	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 23408, 5000);
	if( size != 23408 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// Сразу готовим данные для формирования уровней сигналов на 10 мсек
	for( register i = 0, j = 0; i < 40; i++ )
	{
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = 0;
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_Uh;
		VoltArray[j++] = LSK_Ucc; 		// Питание
		VoltArray[j++] = 0;
		VoltArray[j++] = LSK_InpUl;
	}
	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 320, true);
	if(err) goto GOTO_ERROR;

	// Запускаем генерацию уровней на 10 мсек
	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 320, 5000);
	if( size != 320 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

//	err = LTR34_DACStart(&Conf_LTR34);
//	if(err) goto GOTO_ERROR;

//	Sleep(100);
	for(int i = 0; i < ACQ_BLOCK_QNT; i++) {       // все блоки данных
		AcqBlockReady[i] = 0;              		   // готовы к записи
	}

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
	return RET_OK;
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// Проверка по п.5.2.4 ТЗ и по п.5.2.7 ТЗ.

int __fastcall WLsk::Util1_2_3Step3()throw(Exception)
{
	int err = 0;

	TerminateThread(hnd_acq_thread2, 0);

	Conf_LTR11.LChQnt = 3;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (2 << 4) | (0 << 0); // K1, диапазон - 10В, режим - 32-канальный, канал X1
	Conf_LTR11.LChTbl[1] = (0 << 6) | (2 << 4) | (1 << 0); // K2, диапазон - 10В, режим - 32-канальный, канал X2
	Conf_LTR11.LChTbl[2] = (0 << 6) | (2 << 4) | (2 << 0); // K3, диапазон - 10В, режим - 32-канальный, канал X3

	LTR11_Close(&Conf_LTR11);

	// Номер слота с платой LTR11 - 1
	err = LTR11_Open(&Conf_LTR11, SADDR_DEFAULT, SPORT_DEFAULT, "", 1);

	// Выполняем конфигурацию модуля АЦП
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
// Проверка по п.5.2.5 ТЗ. Сигнал "ЛС2"

void __fastcall WLsk::Util2Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// Необходимо 6 каналов (реально 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(6,0);	// Вх1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(7,0);   // Вх2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(8,0);   // Вх3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(3,0);   // Вх4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(4,0);   // Вх5
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(1,0);   // Включение питания
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// лишний канал
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(5,0);   // ВхУл

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
	ArrayToSend = new DWORD[5208];  	// 8 канала ( нужно 6, но по тех. документации можно не менее 8 )
	VoltArray = new double[5208];		// по 651 ( max) значений для каждого - итого 5208.

	// Выставляем постоянные уровни сигналов и держим их примерно в течение 0.5 сек
	for (int i = 0,j = 0; i < 10; i++) {
	VoltArray[j++] = LSK_Uh;       	// Вх1 - логическая единица
	VoltArray[j++] = LSK_Uh;       	// Вх2 - логическая единица
	VoltArray[j++] = 0;       		// Вх3 - 0В
	VoltArray[j++] = 0;       		// Вх4 - 0В - отелючен
	VoltArray[j++] = LSK_Uh;       	// Вх5 - логическая единица
	VoltArray[j++] = LSK_Ucc; 		// Питание
	VoltArray[j++] = 0;       		// лишний канал
	VoltArray[j++] = LSK_InpUl;    	// ВхУл - логическая единица
	}

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 80, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 5000);
	if( size != 80 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 1;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 249;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(49+1)) = 100 кГц

	Conf_LTR11.LChQnt = 5;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (2 << 4) | (3 << 0); // K4, диапазон - 10В, режим - 32-канальный, канал X4
	Conf_LTR11.LChTbl[1] = (0 << 6) | (3 << 4) | (0 << 0); // K5, диапазон - 10В, режим - 32-канальный, канал Y1
	Conf_LTR11.LChTbl[2] = (0 << 6) | (3 << 4) | (1 << 0); // K6, диапазон - 10В, режим - 32-канальный, канал Y2
	Conf_LTR11.LChTbl[3] = (0 << 6) | (3 << 4) | (2 << 0); // K7, диапазон - 10В, режим - 32-канальный, канал Y3
	Conf_LTR11.LChTbl[4] = (0 << 6) | (2 << 4) | (7 << 0); // K8, диапазон - 10В, режим - 32-канальный, канал X8

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
// Проверка по п.5.2.5 ТЗ.  "ЛС2"
int __fastcall WLsk::Util2Step2()throw(Exception)
{
	int err = 0;
	int size;
	int num_step_prev;

	// Определяем массив VoltArray для формирования сигналов длительностью 1/6 сек
	for( register i = 0, j = 0; i < 651; i++ )
	{
		VoltArray[j++] = LSK_Uh;		// Вх1 - логическая единица
		VoltArray[j++] = 0;     		// Вх2 - 0В
		VoltArray[j++] = 0;     		// Вх3 - 0В
		VoltArray[j++] = 0;     		// Вх4 - 0В - отключен
		VoltArray[j++] = LSK_Uh;     	// Вх5 - логическая единица
		VoltArray[j++] = LSK_Ucc; 		// Питание
		VoltArray[j++] = 0;     		// лишний канал
		VoltArray[j++] = LSK_InpUl;    	// ВхУл - логическая единица
	}

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	Conf_LTR34.RingMode = false; 			// Включен потоковый режим
	// конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 5208, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 5208, 5000);
	if( size != 5208 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// Запускаем генерацию уровней на 32 мсек
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Отправляем порции данных в модуль для формирования импульса на Вх2
	// длительностью 1 сек
	for( register i = 0; i < 5; i++ )
	{
		size = LTR34_Send(&Conf_LTR34, ArrayToSend, 5208, 5000);
		if( size != 5208 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
	}

	// Поднимаем уровень на Вх2
	for( register i = 0, j = 1; i < 651; i++,j += 8 )
	{
		VoltArray[j] = LSK_Uh;     // Вх2 - логическая единица
	}

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 5208, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 5208, 5000);
	if( size != 5208 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// Поднимаем уровень на Вх3
	// Готовим данные для формирования конечных уровней сигналов на 10 мсек
	for( register i = 0, j = 2; i < 40; i++,j += 8 )
	{
//		VoltArray[j] = LSK_Uh;     	// Вх3 - логическая единица
    	VoltArray[j] = 0;     		// Вх3 - 0
	}

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 320, true);
	if(err) goto GOTO_ERROR;

	// Запускаем генерацию уровней на 10 мсек
	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 320, 5000);
	if( size != 320 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}

}
//------------------------/ MEMBER FUNCTION /--------------------------------
// Проверка по п.5.2.7 ТЗ.  Сигнал "ЛС3"

void __fastcall WLsk::Util3Step1()throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;
	VoltArray = 0;

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// Необходимо 6 каналов (реально 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(6,0);	// Вх1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(7,0);   // Вх2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(8,0);   // Вх3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(3,0);   // Вх4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(4,0);   // Вх5
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(1,0);   // Включение питания
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// лишний канал
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(5,0);   // ВхУл

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
	ArrayToSend = new DWORD[2000];  	// 8 канала ( нужно 6, но по тех. документации можно не менее 8 )
	VoltArray = new double[2000];		// по 250 ( max) значений для каждого - итого 2000.

	// Выставляем постоянные уровни сигналов и держим их примерно в течение 0.5 сек
	for (int i = 0,j = 0; i < 10; i++) {
	VoltArray[j++] = LSK_Uh;   			// Вх1 - логическая единица
	VoltArray[j++] = LSK_Uh;   			// Вх2 - логическая единица
	VoltArray[j++] = LSK_Uh;   			// Вх3 - логическая единица
	VoltArray[j++] = 0;        			// Вх4 - 0В отключен
	VoltArray[j++] = LSK_Uh;   			// Вх5 - 9В
	VoltArray[j++] = LSK_Ucc;  			// Питание
	VoltArray[j++] = 0;        			// лишний канал
	VoltArray[j++] = LSK_InpUl; 		// ВхУл - логическая единица
	}

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 80, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 5000);
	if( size != 80 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 5;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (2 << 4) | (3 << 0); // K4, диапазон - 10В, режим - 32-канальный, канал X4
	Conf_LTR11.LChTbl[1] = (0 << 6) | (3 << 4) | (0 << 0); // K5, диапазон - 10В, режим - 32-канальный, канал Y1
	Conf_LTR11.LChTbl[2] = (0 << 6) | (3 << 4) | (1 << 0); // K6, диапазон - 10В, режим - 32-канальный, канал Y2
	Conf_LTR11.LChTbl[3] = (0 << 6) | (3 << 4) | (2 << 0); // K7, диапазон - 10В, режим - 32-канальный, канал Y3
	Conf_LTR11.LChTbl[4] = (0 << 6) | (2 << 4) | (7 << 0); // K8, диапазон - 10В, режим - 32-канальный, канал X8
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
// Проверка по п.5.2.7 ТЗ.
int __fastcall WLsk::Util3Step2()throw(Exception)
{
	int err = 0;
	int size;
	int num_step_prev;

	// Определяем массив VoltArray для формирования сигналов длительностью (1/3906.25)*244=62.464 мс
	for( register i = 0, j = 0; i < 244; i++ )
	{
		VoltArray[j++] = LSK_Uh;  	// Вх1 - логическая единица
		VoltArray[j++] = 0; 		// Вх2 - 0В
		VoltArray[j++] = LSK_Uh;   	// Вх3 - логическая единица
		VoltArray[j++] = 0;         // Вх4 - 0В - отключен
		VoltArray[j++] = LSK_Uh;   	// Вх5 - логическая единица
		VoltArray[j++] = LSK_Ucc;	// Питание
		VoltArray[j++] = 0;         // лишний канал
		VoltArray[j++] = LSK_InpUl;	// ВхУл
	}

	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	Conf_LTR34.RingMode = false; 			// Включен потоковый режим
	// конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1952, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);
	if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// Запускаем генерацию уровней на 32+32 = 64 мсек
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Отправляем порции данных в модуль для формирования сигналов
	// длительностью 0.5 сек
	for( register i = 0; i < 7; i++ )
	{
		size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);
		if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
//		err = LTR34_DACStart(&Conf_LTR34);
//		if(err) goto GOTO_ERROR;
	}

//	// Готовим данные для формирования тех-же сигналов. но уровень Вх1 = 0
//	// длительностью (1/3906.25)*244=62.464 мс
//	for( register i = 0, j = 0; i < 244; i++, j += 8 )
//	{
//		VoltArray[j] = 0;			// Вх1 - 0В
//	}
	// Готовим данные для формирования тех-же сигналов. но уровень Вх1 = 0
	// длительностью (1/3906.25)*98 = 25 мс
	for( register i = 0, j = 0; i < 98; i++, j += 8 )
	{
		VoltArray[j] = 0;			// Вх1 - 0В
	}
	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 784/*1952*/, true);
	if(err) goto GOTO_ERROR;

//	// Запускаем генерацию 2 порций по 62.464 мс * 2 = 124.928 мс
//	// Отправляем данные в модуль
//	for( register i = 0; i < 2; i++ )
//	{
//		size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);
//		if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
//	}
	// Запускаем генерацию сигналов на 25 мс
	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 784, 5000);
	if( size != 784 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// Готовим данные для формирования тех-же сигналов. но уровень Вх1 = логическая единица
	// длительностью (1/3906.25)*225 = 57.6 мс
	for( register i = 0, j = 0; i < 225; i++, j += 8 )
	{
		VoltArray[j] = LSK_Uh;			// Вх1 - логическая единица
	}
	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1800, true);
	if(err) goto GOTO_ERROR;

//	// Запускаем генерацию 4 порций по 62.464 мс * 4 = 249.856 мс
//	// Отправляем данные в модуль
//	for( register i = 0; i < 4; i++ )
//	{
//		size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);
//		if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
//	}
	// Запускаем генерацию 6 порций по 57.6 мс * 6 = 346,2 мс
	// Отправляем данные в модуль
	for( register i = 0; i < 6; i++ )
	{
		size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1800, 5000);
		if( size != 1800 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
	}

//	// Готовим данные для формирования тех-же сигналов. но уровень Вх2 = 0
//	// длительностью (1/3906.25)*244=62.464 мс
//	for( register i = 0, j = 1; i < 244; i++, j += 8 )
//	{
//		VoltArray[j] = 0;			// Вх1 - 0В
//	}
//	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
//	// Указываем, что сигнал задан В ВОЛЬТАХ!
//	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1952, true);
//	if(err) goto GOTO_ERROR;
//
//	// Запускаем генерацию 2 порций по 62.464 мс * 2 = 124.928 мс
//	// Отправляем данные в модуль
//	for( register i = 0; i < 2; i++ )
//	{
//		size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);
//		if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
//	}

	// Сразу готовим данные для формирования конечных уровней сигналов на 10 мсек
	for( register i = 0, j = 0; i < 40; i++ )
	{
		VoltArray[j++] = LSK_Uh;   		// Вх1 - логическая единица
		VoltArray[j++] = LSK_Uh;       	// Вх2 - 0В
		VoltArray[j++] = LSK_Uh;       	// Вх3 - 0В

		VoltArray[j++] = 0;        		// Вх4 - 0В - отключен
		VoltArray[j++] = LSK_Uh;       	// Вх5 - логическая единица
		VoltArray[j++] = LSK_Ucc; 		// Питание
		VoltArray[j++] = 0;         	// лишний канал
		VoltArray[j++] = LSK_InpUl;    	// ВхУл
	}
	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 320, true);
	if(err) goto GOTO_ERROR;

	// Запускаем генерацию уровней на 10 мсек
	// Отправляем данные в модуль
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 320, 5000);
	if( size != 320 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

GOTO_ERROR:
	if( err != NULL )
	{
		ErrorRelease( err );
	}
}
//------------------------/ MEMBER FUNCTION /--------------------------------
// Проверка по п.5.2.8 и п.5.2.9 ТЗ.

void __fastcall WLsk::Util4_5Step1(int num_util)throw(Exception)
{
	int err = 0;
	int size;
	ArrayToSend = 0;
	VoltArray = 0;

//	AcqBlockSize = ACQ_BLOCK_SIZE_2;	 // выставляем размер массива собираемого блока данных 1024, т. к. 8 каналов АЦП

	Init_LTR11_LTR34();
	//-----------------------------------------------------------------------
	//	Программируем LTR34 - ЦАП
	//-----------------------------------------------------------------------
	Conf_LTR34.ChannelQnt = 8;							// Необходимо 6 каналов (реально 8)
	Conf_LTR34.LChTbl[0] = LTR34_CreateLChannel(6,0);	// Вх1
	Conf_LTR34.LChTbl[1] = LTR34_CreateLChannel(7,0);   // Вх2
	Conf_LTR34.LChTbl[2] = LTR34_CreateLChannel(8,0);   // Вх3
	Conf_LTR34.LChTbl[3] = LTR34_CreateLChannel(3,0);   // Вх4
	Conf_LTR34.LChTbl[4] = LTR34_CreateLChannel(4,0);   // Вх5
	Conf_LTR34.LChTbl[5] = LTR34_CreateLChannel(1,0);   // Включение питания
	Conf_LTR34.LChTbl[6] = LTR34_CreateLChannel(2,0);	// лишний канал
	Conf_LTR34.LChTbl[7] = LTR34_CreateLChannel(5,0);   // ВхУл

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
	ArrayToSend = new DWORD[2000];  	// 8 канала ( нужно 6, но по тех. документации можно не менее 8 )
	VoltArray = new double[2000];		// по 250 ( max) значений для каждого - итого 2000.

	// Выставляем постоянные уровни сигналов и держим их примерно в течение 0.5 сек

	for (int i = 0,j = 0; i < 10; i++) {
	VoltArray[j++] = LSK_Uh;        // Вх1 - 9В
	VoltArray[j++] = LSK_Uh;        // Вх2 - 9В
	VoltArray[j++] = LSK_Uh;        // Вх3 - 9В
	VoltArray[j++] = ((num_util == UTIL_4 )? LSK_Uh:0); // для п.5.2.8 Вх4 - LSK_Uh
														// для п.5.2.9 Вх4 - отключен
	VoltArray[j++] = ((num_util == UTIL_4 )? 0:LSK_Uh); // для п.5.2.8 Вх5 - отключен
														// для п.5.2.9 Вх5 - LSK_Uh
	VoltArray[j++] = LSK_Ucc;  		// Питание
	VoltArray[j++] = 0;        		// лишний канал
	VoltArray[j++] = ((num_util == UTIL_4 )? LSK_InpUl:0);  // ВхУл
	}

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 80, true);
	if(err) goto GOTO_ERROR; //throw Exception(  LTR34_GetErrorString(err) );

	// Отправляем данные в модуль
//	if (num_util == UTIL_4 ) {                                        //!!!!!!!!!!!!!!!!!!
//	   size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 2500);
//	}
//  else
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 5000);
	if( size != 80 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; } //throw Exception(  ((size < 0 )? ErrorText1 : ErrorText2 ) );

	//-----------------------------------------------------------------------
	//	Программируем LTR11 - АЦП
	//-----------------------------------------------------------------------
	//---------задание параметров работы АЦП-------
	Conf_LTR11.ADCRate.prescaler = 8;                 		// частота дискретизации - 100 кГц
	Conf_LTR11.ADCRate.divider = 149;                 		// Fadc = LTR11_CLOCK / (prescaler*(divider+1)) = 15 000 / (1*(149+1)) = 100 кГц

	Conf_LTR11.LChQnt = 8;                            		// количество логических каналов - 1
	Conf_LTR11.LChTbl[0] = (0 << 6) | (2 << 4) | (3 << 0); // K4, диапазон - 10В, режим - 32-канальный, канал X4
	Conf_LTR11.LChTbl[1] = (0 << 6) | (3 << 4) | (0 << 0); // K5, диапазон - 10В, режим - 32-канальный, канал Y1
	Conf_LTR11.LChTbl[2] = (0 << 6) | (3 << 4) | (1 << 0); // K6, диапазон - 10В, режим - 32-канальный, канал Y2
	Conf_LTR11.LChTbl[3] = (0 << 6) | (3 << 4) | (2 << 0); // K7, диапазон - 10В, режим - 32-канальный, канал Y3
	Conf_LTR11.LChTbl[4] = (0 << 6) | (2 << 4) | (7 << 0); // K8, диапазон - 10В, режим - 32-канальный, канал X8
	Conf_LTR11.LChTbl[5] = (0 << 6) | (2 << 4) | (0 << 0); // K1, диапазон - 10В, режим - 32-канальный, канал X1
	Conf_LTR11.LChTbl[6] = (0 << 6) | (2 << 4) | (1 << 0); // K2, диапазон - 10В, режим - 32-канальный, канал X2
	Conf_LTR11.LChTbl[7] = (0 << 6) | (2 << 4) | (2 << 0); // K3, диапазон - 10В, режим - 32-канальный, канал X3

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
// Проверка по п.5.2.8 ТЗ.
int __fastcall WLsk::Util4_5Step2(int num_util)throw(Exception)
{
	int err = 0;
	int size;
	int num_step_prev;

	// Определяем массив VoltArray для формирования сигналов длительностью (1/3906.25)*244=62.464 мс
	for( register i = 0, j = 0; i < 244; i++ )
	{
		VoltArray[j++] = LSK_Uh;        // Вх1 - 9В
		VoltArray[j++] = 0;        		// Вх2 - 0В
		VoltArray[j++] = LSK_Uh;        // Вх3 - 9В
		VoltArray[j++] = ((num_util == UTIL_4 )? 9:0); // для п.5.2.8 Вх4 - 9В
												   // для п.5.2.9 Вх4 - отключен
		VoltArray[j++] = ((num_util == UTIL_4 )? 0:9); // для п.5.2.8 Вх5 - отключен
												   // для п.5.2.9 Вх5 - 9В
		VoltArray[j++] = LSK_Ucc;  		// Питание
		VoltArray[j++] = 0;        		// лишний канал
		VoltArray[j++] =( num_util == UTIL_5 )? 0 : LSK_InpUl;    // ВхУл

	}
	err = LTR34_DACStop( &Conf_LTR34 );
	if(err) goto GOTO_ERROR;

	// Перед конфигурацией ОБЯЗАТЕЛЬНО выполняем сброс модуля
	err = LTR34_Reset(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	Conf_LTR34.RingMode = false; 			// Включен потоковый режим
	// конфигурацию модуля
	err = LTR34_Config(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1952, true);
	if(err) goto GOTO_ERROR;

	// Отправляем данные в модуль
//		if (num_util == UTIL_4 ) {                                        //!!!!!!!!!!!!!!!!!!
//	   size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 2500);
//	} else
	size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);
	if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }

	// Запускаем генерацию уровней на 62.464 мсек
	err = LTR34_DACStart(&Conf_LTR34);
	if(err) goto GOTO_ERROR;

	// Отправляем еще 7 порцтй данных в модуль для формирования пачки импульсов на Вх2
	// длительностью 0.5 сек

	for( register i = 0; i < 7; i++ )
	{
//		if (num_util == UTIL_4 ) {                                        //!!!!!!!!!!!!!!!!!!
//				  size = LTR34_Send(&Conf_LTR34, ArrayToSend, 80, 2500);
//			}else
					size = LTR34_Send(&Conf_LTR34, ArrayToSend, 1952, 5000);

		if( size != 1952 ) { err = ((size < 0 )? 1 : 2); goto GOTO_ERROR; }
	}

	// Сразу готовим данные для формирования тех-же сигналов. но уровень Вх1 = 0
	for( register i = 0, j = 0; i < 122; i++, j += 8 )
	{
		VoltArray[j] = 0;			// Вх1 - 0В
	}
	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 1952, true);
	if(err) goto GOTO_ERROR;

	// Запускаем генерацию 8 порций по 62.464 мсек = 499.712 мс
	// Отправляем данные в модуль
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

	// Сразу готовим данные для формирования конечных уровней сигналов на 10 мсек
	for( register i = 0, j = 0; i < 40; i++ )
	{
		VoltArray[j++] = LSK_Uh;			// Вх1 - 9В
		VoltArray[j++] = ((num_util == UTIL_4 )? LSK_Uh:0); 	// для п.5.2.8 Вх2 - 9В
														// для п.5.2.9 Вх2 - 0В
		VoltArray[j++] = LSK_Uh;       		// Вх3 - 0В
		VoltArray[j++] = LSK_Uh;         	// Вх4 - 9В
		VoltArray[j++] = 0;       			// Вх5 - отключен
		VoltArray[j++] = LSK_Ucc;			// Питание
		VoltArray[j++] = 0;         		// лишний канал
		VoltArray[j++] = 0;//LSK_InpUl; 		// ВхУл
	}
	// Вызываем функцию подготовки слов-данных. ArrayToSend - массив, готовый к отправке.
	// Указываем, что сигнал задан В ВОЛЬТАХ!
	err = LTR34_ProcessData(&Conf_LTR34, VoltArray, ArrayToSend, 320, true);
	if(err) goto GOTO_ERROR;

	// Запускаем генерацию уровней на 10 мсек
	// Отправляем данные в модуль
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
// FileName - имя файла, которое будем присваивать ("\\RaportIK.xlsx")
// write - записываем его или потом стираем этот файл
#include "math.hpp"
void __fastcall WLsk::WriteExcel(AnsiString FileName, bool write)
{
	TDateTime t;

	if(zRaport->ExcelInit()) {
		if(zRaport->CopyRepFromSablon(FileName)) {
			zRaport->WriteData(4, 1, "№ блока " + NumberBlock);
			zRaport->WriteData(4, 3, "Дата " + t.CurrentDateTime().FormatString("dd.mm.yyyy"));
			zRaport->WriteData(4, 6, "Время " + t.CurrentDateTime().FormatString("hh:nn"));

			zRaport->WriteData(9, 5, SimpleRoundTo(currentPlus, -2));    // Ток
			zRaport->WriteData(9, 7, note_current);

			zRaport->WriteData(10, 5, frequencyK9);    					 // Время периода
			zRaport->WriteData(10, 7, note_frequencyK9);

			// п.3
		 //  maxK1_K3[0] = 0; maxK1_K3[1] = 0; maxK1_K3[2] = 0;    //закомментировано 16,08
			zRaport->WriteData(12, 5, SimpleRoundTo(maxK1_K3[0], -1));
			if(CheckLSK2_2[0][0] == 1) zRaport->WriteData(12, 7, "Норма");
				else zRaport->WriteData(12, 7, "Неисправность");

			zRaport->WriteData(13, 5, SimpleRoundTo(maxK1_K3[1], -1));
			if(CheckLSK2_2[0][1] == 1) zRaport->WriteData(13, 7, "Норма");
				else zRaport->WriteData(13, 7, "Неисправность");

			zRaport->WriteData(14, 5, SimpleRoundTo(maxK1_K3[2], -1));
			if(CheckLSK2_2[0][2] == 1) zRaport->WriteData(14, 7, "Норма");
				else zRaport->WriteData(14, 7, "Неисправность");

			// п.4
		   	maxK1_K3_Util4[0] = 0; maxK1_K3_Util4[1] = 0; maxK1_K3_Util4[2] = 0;   //закомментировано 16,08
			zRaport->WriteData(16, 5, SimpleRoundTo(maxK1_K3_Util4[0], -1));
			if(CheckLSK2_2[3][0] == 1) zRaport->WriteData(16, 7, "Норма");
				else zRaport->WriteData(16, 7, "Неисправность");

			zRaport->WriteData(17, 5, SimpleRoundTo(maxK1_K3_Util4[1], -1));
			if(CheckLSK2_2[3][1] == 1) zRaport->WriteData(17, 7, "Норма");
				else zRaport->WriteData(17, 7, "Неисправность");

			zRaport->WriteData(18, 5, SimpleRoundTo(maxK1_K3_Util4[2], -1));
			if(CheckLSK2_2[3][2] == 1) zRaport->WriteData(18, 7, "Норма");
				else zRaport->WriteData(18, 7, "Неисправность");

			// п.5
			if(PerepadUtil5[4] == true) {
				zRaport->WriteData(19, 5, "Есть импульс");
				zRaport->WriteData(19, 7, "Норма");
			} else {
				zRaport->WriteData(19, 5, "Нет импульса");
				zRaport->WriteData(19, 7, "Неисправность");
			}

			// п.6
			if(checkBlock) {
				zRaport->WriteData(20, 2, "Блок исправен");
			}	else {
				zRaport->WriteData(20, 2, "Блок неисправен");
			}


			if(write) {
				zRaport->CloseReport("C:\\Документы АКИС\\Протоколы\\КИБ-05\\069\\069-" + NumberBlock + ".xlsx");
			} else {
				zRaport->CloseReport(GetCurrentDir() + "\\~" + NumberBlock + ".xlsx");
			}
        }
    }
}
//---------------------------------------------------------------------------
// Создание отчёта для экрана ЛСК 1
// FileName - имя файла, которое будем присваивать ("\\RaportIK.xlsx")
// write - записываем его или потом стираем этот файл
#include "math.hpp"
void __fastcall WLsk::WriteExcel_LSK_1(AnsiString FileName, bool write)
{
	TDateTime t;

	if(zRaport->ExcelInit()) {
		if(zRaport->CopyRepFromSablon(FileName)) {
			zRaport->WriteData(4, 1, "№ блока " + NumberBlock);
			zRaport->WriteData(4, 3, "Дата " + t.CurrentDateTime().FormatString("dd.mm.yyyy"));
			zRaport->WriteData(4, 6, "Время " + t.CurrentDateTime().FormatString("hh:nn"));

			zRaport->WriteData(9, 5, SimpleRoundTo(currentPlus, -2));    // Ток
			zRaport->WriteData(9, 7, note_current);

			zRaport->WriteData(10, 5, frequencyK9);    					 // Время периода
			zRaport->WriteData(10, 7, note_frequencyK9);

			// п.6
			if(checkBlock) {
				zRaport->WriteData(20, 2, "Блок исправен");
			}	else {
				zRaport->WriteData(20, 2, "Блок неисправен");
			}

			if(write) {
				zRaport->CloseReport("C:\\Документы АКИС\\Протоколы\\КИБ-05\\069\\069-" + NumberBlock + ".xlsx");
			} else {
				zRaport->CloseReport(GetCurrentDir() + "\\~" + NumberBlock + ".xlsx");
			}

		}
	}
}
//-------------------------------/ END /-------------------------------------
