#include "Z80.h"
//#include "..\StdAfx.h"
#include ".\trdos.h"

//TODO: ������� �������, ������� ����� ������������ ��� ������ ��.
//      ��� ������ ������� �����-������ �� ������ ������� ���������
//      ��� ��������� � ������������ �������� ���������� �� ��������� �����.
//#define _save_trdos_debug_

CTrDos::CTrDos(void)
: OffsetInSector(0)
{
	for (int i=0;i<4;i++) diskimage[i]=0;
	disk_readonly = true;
	Reset();

#if defined(_save_trdos_debug_)	
	debug_file=fopen("trdosdbg.txt","w");
#endif
}

CTrDos::~CTrDos(void)
{
	for (int i=0;i<4;i++)
		if (diskimage[i]!=0) delete[] diskimage[i];

#if defined(_save_trdos_debug_)
	fclose(debug_file);
#endif
}

zxbyte CTrDos::InVG93(int port, zxword addr)
{
	switch (port) {
		case 0x1f: //������� ���������
			if (((vg93command>>4)<8) && (track==0)) vg93result^=2; //���� ������� ��������������� � ������� 0
			return vg93result;
		case 0x3f:
			return track;
		case 0x5f:
			return sector;
		case 0x7f:
			switch ((vg93command>>4)) {
				case 9: case 8: //������ 
					if (OffsetInSector==255) vg93result=0; else vg93result=1;
					zxbyte b=diskimage[drive][((track*2+side)*16+(sector-1))*256+(OffsetInSector++)];
					return b;
					break;
				case 0x0c: //������ ������
					vg93result=0x03;
					switch (nbyte++) {
					case 0: return track;
					case 1: return side;
					case 2: return sector;
					case 3: return 1;
					case 4: return 0;
					case 5: vg93result=0x00; return 0;
					};
					break;
			};
		case 0xff:
			if ((vg93command>>4)==0) return 0x80; //��������������
			if ((vg93command>>4)<8) return 0x80; //�����
			if ((vg93command>>4)==0xc) 
				if (nbyte<6) return 0x40; else return 0x80;//������ ������
			if ((vg93command>>5)==4) { //������
				return (OffsetInSector<256)?0x40:0x80; //�� �������/������
			}; 
			return 0;
	};
	return zxbyte();
}

void CTrDos::OutVG93(int port, zxbyte value, zxword addr)
{
#if defined(_save_trdos_debug_)
	fprintf(debug_file,"%04X OUT (%02X),%02X\n",addr,port,value);
#endif
	switch (port) {
		case 0x1f: //������� ������
			vg93command=value;
			switch (value>>4) {
				case 0: //��������������
					track=0;
					vg93result=0;
					break;
				case 1: //�����
					if (data<80) {
						track=data;
						choose_track=data;
						if (track==0) vg93result=4; else vg93result=0;
					} else {
						vg93result=0x10;
					};					
					break;
				case 2: case 3: //��� � ���������� �����������
					break;
				case 4: case 5: //��� ������
					break;
				case 6: case 7: //��� �����
					break;
				case 8: case 9: //������ �������
					if (diskimage[drive]==0) vg93command=0; //���� ��� �����
					OffsetInSector=0;
					break;
				case 10: case 11: //������ �������
					break;
				case 12:	//������ ������
					nbyte=0;
					break;
				case 13:	//�������������� ����������
					break;
				case 14:	//������ �������
					break;
				case 15:	//������ �������
					break;
			};
			break;
		case 0x3f:	//������� �������
			if (value<=80) choose_track=value;
			break;
		case 0x5f:	//������� �������
			if (value<=16) sector=value;
			break;
		case 0x7f:	//������� ������
			data=value; //!!!! ����� ���������?
			break;
		case 0xff:	//��������� �������
			//!!! bit 2=reset vg93 !!!
			//!!! bit 6=FM/MFM
			//!!! bit 3=�������� �������
			drive=value&3;
			side=((value>>4)&1)^1;
			vg93result=0;
			if (value&4==4) Reset();
			break;
	};
}

void CTrDos::Reset(void)
{
	sector=0;
	side=0;
	track=0;
	data=0;
	choose_track=0;
	choose_sector=0;
	vg93command=0;
	vg93result=0;
	nbyte=0;
}

zxbyte* CTrDos::GetImageOffset(int DriveNumber)
{
	if (DriveNumber<0 || DriveNumber>3) return null;
	return diskimage[DriveNumber];
}

zxbyte* CTrDos::CreateImage(int DriveNumber)
{
	if (DriveNumber<0 || DriveNumber>3) return null;
	if (diskimage[DriveNumber]==null)
		diskimage[DriveNumber]=new zxbyte[655360];
	return diskimage[DriveNumber];
}

/*

}

void wave::close()
{
	waveOutUnprepareHeader(hwo,&whdr[NumberOfHeader],sizeof(whdr[NumberOfHeader]));
	waveOutReset(hwo);
	waveOutClose(hwo);
#if defined(_save_trdos_debug_)
	fclose(debug_file);
#endif
}

void wave::push(ays16 data)
{
	//if (BufLen<BUFFERLENGTH)
	//{
	*((ays16*)(whdr[NumberOfHeader].lpData+BufLen))=data;
#if defined(_save_trdos_debug_)
		//data/=8;
		fwrite(&data,sizeof(data),1,debug_file);
#endif
*/
