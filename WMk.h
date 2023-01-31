//---------------------------------------------------------------------------

#ifndef WMkH
#define WMkH
//---------------------------------------------------------------------------
#include "ltr\include\ltr34api.h"
#include "ltr\include\ltr11api.h"
#include "ltr\include\ltr27api.h"
//#include "bass.h"
//-------------------------/ CLASS /-----------------------------------------
// Определения для возвращаемых значений функций
#define RET_OK		0
#define RET_ERROR	1
#define RET_END		2
#define RET_NO_U	3

#define ACQ_BLOCK_SIZE 	4320    // размер собираемых блоков данных
#define ACQ2_BLOCK_SIZE	(2*LTR27_MEZZANINE_NUMBER*64)
#define ACQ_BLOCK_QNT 	  1     // количество блоков для сохранения принятых данных
#define PARAMETERS_MK  	  18    	// количество контролируемых проверок на экране МК

#define MK1			1
#define MK2			2
#define MK3			3
#define MK4			4
#define MK5			5

#define UPLUS_MAX	500
#define UPLUS_MIN	440

#define UMINUS_MAX	-500
#define UMINUS_MIN	-440

#define UTIL2_3_MIN	0.5

#define MK_Ucc	   	9.7		// Напряжение питания модуля

#define CURRENT_MIN		0.1	// было 0.15 15.06.17
#define CURRENT_MIDD    1.0
#define CURRENT_MAX		1.25

#define POROG_X9_X21	6.0

#define TIME_X13_MIN	3.5	// Введено 9.02.17
#define TIME_X13_MAX	8  	// Введено 9.02.17

#define TIME_X27_MIN	130 // Введено 6.04.15
#define TIME_X27_MAX	180 //

class ZRaport;

class WMk
{
friend class TMK_1;
friend class TMK_2;
public:
	WMk();
	~WMk();

protected:
	int __fastcall UtilStop( bool power_off = false );
	void __fastcall Util0Step1()throw(Exception);
//	void __fastcall Util1Step1()throw(Exception);
	void __fastcall Util2_3_4Step1(int num_util)throw(Exception);
	void __fastcall Util5_6Step1(int num_util)throw(Exception);
	void __fastcall Util7Step1( AnsiString &filename)throw(Exception);
	void __fastcall WriteExcel(AnsiString FileName, bool write);
private:
	void __fastcall Init_LTR11_LTR34()throw(Exception);
	void __fastcall Init_LTR27()throw(Exception);
	void __fastcall StartGen_LTR11_LTR34()throw(Exception);
	int __fastcall IsOpenedLTR34();
	void __fastcall ErrorRelease( int err )throw(Exception);
	static DWORD WINAPI AcquireThread (WMk *System);
	static DWORD WINAPI AcquireThread2 (WMk *System);
   //	void Voice(char filename)throw(Exception);
//	static DWORD WINAPI CheckUtil6Thread (WMk *System);

private:
	DWORD *ArrayToSend;
	double *VoltArray;
	int NumStep;					// Номер шага проверки
	TLTR34 Conf_LTR34;
	TLTR11 Conf_LTR11;
    TLTR27 Conf_LTR27;
	int RunFlag,RunFlag2;
	int AcqBlockReady[ACQ_BLOCK_QNT];
	int Acq2BlockReady[ACQ_BLOCK_QNT];
	double AcqBuf[ACQ_BLOCK_QNT][ACQ_BLOCK_SIZE];
	double Acq2Buf[ACQ_BLOCK_QNT][ACQ2_BLOCK_SIZE];
	int BlockNumberThread,BlockNumberThread2;
	bool ReplaceBuf,ReplaceBuf2;
//	bool PerepadUtil1[5];
//	bool PerepadUtil2[5];
	bool PerepadUtil3[4];
	bool PerepadUtil4[7];
	bool PerepadUtil6[2];
	HANDLE hnd_acq_thread2;
	double Uplus, Uminus;
	double Util2MaxK3;
	double Util2MaxK4;
	AnsiString NumberBlock;
	double currentPlus;
	double currentMinus;
	bool PorogK7_X9[2];
	bool PorogK8_X21[2];
	AnsiString note_currentPlus;
	AnsiString note_currentMinus;
	double Util3X9;
	double Util3X21;
	AnsiString note_Util3X9;
	AnsiString note_Util3X21;
	double Util2X9;
	double Util2X21;
	AnsiString note_Util2X9;
	AnsiString note_Util2X21;
	int CheckMK[PARAMETERS_MK];
	AnsiString note_Util5X13;
	double Util5MaxX13;
	double Util5TimeX13;
	int PowerPauseSecond;

	int LengthButhUtil6;
	ZRaport *zRaport;
};

/* CheckMK
0 - первоначальное значение, 1 - норма, 2 - ошибка
[0] - проверка тока и питания
[1] - Util1 в Х9
[2] - Util1 в Х21
[3] - Util2 в Х9
[4] - Util2 в Х21
[5] - Util3 в Х9
[6] - Util3 в Х21
[7] - Util4 в Х9
[8] - Util4 в Х21
[9] - Util4 в Х27
[10] - Util4 в Х13
[11] - Util5 в Х9
[12] - Util5 в Х21
[13] - Util5 в Х27
[14] - Util5 в Х13
[15] - Util5 в Х10
[16] - Util6 в Х10
[17] - Util6 в Х13
*/
//----------------------------------/ STRUCT /-------------------------------
// Структура заголовка WAV файла

//Вначале идет заголовок RIFF файла:
typedef struct {
	char id[4]; 			// идентификатор файла = "RIFF" = 0x46464952
	long len;   			// длина файла без этого заголовка
} IDRiff;
//----------------------------------/ STRUCT /-------------------------------
//Заголовок куска WAV:
typedef struct {
	char id[4];  			// идентификатор = "WAVE" = 0x45564157
	char fmt[4]; 			// идентификатор = "fmt " = 0x20746D66
	long len;    			// длина этого куска WAV - файла,
} IDChuckWave;
//----------------------------------/ STRUCT /-------------------------------
// За ним не посредственно кусок WAV:
 typedef struct {
	int type;   		// тип звуковых данных, бывает - !!!
						// 1 - просто выборка;
						// 0x101 - IBM mu-law;
						// 0x102 - IBM a-law;
						// 0x103 - ADPCM.
	int channels; 		// число каналов 1/2 - !!!
	long SamplesPerSec; // частота выборки - !!!
	long AvgBytesPerSec;// частота выдачи байтов
	int align; 			// выравнивание
	int bits; 			// число бит на выборку  - !!!
} IDWave;
//----------------------------------/ STRUCT /-------------------------------
// Далее идентификатор выборки:
typedef struct {
	char id[4]; 			// идентификатор ="data" =0x61746164
	long len;   			// длина выборки ( кратно 2 )
} IDSampleWave;
//----------------------------------/ STRUCT /-------------------------------
// Общая структура заголовка WAV-файла, включая все перечисленные выше
typedef struct {
	char id_riff[4];    	// идентификатор файла = "RIFF" = 0x46464952
	long len_riff;          // длина файла без этого заголовка

	char id_chuck[4];       // идентификатор = "WAVE" = 0x45564157
	char fmt[4];            // идентификатор = "fmt " = 0x20746D66
	long len_chuck;         // длина этого куска WAV - файла,

	short type;             // тип звуковых данных, бывает - !!!
							// 1 - просто выборка;
							// 0x101 - IBM mu-law;
							// 0x102 - IBM a-law;
							// 0x103 - ADPCM.
	short channels;         // число каналов 1/2 - !!!
	long freq;              // частота выборки - !!!
	long bytes;             // частота выдачи байтов
	short align;            // выравнивание
	short bits;             // число бит на выборку  - !!!

	char id_data[4];		// идентификатор ="data" =0x61746164
	long len_data;			// длина выборки в байтах
} TITLE_WAVE;
//------------------------------/ CLASS /------------------------------------
// Коды ошибок для функции Open
#define WAV_OK				0
#define WAV_ERR_FILEOPEN	1
#define WAV_ERR_ID_RIFF		2
#define WAV_ERR_ID_CHUCK	3
#define WAV_ERR_ID_FMT		4
class WAV
{
public:
	WAV();
	~WAV();
	int __fastcall Open( AnsiString &filename );
	const char* __fastcall GetErrorString( int err );
//protected:
	TITLE_WAVE TitleWave;
	short *BufferWave;
};
//---------------------------------------------------------------------------
#endif