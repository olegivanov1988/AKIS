#ifndef LTRLABAPITYPES_H_
#define LTRLABAPITYPES_H_

typedef enum 
{
	ELMT_V,
	ELMT_mV,	
	ELMT_A,
	ELMT_mA,
	ELMT_Hz,
	ELMT_kHz,
	ELMT_Points,	//	�������
	ELMT_Unknown
}E_LTRLAB_Measure_Units;

typedef enum 
{
	ELDT_Double_Array,
	ELDT_Float_Array,
	ELDT_Dword_Array,
	ELDT_Int_Array,
	ELDT_Unknown
}E_LTRLAB_Data_Type;

/*===============================================================================================*/
#define LTR_LAB_MAX_CHANNELS	128
#define LTR_LAB_MAX_FRAMERATE	25
#define MAX_TLTR_LAB_CHANNEL_MATES	256
/*===============================================================================================*/

#pragma pack(4)

typedef struct 
{	
	PBYTE	Buffer; // ����� ��� ������� � ���������
	PDWORD	DataWrited; // ���������� ������ ���������� - ��������
	DWORD	MaxBufferSize; // ���������� ������, ���������� ������� ���� �����������

	HANDLE	DataReady;		// ������ � ������ ������ ������ ��� ���������� �������� ��� �������
	HANDLE	DataCommited;	// ������ � ������ ������ ������� � ����� ����� ��������� ������
}TLTR_LAB_CHANNEL_MATES,*PTLTR_LAB_CHANNEL_MATES;

typedef struct
{	
	CHAR	Name[256];		// �������� ������
	E_LTRLAB_Measure_Units Measure_Units;	// ������� ���������, �� ��������� �, �, ��,...
	E_LTRLAB_Data_Type Data_Type;	// ��� ������ � �������
	BYTE	OutputChannel;			// ���� 1 - �� ����� �� �����, ���� 0 - �� ����� �� ����
	BYTE	Enabled;			// 1 - ����� �������, 0 - ����� ��������, ������������ � LTRLab

	BYTE	SupportChangeDataRate;	// ����� ������������ ��������� ������� �������������
	double	DataRateMax;		// ������������ �������� � �������/�
    double	DataRateMin;		// ����������� �������� � �������/�	
	double	DataRate;		// ������� �������� ������, � �������/�
	
	DWORD	FrameSize;		// ���������� �������, ����������� �� ���� ���

	DWORD	NumMates;	// ���������� ��������� ������, ��� ��� ����� ���������� �������� ������

	TLTR_LAB_CHANNEL_MATES ChannelMates [MAX_TLTR_LAB_CHANNEL_MATES];
}TLTR_LAB_CHANNEL_CAP,*PTLTR_LAB_CHANNEL_CAP;

#pragma pack()

#endif //LTRLABAPITYPES_H_