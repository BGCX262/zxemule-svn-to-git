//TODO: ПОМЕНЯТЬ ВСЕ ТИПЫ НА ВНУТРЕННИЕ!!! т.е. на zx или uT

#ifndef _WIN32_WCE
//#include <iostream>
#endif

#include "z80.h"
#include "EmulSpec.h"
//#include <iostream.h>
//#include <windows.h>
#include "zxfiles.h"

//#define Msg(txt) printf(txt); printf("\n");
//#define Msg2(txt1,txt2) printf(txt1,txt2); printf("\n");
#define Msg(txt)
#define Msg2(txt1,txt2)
#define Msg3(txt1,txt2,txt3)
//#define Msg(txt) if (tstticker1>2) {printf(txt); printf("\n");};
//#define Msg2(txt1,txt2) if (tstticker1>2) {printf(txt1,txt2); printf("\n");};
//#define Msg3(txt1,txt2,txt3) if (tstticker1>2) {printf(txt1,txt2,txt3); printf("\n");};

#define rrF Register[rF]
#define flagC (rrF&1)
#define rrA Register[rA]
#define rrB Register[rB]
#define rrC Register[rC]
#define rrD Register[rD]
#define rrE Register[rE]
#define rrH Register[rH]
#define rrL Register[rL]
#define rrR Register[rR]
#define rrI Register[rI]

#define bin(b7,b6,b5,b4,b3,b2,b1,b0) (b7*128+b6*64+b5*32+b4*16+b3*8+b2*4+b1*2+b0)

#define incrPCTS(uslov,hlPC,hlTS,ixPC,ixTS,rPC,rTS) \
	if (uslov) { \
		if (flag_DDFD!=_NO_) { t_states+=hlTS; pc+=hlPC; } \
		else { t_states+=ixTS; pc+=ixPC; } \
	} else { t_states+=4; pc+=rPC };

//TODO: Переделать константные переменные в #define
const int rB=0; const int rC=1;
const int rD=2; const int rE=3;
const int rH=4; const int rL=5; 
const int r_HL_=6; const int rA=7; const int rF=8;
const int rB_=9; const int rC_=10;
const int rD_=11; const int rE_=12;
const int rH_=13; const int rL_=14;
const int rA_=16; const int rF_=17;
const int rR=18; const int rI=19;
const int rIXh=20; const int rIXl=21;
const int rIYh=22; const int rIYl=23;

const int rBC=0;
const int rDE=1;
const int rHL=2;
const int rSP=3;
const int rAF=3;

//processor type
int t_states_per_int=71680;

char* szrrr[]={"B","C","D","E","H","L","(HL)","A"};
char* szppp[]={"B","C","D","E","XH","XL","(IX+","A"};
char* szqqq[]={"B","C","D","E","YH","YL","(IY+","A"};
char* szCCC[]={"NZ","Z","NC","C","PO","PE","P","M"};
char* szdd[]={"BC","DE","HL","SP"};
char* szqq[]={"BC","DE","HL","AF"};
char* szpp[]={"BC","DE","IX","SP"};
char* szrr[]={"BC","DE","IY","SP"};

////////////////////////
// ZX EMULATION STATE //

//                   B C D E H L ? A F B'C'D'E'H' L'  ? A'F'R I XhXl Yh   Yl
zxbyte Register[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x38,0,0,0,0,0,0,0,0x5C,0x3a};
zxbyte ram[8*16384];
zxbyte rom[4*16384];
zxbyte keyboard[8]={255,255,255,255,255,255,255,255};

//	Бит  адреса		Сброшенный бит		  Бит  адреса
//			         	D0	D1	D2	D3	D4	D4	D3	D2	D1	D0		
//	11	п	#F7FE	A11	1	2	3	4	5	6	7	8	9	0	А12	#EFFE	п	12
//	10	о	#FBFE	A10	Q	W	E	R	Т	Y	U	I	О	Р	А13	#DFFE	о	13
//	9	Р	#FDFE	A9	A	S	D	F	G	H	J	К	L	EN	А14	#BFFE	Р	14
//	8	т	#FEFE	A8	CS	Z	X	С	V	В	N	М	SS	SP	А15	#7FFE	т	15

zxword pc=0;
zxword sp=0;

zxbyte RomBank;
zxbyte RamBank;
zxbyte port7ffd=0;
zxbyte portfe=0;
bool trdos_enabled=false;

zxbyte ayselectedport=0;
zxbyte ayport[16];

zxsdword pg_shift[4]={0,0x4000*5-0x4000,0x4000*2-0x8000,0x4000*0-0xc000}; //int larger then 2byte!!!

int t_states=0; //0..~80000
int im_mode=0;
int IFF1=0;
int IFF2=0;
const int _NO_=0;
const int _DD_=rIXh-rH;
const int _FD_=rIYh-rH;
int flag_DDFD=_NO_;

const int i_diei=2;
const int i_prefix=3;

unsigned char opcode;
int param1;
int param2;
zxbyte param3; //for ix-iy operations

int i;


void EnableTrdos(void) {
	SetRomBank(TRDOS_BANK);
	trdos_enabled=true;
}

//functions for no_prefix instructions
int func00000(void); int func00001(void); int func00010(void); int func00011(void);
int func00100(void); int func00101(void); int func00110(void); int func00111(void);
int func01xxx(void); int func10xxx(void); 
int func11000(void); int func11001(void); int func11010(void); int func11011(void);
int func11100(void); int func11101(void); int func11110(void); int func11111(void);
//functions for CB_prefix instructions
void funcCB00xxx(void); void funcCB01xxx(void); void funcCB10xxx(void); void funcCB11xxx(void);
//functions for ED_prefix instructions
void funcED01000(void); void funcED01001(void); void funcED01010(void); void funcED01011(void);
void funcED01100(void); void funcED01101(void); void funcED01110(void); void funcED01111(void);
void funcED10000(void); void funcED10001(void); void funcED10010(void); void funcED10011(void);
void funcEDxxxxx(void);

// tst corresponding flag
bool testCCC (int ccc);
// get value of BCDEHL(HL)A->ix,iy register
int get_rpq_reg(int reg);
// set value of BCDEHL(HL)A->ix,iy register
void set_rpq_reg(int reg, zxbyte value);

int get_reg(int reg);
void set_reg(int reg, zxbyte value);

//work with zx memory
zxbyte GetMemZX(zxword address);
void SetMemZX(zxword address, zxbyte data);
zxword GetWordMemZX( zxword address );
void SetWordMemZX( zxword address, zxword data );
//push and pop word to/from stack
void PushZX( zxword val );
zxword PopZX( void );
//get word from [pc+1][pc+2]
zxword GetWordPCShift1(void);
//works with zx i/o ports
zxbyte InPortZX(zxword port);
void OutPortZX(zxword port, zxbyte value);
//tst parity of testbyte
zxbyte parity(zxbyte testbyte);
//tst overflow of testbyte
zxbyte overflow(zxsword testsword);
//tst null of testbyte
zxbyte zero(zxbyte testbyte);
//give rezult of adding a1&a2, changes F register
zxbyte add(zxbyte a1, zxbyte a2, zxbyte a3);
zxbyte sub(zxbyte a1, zxbyte a2, zxbyte a3);
zxword add16(zxword a1, zxword a2);
zxword adc16(zxword a1, zxword a2, zxbyte a3);
zxword sbc16(zxword a1, zxword a2, zxbyte a3);

int (*func_table[32])() = {
	func00000,func00001,func00010,func00011,
	func00100,func00101,func00110,func00111,
	func01xxx,func01xxx,func01xxx,func01xxx,
	func01xxx,func01xxx,func01xxx,func01xxx,
	func10xxx,func10xxx,func10xxx,func10xxx,
	func10xxx,func10xxx,func10xxx,func10xxx,
	func11000,func11001,func11010,func11011,
	func11100,func11101,func11110,func11111
};

void (*funcCB_table[4])() = {
	funcCB00xxx,funcCB01xxx,funcCB10xxx,funcCB11xxx
};

void (*funcED_table[32])() = {
	funcEDxxxxx,funcEDxxxxx,funcEDxxxxx,funcEDxxxxx,
	funcEDxxxxx,funcEDxxxxx,funcEDxxxxx,funcEDxxxxx,
	funcED01000,funcED01001,funcED01010,funcED01011,
	funcED01100,funcED01101,funcED01110,funcED01111,
	funcED10000,funcED10001,funcED10010,funcED10011,
	funcEDxxxxx,funcEDxxxxx,funcEDxxxxx,funcEDxxxxx,
	funcEDxxxxx,funcEDxxxxx,funcEDxxxxx,funcEDxxxxx,
	funcEDxxxxx,funcEDxxxxx,funcEDxxxxx,funcEDxxxxx
};

zxword getHLIXIY() {return (get_rpq_reg(rH)<<8)+get_rpq_reg(rL);};
zxword getHL() {return (Register[rH]<<8)+Register[rL];};
zxword getBC() {return (Register[rB]<<8)+Register[rC];};
zxword getDE() {return (Register[rD]<<8)+Register[rE];};
void setHLIXIY(zxword value) {set_rpq_reg(rH,HIBYTE(value)); set_rpq_reg(rL,LOBYTE(value));};
void setHL(zxword value) {Register[rH]=HIBYTE(value); Register[rL]=LOBYTE(value);};
void setDE(zxword value) {Register[rD]=HIBYTE(value); Register[rE]=LOBYTE(value);};
void setBC(zxword value) {Register[rB]=HIBYTE(value); Register[rC]=LOBYTE(value);};
void setAF(zxword value) {Register[rA]=HIBYTE(value); Register[rF]=LOBYTE(value);};
void setHL_(zxword value) {Register[rH_]=HIBYTE(value); Register[rL_]=LOBYTE(value);};
void setDE_(zxword value) {Register[rD_]=HIBYTE(value); Register[rE_]=LOBYTE(value);};
void setBC_(zxword value) {Register[rB_]=HIBYTE(value); Register[rC_]=LOBYTE(value);};
void setAF_(zxword value) {Register[rA_]=HIBYTE(value); Register[rF_]=LOBYTE(value);};
void setIX(zxword value) {Register[rIXh]=HIBYTE(value); Register[rIXl]=LOBYTE(value);};
void setIY(zxword value) {Register[rIYh]=HIBYTE(value); Register[rIYl]=LOBYTE(value);};
void setIR(zxword value) {Register[rI]=HIBYTE(value); Register[rR]=LOBYTE(value);};
void setSP(zxword value) {sp=value;};
void setPC(zxword value) {pc=value;};
void setTS(zxsdword value) {t_states=value;};
void setIFF(zxbyte value) {IFF1=value&1; IFF2=value>>1;};
void setIMM(zxbyte value) {im_mode=value;};
zxbyte GetBorderColor(void) {return portfe&7;};

int tstticker1;

CTrDos* lptrdos;

void ConnectTrDos(CTrDos* lptrdosclass) {
	lptrdos=lptrdosclass;
};

void ResetZX(void) {
	port7ffd=0;
	portfe=0;
	trdos_enabled=false;
	IFF1=0;
	IFF2=0;
	for (i=0;i<24;i++) Register[i]=0;
	for (i=0;i<16;i++) ayport[i]=0;
	ayselectedport=0;
	t_states=0;
	pc=0;
	sp=0;
	im_mode=0;
	flag_DDFD=_NO_;
};

// make one step
bool step(void)
{
	//zxword pc1=pc;
	//int t_states1=t_states;
	flag_DDFD=0;
	do {
		if (pc==0x5d94) {
			int tttttt=1;
		};
		//tstticker1=3;
		//if ((pc==0x5b02)) tstticker1=3; //else tstticker1=0;
		//if (tstticker1==3) print_z80_state(pc<0x5b8c);
		//print_z80_state(tstticker1>2);
		if (trdos_enabled) {
			if (pc>0x3fff)	{ SetRomBank((port7ffd>>4)&1); trdos_enabled=false; };
		} else {
			if (HIBYTE(pc)==0x3d && trdos_feature==true) { SetRomBank(TRDOS_BANK); trdos_enabled=true; }
		};
		rrR=(rrR&0x80)|((rrR+1)&0x7f);
		opcode=GetMemZX(pc);					//aabbbccc->000aaccc,00000bbb
		param1=(opcode>>3)&7;					//00000bbb
		param2=(opcode&7);						//00000ccc !!! изменил
		param3=GetMemZX(pc+1);
//		printf("opcode=%02X p1=%1X p2=%X(%X) p3=%d \n",opcode,param1,param2,param2&7,param3);
	} while ( (*func_table[param2|((opcode>>3)&0x18)])()!=(int)true ); //TODO: страшный цикл... перепроверить! А еще - убрать постоянные возвращения значения true или false. это делается только для префиксов - лучше проверить, если префикс - сделать еще шаг...
	//if (pc1==pc) printf( "Warning!!! pc don't changed!!!\n" );
	//if (t_states1==t_states) printf( "Warning!!! t_states don't changed!!!\n" );
	
	if (t_states>=t_states_per_int) {
		t_states=0;
		if (IFF1!=0) {
			IFF1=0;
			IFF2=0;
			if (im_mode==2) {
				PushZX(pc);
				pc=GetWordMemZX((rrI<<8)+255); t_states+=19;
			} else {
				PushZX(pc);
				pc=0x38; t_states+=13; //oficialy need execute rst #38
			};
		};
		return true;
	};
	
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////

int func00000(void) {
	zxbyte	ttt;
	switch (param1) {
		case 0:		Msg( "NOP" );
			break;
		case 1:		Msg( "EX AF,AF'" );
			ttt=rrA; rrA=Register[rA_]; Register[rA_]=ttt;
			ttt=rrF; rrF=Register[rF_]; Register[rF_]=ttt;
			break;
		case 2:		Msg2( "DJNZ %04X",pc+2+(zxsbyte)param3);
			if ((--Register[rB])!=0) { pc+=2+(zxsbyte)param3; t_states+=13; }
			else { pc+=2; t_states+=8; };
			return true;
		case 3:		Msg2( "JR %04X", pc+2+(zxsbyte)param3);
			pc+=2+(zxsbyte)param3;
			t_states+=12;
			return true;
		default:	Msg3( "JR %s,%04X",szCCC[param1-4],pc+2+(zxsbyte)param3 );
			if ( testCCC(param1-4) ) {
				pc+=2+(zxsbyte)param3;
				t_states+=12; //???
				return true;
			} else {
				pc+=2; t_states+=7; return true;
			}; 
	};
	pc++;
	t_states+=4;
	return true;
}

int func00001(void) {
	switch (param1) {
		case 0: case 2: case 4:		Msg3("LD %s,%04X",szdd[param1/2],GetWordPCShift1());
			set_rpq_reg(param1,GetMemZX(pc+2));
			set_rpq_reg(param1+1,GetMemZX(pc+1));
			pc+=3; t_states+=10; break;
		case 6: Msg2("LD SP,%04X",GetWordPCShift1());
			sp=GetWordPCShift1();
			pc+=3; t_states+=10; break;
		case 1:	Msg ("ADD HL(IX),BC");
			setHLIXIY(add16( getHLIXIY(), getBC()));
			pc++; t_states+=11; break;
		case 3: Msg ("ADD HL(IX),DE");
			setHLIXIY(add16( getHLIXIY(), getDE()));
			pc++; t_states+=11; break;
		case 5:	Msg ("ADD HL(IX),HL(IX)");
			setHLIXIY(add16( getHLIXIY(), getHLIXIY()));
			pc++; t_states+=11; break;
		case 7:	Msg ("ADD HL(IX),SP");
			setHLIXIY(add16( getHLIXIY(), sp));
			pc++; t_states+=11; break;
	};
	return true;
}

int func00010(void) {
	switch (param1) {
		case 0: case 2:			Msg2("LD (%s),A",szdd[param1/2]);
			SetMemZX((Register[rB+param1]<<8)+Register[rC+param1],rrA);
			pc++; t_states+=7; break;
		case 1: case 3:			Msg2("LD A,(%s)",szdd[param1/2]);
			rrA=GetMemZX((Register[rB+param1-1]<<8)+Register[rC+param1-1]);
			pc++; t_states+=7; break;
		case 4:				Msg2("LD (%04X), HL(IX,IY)",GetWordPCShift1());
			SetWordMemZX( GetWordPCShift1(), getHLIXIY());
			pc+=3; t_states+=16; break;
		case 5:				Msg2("LD HL(IX,IY),(%04X)",GetWordPCShift1());
			setHLIXIY( GetWordMemZX(GetWordPCShift1()) );
			pc+=3; t_states+=16; break;
		case 6:				Msg2("LD (%04X),A",GetWordPCShift1());
			SetMemZX(GetWordPCShift1(),rrA);
			pc+=3; t_states+=13; break;
		case 7:				Msg2("LD A,(%04X)",GetWordPCShift1());
			rrA=GetMemZX(GetWordPCShift1());
			pc+=3; t_states+=13; break;
	};
	return true;
}

int func00011(void)
{
	switch (param1) {
        case 0: Msg("INC BC");
			setBC(getBC()+1); break;
		case 1: Msg("DEC BC");
			setBC(getBC()-1); break;
		case 2: Msg("INC DE");
			setDE(getDE()+1); break;
		case 3: Msg("DEC DE");
			setDE(getDE()-1); break;
		case 4: Msg("INC HL(IX,IY)");
			setHLIXIY(getHLIXIY()+1); break;
		case 5: Msg("DEC HL(IX,IY)");
			setHLIXIY(getHLIXIY()-1); break;
		case 6: Msg("INC SP");
			sp++; break;
		case 7: Msg("DEC SP");
			sp--; break;
	};
	pc++; t_states+=6;
	return true;
}

int func00100(void) {
	Msg2("INC %s",szrrr[param1]);
	zxbyte ttt=rrF&1;
	set_rpq_reg(param1, add(get_rpq_reg(param1),1,0));
	rrF=(rrF&0xfe)|ttt;
	if ((param1)==r_HL_) {
		if (flag_DDFD!=_NO_) { t_states+=23-4; pc+=2;} else { t_states+=11;	pc++; }
	} else { t_states+=4; pc++; };
	return true;
}

int func00101(void) {
	Msg2("DEC %s",szrrr[param1]);
	zxbyte ttt=rrF&1;
	set_rpq_reg(param1, sub(get_rpq_reg(param1),1,0));
	rrF=(rrF&0xfe)|ttt;
	if ((param1)==r_HL_) {
		if (flag_DDFD!=_NO_) { t_states+=23-4; pc+=2;} else { t_states+=11; pc++; }
	} else { t_states+=4; pc++; };
	return true;
}

int func00110(void) {
	Msg3("LD %s,%02X",szrrr[param1],param3);
	//Msg("ld r,n");
	if (param1==r_HL_) {
		if (flag_DDFD!=_NO_) {
			set_rpq_reg(param1,GetMemZX(pc+2)); //ld (ix+d),n
			pc++; t_states+=19-4;
		} else {
			set_rpq_reg(param1,GetMemZX(pc+1)); //ld (hl),n
			t_states+=10;
		};
	} else {
		set_rpq_reg(param1,GetMemZX(pc+1));
		t_states+=7;
	};
	pc+=2;
	return true;
}

int func00111(void) {
	zxbyte ttt;
	switch (param1) {
		case 0:			Msg("RLCA");
			ttt=(rrA&0x80)>>7;
			rrA=(rrA<<1) | ttt;
			rrF=(rrF&0xc4) | (rrA&0x29);
			break;
		case 2:			Msg("RLA");
			ttt=(rrA&0x80)>>7;
			rrA=(rrA<<1)|(rrF&1);
			rrF=(rrF&0xc4)|(rrA&0x28)|ttt;
			break;
		case 1:			Msg("RRCA");
			ttt=rrA&1;
			rrA=(rrA>>1) | (ttt<<7);
			rrF=(rrF&0xc4)|(rrA&0x28)|ttt;
			break;
		case 3:			Msg("RRA");
			ttt=rrA&1;
			rrA=(rrA>>1) | ((rrF&1)<<7);
			rrF=(rrF&0xc4)|(rrA&0x28)|ttt;
			break;
		case 4:			Msg("DAA");
			ttt=(((rrF&1)!=0)||((rrA&0xf0)>0x90)?0x60:0) | (((rrF&0x10)!=0)||((rrA&0x0f)>9)?6:0);
			if ((rrF&2)==0) rrA=add(rrA,ttt,0);
			else rrA=sub(rrA,ttt,0);
			break;
		case 5:			Msg("CPL");
			rrF=( ( ( (rrA^=0xff)^rrF )&0x28 )^rrF) | 0x12;
			break;
		case 6:			Msg("SCF");
			rrF=(rrF&0xc4)|(rrA&0x28)|0x01;
			break;
		case 7:			Msg("CCF");
			rrF=(rrF&0xc4)|(rrA&0x28)|((rrF&1)<<4)|((rrF&1)^1);
			break;
	};
	pc++; t_states+=4;
	return true;
}

int func01xxx(void) {
	if (opcode==0x76) {
		Msg("HALT");
		t_states=t_states_per_int; //quickly jumps to next frame
		if (IFF1==1) pc++; //if EI, next cmd :)
	}
	else {
		Msg3("LD %s,%s",szrrr[param1],szrrr[param2]);
		if (param1==r_HL_) set_rpq_reg(param1,get_reg(param2));
		else if (param2==r_HL_) set_reg(param1,get_rpq_reg(param2));
		else set_rpq_reg(param1,get_rpq_reg(param2));
		if ((param1==r_HL_)||((param2)==r_HL_)) {
			if (flag_DDFD==_NO_) t_states+=7;
			else { t_states+=19-4; pc++; };
		} else t_states+=4;
		pc++;
	};
	return true;
}

int func10xxx(void) {
	switch (param1) {
		case 0:			Msg2("ADD a,%s",szrrr[param2]);
			rrA=add(rrA,get_rpq_reg(param2),0);
			break;
		case 1:			Msg2("ADC a,%s",szrrr[param2]);
			rrA=add(rrA,get_rpq_reg(param2),rrF&1);
			break;
		case 2:			Msg2("SUB %s",szrrr[param2]);
			rrA=sub(rrA,get_rpq_reg(param2),0);
			break;
		case 3:			Msg2("SBC a,%s",szrrr[param2]);
			rrA=sub(rrA,get_rpq_reg(param2),rrF&1);
			break;
		case 4:			Msg2("AND %s",szrrr[param2]);
			rrA&=get_rpq_reg(param2);
			rrF=(rrA&0xa8) | parity(rrA) | 0x10 | zero(rrA);
			break;
		case 5:			Msg2("XOR %s",szrrr[param2]);
			rrA^=get_rpq_reg(param2);
			rrF=(rrA&0xa8) | parity(rrA) | zero(rrA);
			break;
		case 6:			Msg2("OR %s",szrrr[param2]);
			rrA|=get_rpq_reg(param2);
			rrF=(rrA&0xa8) | parity(rrA) | zero(rrA);
			break;
		case 7:			Msg2("CP %s",szrrr[param2]);
			sub(rrA,get_rpq_reg(param2),0);
			rrF=(rrF&0xc7)|(get_rpq_reg(param2)&0x28); //coz rezult drops
			break;
	};
	if ((param2)==r_HL_) {
		if (flag_DDFD!=_NO_) {
			t_states+=19-4;
			pc+=2;
		} else {
			t_states+=7;
			pc++;
		}
	} else {
		t_states+=4;
		pc++;
	};
	return true;
}

int func11000(void) {
	Msg2("RET %s",szCCC[param1]);
	if ( testCCC(param1) ) { pc=PopZX(); t_states+=11;}
	else { pc++; t_states+=5; }
	return true;
}

int func11001(void) {
	zxbyte	ttt;
	zxword  w;
	switch (param1) {
		case 0: case 2:			Msg2("POP %s",szdd[param1/2]);
			w=PopZX(); Register[param1]=HIBYTE(w); Register[param1+1]=LOBYTE(w); pc++; t_states+=10; break;
		case 3:					Msg("EXX");
			for (i=rB;i<(rL+1);i++) {
				ttt=Register[i]; Register[i]=Register[i+rB_-rB]; Register[i+rB_-rB]=ttt;
			};
			pc++; t_states+=4;
			break;
		case 4:					Msg("POP HL/IX/IY");
			w=PopZX(); Register[rH+flag_DDFD]=HIBYTE(w); Register[rL+flag_DDFD]=LOBYTE(w); pc++; t_states+=10; break;
		case 6:					Msg("POP AF");
			w=PopZX(); rrA=HIBYTE(w); rrF=LOBYTE(w); pc++; t_states+=10; break;
		case 7:					Msg("LD SP,HL/IX/IY");
			sp=(get_rpq_reg(rH)<<8)+get_rpq_reg(rL);
			t_states+=10;
			pc++;
			break;
		case 1:					Msg("RET");
			pc=PopZX(); t_states+=10; break;
		case 5:					Msg("JP hl/ix/iy");
			pc=(get_rpq_reg(rH)<<8)+get_rpq_reg(rL); t_states+=4; return true;
		default:
			break;
	};
	return true;
}

int func11010(void) {
	Msg3("JP %s,%04X",szCCC[param1],GetWordPCShift1());
	if ( testCCC(param1) ) 	pc=GetWordPCShift1();
	else pc+=3;
	t_states+=10;
	return true;
}

int func11011(void) {
	zxbyte	ttt;
	switch (param1) {
		case 0:					Msg2("JP %04X",GetWordPCShift1());
			pc=GetWordPCShift1(); t_states+=10;
			break;
		case 1:					Msg("prefix CB");
			pc++;
			rrR=(rrR&0x80)|((rrR+1)&0x7f);
			if (flag_DDFD!=0) param3=GetMemZX(pc++);
			opcode=GetMemZX(pc);
			param1=(opcode>>3)&7;
			param2=(opcode&7);
			pc++;
			//printf("opcode=%02X p1=%1X p2=%X p3=%d \n",opcode,param1,param2,param3);
			(*funcCB_table[(opcode&0xc0)>>6])();
			break;
		case 2:
			OutPortZX(param3+(rrA<<8),rrA);
			pc+=2; t_states+=11; break;
		case 3:
			rrA=InPortZX(param3+(rrA<<8));
			pc+=2; t_states+=11; break;
		case 4:					Msg("EX (SP),HL (IX,IY)");
			ttt=GetMemZX(sp+1); SetMemZX(sp+1,get_rpq_reg(rH)); set_rpq_reg(rH,ttt);
			ttt=GetMemZX(sp); SetMemZX(sp,get_rpq_reg(rL)); set_rpq_reg(rL,ttt);			
			pc++; t_states+=19; break;
		case 5:					Msg("EX DE,HL");
			ttt=Register[rD]; Register[rD]=Register[rH]; Register[rH]=ttt;
			ttt=Register[rE]; Register[rE]=Register[rL]; Register[rL]=ttt;
			pc++; t_states+=4; break;
		case 6:					Msg("DI");
			IFF1=0; IFF2=0; pc++; t_states+=4; return i_diei;
		case 7:					Msg("EI");
			IFF1=1; IFF2=1; pc++; t_states+=4; return i_diei;
		default:
			break;
	};
	return true;
}

int func11100(void) {
	Msg3("CALL %s,%04X",szCCC[param1],GetWordPCShift1());
	if ( testCCC(param1) ) { PushZX(pc+3); pc=GetWordPCShift1(); t_states+=17;}
	else { pc+=3; t_states+=10; }
	return true;
}

int func11101(void) {
	switch (param1) {
		case 0: case 2:			Msg2("PUSH %s",szdd[param1/2]); //push bc or de
			PushZX( (Register[param1]<<8)+Register[param1+1] ); t_states+=11; break;
		case 4:					Msg("PUSH hl/ix/iy"); //push hl/ix/iy
			PushZX( (Register[rH+flag_DDFD]<<8)+Register[rL+flag_DDFD] ); t_states+=11; break;
		case 6:					Msg("PUSH AF"); //push af
			PushZX( (rrA<<8)+rrF ); t_states+=11; break;
		case 1:					Msg2("CALL %04X",GetWordPCShift1()); //call nn
			PushZX(pc+3); pc=GetWordPCShift1(); t_states+=17;
			return true;
		case 3:					//prefix DD
			Msg("prefix DD");
			flag_DDFD=_DD_;
			t_states+=4; pc++;
			//rrR=(rrR&0x80)|((rrR+1)&0x7f);
			return i_prefix;
		case 5:					//prefix ED
			Msg("prefix ED");
			flag_DDFD=_NO_;
			pc++;
			rrR=(rrR&0x80)|((rrR+1)&0x7f);
			opcode=GetMemZX(pc);
			param1=(opcode>>3)&7;
			param2=(opcode&7);
			//printf("opcode=%02X p1=%1X p2=%X \n",opcode,param1,param2);
			(*funcED_table[((opcode&0xc0)>>3)|param2])(); //TODO здесь возможен вылет по стеку если будет много префиксов.
			return true;
		case 7:					//prefix FD
			Msg("prefix FD");
			flag_DDFD=_FD_;
			t_states+=4; pc++;
			//rrR=(rrR&0x80)|((rrR+1)&0x7f); косяк ))
			return i_prefix;
	};
	pc++;
	return true;
}

int func11110(void) {
	switch (param1) {
		case 0:			Msg2("ADD a,%02X",param3);
			rrA=add(rrA,param3,0); break;
		case 1:			Msg2("ADC a,%02X",param3);
			rrA=add(rrA,param3,rrF&1); break;
		case 2:			Msg2("SUB a,%02X",param3);
			rrA=sub(rrA,param3,0); break;
		case 3:			Msg2("SBC a,%02X",param3);
			rrA=sub(rrA,param3,rrF&1); break;
		case 4:			Msg2("AND %02X",param3);
			rrA&=param3;
			rrF=(rrA&0xa8) | parity(rrA) | 0x10 | zero(rrA);
			break;
		case 5:			Msg2("XOR %02X",param3);
			rrA^=param3;
			rrF=(rrA&0xa8) | parity(rrA) | zero(rrA);
			break;
		case 6:			Msg2("OR %02X",param3);
			rrA|=param3;
			rrF=(rrA&0xa8) | parity(rrA) | zero(rrA);
			break;
		case 7:			Msg2("CP %02X",param3);
			sub(rrA,param3,0);
			rrF=(rrF&0xc7)|(param3&0x28);
			break;
	};
	pc+=2; t_states+=8;
	return true;
}

int func11111(void) {
	Msg2("RST %02X",param1<<3);
	PushZX(pc+1); pc=param1<<3; t_states+=11;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// functions for CB_prefix instructions																//
//////////////////////////////////////////////////////////////////////////////////////////////////////
void funcCB00xxx(void) {
	//RLC,RL,RRC,RR,SLA,SLL,SRA,SRL
	zxbyte ttt;
	zxbyte rez=get_rpq_reg(flag_DDFD==0?param2:r_HL_);
	switch (param1) {
		case 0: 	Msg2("RLC %s",szrrr[param2]);	ttt=(rez&0x80)>>7;	rez=(rez<<1) | ttt;			break;
		case 1: 	Msg2("RRC %s",szrrr[param2]);	ttt=rez&1;			rez=(rez>>1) | (ttt<<7);	break;
		case 2: 	Msg2("RL %s",szrrr[param2]);	ttt=(rez&0x80)>>7;	rez=(rez<<1) | flagC;		break;
		case 3: 	Msg2("RR %s",szrrr[param2]);	ttt=(rez&1);		rez=(rez>>1) | (flagC<<7);	break;
		case 4: 	Msg2("SLA %s",szrrr[param2]);	ttt=(rez&0x80)>>7;	rez=rez<<1;					break;
		case 5: 	Msg2("SRA %s",szrrr[param2]);	ttt=(rez&1);		rez=(rez&0x80)|(rez>>1);	break;
		case 6: 	Msg2("SLL %s",szrrr[param2]);	ttt=(rez&0x80)>>7;	rez=(rez<<1)|1;				break;
		case 7: 	Msg2("SRL %s",szrrr[param2]);	ttt=(rez&1);		rez=rez>>1;					break;
	};
	rrF=(rez&bin(1,0,1,0,1,0,0,0)) | ttt | zero(rez) | parity (rez);
    set_rpq_reg(param2, rez);
	if (flag_DDFD!=0) {
		set_rpq_reg(r_HL_, rez);
		t_states+=23;
	} else t_states+=(param2==r_HL_)?15:8;
};

void funcCB01xxx(void) {
	//bit b,r
	Msg3("BIT %d,%s",param1,szrrr[param2]);
	if ((get_rpq_reg((flag_DDFD==0)?param2:r_HL_)&(1<<param1))==0) {
		//set z=1, pv=1
		rrF=(rrF&1) | bin(0,1,0,1,0,1,0,0);
	} else {
		//set z=0, pv=0, S if b=7, 5 if b=5, 3 if b=3
		rrF=(rrF&1) | ((1<<param1)&0xa8) | 0x10;
	};
	t_states+=(flag_DDFD==0 ? (param2==r_HL_?12:8):20);
};

void funcCB10xxx(void) {
	//res b,r
	Msg3("RES %d,%s",param1,szrrr[param2]);
	if (flag_DDFD==0) {
		set_rpq_reg(param2, get_rpq_reg(param2)&(0xff^(1<<param1)));
		if (param2==r_HL_) t_states+=15; else t_states+=8;
	} else {
		set_rpq_reg(param2,get_rpq_reg(r_HL_)&(0xff^(1<<param1)));
		set_rpq_reg(r_HL_,get_rpq_reg(param2));
		t_states+=23;
	};
};

void funcCB11xxx(void) {
	//set b,r
	Msg3("SET %d,%s",param1,szrrr[param2]);
	if (flag_DDFD==0) {
		set_rpq_reg(param2, get_rpq_reg(param2)|(1<<param1));
		if (param2==r_HL_) t_states+=15; else t_states+=8;
	} else {
		set_rpq_reg(param2,get_rpq_reg(r_HL_)|(1<<param1));
		set_rpq_reg(r_HL_,get_rpq_reg(param2));
		t_states+=23;
	};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// functions for ED_prefix instructions																//
//////////////////////////////////////////////////////////////////////////////////////////////////////

void funcED01000(void) {		Msg2("IN %s,(C)",szrrr[param1]);
	zxbyte ttt = InPortZX( (rrB<<8)+rrC );
	rrF=(rrF&1)|(ttt&0xa4)|zero(ttt)|parity(ttt);
	if (param1!=r_HL_) set_rpq_reg(param1,ttt);
	t_states+=12; pc++;
};

void funcED01001(void) {		Msg2("OUT (C),%s",szrrr[param1]);
	OutPortZX((rrB<<8)+rrC,(param1!=r_HL_)?get_rpq_reg(param1):0);
	t_states+=12; pc++;
};

void funcED01010(void) {
	switch (param1) {
		case 0:	Msg("SBC HL,BC"); setHL(sbc16(getHL(),getBC(),rrF&1));	break;
		case 1:	Msg("ADC HL,BC"); setHL(adc16(getHL(),getBC(),rrF&1));	break;
		case 2:	Msg("SBC HL,DE"); setHL(sbc16(getHL(),getDE(),rrF&1));	break;
		case 3:	Msg("ADC HL,DE"); setHL(adc16(getHL(),getDE(),rrF&1));	break;
		case 4:	Msg("SBC HL,HL"); setHL(sbc16(getHL(),getHL(),rrF&1));	break;
		case 5:	Msg("ADC HL,HL"); setHL(adc16(getHL(),getHL(),rrF&1));	break;
		case 6:	Msg("SBC HL,SP"); setHL(sbc16(getHL(),sp,rrF&1));		break;
		case 7:	Msg("ADC HL,SP"); setHL(adc16(getHL(),sp,rrF&1));		break;
	};
	t_states+=15; pc++;
};

void funcED01011(void) {
	switch (param1) {
		case 0: Msg2("LD (%04X),BC",GetWordPCShift1());	SetWordMemZX( GetWordPCShift1(), getBC());		break;
		case 1:	Msg2("LD BC,(%04X)",GetWordPCShift1());	setBC( GetWordMemZX(GetWordPCShift1()) );		break;
		case 2:	Msg2("LD (%04X),DE",GetWordPCShift1());	SetWordMemZX( GetWordPCShift1(), getDE());		break;
		case 3:	Msg2("LD DE,(%04X)",GetWordPCShift1());	setDE( GetWordMemZX(GetWordPCShift1()) );		break;
		case 4:	Msg2("LD (%04X),HL",GetWordPCShift1());	SetWordMemZX( GetWordPCShift1(), getHL());	break;
		case 5:	Msg2("LD HL,(%04X)",GetWordPCShift1());	setHL( GetWordMemZX(GetWordPCShift1()) );	break;
		case 6:	Msg2("LD (%04X),SP",GetWordPCShift1());	SetWordMemZX( GetWordPCShift1(), sp);		break;
		case 7: Msg2("LD SP,(%04X)",GetWordPCShift1());	sp=GetWordMemZX(GetWordPCShift1());			break;
	};
	pc+=3; t_states+=20;
};

void funcED01100(void) {	Msg("NEG");
	rrA=sub(0,rrA,0);
	pc++; t_states+=8;
};

void funcED01101(void) {	Msg("RETI/RETN");
	pc=PopZX(); t_states+=14; IFF1=IFF2;
};

void funcED01110(void) {
//im
	switch (param1&3) {
		case 0: case 1:	Msg("IM 0");	im_mode=0; break;
		case 2:			Msg("IM 1");	im_mode=1; break;
		case 3:			Msg("IM 2");	im_mode=2; break;
	};
	t_states+=8;
	pc++;
};

void funcED01111(void) {
	zxword www;
	switch (param1) {
		case 0: Msg("LD I,A");
			rrI=rrA;
			t_states+=9; break;
		case 1: Msg("LD R,A"); rrR=rrA; t_states+=9; break;
		case 2: Msg("LD A,I"); rrA=rrI; rrF=(rrF&1)|(rrA&0xA4)|zero(rrA)|IFF2<<2; t_states+=9; break;
		case 3: Msg("LD A,R"); rrA=rrR; rrF=(rrF&1)|(rrA&0xA4)|zero(rrA)|IFF2<<2; t_states+=9; break;
		case 4: Msg("RRD");
			set_rpq_reg( r_HL_, ((www=(get_rpq_reg(r_HL_)))>>4)|(rrA<<4) );
			rrA=(rrA&0xf0)|(www&0x0f);
			rrF=(rrF&1)|(rrA&0xa4)|zero(rrA)|parity(rrA);
			t_states+=18;
			break;
		case 5: Msg("RLD");
			set_rpq_reg( r_HL_, (zxbyte)((www=(get_rpq_reg(r_HL_)<<4)|(rrA&0xf))) );
			rrA=(rrA&0xf0)|((www>>8)&0x0f);
			rrF=(rrF&1)|(rrA&0xa4)|zero(rrA)|parity(rrA);
			t_states+=18;
			break;
		case 6: case 7: Msg("NOP");
			t_states+=4;
			break;
	};
	pc++;
};

void funcED10000(void) {
	zxbyte ttt;
	if (param1<4) {Msg("NOP"); t_states+=4; pc++; return;};

	SetMemZX(getDE(),ttt=GetMemZX(getHL()));
	setBC(getBC()-1);
	switch (param1-4) {
		case 0:		Msg("LDI");
			setHL(getHL()+1); setDE(getDE()+1);
			pc++; t_states+=16; break;
		case 1:		Msg("LDD");
			setHL(getHL()-1); setDE(getDE()-1);
			pc++; t_states+=16; break;
		case 2:		Msg("LDIR");
			setHL(getHL()+1); setDE(getDE()+1);
			if (getBC()==0) { 
				pc++; t_states+=16; 
			} else { 
				pc--; t_states+=21;
			};
			break;
		case 3:		Msg("LDDR");
			setHL(getHL()-1); setDE(getDE()-1);
			if (getBC()==0) { pc++; t_states+=16; } else { pc--; t_states+=21; };
			break;
	};
	rrF=(rrF&0xc1)|(getBC()==0?0:4)|(((rrA+ttt)&1)<<4)|((rrA+ttt)&8);
};

void funcED10001(void) {
	if (param1<4) {Msg("NOP"); t_states+=4; pc++; return;};
	zxbyte ttt=rrF&1; sub(rrA,GetMemZX(getHL()),0); rrF=(rrF&0xfe)|(ttt);
	ttt=rrA-GetMemZX(getHL())-((rrF&0x10)>>4);
	setBC(getBC()-1);
	rrF=(rrF&0xd0)|2|((ttt&2)<<4)|(ttt&8)|(getBC()==0?0:4);
	switch (param1-4) {
		case 0:		Msg("CPI");
			setHL(getHL()+1); pc++; t_states+=16; break;
		case 1:		Msg("CPD");
			setHL(getHL()-1); pc++; t_states+=16; break;
		case 2:		Msg("CPIR");
			setHL(getHL()+1);
			if ((getBC()==0)||(rrF&0x40)!=0) { pc++; t_states+=16; } else { pc--; t_states+=21; };
			break;
		case 3:		Msg("CPDR");
			setHL(getHL()-1);
			if ((getBC()==0)||(rrF&0x40)!=0) { pc++; t_states+=16; } else { pc--; t_states+=21; };
			break;
	};
};

void funcED10010(void) {
	if (param1<4) {Msg("NOP"); t_states+=4; pc++; return;};
	zxbyte ttt;
	switch (param1-4) {
		case 0:		Msg("INI");
			SetMemZX(getHL(),ttt=InPortZX(getBC())); setHL(getHL()+1); pc++; t_states+=16;
			rrF=(rrF&0xe4)|((ttt&0x80)>>6)|(HIBYTE(((zxbyte)(rrC+1))+ttt)!=0)?0x11:0;
			break;
		case 1:		Msg("IND");
			SetMemZX(getHL(),ttt=InPortZX(getBC())); setHL(getHL()-1); pc++; t_states+=16;
			rrF=(rrF&0xe4)|((ttt&0x80)>>6)|(HIBYTE(((zxbyte)(rrC+1))+ttt)!=0)?0x11:0;
			break;
		case 2:		Msg("INIR");
			SetMemZX(getHL(),ttt=InPortZX(getBC())); setHL(getHL()+1); pc++; t_states+=16;
			rrF=(rrF&0xe4)|((ttt&0x80)>>6)|(HIBYTE(((zxbyte)(rrC+1))+ttt)!=0)?0x11:0;
			if (rrB==0) { pc++; t_states+=16; } else { pc--; t_states+=21; };
			break;
		case 3:		Msg("INDR");
			SetMemZX(getHL(),ttt=InPortZX(getBC())); setHL(getHL()-1); pc++; t_states+=16;
			rrF=(rrF&0xe4)|((ttt&0x80)>>6)|(HIBYTE(((zxbyte)(rrC-1))+ttt)!=0)?0x11:0;
			if (rrB==0) { pc++; t_states+=16; } else { pc--; t_states+=21; };
			break;
	};
	rrB=sub(rrB,1,0);
};

void funcED10011(void) {
	if (param1<4) {Msg("NOP"); t_states+=4; pc++; return;};
	zxbyte ttt;
	switch (param1-4) {
		case 0:		Msg("OUTI");
			OutPortZX(getBC(),ttt=GetMemZX(getHL())); setHL(getHL()+1); pc++; t_states+=16;
			rrF=(rrF&0xe4)|((ttt&0x80)>>6)|(HIBYTE(((zxbyte)(rrC+1))+ttt)!=0)?0x11:0;
			break;
		case 1:		Msg("OUTD");
			OutPortZX(getBC(),ttt=GetMemZX(getHL())); setHL(getHL()-1); pc++; t_states+=16;
			rrF=(rrF&0xe4)|((ttt&0x80)>>6)|(HIBYTE(((zxbyte)(rrC-1))+ttt)!=0)?0x11:0;
			break;
		case 2:		Msg("OTIR");
			OutPortZX(getBC(),ttt=GetMemZX(getHL())); setHL(getHL()+1);
			rrF=(rrF&0xe4)|((ttt&0x80)>>6)|(HIBYTE(((zxbyte)(rrC+1))+ttt)!=0)?0x11:0;
			if (rrB==0) { pc++; t_states+=16; } else { pc--; t_states+=21; };
			break;
		case 3:		Msg("OTDR");
			OutPortZX(getBC(),ttt=GetMemZX(getHL())); setHL(getHL()-1);
			rrF=(rrF&0xe4)|((ttt&0x80)>>6)|(HIBYTE(((zxbyte)(rrC-1))+ttt)!=0)?0x11:0;
			if (rrB==0) { pc++; t_states+=16; } else { pc--; t_states+=21; };
			break;
	};
	rrB=sub(rrB,1,0);
};

void funcEDxxxxx(void) {
	Msg("NOP");
	t_states+=4;
	pc++;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

//tst corresponding flag
bool testCCC (int ccc) {
	switch (ccc) {
		case 0:	return (rrF&0x40)==0;	//NZ
		case 1:	return (rrF&0x40)!=0;	//Z
		case 2:	return (rrF&0x01)==0;	//NC
		case 3:	return (rrF&0x01)!=0;	//C
		case 4:	return (rrF&0x04)==0;	//PO
		case 5:	return (rrF&0x04)!=0;	//PE
		case 6:	return (rrF&0x80)==0;	//P
		case 7:	return (rrF&0x80)!=0;	//M
	};
	return false; //anti-warning
};

// get value of BCDEHL(HL)A->ix,iy register
int get_rpq_reg(int reg) {
	switch (reg) {
		case r_HL_:			return GetMemZX( (Register[rH+flag_DDFD]<<8)+Register[rL+flag_DDFD]+(flag_DDFD==_NO_?0:(zxsbyte)param3) );
		case rH: case rL:	return Register[reg+flag_DDFD];
		default:			return Register[reg];
	};
}

// set value of BCDEHL(HL)A->ix,iy register
void set_rpq_reg(int reg, zxbyte value) {
	switch (reg) {
		case r_HL_:			{ SetMemZX( (Register[rH+flag_DDFD]<<8)+Register[rL+flag_DDFD]+(flag_DDFD==_NO_?0:(zxsbyte)param3), value ); break; };
		case rH: case rL:	{ Register[reg+flag_DDFD]=value; break; }
		default:			{ Register[reg]=value; break; }
	};
}
int get_reg(int reg) {
	switch (reg) {
		case r_HL_:			return GetMemZX( (Register[rH]<<8)+Register[rL]);
		default:			return Register[reg];
	};
}

void set_reg(int reg, zxbyte value) {
	switch (reg) {
		case r_HL_:			{ SetMemZX( (Register[rH]<<8)+Register[rL], value ); break; };
		default:			{ Register[reg]=value; break; }
	};
}

void PushZX( zxword val ) {
	SetMemZX( --sp,HIBYTE(val) );
	SetMemZX( --sp,LOBYTE(val) );
};

zxword PopZX( void ) {
	zxword rez = GetWordMemZX(sp); 
	sp++; sp++;
	return rez;
};

zxbyte GetMemZX( zxword address ) {
	if (address>=0x4000) return ram[address+pg_shift[address>>14]];
	else return rom[address+pg_shift[address>>14]];
};

void SetMemZX( zxword address, zxbyte data) {
	if (address>=0x4000) ram[address+pg_shift[address>>14]]=data;
};

zxword GetWordMemZX( zxword address ) {
	return GetMemZX(address)+(GetMemZX(address+1)<<8);
};

void SetWordMemZX( zxword address, zxword data ) {
	SetMemZX(address, LOBYTE(data));
	SetMemZX(address+1, HIBYTE(data));
};

zxword GetWordPCShift1(void) {
	return (GetMemZX(pc+2)<<8)+GetMemZX(pc+1);
};

//////////////////////////////////////////////////////////////////////////
void SetRomBank(zxbyte page) {
//	printf("page=%X",page);
	RomBank=page&3;
	pg_shift[0]=(page&3)*0x4000;
};

void SetRamBank(zxbyte page) {
	RamBank=page&7;
	pg_shift[3]=(page&7)*0x4000-0xc000;
};

LPVOID GetRamPtr(int adr) {
	return &ram[adr];
};

LPVOID GetRomPtr(int adr) {
	return &rom[adr];
};

LPVOID GetScrPtr(void) {
	if ((port7ffd&8)==0) 
		return &ram[0x4000*5];
	else
		return &ram[0x4000*7];
};

LPVOID GetAYRegs(void) {
	return &ayport[0];
}
//////////////////////////////////////////////////////////////////////////

zxbyte parity(zxbyte testbyte) {
	int temp=1;
	do {
		temp^=testbyte&1;
		testbyte=testbyte>>1;
	} while (testbyte);
	return temp<<2;
};

zxbyte overflow(zxsword testsword) {
	return ((testsword<-128 || testsword>127)?0x4:0);
};

zxbyte zero(zxbyte testbyte) {
	return (testbyte!=0?0:0x40);
};

//-[ports]-//////////////////////////////////////////////////////////////////////////
zxbyte InPortZX(zxword port) {
	zxbyte rez=0xff;
//	printf("pc=%04x ts=%04x in %04x\n",pc,t_states,port);
	///tstticker1=3;
	if (trdos_enabled) {
		switch (LOBYTE(port)) {
			case 0x1f: return lptrdos->InVG93(0x1f,pc);
			case 0x3f: return lptrdos->InVG93(0x3f,pc);
			case 0x5f: return lptrdos->InVG93(0x5f,pc);
			case 0x7f: return lptrdos->InVG93(0x7f,pc);
			case 0xff: return lptrdos->InVG93(0xff,pc);
		};
	};

	if ((LOBYTE(port))==0x1f) { //kempston joystick
		return 0;
	};
	if ((port&0xff)==0xfe) {
		rez=0xff;
		for (int i=0;i<8;i++) {
			if ((((HIBYTE(port))>>i)&1)==0) rez&=keyboard[i];
		};
		return rez;
	} else return 0xff;
	if (port==0xfffd) {
//		printf("in(0xfffd)\n");
		return ayport[ayselectedport];
	};
};

void OutPortZX(zxword port, zxbyte value) {
//	static int nmbr=0;
//	static int nmax=0;
//	static int t_tmp=0;
	if (trdos_enabled) {
		switch (LOBYTE(port)) {
			case 0x1f: lptrdos->OutVG93(0x1f,value,pc); return;
			case 0x3f: lptrdos->OutVG93(0x3f,value,pc); return;
			case 0x5f: lptrdos->OutVG93(0x5f,value,pc); return;
			case 0x7f: lptrdos->OutVG93(0x7f,value,pc); return;
			case 0xff: lptrdos->OutVG93(0xff,value,pc); return;
		};
	};
	if ((port&0xff)==0xfe) {
		portfe=value;
//		if (t_tmp>t_states) {
//			if (nmax<nmbr) nmax=nmbr;
//			nmbr=0;
//		};
//		t_tmp=t_states;
//		nmbr++;
//		printf("t=%05d out(%04X),%02X n=%05d (%d)\n",t_states,port,value,nmbr,nmax);
	};
	if (port==0xfffd) {
//		printf("pc=%04X out(0xfffd),%02X\n",pc,value);
		ayselectedport=value&15;
		return;
	};
	if (port==0xbffd) {
//		printf("pc=%04X out(0xbffd),%02X\n",pc,value);
		ayport[ayselectedport]=value;
		return;
	};
	//TODO: Разобраться с корявостью. сборка Silicon Dreams от Energy Minds глючит.
	if (((port==0x7ffd)||(port&0x80ff==0xfd/*попытка реализовать неполную дешифрацию*/)) && ((port7ffd&0x20)==0)) {
//	if (((port&255)==0xfd) && ((port7ffd&0x20)==0)) {
		port7ffd=value;
		SetRamBank(value&7);
		SetRomBank((value>>4)&1);
//		zxword tmp=0xc000; printf("7ffd=%02X %02X %02X %02X %02X %02X %02X %02X %02X\n",port7ffd,GetMemZX(tmp),GetMemZX(tmp+1),GetMemZX(tmp+2),GetMemZX(tmp+3),GetMemZX(tmp+4),GetMemZX(tmp+5),GetMemZX(tmp+6),GetMemZX(tmp+7));
	};

};

///////////////////////////////////////////////////////////////////////////////////
void SetKeyZX(zxbyte row, zxbyte nkey, bool s) {
	if (s) keyboard[row]&=255^(1<<nkey);
	else keyboard[row]|=1<<nkey;
};

void ResetKeyboardZX(void) {
	for (int i=0;i<8;i++) keyboard[i]=0xff;
};
////////////////////////////////////////////////////////////////////////////////////
zxbyte add(zxbyte a1, zxbyte a2, zxbyte a3) {
	zxbyte rez=a1+a2+a3;
	zxsword srez=(zxsbyte)a1+(zxsbyte)a2+a3;
	rrF=(rez&0xa8) | zero(rez) | (((a1&0xf)+(a2&0xf)+(a3&0xf))&0x10) | (HIBYTE(a1+a2+a3)&1) | overflow(srez); //SZ5H3VNC
	return rez;
};

zxbyte sub(zxbyte a1, zxbyte a2, zxbyte a3) {
	zxbyte rez=a1-a2-a3;
	zxsword srez=(zxsbyte)a1-(zxsbyte)a2-a3;
	rrF=(rez&0xa8) | zero(rez) | (((a1&0xf)-(a2&0xf)-(a3&0xf))&0x10) | (HIBYTE(a1-a2-a3)&1) | overflow(srez) | 0x2; //SZ5H3VNC
	return rez;
};

zxword add16(zxword a1, zxword a2) {
	zxword rez=a1+a2;
	//TODO: check that srez really do not need
	//zxsdword srez=(zxsword)a1+(zxsword)a2;
	rrF=(rrF&0xc4) | (HIBYTE(rez)&0x28) | ((HIBYTE((a1&0xfff)+(a2&0xfff)))&0x10) | (HIWORD(a1+a2)&1);
	return rez;
};
zxword adc16(zxword a1, zxword a2, zxbyte a3) {
	zxword rez=a1+a2+a3;
	zxsdword srez=(zxsword)a1+(zxsword)a2+a3;
	rrF=(HIBYTE(rez)&0xa8) | ((rez==0)?0x40:0) | ((HIBYTE((a1&0xfff)+(a2&0xfff)+(a3&0xfff)))&0x10) | (HIWORD(a1+a2+a3)&1) | ((srez<-32768 || srez>32767)?0x4:0); //SZ5H3VNC
	return rez;
};
zxword sbc16(zxword a1, zxword a2, zxbyte a3) {
	zxword rez=a1-a2-a3;
	zxsdword srez=(zxsword)a1-(zxsword)a2-a3;
	rrF=(HIBYTE(rez)&0xa8) | ((rez==0)?0x40:0) | ((HIBYTE((a1&0xfff)-(a2&0xfff)-(a3&0xfff)))&0x10) | (HIWORD(a1-a2-a3)&1) | (((srez<-32768) || (srez>32767))?0x4:0) | 0x2; //SZ5H3VNC
//	printf("sbc %04X,%04X (C=%01X) rez=%04X HB&A8=%02X H=%02X C=%02X f=%02X \n",a1,a2,a3,rez,(HIBYTE(rez)&0xa8),((HIBYTE((a1&0xfff)-(a2&0xfff)-(a3&0xfff)))&0x10),(HIWORD(a1-a2-a3)&1),rrF);
//	printf("sa1=%X, sa2=%X, srez=%X\n",(zxsword)a1,(zxsword)a2,srez);
	return rez;
};

////////////////////////////////////////////////////////////////////////////////////

void print_z80_state(bool ttt) {
//	zxword tmp;
//	if (ttt) {
//	printf("PC=%04X ",pc);
//	printf("SP=%04X ",sp);
//	printf(rrF&128?"S":"s");
//	printf(rrF&64?"Z":"z");
//	printf(rrF&32?"5":".");
//	printf(rrF&16?"H":"h");
//	printf(rrF&8?"3":".");
//	printf(rrF&4?"P":"p");
//	printf(rrF&2?"N":"n");
//	printf(rrF&1?"C":"c");
//	printf(" 7ffd=%02x",port7ffd);
//	tmp=0xc000; printf(" %02X %02X %02X %02X %02X %02X %02X %02X",GetMemZX(tmp),GetMemZX(tmp+1),GetMemZX(tmp+2),GetMemZX(tmp+3),GetMemZX(tmp+4),GetMemZX(tmp+5),GetMemZX(tmp+6),GetMemZX(tmp+7));
//	printf("\nAF=%02X%02X ",rrA,rrF);
//	printf("BC=%04X ",(Register[rB]<<8)+Register[rC]);
//	printf("DE=%04X ",(Register[rD]<<8)+Register[rE]);
//	printf("HL=%04X (HL)=%02X\n",(Register[rH]<<8)+Register[rL],GetMemZX((Register[rH]<<8)+Register[rL]));
//	printf("AF'%02X%02X ",Register[rA_],Register[rF_]);
//	printf("BC'%04X ",(Register[rB_]<<8)+Register[rC_]);
//	printf("DE'%04X ",(Register[rD_]<<8)+Register[rE_]);
//	printf("HL'%04X \n",(Register[rH_]<<8)+Register[rL_]);
//	printf("IX=%04X ",(Register[rIXh]<<8)+Register[rIXl]);
//	printf("IY=%04X ",(Register[rIYh]<<8)+Register[rIYl]);
//	printf("IR=%04X ",(rrI<<8)+rrR);
//	printf("t:%05d im%1d ",t_states,im_mode);
//	printf(IFF1==0?"di":"ei");
//	printf("\n%02X %02X %02X %02X %02X %02X %02X %02X",GetMemZX(pc),GetMemZX(pc+1),GetMemZX(pc+2),GetMemZX(pc+3),GetMemZX(pc+4),GetMemZX(pc+5),GetMemZX(pc+6),GetMemZX(pc+7));
//	printf("\n\n");
//	} else {
//		/*if(tstticker1>2)*/ printf("t:%05d PC=%04X HL=%04X DE=%04X BC=%04X 7ffd=%02x",t_states,pc,getHL(),getDE(),getBC(),port7ffd); 
//		tmp=0xc000; printf("%02X %02X %02X %02X %02X %02X %02X %02X\n",GetMemZX(tmp),GetMemZX(tmp+1),GetMemZX(tmp+2),GetMemZX(tmp+3),GetMemZX(tmp+4),GetMemZX(tmp+5),GetMemZX(tmp+6),GetMemZX(tmp+7));
//	};
};
