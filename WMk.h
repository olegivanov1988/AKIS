//---------------------------------------------------------------------------

#ifndef WMkH
#define WMkH
//---------------------------------------------------------------------------
#include "ltr\include\ltr34api.h"
#include "ltr\include\ltr11api.h"
#include "ltr\include\ltr27api.h"
//#include "bass.h"
//-------------------------/ CLASS /-----------------------------------------
// ����������� ��� ������������ �������� �������
#define RET_OK		0
#define RET_ERROR	1
#define RET_END		2
#define RET_NO_U	3

#define ACQ_BLOCK_SIZE 	4320    // ������ ���������� ������ ������
#define ACQ2_BLOCK_SIZE	(2*LTR27_MEZZANINE_NUMBER*64)
#define ACQ_BLOCK_QNT 	  1     // ���������� ������ ��� ���������� �������� ������
#define PARAMETERS_MK  	  18    	// ���������� �������������� �������� �� ������ ��

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

#define MK_Ucc	   	9.7		// ���������� ������� ������

#define CURRENT_MIN		0.1	// ���� 0.15 15.06.17
#define CURRENT_MIDD    1.0
#define CURRENT_MAX		1.25

#define POROG_X9_X21	6.0

#define TIME_X13_MIN	3.5	// ������� 9.02.17
#define TIME_X13_MAX	8  	// ������� 9.02.17

#define TIME_X27_MIN	130 // ������� 6.04.15
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
	int NumStep;					// ����� ���� ��������
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
0 - �������������� ��������, 1 - �����, 2 - ������
[0] - �������� ���� � �������
[1] - Util1 � �9
[2] - Util1 � �21
[3] - Util2 � �9
[4] - Util2 � �21
[5] - Util3 � �9
[6] - Util3 � �21
[7] - Util4 � �9
[8] - Util4 � �21
[9] - Util4 � �27
[10] - Util4 � �13
[11] - Util5 � �9
[12] - Util5 � �21
[13] - Util5 � �27
[14] - Util5 � �13
[15] - Util5 � �10
[16] - Util6 � �10
[17] - Util6 � �13
*/
//----------------------------------/ STRUCT /-------------------------------
// ��������� ��������� WAV �����

//������� ���� ��������� RIFF �����:
typedef struct {
	char id[4]; 			// ������������� ����� = "RIFF" = 0x46464952
	long len;   			// ����� ����� ��� ����� ���������
} IDRiff;
//----------------------------------/ STRUCT /-------------------------------
//��������� ����� WAV:
typedef struct {
	char id[4];  			// ������������� = "WAVE" = 0x45564157
	char fmt[4]; 			// ������������� = "fmt " = 0x20746D66
	long len;    			// ����� ����� ����� WAV - �����,
} IDChuckWave;
//----------------------------------/ STRUCT /-------------------------------
// �� ��� �� ������������� ����� WAV:
 typedef struct {
	int type;   		// ��� �������� ������, ������ - !!!
						// 1 - ������ �������;
						// 0x101 - IBM mu-law;
						// 0x102 - IBM a-law;
						// 0x103 - ADPCM.
	int channels; 		// ����� ������� 1/2 - !!!
	long SamplesPerSec; // ������� ������� - !!!
	long AvgBytesPerSec;// ������� ������ ������
	int align; 			// ������������
	int bits; 			// ����� ��� �� �������  - !!!
} IDWave;
//----------------------------------/ STRUCT /-------------------------------
// ����� ������������� �������:
typedef struct {
	char id[4]; 			// ������������� ="data" =0x61746164
	long len;   			// ����� ������� ( ������ 2 )
} IDSampleWave;
//----------------------------------/ STRUCT /-------------------------------
// ����� ��������� ��������� WAV-�����, ������� ��� ������������� ����
typedef struct {
	char id_riff[4];    	// ������������� ����� = "RIFF" = 0x46464952
	long len_riff;          // ����� ����� ��� ����� ���������

	char id_chuck[4];       // ������������� = "WAVE" = 0x45564157
	char fmt[4];            // ������������� = "fmt " = 0x20746D66
	long len_chuck;         // ����� ����� ����� WAV - �����,

	short type;             // ��� �������� ������, ������ - !!!
							// 1 - ������ �������;
							// 0x101 - IBM mu-law;
							// 0x102 - IBM a-law;
							// 0x103 - ADPCM.
	short channels;         // ����� ������� 1/2 - !!!
	long freq;              // ������� ������� - !!!
	long bytes;             // ������� ������ ������
	short align;            // ������������
	short bits;             // ����� ��� �� �������  - !!!

	char id_data[4];		// ������������� ="data" =0x61746164
	long len_data;			// ����� ������� � ������
} TITLE_WAVE;
//------------------------------/ CLASS /------------------------------------
// ���� ������ ��� ������� Open
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