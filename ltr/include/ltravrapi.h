//---------------------------------------------------------------------------
// avr programming part
//---------------------------------------------------------------------------
#ifndef __ltravrapi__
#define __ltravrapi__
#include "ltr\\include\\ltrapi.h"
#ifdef __cplusplus
extern "C" {
#endif
// коды ошибок
#define LTRAVR_OK                          (LTR_OK)    /*Выполнено без ошибок.*/
#define LTRAVR_ERROR_RECV_PRG_DATA_ECHO    (-200l)     /*Ошибка приема эхо ответа данных для программирования.*/
#define LTRAVR_ERROR_SEND_PRG_DATA         (-201l)     /*Ошибка отправки данных команды програмирования avr.*/
#define LTRAVR_ERROR_RECV_PRG_ENABLE_ACK   (-202l)     /*Ошибка приема подтверждения команды входа в режим программирования.*/
#define LTRAVR_ERROR_SEND_PRG_ENB_CMD      (-203l)     /*Ошибка отправки команды входа в режим программирования.*/
#define LTRAVR_ERROR_CHIP_ERASE            (-204l)     /*Ошибка стирания flash команд avr.*/
#define LTRAVR_ERROR_READ_PRG_MEM          (-205l)     /*Ошибка считывания flash команд avr.*/
#define LTRAVR_ERROR_WRITE_PRG_MEM         (-206l)     /*Ошибка программирования flash команд avr.*/
#define LTRAVR_ERROR_READ_FUSE_BITS        (-207l)     /*Ошибка считывания fuse витов avr.*/
#define LTRAVR_ERROR_WRITE_FUSE_BITS       (-208l)     /*Ошибка программирования fuse витов avr.*/
#define LTRAVR_ERROR_READ_SIGN             (-209l)     /*Ошибка считывания сигнатуры avr.*/
//
#define ATMEGA8515_FLASH_SIZE  (4*1024)
#define ATMEGA8515_PAGE_SIZE   (32)
#define ATMEGA8515_EEPROM_SIZE (512)
#define FUSE_BIT_S8515C   (1<<15)
#define FUSE_BIT_WDTON    (1<<14)
#define FUSE_BIT_SPIEN    (1<<13)
#define FUSE_BIT_CKOPT    (1<<12)
#define FUSE_BIT_EESAVE   (1<<11)
#define FUSE_BIT_BOOTSZ1  (1<<10)
#define FUSE_BIT_BOOTSZ0  (1<< 9)
#define FUSE_BIT_BOOTRST  (1<< 8)
#define FUSE_BIT_BODLEVEL (1<< 7)
#define FUSE_BIT_BODEN    (1<< 6)
#define FUSE_BIT_SUT1     (1<< 5)
#define FUSE_BIT_SUT0     (1<< 4)
#define FUSE_BIT_CKSEL3   (1<< 3)
#define FUSE_BIT_CKSEL2   (1<< 2)
#define FUSE_BIT_CKSEL1   (1<< 1)
#define FUSE_BIT_CKSEL0   (1<< 0)
//-----------------------------------------------------------------------------
#pragma pack(4)
typedef struct                      //
{                                   //
    TLTR ltr;                       //
    BOOL low_speed_flag;            // флаг переключения крейта в режим низкой с
                                    // корости обмена с модулями
} TLTRAVR;                          //
#pragma pack()
//-----------------------------------------------------------------------------
EXPORTCALL(INT) LTRAVR_Init(TLTRAVR *module);
EXPORTCALL(INT) LTRAVR_Open(TLTRAVR *module, DWORD saddr, WORD sport, CHAR *csn, WORD cc);
EXPORTCALL(INT) LTRAVR_Close(TLTRAVR *module);
EXPORTCALL(INT) LTRAVR_ChipErase(TLTRAVR *module);
EXPORTCALL(INT) LTRAVR_ReadProgrammMemory(TLTRAVR *module, WORD *buf, DWORD size, DWORD addr);
EXPORTCALL(INT) LTRAVR_WriteProgrammMemory(TLTRAVR *module, WORD *buf, DWORD size, DWORD addr);
EXPORTCALL(INT) LTRAVR_ReadFuseBits(TLTRAVR *module, BYTE *buf);
EXPORTCALL(INT) LTRAVR_WriteFuseBits(TLTRAVR *module, BYTE *buf);
EXPORTCALL(INT) LTRAVR_ReadSignature(TLTRAVR *module, BYTE *buf);
EXPORTCALL(INT) LTRAVR_SetSpeedFlag(TLTRAVR *module, BOOL sflag);
//
EXPORTCALL(LPCSTR) LTRAVR_GetErrorString(INT error);
#ifdef __cplusplus
}
#endif
#endif
