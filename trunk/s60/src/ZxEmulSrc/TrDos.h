#ifndef TRDOS_H_
#define TRDOS_H_

#include "Z80.h"

class CTrDos
{
public:
	//FILE* debug_file;
	zxbyte* CreateImage(int DriveNumber);
	zxbyte* GetImageOffset(int DriveNumber);
	CTrDos(void);
	~CTrDos(void);
	zxbyte InVG93(int port, zxword addr);
	void OutVG93(int port, zxbyte value, zxword addr);
	void Reset(void);
	zxbyte* diskimage[4];
	zxbyte track;
	zxbyte sector;
	zxbyte choose_track;
	zxbyte choose_sector;
	zxbyte side;
	zxbyte vg93command;
	zxbyte vg93result;
	zxbyte drive;
	bool disk_readonly;
	zxbyte data;
	zxword nbyte; //номер запрашиваемого байта
	int OffsetInSector;
};

#endif
