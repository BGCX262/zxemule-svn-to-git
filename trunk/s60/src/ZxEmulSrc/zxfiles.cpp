//zx files :)

//#include <windows.h>
#include <stdio.h>
#include "zxfiles.h"
#include "uApplication.h"
#include "Z80.h"

#ifndef _WIN32_WCE
#define FILEOPEN(a,b) fopen(a,b)
#else
#define FILEOPEN(a,b) _wfopen(a,b)
#endif

/*
Format of .SNA files:

	48k models:

	Offset   Size   Description
   ------------------------------------------------------------------------
   0        1      byte   I
   1        8      word   HL',DE',BC',AF'
   9        10     word   HL,DE,BC,IY,IX
   19       1      byte   Interrupt (bit 2 contains IFF2, 1=EI/0=DI)
   20       1      byte   R
   21       4      words  AF,SP
   25       1      byte   IntMode (0=IM0/1=IM1/2=IM2)
   26       1      byte   BorderColor (0..7, not used by Spectrum 1.7)
   27       49152  bytes  RAM dump 16384..65535
   ------------------------------------------------------------------------
   Total: 49179 bytes

	128k models:

	Offset   Size   Description
   ------------------------------------------------------------------------
   0        27     bytes  SNA header (see above)
   27       16Kb   bytes  RAM bank 5 \
   16411    16Kb   bytes  RAM bank 2  } - as standard 48Kb SNA file
   32795    16Kb   bytes  RAM bank n / (currently paged bank)
   49179    2      word   PC
   49181    1      byte   port 0x7ffd setting
   49182    1      byte   TR-DOS rom paged (1) or not (0)
   49183    16Kb   bytes  remaining RAM banks in ascending order
   ...
   ------------------------------------------------------------------------
   Total: 131103 or 147487 bytes  */

void LoadSNA(LPTSTR filename) {
	zxbyte zbuf[256];

  	ResetZX();

	//FILE* bfile=FILEOPEN(filename,TEXT("rb")); //Русские понимать не будет... жаль
	uSys->uFileOpen(filename);
	//fread(zbuf,27,1,bfile);
	//fread(GetRamPtr(0x4000*5),0x4000,1,bfile);
	//fread(GetRamPtr(0x4000*2),0x4000,1,bfile);
	//fread(GetRamPtr(0x4000*0),0x4000,1,bfile);
	uSys->uFileRead(zbuf,27);
	uSys->uFileRead(GetRamPtr(0x4000*5),0x4000);
	uSys->uFileRead(GetRamPtr(0x4000*2),0x4000);
	uSys->uFileRead(GetRamPtr(0x4000*0),0x4000);

	
	setIR((zbuf[0]<<8)+zbuf[20]); //Register[rR]=zbuf[20];
	setHL_((zbuf[2]<<8)+zbuf[1]); //Register[rL_]=zbuf[1]; Register[rH_]=zbuf[2];
	setDE_((zbuf[4]<<8)+zbuf[3]); //Register[rE_]=zbuf[3]; Register[rD_]=zbuf[4];
	setBC_((zbuf[6]<<8)+zbuf[5]); //Register[rC_]=zbuf[5]; Register[rB_]=zbuf[6];
	setAF_((zbuf[8]<<8)+zbuf[7]); //Register[rF_]=zbuf[7]; Register[rA_]=zbuf[8];
	setHL((zbuf[10]<<8)+zbuf[9]); //Register[rL]=zbuf[9]; Register[rH]=zbuf[10];
	setDE((zbuf[12]<<8)+zbuf[11]); //Register[rE]=zbuf[11]; Register[rD]=zbuf[12];
	setBC((zbuf[14]<<8)+zbuf[13]); //Register[rC]=zbuf[13]; Register[rB]=zbuf[14];
	setIY((zbuf[16]<<8)+zbuf[15]); //Register[rIYl]=zbuf[15]; Register[rIYh]=zbuf[16];
	setIX((zbuf[18]<<8)+zbuf[17]); //Register[rIXl]=zbuf[17]; Register[rIXh]=zbuf[18];
	setIFF(zbuf[19]);
	setAF((zbuf[22]<<8)+zbuf[21]); //Register[rF]=zbuf[21]; Register[rA]=zbuf[22];
	setSP((zbuf[24]<<8)+zbuf[23]); //sp=zbuf[23]+((zbuf[24]*256));
	setIMM(zbuf[25]); //im_mode=zbuf[25];
	//fread(zbuf,4,1,bfile);
	uSys->uFileRead(zbuf,4);
	
	//if (feof(bfile)!=0) {
	if ((uSys->uFileEOF())!=0) { //TODO: 0 непонятен. заменить на константу
		//fclose(bfile);
		uSys->uFileClose();

		setPC(PopZX()); //pc=PopZX();
		SetRamBank(0);
		//bfile=FILEOPEN(TEXT("48.rom"),TEXT("rb"));
		uSys->uFileOpen("c:\\data\\zxemule\\48.rom");
		//fread(GetRomPtr(0x0000),16384,1,bfile);
		uSys->uFileRead(GetRomPtr(0x0000),16384);
		//fclose(bfile);
		uSys->uFileClose();
		SetRomBank(0);

	} else {
		setPC((zbuf[1]<<8)+zbuf[0]);
		OutPortZX(0x7ffd,zbuf[2]);
		memcpy(GetRamPtr(0x4000*(zbuf[2]&7)),GetRamPtr(0x4000*0),0x4000);
		for (int i=0; i<8; i++) {
			if ((i!=2) && (i!=5) && (i!=(zbuf[2]&7)))
				//fread(GetRamPtr(0x4000*i),0x4000,1,bfile);
				uSys->uFileRead(GetRamPtr(0x4000*i),0x4000);
		};
		//fclose(bfile);
		uSys->uFileClose();

		//bfile=FILEOPEN(TEXT("128.rom"),TEXT("rb"));
		uSys->uFileOpen("c:\\data\\zxemule\\128.rom");
		//fread(GetRomPtr(0),0x8000,1,bfile);
		uSys->uFileRead(GetRomPtr(0),0x8000);
		//fclose(bfile);
		uSys->uFileClose();
	};

	//bfile=FILEOPEN(TEXT("trdos.rom"),TEXT("rb"));
	uSys->uFileOpen("c:\\data\\zxemule\\trdos.rom");
	//fread(GetRomPtr(0x8000),0x4000,1,bfile);	//page2
	uSys->uFileRead(GetRomPtr(0x8000),0x4000);
	//fclose(bfile);
	uSys->uFileClose();

	//border=zbuf[26];
};

/*
z80 v.1

The .z80 format is arguably the most widely supported by emulators
across all platforms. .z80 files are memory snapshots - they contain
an image of the contents of the ZX Spectrum memory at a particular instance
in time. As a result of this, they cannot be used to reproduce the original
tape from a snapshot file, but do load almost instantaneously.

The .z80 format was originally developed by Gerton Lunter for use in his Z80 
emulator, and three versions of the format, as saved by Z80 
versions 1.45 (and earlier), 2.x and 3.x (and later) are in use. 
For ease of notation, these will be referred to as versions 1, 2 and 3 of 
the format respectively. Various extensions to the .z80 format have 
also been made by other emulators.

Version 1 of the .z80 format can save only 48K snapshots, and has the following
header:
	
	  Offset  Length  Description
        ---------------------------
        0       1       A register
        1       1       F register
        2       2       BC register pair (LSB, i.e. C, first)
        4       2       HL register pair
        6       2       Program counter
        8       2       Stack pointer
        10      1       Interrupt register
        11      1       Refresh register (Bit 7 is not significant!)
        12      1       Bit 0  : Bit 7 of the R-register
                        Bit 1-3: Border colour
                        Bit 4  : 1=Basic SamRom switched in
                        Bit 5  : 1=Block of data is compressed
                        Bit 6-7: No meaning
        13      2       DE register pair
        15      2       BC' register pair
        17      2       DE' register pair
        19      2       HL' register pair
        21      1       A' register
        22      1       F' register
        23      2       IY register (Again LSB first)
        25      2       IX register
        27      1       Interrupt flipflop, 0=DI, otherwise EI
        28      1       IFF2 (not particularly important...)
        29      1       Bit 0-1: Interrupt mode (0, 1 or 2)
                        Bit 2  : 1=Issue 2 emulation
                        Bit 3  : 1=Double interrupt frequency
                        Bit 4-5: 1=High video synchronisation
                                 3=Low video synchronisation
                                 0,2=Normal
                        Bit 6-7: 0=Cursor/Protek/AGF joystick
                                 1=Kempston joystick
                                 2=Sinclair 2 Left joystick (or user
                                   defined, for version 3 .z80 files)
                                 3=Sinclair 2 Right joystick


        Offset  Length  Description
        ---------------------------
      * 30      2       Length of additional header block (see below)
      * 32      2       Program counter
      * 34      1       Hardware mode (see below)
      * 35      1       If in SamRam mode, bitwise state of 74ls259.
                        For example, bit 6=1 after an OUT 31,13 (=2*6+1)
                        If in 128 mode, contains last OUT to 0x7ffd
If in Timex mode, contains last OUT to 0xf4
      * 36      1       Contains 0xff if Interface I rom paged
If in Timex mode, contains last OUT to 0xff
      * 37      1       Bit 0: 1 if R register emulation on
                        Bit 1: 1 if LDIR emulation on
Bit 2: AY sound in use, even on 48K machines
Bit 6: (if bit 2 set) Fuller Audio Box emulation
Bit 7: Modify hardware (see below)
      * 38      1       Last OUT to port 0xfffd (soundchip register number)
      * 39      16      Contents of the sound chip registers
        55      2       Low T state counter
        57      1       Hi T state counter
        58      1       Flag byte used by Spectator (QL spec. emulator)
                        Ignored by Z80 when loading, zero when saving
        59      1       0xff if MGT Rom paged
        60      1       0xff if Multiface Rom paged. Should always be 0.
        61      1       0xff if 0-8191 is ROM, 0 if RAM
        62      1       0xff if 8192-16383 is ROM, 0 if RAM
        63      10      5 x keyboard mappings for user defined joystick
        73      10      5 x ASCII word: keys corresponding to mappings above
        83      1       MGT type: 0=Disciple+Epson,1=Disciple+HP,16=Plus D
        84      1       Disciple inhibit button status: 0=out, 0ff=in
        85      1       Disciple inhibit flag: 0=rom pageable, 0ff=not
     ** 86      1       Last OUT to port 0x1ffd

The value of the word at position 30 is 23 for version 2 files, and 54 or 55 for 
version 3; the fields marked '*' are the ones that are present in the version 2 
header. The final byte (marked '**') is present only if the word at 
position 30 is 55.

In general, the fields have the same meaning in version 2 and 3 files, with
 the exception of byte 34:

        Value:          Meaning in v2           Meaning in v3
        -----------------------------------------------------
          0             48k                     48k
          1             48k + If.1              48k + If.1
          2             SamRam                  SamRam
          3             128k                    48k + M.G.T.
          4             128k + If.1             128k
          5             -                       128k + If.1
          6             -                       128k + M.G.T.

[As an aside, the documentation for versions 3.00 to 3.02 of Z80 had the 
entries for 'SamRam' and '48k + M.G.T.' in the second column of the 
above table reversed; also bytes 61 and 62 of the format were incorrectly 
documented up to version 3.04. The snaps produced by the older 
versions of Z80 follow what is above; it's just the documentation is wrong.]

Other emulators have extended the .z80 format to support more machine types:

        Value:          Meaning
        -----------------------------------------------------
          7             Spectrum +3
          8             [mistakenly used by some versions of
                         XZX-Pro to indicate a +3]
          9             Pentagon (128K)
         10             Scorpion (256K)
         11             Didaktik-Kompakt
         12             Spectrum +2
         13             Spectrum +2A
         14             TC2048
         15             TC2068
        128             TS2068

While most emulators using these extensions write version 3 files, some write
version 2 files so it's probably best to assume any of these values can be seen 
in either version 2 or version 3 files.

If bit 7 of byte 37 is set, the hardware types are modified slightly: any 48K 
machine becomes a 16K machine, any 128K machines becomes a +2 and any +3 machine 
becomes a +2A.

The hi T state counter counts up modulo 4. Just after the ULA generates its 
once-in-every-20-ms interrupt, it is 3, and is increased by one every 5 emulated 
milliseconds. In these 1/200s intervals, the low T state counter counts down 
from 17471 to 0 (17726 in 128K modes), which make a total of 69888 (70908) T 
states per frame.

The 5 ASCII words (high byte always 0) at 73-82 are the keys corresponding to 
the joystick directions left, right, down, up, fire respectively. Shift, 
Symbol Shift, Enter and Space are denoted by [,],/,\ respectively. The ascii 
values are used only to display the joystick keys; the information in the 5 
keyboard mapping words determine which key is actually pressed (and should 
correspond to the ascii values). The low byte is in the range 0-7 and determines 
the keyboard row. The high byte is a mask byte and determines the column. 
Enter for example is stored as 0x0106 (row 6 and column 1) and 'g' as 0x1001 
(row 1 and column 4).

Byte 60 must be zero, because the contents of the Multiface RAM is not saved 
in the snapshot file. If the Multiface was paged when the snapshot was saved, 
the emulated program will most probably crash when loaded back.

Bytes 61 and 62 are a function of the other flags, such as byte 34, 59, 60 
and 83.

Hereafter a number of memory blocks follow, each containing the compressed 
data of a 16K block. The compression is according to the old scheme, except 
for the end-marker, which is now absent. The structure of a memory block is:

        Byte    Length  Description
        ---------------------------
        0       2       Length of compressed data (without this 3-byte header)
                        If length=0xffff, data is 16384 bytes long and not compressed
        2       1       Page number of block
        3       [0]     Data

The pages are numbered, depending on the hardware mode, in the following way:

        Page    In '48 mode     In '128 mode    In SamRam mode
        ------------------------------------------------------
         0      48K rom         rom (basic)     48K rom
         1      Interface I, Disciple or Plus D rom, according to setting
         2      -               rom (reset)     samram rom (basic)
         3      -               page 0          samram rom (monitor,..)
         4      8000-bfff       page 1          Normal 8000-bfff
         5      c000-ffff       page 2          Normal c000-ffff
         6      -               page 3          Shadow 8000-bfff
         7      -               page 4          Shadow c000-ffff
         8      4000-7fff       page 5          4000-7fff
         9      -               page 6          -
        10      -               page 7          -
        11      Multiface rom   Multiface rom   -
*/

void LoadZ80(LPTSTR filename) {
	//zxbyte zbuf[256];

	ResetZX();

	//FILE* bfile=FILEOPEN(filename,TEXT("rb")); //Русские понимать не будет... жаль

};

void LoadTRD(LPTSTR filename, zxbyte* buffer) {

	//fread(GetRomPtr(0),0x8000,1,bfile);
	//fclose(bfile);
	uSys->uFileOpen("c:\\data\\zxemule\\128.rom");
	uSys->uFileRead(GetRomPtr(0),0x8000);
	uSys->uFileClose();

//	bfile=FILEOPEN(TEXT("trdos.rom"),TEXT("rb"));
//	fread(GetRomPtr(0x8000),0x4000,1,bfile);	//page2
//	fclose(bfile);
	uSys->uFileOpen("c:\\data\\zxemule\\trdos.rom");
	uSys->uFileRead(GetRomPtr(0x8000),0x4000);
	uSys->uFileClose();
//
//
//	//Читаю образ trd
//	bfile=FILEOPEN(filename,TEXT("rb"));
//	fread(buffer,655360,1,bfile); //TODO: trd бывают меньшего размера...
//	fclose(bfile);
	uSys->uFileOpen(filename);
	uSys->uFileRead(buffer,655360);
	uSys->uFileClose();

	ResetZX();
	OutPortZX(0x7ffd,7+16);
	setPC(0); //pc=PopZX();
	EnableTrdos();
};
