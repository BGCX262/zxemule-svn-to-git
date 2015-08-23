#ifndef Z80_H_
#define Z80_H_

#include "uTypes.h"

//typedef		unsigned char	zxbyte;
//typedef		unsigned short	zxword;
//typedef		signed char		zxsbyte;
//typedef		signed short	zxsword;
//typedef		signed int		zxsdword;

#define far
//typedef unsigned long       DWORD;
//typedef int                 BOOL;
//typedef unsigned char       BYTE;
//typedef unsigned short      WORD;
//typedef float               FLOAT;
//typedef FLOAT               *PFLOAT;
//typedef BOOL near           *PBOOL;
//typedef BOOL far            *LPBOOL;
//typedef BYTE near           *PBYTE;
//typedef BYTE far            *LPBYTE;
//typedef int near            *PINT;
//typedef int far             *LPINT;
//typedef WORD near           *PWORD;
//typedef WORD far            *LPWORD;
//typedef long far            *LPLONG;
//typedef DWORD near          *PDWORD;
//typedef DWORD far           *LPDWORD;
//typedef short far           *LPSHORT;
typedef void far            *LPVOID;
//typedef CONST void far      *LPCVOID;



//typedef int                 INT;
//typedef unsigned int        UINT;
//typedef unsigned int        *PUINT;


#define LOBYTE(b) ((zxbyte)(b&0xff))
#define HIBYTE(b) ((zxbyte)((b>>8)&0xff))
//TODO: HIWORD перепроверить
#define HIWORD(b) ((zxword)((b>>16)&0xffff))

#define NULL 0
#define null 0

//#include <windows.h>
#include "TrDos.h"

void ResetZX(void);
bool step(void);
void print_z80_state(bool);
LPVOID GetRamPtr(int adr); //Хрень, убрать нафиг int
LPVOID GetRomPtr(int adr);
LPVOID GetScrPtr(void);
LPVOID GetAYRegs(void);
zxbyte GetBorderColor(void);

zxbyte GetMemZX( zxword address );
void SetMemZX( zxword address, zxbyte data);
zxword GetWordMemZX( zxword address );
void SetWordMemZX( zxword address, zxword data );

void SetRomBank(zxbyte page);
void SetRamBank(zxbyte page);
void OutPortZX(zxword port, zxbyte value);

const int TRDOS_BANK=2;

void EnableTrdos(void);

void SetKeyZX(zxbyte row, zxbyte nkey, bool s);
void ResetKeyboardZX(void);
zxword PopZX( void );

void setHL(zxword value);
void setDE(zxword value); 
void setBC(zxword value); 
void setAF(zxword value); 
void setHL_(zxword value);
void setDE_(zxword value);
void setBC_(zxword value);
void setAF_(zxword value);
void setSP(zxword value);
void setPC(zxword value);
void setTS(zxsdword value);
void setIFF(zxbyte value); 
void setIMM(zxbyte value);
void setIR(zxword value);
void setIX(zxword value);
void setIY(zxword value);

void ConnectTrDos(CTrDos* lptrdosclass);

#endif
