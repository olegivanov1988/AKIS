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
	ELMT_Points,	//	отсчеты
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
	PBYTE	Buffer; // буфер для общения с адресатом
	PDWORD	DataWrited; // количество данных переданных - принятых
	DWORD	MaxBufferSize; // количество данных, макимально могущих быть переданными

	HANDLE	DataReady;		// данные в буфере канала готовы для считывания системой или модулем
	HANDLE	DataCommited;	// данные в буфере канала считына и можно буфер заполнять дальше
}TLTR_LAB_CHANNEL_MATES,*PTLTR_LAB_CHANNEL_MATES;

typedef struct
{	
	CHAR	Name[256];		// название канала
	E_LTRLAB_Measure_Units Measure_Units;	// Еденицы измерения, по умолчанию В, А, мВ,...
	E_LTRLAB_Data_Type Data_Type;	// Тип данных в массиве
	BYTE	OutputChannel;			// если 1 - то канал на выход, если 0 - то канал на вход
	BYTE	Enabled;			// 1 - канал включен, 0 - канал выключен, используется в LTRLab

	BYTE	SupportChangeDataRate;	// канал поддерживает изменение частоты индивидуально
	double	DataRateMax;		// максимальная скорость в семплов/с
    double	DataRateMin;		// минимальная скорость в семплов/с	
	double	DataRate;		// текущая скорость канала, в семплов/с
	
	DWORD	FrameSize;		// количество семплов, считываемое за один раз

	DWORD	NumMates;	// количество адресатов канала, или что почти невероятно исходных данных

	TLTR_LAB_CHANNEL_MATES ChannelMates [MAX_TLTR_LAB_CHANNEL_MATES];
}TLTR_LAB_CHANNEL_CAP,*PTLTR_LAB_CHANNEL_CAP;

#pragma pack()

#endif //LTRLABAPITYPES_H_